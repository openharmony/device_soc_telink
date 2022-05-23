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

#ifndef _RESET_VECTOR_S
#define _RESET_VECTOR_S

#define LOSCFG_IRQ_STACK_SIZE 0x800
#define LOSCFG_NMI_STACK_SIZE 0x800
#define LOSCFG_STARTUP_STACK_SIZE 0x800
#define LOSCFG_EXC_STACK_SIZE 0x800

.extern HalTrapVector
.global __start_and_irq_stack_top
.global __nmi_stack_top
.global __except_stack_top
.global reset_vector
.extern BoardConfig

.option rvc
.section .entry.text, "ax"
reset_vector:
    tail HandleReset

.section .text
.option rvc
HandleReset:
    la      t0, HalTrapVector
    csrw    mtvec, t0
    csrwi   mstatus, 0
    csrwi   mie, 0

#ifdef LOSCFG_ARCH_FPU_ENABLE
   /* set to initial state of FPU */
    li      t0, RISCV_MSTATUS_FS
    csrs    mstatus, t0
    fssr    x0
#endif

    .option push
    .option norelax
    /* initialize global pointer */
    la      gp, __global_pointer$
    .option pop

    /* initialize stack pointer */
    la      sp, __start_and_irq_stack_top

    /* Board features. */
    call    BoardConfig

    /* jump to main func. */
    tail    main

.section ".int_stack", "wa", %nobits

__start_and_irq_stack:
    .space LOSCFG_STARTUP_STACK_SIZE
__start_and_irq_stack_top:

__except_stack:
    .space LOSCFG_EXC_STACK_SIZE
__except_stack_top:

__nmi_stack:
    .space LOSCFG_NMI_STACK_SIZE
__nmi_stack_top:

#endif
