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

#include <B91/clock.h>
#include "hdf_log_adapter_debug.h"
#include "uart_tlsr9518.h"

#define HZ_IN_MHZ (1000 * 1000)
#define MAX_BITS_PER_BYTE 12


uart_parity_e parity_from_uattr(struct UartAttribute uattr)
{
    uart_parity_e parity;

    switch (uattr.parity) {
        case UART_ATTR_PARITY_NONE:
            parity = UART_PARITY_NONE;
            break;

        case UART_ATTR_PARITY_ODD:
            parity = UART_PARITY_ODD;
            break;

        case UART_ATTR_PARITY_EVEN:
            parity = UART_PARITY_EVEN;
            break;

        default:
            parity = UART_PARITY_NONE;
            break;
    }

    return parity;
}


uart_stop_bit_e stopbit_from_uattr(struct UartAttribute uattr)
{
    uart_stop_bit_e stopbit;

    switch (uattr.stopBits) {
        case UART_ATTR_STOPBIT_1:
            stopbit = UART_STOP_BIT_ONE;
            break;

        case UART_ATTR_STOPBIT_1P5:
            stopbit = UART_STOP_BIT_ONE_DOT_FIVE;
            break;

        case UART_ATTR_STOPBIT_2:
            stopbit = UART_STOP_BIT_TWO;
            break;

        default:
            stopbit = UART_STOP_BIT_ONE;
    }

    return stopbit;
}


void uart_dma_init(uart_driver_data_t *driver_data)
{
    uart_set_pin(driver_data->tx, driver_data->rx);

    uart_reset(driver_data->port->num);

    unsigned short div;
    unsigned char bwpc;

    uart_cal_div_and_bwpc(driver_data->baudrate, sys_clk.pclk * HZ_IN_MHZ, &div, &bwpc);
    uart_init(driver_data->port->num, div, bwpc, parity_from_uattr(driver_data->uattr),
                                                stopbit_from_uattr(driver_data->uattr));
    uart_set_dma_rx_timeout(driver_data->port->num, bwpc, MAX_BITS_PER_BYTE, UART_BW_MUL1);
    uart_set_tx_dma_config(driver_data->port->num, DMA2);
    dma_clr_irq_mask(DMA2, TC_MASK|ABT_MASK|ERR_MASK);
    uart_clr_tx_done(driver_data->port->num);
}

