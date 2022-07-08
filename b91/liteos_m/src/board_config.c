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

#include "los_compiler.h"

#include <nds_intrinsic.h>
#include <stdint.h>

#define MCACHE_CTL_ICACHE 1
#define MCACHE_CTL_DCACHE 2

typedef void (*InitFunc)(void);

extern UINT32 _ITB_BASE_;

extern UINT32 SEG_RETENTION_DATA_LMA_START[], SEG_RETENTION_DATA_VMA_START[], SEG_RETENTION_DATA_VMA_END[];
extern UINT32 SEG_RAMCODE_LMA_START[], SEG_RAMCODE_VMA_START[], SEG_RAMCODE_VMA_END[];
extern UINT32 SEG_DATA_LMA_START[], SEG_DATA_VMA_START[], SEG_DATA_VMA_END[];
extern UINT32 SEG_BSS_VMA_START[], SEG_BSS_VMA_END[];
extern UINT32 __int_stack_start[], __int_stack_end[];

extern InitFunc __preinit_array_start[] __attribute__((weak));
extern InitFunc __preinit_array_end[] __attribute__((weak));
extern InitFunc __init_array_start[] __attribute__((weak));
extern InitFunc __init_array_end[] __attribute__((weak));
extern InitFunc __fini_array_start[] __attribute__((weak));
extern InitFunc __fini_array_end[] __attribute__((weak));

#define STACK_MAGIC UINT32_C(0xDEADBEEF)

STATIC VOID BoardConfigInnerSafe(VOID);

#ifdef __GNUC__
#pragma GCC push_options
#pragma GCC optimize("-fno-stack-protector")
#endif /* __GNUC__ */

__attribute__((noinline)) STATIC VOID CopyBuf32(UINT32 *dst, const UINT32 *dstEnd, const UINT32 *src)
{
    while (dst < dstEnd) {
        *dst++ = *src++;
    }
}

#define COPY_SEGMENT(_SEGNAME_) CopyBuf32((_SEGNAME_##_VMA_START), (_SEGNAME_##_VMA_END), (_SEGNAME_##_LMA_START))

__attribute__((used)) static void BoardConfigInner(void)
{
    for (UINT32 *p = SEG_BSS_VMA_START; p < SEG_BSS_VMA_END; ++p) {
        *p = 0;
    }

    COPY_SEGMENT(SEG_RETENTION_DATA);
    COPY_SEGMENT(SEG_RAMCODE);
    COPY_SEGMENT(SEG_DATA);

    BoardConfigInnerSafe();
}

/*
 * Use "naked" attribute to prevent stack usage before stack is initialized
 */
__attribute__((naked)) void BoardConfig(void)
{
#ifdef __nds_execit
    /* Initialize EXEC.IT table */
    __builtin_riscv_csrw((UINT32)&_ITB_BASE_, NDS_UITB);
#endif /* __nds_execit */

    /* Enable I/D-Cache */
    UINT32 mcacheCtl = __builtin_riscv_csrr(NDS_MCACHE_CTL);
    mcacheCtl |= MCACHE_CTL_ICACHE; /* I-Cache */
    mcacheCtl |= MCACHE_CTL_DCACHE; /* D-Cache */
    __builtin_riscv_csrw(mcacheCtl, NDS_MCACHE_CTL);
    __asm__ volatile("fence.i");

    for (UINT32 *p = __int_stack_start; p < __int_stack_end; ++p) {
        *p = STACK_MAGIC;
    }

    __asm__ volatile("j BoardConfigInner");
}

#ifdef __GNUC__
#pragma GCC pop_options
#endif /* __GNUC__ */

/**
 * @brief BoardConfigInnerSafe is safe part of BoardConfig without disabled stack protection
 */
STATIC VOID BoardConfigInnerSafe(VOID)
{
    for (InitFunc *f = __preinit_array_start; f < __preinit_array_end; ++f) {
        (*f)();
    }

    for (InitFunc *f = __init_array_start; f < __init_array_end; ++f) {
        (*f)();
    }

    for (InitFunc *f = __fini_array_start; f < __fini_array_end; ++f) {
        (*f)();
    }
}
