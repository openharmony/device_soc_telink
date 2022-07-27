/******************************************************************************
 * Copyright (c) 2022 Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 * All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *****************************************************************************/

#include "device_resource_if.h"
#include "gpio/gpio_core.h"
#include "hdf_device_desc.h"
#include "osal.h"

#include <B91/gpio.h>

#include <b91_irq.h>

#define GPIO_INDEX_MAX ((sizeof(g_GpioIndexToActualPin) / sizeof(gpio_pin_e)))

struct B91GpioCntlr {
    struct GpioCntlr cntlr;

    uint8_t *pinReflectionMap;

    struct {
        bool irq_enabled;
    }* config;

    uint8_t pinNum;
};

static struct B91GpioCntlr g_B91GpioCntlr = {};

static const gpio_pin_e g_GpioIndexToActualPin[] = {
    GPIO_PA0, /* 0  */
    GPIO_PA1, /* 1  */
    GPIO_PA2, /* 2  */
    GPIO_PA3, /* 3  */
    GPIO_PA4, /* 4  */
    GPIO_PA5, /* 5  */
    GPIO_PA6, /* 6  */
    GPIO_PA7, /* 7  */

    GPIO_PB0, /* 8  */
    GPIO_PB1, /* 9  */
    GPIO_PB2, /* 10 */
    GPIO_PB3, /* 11 */
    GPIO_PB4, /* 12 */
    GPIO_PB5, /* 13 */
    GPIO_PB6, /* 14 */
    GPIO_PB7, /* 15 */

    GPIO_PC0, /* 16 */
    GPIO_PC1, /* 17 */
    GPIO_PC2, /* 18 */
    GPIO_PC3, /* 19 */
    GPIO_PC4, /* 20 */
    GPIO_PC5, /* 21 */
    GPIO_PC6, /* 22 */
    GPIO_PC7, /* 23 */

    GPIO_PD0, /* 24 */
    GPIO_PD1, /* 25 */
    GPIO_PD2, /* 26 */
    GPIO_PD3, /* 27 */
    GPIO_PD4, /* 28 */
    GPIO_PD5, /* 29 */
    GPIO_PD6, /* 30 */
    GPIO_PD7, /* 31 */

    GPIO_PE0, /* 32 */
    GPIO_PE1, /* 33 */
    GPIO_PE2, /* 34 */
    GPIO_PE3, /* 35 */
    GPIO_PE4, /* 36 */
    GPIO_PE5, /* 37 */
    GPIO_PE6, /* 38 */
    GPIO_PE7, /* 39 */

    GPIO_PF0, /* 40 */
    GPIO_PF1, /* 41 */
    GPIO_PF2, /* 42 */
    GPIO_PF3, /* 43 */
};

#define RETURN_ERR_IF_OUT_OF_RANGE(gpio)                                                                              \
    do {                                                                                                              \
        if (gpio >= pB91GpioCntlr->pinNum) {                                                                          \
            return HDF_ERR_INVALID_PARAM;                                                                             \
        }                                                                                                             \
        if (pB91GpioCntlr->pinReflectionMap[gpio] >= GPIO_INDEX_MAX) {                                                \
            return HDF_ERR_INVALID_PARAM;                                                                             \
        }                                                                                                             \
    } while (0)

static int32_t GpioDriverBind(struct HdfDeviceObject *device);
static int32_t GpioDriverInit(struct HdfDeviceObject *device);
static void GpioDriverRelease(struct HdfDeviceObject *device);

struct HdfDriverEntry g_GpioDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "TELINK_HDF_PLATFORM_GPIO",
    .Bind = GpioDriverBind,
    .Init = GpioDriverInit,
    .Release = GpioDriverRelease,
};

HDF_INIT(g_GpioDriverEntry);

static int32_t GpioDevWrite(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t val);
static int32_t GpioDevRead(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t *val);
static int32_t GpioDevSetDir(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t dir);
static int32_t GpioDevGetDir(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t *dir);
static int32_t GpioDevSetIrq(struct GpioCntlr *cntlr, uint16_t local, uint16_t mode);
static int32_t GpioDevEnableIrq(struct GpioCntlr *cntlr, uint16_t local);
static int32_t GpioDevDisableIrq(struct GpioCntlr *cntlr, uint16_t local);
static int32_t GpioDevUnsetIrq(struct GpioCntlr *cntlr, uint16_t local);

/* GpioMethod Definitions */
struct GpioMethod g_GpioCntlrMethod = {
    .request = NULL,
    .release = NULL,
    .write = GpioDevWrite,
    .read = GpioDevRead,
    .setDir = GpioDevSetDir,
    .getDir = GpioDevGetDir,
    .toIrq = NULL,
    .setIrq = GpioDevSetIrq,
    .unsetIrq = GpioDevUnsetIrq,
    .enableIrq = GpioDevEnableIrq,
    .disableIrq = GpioDevDisableIrq,
};

_attribute_ram_code_ static void GpioIrqHandler(void)
{
    struct B91GpioCntlr *pB91GpioCntlr = &g_B91GpioCntlr;

    for (size_t i = 0; i < pB91GpioCntlr->pinNum; ++i) {
        if (pB91GpioCntlr->config[i].irq_enabled) {
            GpioCntlrIrqCallback(&pB91GpioCntlr->cntlr, i);
        }
    }

    gpio_clr_irq_status(FLD_GPIO_IRQ_CLR);
}

static int32_t GetGpioDeviceResource(struct B91GpioCntlr *cntlr, const struct DeviceResourceNode *resourceNode)
{
    uint32_t pinIndex;
    struct DeviceResourceIface *dri = NULL;

    dri = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (dri == NULL) {
        HDF_LOGE("DeviceResourceIface is invalid!");
        return HDF_ERR_INVALID_OBJECT;
    }

    if (dri->GetUint8(resourceNode, "pinNum", &cntlr->pinNum, 0) != HDF_SUCCESS) {
        HDF_LOGE("Failed to read pinNum!");
        return HDF_FAILURE;
    }

    cntlr->pinReflectionMap = (uint8_t *)OsalMemAlloc(sizeof(uint8_t) * cntlr->pinNum);
    if (cntlr->pinReflectionMap == NULL) {
        HDF_LOGE("%s: OsalMemAlloc error", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    cntlr->config = OsalMemAlloc(sizeof(cntlr->config[0]) * cntlr->pinNum);
    if (cntlr->config == NULL) {
        HDF_LOGE("%s: OsalMemAlloc error", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    for (uint32_t i = 0; i < cntlr->pinNum; i++) {
        if (dri->GetUint32ArrayElem(resourceNode, "pinMap", i, &pinIndex, 0) != HDF_SUCCESS) {
            HDF_LOGE("Failed to read pinMap!");
            return HDF_FAILURE;
        }

        cntlr->pinReflectionMap[i] = pinIndex;
    }

    return HDF_SUCCESS;
}

static int32_t GpioDriverInit(struct HdfDeviceObject *device)
{
    int32_t ret;
    struct B91GpioCntlr *pB91GpioCntlr = &g_B91GpioCntlr;

    HDF_LOGD("%s: Enter", __func__);

    if (device == NULL || device->property == NULL) {
        HDF_LOGE("%s: device or property NULL!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    ret = GetGpioDeviceResource(pB91GpioCntlr, device->property);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: GetGpioDeviceResource failed: %ld", __func__, ret);
        return ret;
    }

    pB91GpioCntlr->cntlr.count = pB91GpioCntlr->pinNum;
    pB91GpioCntlr->cntlr.priv = (void *)device->property;
    pB91GpioCntlr->cntlr.ops = &g_GpioCntlrMethod;

    struct GpioCntlr *gpioCntlr = &pB91GpioCntlr->cntlr;
    (void)PlatformDeviceBind(&gpioCntlr->device, device);

    ret = GpioCntlrAdd(&pB91GpioCntlr->cntlr);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: GpioCntlrAdd failed: %ld", __func__, ret);
        return ret;
    }

    B91IrqRegister(IRQ25_GPIO, (HWI_PROC_FUNC)GpioIrqHandler, 0);
    plic_interrupt_enable(IRQ25_GPIO);

    HDF_LOGD("%s: dev service:%s init success!", __func__, HdfDeviceGetServiceName(device));
    return ret;
}

static void GpioDriverRelease(struct HdfDeviceObject *device)
{
    struct GpioCntlr *gpioCntlr = NULL;
    struct B91GpioCntlr *pB91GpioCntlr = &g_B91GpioCntlr;

    HDF_LOGD("%s: Enter", __func__);

    if (device == NULL) {
        HDF_LOGE("%s: device is null!", __func__);
        return;
    }

    if (pB91GpioCntlr->pinReflectionMap) {
        OsalMemFree(pB91GpioCntlr->pinReflectionMap);
        pB91GpioCntlr->pinReflectionMap = NULL;
    }

    gpioCntlr = GpioCntlrFromHdfDev(device);
    if (gpioCntlr == NULL) {
        HDF_LOGE("%s: no service bound!", __func__);
        return;
    }
    GpioCntlrRemove(gpioCntlr);
}

static int32_t GpioDriverBind(struct HdfDeviceObject *device)
{
    (void)device;

    return HDF_SUCCESS;
}

static int32_t GpioDevWrite(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t val)
{
    (void)cntlr;
    struct B91GpioCntlr *pB91GpioCntlr = &g_B91GpioCntlr;

    RETURN_ERR_IF_OUT_OF_RANGE(gpio);

    gpio_pin_e gpioPin = g_GpioIndexToActualPin[pB91GpioCntlr->pinReflectionMap[gpio]];
    HDF_LOGD("%s: %d - %d", __func__, gpioPin, val);

    if (val == GPIO_VAL_HIGH) {
        gpio_set_level(gpioPin, 1);
    } else if (val == GPIO_VAL_LOW) {
        gpio_set_level(gpioPin, 0);
    } else {
        return HDF_ERR_NOT_SUPPORT;
    }

    return HDF_SUCCESS;
}

static int32_t GpioDevRead(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t *val)
{
    (void)cntlr;
    struct B91GpioCntlr *pB91GpioCntlr = &g_B91GpioCntlr;

    RETURN_ERR_IF_OUT_OF_RANGE(gpio);

    gpio_pin_e gpioPin = g_GpioIndexToActualPin[pB91GpioCntlr->pinReflectionMap[gpio]];
    HDF_LOGD("%s: %d", __func__, gpioPin);

    if (gpio_get_level(gpioPin) == 1) {
        *val = GPIO_VAL_HIGH;
    } else {
        *val = GPIO_VAL_LOW;
    }

    return HDF_SUCCESS;
}

static int32_t GpioDevSetDir(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t dir)
{
    (void)cntlr;
    struct B91GpioCntlr *pB91GpioCntlr = &g_B91GpioCntlr;

    RETURN_ERR_IF_OUT_OF_RANGE(gpio);

    gpio_pin_e gpioPin = g_GpioIndexToActualPin[pB91GpioCntlr->pinReflectionMap[gpio]];
    HDF_LOGD("%s: %d - %d", __func__, gpioPin, dir);

    if (dir == GPIO_DIR_OUT) {
        gpio_function_en(gpioPin);
        gpio_input_dis(gpioPin);
        gpio_output_en(gpioPin);
    } else if (dir == GPIO_DIR_IN) {
        gpio_function_en(gpioPin);
        gpio_input_en(gpioPin);
        gpio_output_dis(gpioPin);
    } else {
        return HDF_ERR_NOT_SUPPORT;
    }

    return HDF_SUCCESS;
}

static int32_t GpioDevGetDir(struct GpioCntlr *cntlr, uint16_t gpio, uint16_t *dir)
{
    (void)cntlr;
    struct B91GpioCntlr *pB91GpioCntlr = &g_B91GpioCntlr;

    RETURN_ERR_IF_OUT_OF_RANGE(gpio);

    gpio_pin_e gpioPin = g_GpioIndexToActualPin[pB91GpioCntlr->pinReflectionMap[gpio]];
    HDF_LOGD("%s: %d", __func__, gpio);

    if (gpio_is_input_en(gpioPin)) {
        *dir = GPIO_DIR_IN;
    } else if (gpio_is_output_en(gpioPin)) {
        *dir = GPIO_DIR_OUT;
    } else {
        *dir = GPIO_DIR_ERR;
    }

    return HDF_SUCCESS;
}

static int32_t GpioDevSetIrq(struct GpioCntlr *cntlr, uint16_t local, uint16_t mode)
{
    (void)cntlr;
    struct B91GpioCntlr *pB91GpioCntlr = &g_B91GpioCntlr;

    RETURN_ERR_IF_OUT_OF_RANGE(local);

    gpio_pin_e gpioPin = g_GpioIndexToActualPin[pB91GpioCntlr->pinReflectionMap[local]];
    HDF_LOGD("%s: %d", __func__, local);

    switch (mode & 0x0F) {
        case GPIO_IRQ_TRIGGER_HIGH: {
            gpio_set_irq(gpioPin, INTR_HIGH_LEVEL);
            break;
        }
        case GPIO_IRQ_TRIGGER_LOW: {
            gpio_set_irq(gpioPin, INTR_LOW_LEVEL);
            break;
        }
        case GPIO_IRQ_TRIGGER_RISING: {
            gpio_set_irq(gpioPin, INTR_RISING_EDGE);
            break;
        }
        case GPIO_IRQ_TRIGGER_FALLING: {
            gpio_set_irq(gpioPin, INTR_FALLING_EDGE);
            break;
        }
        default: {
            return HDF_ERR_BSP_PLT_API_ERR;
        }
    }

    return HDF_SUCCESS;
}

static int32_t GpioDevUnsetIrq(struct GpioCntlr *cntlr, uint16_t local)
{
    (void)cntlr;
    struct B91GpioCntlr *pB91GpioCntlr = &g_B91GpioCntlr;

    RETURN_ERR_IF_OUT_OF_RANGE(local);

    return HDF_SUCCESS;
}

static int32_t GpioDevEnableIrq(struct GpioCntlr *cntlr, uint16_t local)
{
    (void)cntlr;
    struct B91GpioCntlr *pB91GpioCntlr = &g_B91GpioCntlr;

    RETURN_ERR_IF_OUT_OF_RANGE(local);

    gpio_pin_e gpioPin = g_GpioIndexToActualPin[pB91GpioCntlr->pinReflectionMap[local]];
    HDF_LOGD("%s: %d", __func__, local);

    gpio_irq_en(gpioPin);

    pB91GpioCntlr->config[local].irq_enabled = true;

    return HDF_SUCCESS;
}

static int32_t GpioDevDisableIrq(struct GpioCntlr *cntlr, uint16_t local)
{
    (void)cntlr;
    struct B91GpioCntlr *pB91GpioCntlr = &g_B91GpioCntlr;

    RETURN_ERR_IF_OUT_OF_RANGE(local);

    gpio_pin_e gpioPin = g_GpioIndexToActualPin[pB91GpioCntlr->pinReflectionMap[local]];
    HDF_LOGD("%s: %d", __func__, local);

    gpio_irq_dis(gpioPin);

    pB91GpioCntlr->config[local].irq_enabled = false;

    return HDF_SUCCESS;
}
