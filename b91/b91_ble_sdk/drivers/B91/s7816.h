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
#ifndef S7816_H_
#define S7816_H_

#include "gpio.h"
#include "stimer.h"
#include "uart.h"

#define s7816_en(uart_num)               uart_rtx_en(uart_num)
#define s7816_set_rtx_pin(s7816_trx_pin) uart_set_rtx_pin(s7816_trx_pin)

/**
 *  @brief  Define 7816 TRX pin.
 */
typedef enum {
    S7816_UART0_RTX_A4 = GPIO_PA4,
    S7816_UART0_RTX_B3 = GPIO_PB3,
    S7816_UART0_RTX_D3 = GPIO_PD3,

    S7816_UART1_RTX_C7 = GPIO_PC7,
    S7816_UART1_RTX_D7 = GPIO_PD7,
    S7816_UART1_RTX_E2 = GPIO_PE2,
} s7816_rtx_pin_e;

/**
 *  @brief  Define 7816 clock.
 */
typedef enum {
    S7816_4MHZ = 4,
    S7816_6MHZ = 6,
    S7816_12MHZ = 12,
} s7816_clock_e;

/**
 *  @brief  Define 7816 clk pin.
 */
typedef enum {
    S7817_CLK_PA0 = GPIO_PA0,
} s7816_clk_pin_e;

/**
 * @brief      	This function is used to set the s7816 clock.
 * @param[in]  	div	- set the divider of clock of 7816 module.
 * @return     	none.
 * @note        system clk is 24MHZ
 * 				7816clk:    0x60-4Mhz     0x40-6Mhz   0x20-12Mhz
 * 				baudrate:   0x60-10752    0x40-16194  0x20-32388
 * 				the clk-pin is PA0 by default.
 */
extern void s7816_set_clk(unsigned char div);

/**
 * @brief      	This function is used to initialize the s7816 module.
 * @param[in]  	uart_num     - UART0 or UART1.
 * @param[in]  	clock        - the clock of s7816.
 * @param[in]  	f            - the clock frequency conversion factor of s7816.
 * @param[in]  	d            - the bitrate regulator of s7816.
 * @return     	none.
 */
extern void s7816_init(uart_num_e uart_num, s7816_clock_e clock, int f, int d);

/**
 * @brief      	This function is used to active the IC card,set the trx pin and coldreset.
 * @param[in]  	none.
 * @return     	none.
 */
extern void s7816_coldreset(void);

/**
 * @brief      	This function is used to set all the pin of s7816 module.
 * @param[in]  	rst_pin     - the rst pin of s7816.
 * @param[in]  	vcc_pin     - the vcc pin of s7816.
 * @param[in]  	clk_pin     - the clk pin of s7816.
 * @param[in]  	trx_pin     - the trx pin of s7816.
 * @return     	none.
 */
extern void s7816_set_pin(gpio_pin_e rst_pin, gpio_pin_e vcc_pin, s7816_clk_pin_e clk_pin, s7816_rtx_pin_e trx_pin);

/**
 * @brief      	This function is used to release the trigger
 * @param[in]  	none.
 * @return     	none.
 */
extern void s7816_release_trig(void);

/**
 * @brief      	This function is used to set the RST pin of s7816.
 * @param[in]  	pin_7816_rst - the RST pin of s7816.
 * @return     	none.
 */
extern void s7816_set_rst_pin(gpio_pin_e pin_7816_rst);

/**
 * @brief      	This function is used to set the VCC pin of s7816.
 * @param[in]   pin_7816_vcc - the VCC pin of s7816.
 * @return     	none.
 */
extern void s7816_set_vcc_pin(gpio_pin_e pin_7816_vcc);

/**
 * @brief      	This function is used to warmreset.
 * @param[in]  	none.
 * @return     	none.
 * @note        the warmreset is required after the IC-CARD active.
 */
extern void s7816_warmreset(void);

/**
 * @brief      	This function is used to set the rst-wait time of the s7816 module.
 * @param[in]  	rst_time_us - set the s7816_rst_time.
 * @param[in]  	atr_time_us - set the s7816_atr_time.
 * @return     	none.
 */
extern void s7816_set_time(int rst_time_us, int atr_time_us);

/**
 * @brief      	This function is used to warmreset.
 * @param[in]  	uart_num - UART0 or UART1.
 * @param[in]   tx_data  - the data need to send.
 * return       none.
 */
extern void s7816_send_byte(uart_num_e uart_num, unsigned char tx_data);

#endif /* S7816_H_ */
