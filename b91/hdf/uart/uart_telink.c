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

#include <string.h>
#include <B91/dma.h>
#include "hdf_device_desc.h"
#include "device_resource_if.h"
#include "hdf_log.h"
#include "uart/uart_core.h"
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

    port->driver_data = driver_data;
    driver_data->port = port;

    int32_t ret = get_config_from_hcs(port, device->property);
    if (ret != HDF_SUCCESS) {
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

    host->method = &g_uartHostMethod;

    return HDF_SUCCESS;
}


static void uart_device_release(struct HdfDeviceObject *device)
{
    if (device == NULL) {
        HDF_LOGE("%s: device is null", __func__);
        return;
    }

    struct UartHost *host = UartHostFromDevice(device);

    if (host == NULL) {
        HDF_LOGE("%s: host is null", __func__);
        return;
    }

    if (host->priv != NULL) {
        uart_driver_data_t *driver_data = (uart_driver_data_t *)host->priv;
        OsalMemFree(driver_data->port);
        OsalMemFree(driver_data);
        host->priv = NULL;
    }
}


struct HdfDriverEntry g_uartDevice = {
    .moduleVersion = 1,
    .moduleName = "TELINK_HDF_PLATFORM_UART",
    .Bind = uart_device_bind,
    .Init = uart_device_init,
    .Release = uart_device_release,
};

HDF_INIT(g_uartDevice);


static uart_tx_pin_e tx_pin_from_str(const char *str)
{
    if (!strcmp(str, "UART0_TX_PA3")) {
        return UART0_TX_PA3;
    } else if (!strcmp(str, "UART0_TX_PB2")) {
        return UART0_TX_PB2;
    } else if (!strcmp(str, "UART0_TX_PD2")) {
        return UART0_TX_PD2;
    } else if (!strcmp(str, "UART1_TX_PC6")) {
        return UART1_TX_PC6;
    } else if (!strcmp(str, "UART1_TX_PD6")) {
        return UART1_TX_PD6;
    } else if (!strcmp(str, "UART1_TX_PE0")) {
        return UART1_TX_PE0;
    } else {
        return UART_TX_ENUM_SIZE;
    }
}


static uart_rx_pin_e rx_pin_from_str(const char *str)
{
    if (!strcmp(str, "UART0_RX_PA4")) {
        return UART0_RX_PA4;
    } else if (!strcmp(str, "UART0_RX_PB3")) {
        return UART0_RX_PB3;
    } else if (!strcmp(str, "UART0_RX_PD3")) {
        return UART0_RX_PD3;
    } else if (!strcmp(str, "UART1_RX_PC7")) {
        return UART1_RX_PC7;
    } else if (!strcmp(str, "UART1_RX_PD7")) {
        return UART1_RX_PD7;
    } else if (!strcmp(str, "UART1_RX_PE2")) {
        return UART1_RX_PE2;
    } else {
        return UART_RX_ENUM_SIZE;
    }
}


static uint8_t stop_bit_from_str(const char *str)
{
    if (!strcmp(str, "UART_STOP_BIT_ONE")) {
        return UART_ATTR_STOPBIT_1;
    } else if (!strcmp(str, "UART_STOP_BIT_ONE_DOT_FIVE")) {
        return UART_ATTR_STOPBIT_1P5;
    } else if (!strcmp(str, "UART_STOP_BIT_TWO")) {
        return UART_ATTR_STOPBIT_2;
    } else {
        HDF_LOGW("%s: Uart stop bit config is illegal. Assumed there's 1 stop bit", __func__);
        return UART_ATTR_STOPBIT_1;
    }
}


static uint8_t parity_from_str(const char *str)
{
    if (!strcmp(str, "UART_PARITY_NONE")) {
        return UART_ATTR_PARITY_NONE;
    } else if (!strcmp(str, "UART_PARITY_EVEN")) {
        return UART_ATTR_PARITY_EVEN;
    } else if (!strcmp(str, "UART_PARITY_ODD")) {
        return UART_ATTR_PARITY_ODD;
    } else {
        HDF_LOGW("%s: Uart parity config is illegal. Assumed there's no parity bit", __func__);
        return UART_ATTR_PARITY_NONE;
    }
}


static int32_t get_port_config(const struct DeviceResourceNode *node, const char *str, uint32_t *dst)
{
    struct DeviceResourceIface *iface = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);

    if (iface->GetUint32(node, str, dst, HDF_FAILURE) != HDF_SUCCESS) {
        HDF_LOGE("%s: Failed to get port config", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}


static int32_t get_config_from_hcs(uart_port_t *port, const struct DeviceResourceNode *node)
{
    struct DeviceResourceIface *iface = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);

    if (iface == NULL || iface->GetUint32 == NULL || iface->GetString == NULL) {
        HDF_LOGE("%s: Interface is invalid", __func__);
        return HDF_FAILURE;
    }

    int32_t ret = get_port_config(node, "port", &port->num);
    ret |= get_port_config(node, "regPbase", &port->addr);
    ret |= get_port_config(node, "irqNum", &port->interrupt);

    uart_driver_data_t *driver_data = port->driver_data;

    if (iface->GetUint32(node, "baudrate", &driver_data->baudrate, HDF_FAILURE) != HDF_SUCCESS) {
        HDF_LOGE("%s: Failed to read baudrate", __func__);
        return HDF_FAILURE;
    }

    char *tmp;
    if (iface->GetString(node, "tx_pin", &tmp, "HDF_FAILURE") != HDF_SUCCESS) {
        HDF_LOGE("%s: Failed to read tx pin", __func__);
        return HDF_FAILURE;
    }
    driver_data->tx = tx_pin_from_str(tmp);
    if (driver_data->tx >= UART_TX_ENUM_SIZE) {
        HDF_LOGE("%s: Tx pin config is illegal", __func__);
        return HDF_FAILURE;
    }

    if (iface->GetString(node, "rx_pin", &tmp, "HDF_FAILURE") != HDF_SUCCESS) {
        HDF_LOGE("%s: Failed to read rx pin", __func__);
        return HDF_FAILURE;
    }
    driver_data->rx = rx_pin_from_str(tmp);
    if (driver_data->rx >= UART_RX_ENUM_SIZE) {
        HDF_LOGE("%s: Rx pin config is illegal", __func__);
        return HDF_FAILURE;
    }

    if (iface->GetString(node, "stop_bits", &tmp, "HDF_FAILURE") != HDF_SUCCESS) {
        HDF_LOGE("%s: Failed to read stop bits", __func__);
        return HDF_FAILURE;
    }
    driver_data->uattr.stopBits = stop_bit_from_str(tmp);

    if (iface->GetString(node, "parity", &tmp, "HDF_FAILURE") != HDF_SUCCESS) {
        HDF_LOGE("%s: Failed to read parity", __func__);
        return HDF_FAILURE;
    }
    driver_data->uattr.parity = parity_from_str(tmp);

    return (ret == HDF_SUCCESS) ? HDF_SUCCESS : HDF_FAILURE;
}


// Uart Host methods -----------------------------------------------------------

static int32_t UartHostDevInit(struct UartHost *host)
{
    if (host == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    uart_driver_data_t *driver_data = (uart_driver_data_t *)host->priv;
    if (driver_data == NULL) {
        HDF_LOGE("%s: Failed to get driver data from uart host", __func__);
        return HDF_FAILURE;
    }

    uart_dma_init(driver_data);
    driver_data->port->enable = 1;

    return HDF_SUCCESS;
}


static int32_t UartHostDevDeinit(struct UartHost *host)
{
    if (host == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    uart_driver_data_t *driver_data = (uart_driver_data_t *)host->priv;
    if (driver_data == NULL) {
        HDF_LOGE("%s: Failed to get driver data from uart host", __func__);
        return HDF_FAILURE;
    }

    driver_data->port->enable = 0;

    return HDF_SUCCESS;
}


static int32_t UartHostDevRead(struct UartHost *host, uint8_t *data, uint32_t size)
{
    HDF_LOGD("%s: not implemented yet", __func__);
    return HDF_FAILURE;
}


static int32_t UartHostDevWrite(struct UartHost *host, uint8_t *data, uint32_t size)
{
    uint8_t ret = uart_send_dma(host->num, data, size);
    return (ret == 1) ? HDF_SUCCESS : HDF_FAILURE;
}


static int32_t UartHostDevSetBaud(struct UartHost *host, uint32_t baudRate)
{
    if (host == NULL) {
        HDF_LOGE("%s: UartHost is NULL!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    uart_driver_data_t *driver_data = (uart_driver_data_t *)host->priv;
    driver_data->baudrate = baudRate;

    uart_dma_init(driver_data);

    return HDF_SUCCESS;
}


static int32_t UartHostDevGetBaud(struct UartHost *host, uint32_t *baudRate)
{
    if (host == NULL || baudRate == NULL) {
        HDF_LOGE("%s: UartHost is NULL!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    uart_driver_data_t *driver_data = (uart_driver_data_t *)host->priv;
    *baudRate = driver_data->baudrate;

    return HDF_SUCCESS;
}


static int32_t UartHostDevSetAttribute(struct UartHost *host, struct UartAttribute *attribute)
{
    if (host == NULL || attribute == NULL) {
        HDF_LOGE("%s: UartHost is NULL!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    uart_driver_data_t *driver_data = (uart_driver_data_t *)host->priv;
    driver_data->uattr = *attribute;

    uart_dma_init(driver_data);

    return HDF_SUCCESS;
}


static int32_t UartHostDevGetAttribute(struct UartHost *host, struct UartAttribute *attribute)
{
    if (host == NULL || attribute == NULL) {
        HDF_LOGE("%s: UartHost is NULL!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    uart_driver_data_t *driver_data = (uart_driver_data_t *)host->priv;
    *attribute = driver_data->uattr;

    return HDF_SUCCESS;
}


static int32_t UartHostDevSetTransMode(struct UartHost *host, enum UartTransMode mode)
{
    HDF_LOGD("%s: not implemented yet", __func__);
    return HDF_FAILURE;
}
