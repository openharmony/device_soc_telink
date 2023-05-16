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

#include "uart_tlsr9518.h"
#include "hdf_log_adapter_debug.h"

static int32_t uart_tlsr9518_startup(uart_driver_data_t *driver_data);
static int32_t uart_tlsr9518_shutdown(uart_driver_data_t *driver_data);
static int32_t uart_tlsr9518_dma_startup(uart_driver_data_t *driver_data, dma_dir_t dir);
static int32_t uart_tlsr9518_dma_shutdown(uart_driver_data_t *driver_data, dma_dir_t dir);
static int32_t uart_tlsr9518_start_tx(uart_driver_data_t *driver_data, const char *buf, size_t count);
static int32_t uart_tlsr9518_config(uart_driver_data_t *driver_data);

uart_ops_t g_uart_ops = {
    .StartUp = uart_tlsr9518_startup,
    .ShutDown = uart_tlsr9518_shutdown,
    .DmaStartUp = uart_tlsr9518_dma_startup,
    .DmaShutDown = uart_tlsr9518_dma_shutdown,
    .StartTx = uart_tlsr9518_start_tx,
    .Config = uart_tlsr9518_config,
};

uart_ops_t *get_uart_ops(void)
{
    return &g_uart_ops;
}

int32_t receive_notify(uart_driver_data_t *driver_data, const char *buf, size_t count)
{
    HDF_LOGE("%s: not implemented", __func__);
    return HDF_FAILURE;
}


// UART ops --------------------------------------------------------------------

static int32_t uart_tlsr9518_startup(uart_driver_data_t *driver_data)
{
    HDF_LOGE("%s: not implemented", __func__);
    return HDF_FAILURE;
}

static int32_t uart_tlsr9518_shutdown(uart_driver_data_t *driver_data)
{
    HDF_LOGE("%s: not implemented", __func__);
    return HDF_FAILURE;
}

static int32_t uart_tlsr9518_dma_startup(uart_driver_data_t *driver_data, dma_dir_t dir)
{
    HDF_LOGE("%s: not implemented", __func__);
    return HDF_FAILURE;
}

static int32_t uart_tlsr9518_dma_shutdown(uart_driver_data_t *driver_data, dma_dir_t dir)
{
    HDF_LOGE("%s: not implemented", __func__);
    return HDF_FAILURE;
}

static int32_t uart_tlsr9518_start_tx(uart_driver_data_t *driver_data, const char *buf, size_t count)
{
    HDF_LOGE("%s: not implemented", __func__);
    return HDF_FAILURE;
}

static int32_t uart_tlsr9518_config(uart_driver_data_t *driver_data)
{
    HDF_LOGE("%s: not implemented", __func__);
    return HDF_FAILURE;
}



