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

#include <stdio.h>

#include <soc.h>
#include <target_config.h>

#include <los_arch_interrupt.h>
#include <los_interrupt.h>

#include <riscv_hal.h>

#include <B91/plic.h>

#define PLIC_IRQ_LIMIT 64

typedef VOID (*HwiProcFunc)(VOID *arg);

VOID default_irq_handler(VOID);

STATIC HWI_HANDLE_FORM_S irq_handlers[PLIC_IRQ_LIMIT] = {
    [0 ...(PLIC_IRQ_LIMIT - 1)] = {(HWI_PROC_FUNC)default_irq_handler, NULL, 0}};

STATIC UINT32 EnableIrq(UINT32 hwiNum)
{
    if (hwiNum > OS_HWI_MAX_NUM) {
        return OS_ERRNO_HWI_NUM_INVALID;
    }

    UINT32 intSave = LOS_IntLock();
    if (hwiNum >= OS_RISCV_SYS_VECTOR_CNT) {
        plic_interrupt_enable(hwiNum - OS_RISCV_SYS_VECTOR_CNT);
        SET_CSR(mie, RISCV_MACH_EXT_IRQ);
    } else {
        UINT32 irq = hwiNum;
        UINT32 mask = 1 << irq;
        __asm__ volatile("csrs mie, %0" ::"r"(mask));
    }

    LOS_IntRestore(intSave);
    return LOS_OK;
}

VOID HalIrqEnable(UINT32 vector)
{
    (VOID) EnableIrq(vector);
}

STATIC UINT32 DisableIrq(UINT32 hwiNum)
{
    if (hwiNum > OS_HWI_MAX_NUM) {
        return OS_ERRNO_HWI_NUM_INVALID;
    }

    UINT32 intSave = LOS_IntLock();
    if (hwiNum >= OS_RISCV_SYS_VECTOR_CNT) {
        plic_interrupt_disable(hwiNum - OS_RISCV_SYS_VECTOR_CNT);
    } else {
        UINT32 irq = hwiNum;
        UINT32 mask = 1 << irq;
        __asm__ volatile("csrc mie, %0" ::"r"(mask));
    }

    LOS_IntRestore(intSave);
    return LOS_OK;
}

VOID HalIrqDisable(UINT32 vector)
{
    (VOID) DisableIrq(vector);
}

VOID HalSetLocalInterPri(UINT32 interPriNum, UINT16 prior)
{
    plic_set_priority(interPriNum, prior);
}

_attribute_ram_code_ void mext_irq_handler(void)
{
    unsigned int periph_irq = plic_interrupt_claim();

    HWI_HANDLE_FORM_S *hwiForm = &irq_handlers[periph_irq];
    HwiProcFunc func = (HwiProcFunc)(hwiForm->pfnHook);
    func(hwiForm->uwParam);

    plic_interrupt_complete(periph_irq); /* complete interrupt */
}

UINT32 B91IrqRegister(UINT32 irq_num, HWI_PROC_FUNC handler, HWI_ARG_T irqParam)
{
    if (irq_num >= PLIC_IRQ_LIMIT) {
        return OS_ERRNO_HWI_NUM_INVALID;
    }

    UINT32 intSave = LOS_IntLock();

    if (NULL != handler) {
        irq_handlers[irq_num].pfnHook = handler;
        irq_handlers[irq_num].uwParam = (VOID *)irqParam;
    } else {
        irq_handlers[irq_num].pfnHook = (HWI_PROC_FUNC)default_irq_handler;
        irq_handlers[irq_num].uwParam = NULL;
    }

    LOS_IntRestore(intSave);

    return LOS_OK;
}

VOID B91IrqInit(VOID)
{
    UINT32 ret = LOS_HwiCreate(RISCV_MACH_EXT_IRQ, OS_HWI_PRIO_LOWEST, 0, (HWI_PROC_FUNC)mext_irq_handler, 0);
    if (ret != LOS_OK) {
        printf("ret of LOS_HwiCreate(RISCV_MACH_EXT_IRQ) = %#x\r\n", ret);
    }

    core_interrupt_enable();
}
