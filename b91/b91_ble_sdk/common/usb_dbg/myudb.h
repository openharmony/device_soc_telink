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
#ifndef __MYUDB_H__
#define __MYUDB_H__
#pragma once

#include "common/config/user_config.h"
#include "log_def_stack.h"

/* Trace global enable macro */
#ifndef VCD_EN
#define VCD_EN 0
#endif
/* USB print log enable macro */
#ifndef DUMP_STR_EN
#define DUMP_STR_EN 0
#endif

#define SL_STACK_EN 0

#if (DUMP_STR_EN)
#define my_usb_init(id, p_print) myudb_usb_init(id, p_print)
#define usb_send_str(s)          usb_send_str_data(s, 0, 0)
#define usb_send_data(p, n)      usb_send_str_data(0, p, n)
#define my_dump_str_data(en, s, p, n)                                                                                 \
    if (en) {                                                                                                         \
        usb_send_str_data(s, (u8 *)(p), n);                                                                           \
    }
#define my_dump_str_u32s(en, s, d0, d1, d2, d3)                                                                       \
    if (en) {                                                                                                         \
        usb_send_str_u32s(s, (u32)(d0), (u32)(d1), (u32)(d2), (u32)(d3));                                             \
    }
#define my_uart_send_str_data  usb_send_str_data
#define my_uart_send_str_int   usb_send_str_int
#define myudb_usb_handle_irq() udb_usb_handle_irq()
#else
#define my_usb_init(id, p_print)
#define usb_send_str(s)
#define usb_send_data(p, n)
#define my_dump_str_data(en, s, p, n)
#define my_dump_str_u32s(en, s, d0, d1, d2, d3)
#define my_uart_send_str_data
#define my_uart_send_str_int
#define myudb_usb_handle_irq()
#endif

typedef int (*func_myudb_hci_cmd_cb_t)(unsigned char *, int);

void myudb_register_hci_cb(void *p);
void myudb_register_hci_debug_cb(void *p);

void myudb_usb_init(u16 id, void *p_print);

void myudb_usb_bulkout_ready(void);

void udb_usb_handle_irq(void);

void usb_send_status_pkt(u8 status, u8 buffer_num, u8 *pkt, u16 len);

void myudb_capture_enable(int en);

void usb_send_str_data(char *str, u8 *ph, int n);

void usb_send_str_u32s(char *str, u32 d0, u32 d1, u32 d2, u32 d3);

void usb_send_upper_tester_result(u8 err);

#define my_irq_disable()    u32 rie = core_interrupt_disable()
#define my_irq_restore()    core_restore_interrupt(rie)
#define LOG_EVENT_TIMESTAMP 0
#define LOG_DATA_B1_0       0
#define LOG_DATA_B1_1       1

#define get_systemtick() stimer_get_tick()

#define log_uart(d) reg_usb_ep8_dat = d

#define DEBUG_PORT       GPIO_PB2
#define log_ref_gpio_h() gpio_set_high_level(DEBUG_PORT)
#define log_ref_gpio_l() gpio_set_low_level(DEBUG_PORT)

#define log_hw_ref()                                                                                                  \
    if (VCD_EN) {                                                                                                     \
        my_irq_disable();                                                                                             \
        log_ref_gpio_h();                                                                                             \
        log_uart(0x20);                                                                                               \
        int t = get_systemtick();                                                                                     \
        log_uart(t);                                                                                                  \
        log_uart(t >> 8);                                                                                             \
        log_uart(t >> 16);                                                                                            \
        log_ref_gpio_l();                                                                                             \
        my_irq_restore();                                                                                             \
    }

// 4-byte sync word: 00 00 00 00
#define log_sync(en)                                                                                                  \
    if (VCD_EN && (en)) {                                                                                               \
        my_irq_disable();                                                                                             \
        log_uart(0);                                                                                                  \
        log_uart(0);                                                                                                  \
        log_uart(0);                                                                                                  \
        log_uart(0);                                                                                                  \
        my_irq_restore();                                                                                             \
    }
// 4-byte (001_id-5bits) id0: timestamp align with hardware gpio output; id1-31: user define
#define log_tick(en, id)                                                                                              \
    if (VCD_EN && (en)) {                                                                                               \
        my_irq_disable();                                                                                             \
        log_uart(0x20 | ((id) & 31));                                                                                   \
        int t = get_systemtick();                                                                                     \
        log_uart(t);                                                                                                  \
        log_uart(t >> 8);                                                                                             \
        log_uart(t >> 16);                                                                                            \
        my_irq_restore();                                                                                             \
    }

// 1-byte (000_id-5bits)
#define log_event(en, id)                                                                                             \
    if (VCD_EN && (en)) {                                                                                               \
        my_irq_disable();                                                                                             \
        log_uart(0x00 | ((id) & 31));                                                                                   \
        my_irq_restore();                                                                                             \
    }

// 1-byte (01x_id-5bits) 1-bit data: id0 & id1 reserved for hardware
#define log_task(en, id, b)                                                                                           \
    if (VCD_EN && (en)) {                                                                                               \
        my_irq_disable();                                                                                             \
        log_uart(((b) ? 0x60 : 0x40) | ((id) & 31));                                                                    \
        int t = get_systemtick();                                                                                     \
        log_uart(t);                                                                                                  \
        log_uart(t >> 8);                                                                                             \
        log_uart(t >> 16);                                                                                            \
        my_irq_restore();                                                                                             \
    }

// 2-byte (10-id-6bits) 8-bit data
#define log_b8(en, id, d)                                                                                             \
    if (VCD_EN && (en)) {                                                                                               \
        my_irq_disable();                                                                                             \
        log_uart(0x80 | ((id) & 63));                                                                                   \
        log_uart(d);                                                                                                  \
        my_irq_restore();                                                                                             \
    }

// 3-byte (11-id-6bits) 16-bit data
#define log_b16(en, id, d)                                                                                            \
    if (VCD_EN && (en)) {                                                                                               \
        my_irq_disable();                                                                                             \
        log_uart(0xc0 | ((id) & 63));                                                                                   \
        log_uart(d);                                                                                                  \
        log_uart((d) >> 8);                                                                                           \
        my_irq_restore();                                                                                             \
    }

#define log_tick_irq(en, id)                                                                                          \
    if (VCD_EN && (en)) {                                                                                               \
        log_uart(0x20 | (id));                                                                                        \
        int t = get_systemtick();                                                                                     \
        log_uart(t);                                                                                                  \
        log_uart(t >> 8);                                                                                             \
        log_uart(t >> 16);                                                                                            \
    }
#define log_tick_irq_2(en, id, t)                                                                                     \
    if (VCD_EN && (en)) {                                                                                               \
        log_uart(0x20 | (id));                                                                                        \
        log_uart(t);                                                                                                  \
        log_uart((t) >> 8);                                                                                             \
        log_uart((t) >> 16);                                                                                            \
    }

#define log_event_irq(en, id)                                                                                         \
    if (VCD_EN && (en)) {                                                                                               \
        log_uart(0x00 | (id));                                                                                        \
    }

#define log_task_irq(en, id, b)                                                                                       \
    if (VCD_EN && (en)) {                                                                                               \
        log_uart(((b) ? 0x60 : 0x40) | (id));                                                                         \
        int t = get_systemtick();                                                                                     \
        log_uart(t);                                                                                                  \
        log_uart(t >> 8);                                                                                             \
        log_uart(t >> 16);                                                                                            \
    }

#define log_task_begin_irq(en, id)                                                                                    \
    if (VCD_EN && (en)) {                                                                                               \
        log_uart(0x60 | (id));                                                                                        \
        int t = get_systemtick();                                                                                     \
        log_uart(t);                                                                                                  \
        log_uart(t >> 8);                                                                                             \
        log_uart(t >> 16);                                                                                            \
    }
#define log_task_end_irq(en, id)                                                                                      \
    if (VCD_EN && (en)) {                                                                                               \
        log_uart(0x40 | (id));                                                                                        \
        int t = get_systemtick();                                                                                     \
        log_uart(t);                                                                                                  \
        log_uart(t >> 8);                                                                                             \
        log_uart(t >> 16);                                                                                            \
    }

#define log_task_begin_irq_2(en, id, t)                                                                               \
    if (VCD_EN && (en)) {                                                                                               \
        log_uart(0x60 | (id));                                                                                        \
        log_uart(t);                                                                                                  \
        log_uart((t) >> 8);                                                                                             \
        log_uart((t) >> 16);                                                                                            \
    }
#define log_task_end_irq_2(en, id, t)                                                                                 \
    if (VCD_EN && (en)) {                                                                                               \
        log_uart(0x40 | (id));                                                                                        \
        log_uart(t);                                                                                                  \
        log_uart((t) >> 8);                                                                                             \
        log_uart((t) >> 16);                                                                                            \
    }

#define log_b8_irq(en, id, d)                                                                                         \
    if (VCD_EN && (en)) {                                                                                               \
        log_uart(0x80 | (id));                                                                                        \
        log_uart(d);                                                                                                  \
    }

#define log_b16_irq(en, id, d)                                                                                        \
    if (VCD_EN && (en)) {                                                                                               \
        log_uart(0xc0 | (id));                                                                                        \
        log_uart(d);                                                                                                  \
        log_uart((d) >> 8);                                                                                           \
    }

#endif
