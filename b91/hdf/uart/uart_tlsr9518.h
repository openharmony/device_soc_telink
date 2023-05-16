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

#ifndef UART_TLSR9518_H
#define UART_TLSR9518_H

#include "uart_if.h"


struct _uart_driver_data;
typedef int32_t (*RecvNotify)(struct _uart_driver_data *driver_data, const char *buf, size_t count);


typedef enum
{
    UART_FLG_DMA_RX = 1 << 0,
    UART_FLG_DMA_TX = 1 << 1,
    UART_FLG_RD_BLK = 1 << 2,

} uart_flg_t;


struct _uart_ops;
struct _uart_port;
typedef struct _uart_driver_data
{
    uint32_t baudrate;
    struct UartAttribute uattr;
    struct _uart_ops *ops;
    int32_t count;
    RecvNotify recv;
    uart_flg_t flags;
    struct _uart_port *port;

} uart_driver_data_t;


typedef struct _base_addr
{
    unsigned long phys_base;
    unsigned long size;

} base_addr_t;


typedef struct _uart_port
{
    uint32_t num;
    uint32_t enable;
    uint32_t interrupt;
    uart_driver_data_t *driver_data;
    base_addr_t addr;

} uart_port_t;


typedef enum
{
    UART_DMA_DIR_RX,
    UART_DMA_DIR_TX,

} dma_dir_t;


typedef struct _uart_ops
{
    int32_t (*StartUp)(uart_driver_data_t *driver_data);
    int32_t (*ShutDown)(uart_driver_data_t *driver_data);
    int32_t (*DmaStartUp)(uart_driver_data_t *driver_data, dma_dir_t dir);
    int32_t (*DmaShutDown)(uart_driver_data_t *driver_data, dma_dir_t dir);
    int32_t (*StartTx)(uart_driver_data_t *driver_data, const char *buf, size_t count);
    int32_t (*Config)(uart_driver_data_t *driver_data);

} uart_ops_t;


uart_ops_t *get_uart_ops(void);
int32_t receive_notify(uart_driver_data_t *driver_data, const char *buf, size_t count); // Refine name

#endif // UART_TLSR9518_H
