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


#include <B91/uart.h>
#include "uart_if.h"


struct _uart_port;
typedef struct _uart_driver_data {
    uint32_t baudrate;
    struct UartAttribute uattr;
    uart_tx_pin_e tx;
    uart_rx_pin_e rx;
    struct _uart_port *port;
} uart_driver_data_t;


typedef struct _uart_port {
    uint32_t num;
    uint32_t enable;
    uint32_t interrupt;
    uint32_t addr;
    uart_driver_data_t *driver_data;
} uart_port_t;


uart_parity_e parity_from_uattr(struct UartAttribute uattr);
uart_stop_bit_e stopbit_from_uattr(struct UartAttribute uattr);
void uart_dma_init(uart_driver_data_t *driver_data);


#endif // UART_TLSR9518_H
