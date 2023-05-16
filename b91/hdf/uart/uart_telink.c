/******************************************************************************
 * Copyright (c) 2023 Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#include "hdf_device_desc.h"
#include "device_resource_if.h"
#include "hdf_log.h"
#include "uart/uart_core.h"
#include "uart_dev.h"
#include "hiview_log.h"
#include "osal_mem.h"
#include "uart_tlsr9518.h"
#include "hdf_log_adapter_debug.h" // workaround for log print

#define HDF_LOG_TAG uart_telink

static int32_t get_config_from_hcs(uart_port_t *port, const struct DeviceResourceNode *node);

static int32_t UartHostDevInit(struct UartHost *host);
static int32_t UartHostDevDeinit(struct UartHost *host);
static int32_t UartHostDevRead(struct UartHost *host, uint8_t *data, uint32_t size);
static int32_t UartHostDevWrite(struct UartHost *host, uint8_t *data, uint32_t size);
static int32_t UartHostDevSetBaud(struct UartHost *host, uint32_t baudRate);
static int32_t UartHostDevGetBaud(struct UartHost *host, uint32_t *baudRate);
static int32_t UartHostDevSetAttribute(struct UartHost *host, struct UartAttribute *attribute);
static int32_t UartHostDevGetAttribute(struct UartHost *host, struct UartAttribute *attribute);
static int32_t UartHostDevSetTransMode(struct UartHost *host, enum UartTransMode mode);

struct UartHostMethod g_uartHostMethod = {
    .Init = UartHostDevInit,
    .Deinit = UartHostDevDeinit,
    .Read = UartHostDevRead,
    .Write = UartHostDevWrite,
    .SetBaud = UartHostDevSetBaud,
    .GetBaud = UartHostDevGetBaud,
    .SetAttribute = UartHostDevSetAttribute,
    .GetAttribute = UartHostDevGetAttribute,
    .SetTransMode = UartHostDevSetTransMode,
};

static int32_t uart_device_attach(struct UartHost *host, struct HdfDeviceObject *device)
{
    if (device->property == NULL) {
        HDF_LOGE("%s: device->property is null", __func__);
        return HDF_FAILURE;
    }

    uart_driver_data_t *driver_data = (uart_driver_data_t *)OsalMemCalloc(sizeof(uart_driver_data_t));

    if (driver_data == NULL) {
        HDF_LOGE("%s: driver_data memory allocation fail", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    uart_port_t *port = (uart_port_t *)OsalMemCalloc(sizeof(uart_port_t));

    if (port == NULL) {
        HDF_LOGE("%s: port memory allocation fail", __func__);
        OsalMemFree(driver_data);
        return HDF_ERR_MALLOC_FAIL;
    }

    driver_data->ops = get_uart_ops();
    driver_data->recv = receive_notify;
    port->driver_data = driver_data;
    driver_data->port = port;

    int32_t ret = get_config_from_hcs(port, device->property);

    if (ret != HDF_SUCCESS || port->addr.phys_base == 0) {
        OsalMemFree(driver_data);
        OsalMemFree(port);
        return HDF_FAILURE;
    }

    host->priv = driver_data;
    host->num = port->num;

    return HDF_SUCCESS;
}


static int32_t uart_device_bind(struct HdfDeviceObject *device)
{
    if (device == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    return (UartHostCreate(device) == NULL) ? HDF_FAILURE : HDF_SUCCESS;
}


static int32_t uart_device_init(struct HdfDeviceObject *device)
{
    if (device == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t ret;
    struct UartHost *host = UartHostFromDevice(device);

    if (host == NULL) {
        HDF_LOGE("%s: host is null", __func__);
        return HDF_FAILURE;
    }

    ret = uart_device_attach(host, device);

    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: attach failed", __func__);
        return HDF_FAILURE;
    }

    // host->method = &g_uartHostMethod;

    return HDF_SUCCESS;
}


static void uart_device_release(struct HdfDeviceObject *device)
{
    // if (device == NULL) {
    //     HDF_LOGE("%s: device is null", __func__);
    //     return;
    // }

    // struct UartHost *host = UartHostFromDevice(device);

    // if (host == NULL) {
    //     HDF_LOGE("%s: host is null", __func__);
    //     return;
    // }

    // if (host->priv != NULL) {
    //     OsalMemFree(host->priv);
    //     host->priv = NULL;
    // }
}


struct HdfDriverEntry g_uartDevice = {
    .moduleVersion = 1,
    .moduleName = "TELINK_HDF_PLATFORM_UART",
    .Bind = uart_device_bind,
    .Init = uart_device_init,
    .Release = uart_device_release,
};

HDF_INIT(g_uartDevice);


static int32_t get_config_from_hcs(uart_port_t *port, const struct DeviceResourceNode *node)
{
    struct DeviceResourceIface *iface = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);

    if (iface == NULL || iface->GetUint32 == NULL) {
        HDF_LOGE("%s: Interface is invalid", __func__);
        return HDF_FAILURE;
    }

    if (iface->GetUint32(node, "num", &port->num, HDF_FAILURE) != HDF_SUCCESS) {
        HDF_LOGE("%s: Failed to read port number", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGD("%s: port->num = %u", __func__, port->num);

    uart_driver_data_t *driver_data = port->driver_data;

    if (iface->GetUint32(node, "baudrate", &driver_data->baudrate, HDF_FAILURE) != HDF_SUCCESS) {
        HDF_LOGE("%s: Failed to read baudrate", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGD("%s: driver_data->baudrate = %u", __func__, driver_data->baudrate);

    if (iface->GetUint32(node, "regPbase", &port->addr.phys_base, HDF_FAILURE) != HDF_SUCCESS) {
        HDF_LOGE("%s: Failed to read register base address", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGD("%s: port->addr.phys_base = 0x%x", __func__, port->addr.phys_base);

    if (iface->GetUint32(node, "iomemCount", &port->addr.size, HDF_FAILURE) != HDF_SUCCESS) {
        HDF_LOGE("%s: Failed to read the size of the physical memory to remap", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGD("%s: port->addr.size = 0x%x", __func__, port->addr.size);

    if (iface->GetUint32(node, "irqNum", &port->interrupt, HDF_FAILURE) != HDF_SUCCESS) {
        HDF_LOGE("%s: Failed to read uart interrupt number", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGD("%s: port->interrupt = %u", __func__, port->interrupt);

    uint32_t tmp;
    if (iface->GetUint32(node, "fifoRxEn", &tmp, HDF_FAILURE) != HDF_SUCCESS) {
        HDF_LOGE("%s: Failed to read fifoRxEn", __func__);
        return HDF_FAILURE;
    }
    driver_data->uattr.fifoRxEn = tmp;
    HDF_LOGD("%s: driver_data->uattr.fifoRxEn = %u", __func__, driver_data->uattr.fifoRxEn);

    if (iface->GetUint32(node, "fifoTxEn", &tmp, HDF_FAILURE) != HDF_SUCCESS) {
        HDF_LOGE("%s: Failed to read fifoTxEn", __func__);
        return HDF_FAILURE;
    }
    driver_data->uattr.fifoTxEn = tmp;
    HDF_LOGD("%s: driver_data->uattr.fifoTxEn = %u", __func__, driver_data->uattr.fifoTxEn);

    if (iface->GetUint32(node, "flags", &driver_data->flags, HDF_FAILURE) != HDF_SUCCESS) {
        HDF_LOGE("%s: Failed to read uart flags", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGD("%s: driver_data->flags = %u", __func__, driver_data->flags);

    return HDF_SUCCESS;
}


// Uart Host methods  ----------------------------------------------------------

static int32_t UartHostDevInit(struct UartHost *host)
{
    /*
    config:
    - port
    - baudrate
    - tx, rx pins
    - data bits
    - parity
    - stop bits
    */
}




