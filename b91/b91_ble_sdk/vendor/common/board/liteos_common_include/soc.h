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

#ifndef _SOC_H
#define _SOC_H

#include "soc_common.h"

/*
 * Get the response interrupt number via mcause.
 * id = mcause & MCAUSE_INT_ID_MASK
 */
#define MCAUSE_INT_ID_MASK                            0x7FFFFFF
#define MTIMERCMP                                     0xE6000008
#define MTIMER                                        0xE6000000

#define RISCV_SYS_MAX_IRQ                             11

/*
 * Because of strange implementation of los_interrupt.c in kernel sum of
 * (RISCV_SYS_MAX_IRQ + RISCV_PLIC_VECTOR_CNT) should be at least 25
 */
#define RISCV_PLIC_VECTOR_CNT                         64

#endif
