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

#include <stdlib.h>

#include <los_compiler.h>

#include <soc.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#ifdef __GNUC__
/* stack protector */
// UINTPTR __stack_chk_guard = 0x000a0dff;
extern UINTPTR __stack_chk_guard;

STATIC UINT32 ArchGetTimerCnt(VOID)
{
    UINT32 cntpct;
    cntpct = READ_CSR(0xB00); /* MCYCLE REG Address */
    return cntpct;
}

/*
 * If the SP compiling options:-fstack-protector-strong or -fstack-protector-all is enabled,
 * We recommend to implement true random number generator function for __stack_chk_guard
 * value to replace the function implementation template shown as below.
 */
#pragma GCC push_options
#pragma GCC optimize ("-fno-stack-protector")
LITE_OS_SEC_TEXT_INIT WEAK VOID ArchStackGuardInit(VOID)
{
    int rnd;
    UINT32 seed;

    seed = ArchGetTimerCnt();
    srand(seed);
    rnd = rand();
    __stack_chk_guard = (UINTPTR)rnd;
}
#pragma GCC pop_options
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
