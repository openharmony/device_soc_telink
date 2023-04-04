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

#include "multithread.h"
#include "common.h"

#include <los_mux.h>

#include <ohos_init.h>

/****************************************************************
 * If RTOS is used and cryptography stuffs are used
 * from more than one thread implement exclusive access
 * for HW crypto units depending on used operation system.
 * See documentation for your RTOS.
 ****************************************************************/

static struct {
    UINT32 entropy;
    UINT32 ecp;
    UINT32 aes;
} g_mutexes;

void mbedtls_entropy_lock(void)
{
    LOS_MuxPend(g_mutexes.entropy, LOS_WAIT_FOREVER);
}

void mbedtls_entropy_unlock(void)
{
    LOS_MuxPost(g_mutexes.entropy);
}

void mbedtls_ecp_lock(void)
{
    LOS_MuxPend(g_mutexes.ecp, LOS_WAIT_FOREVER);
}

void mbedtls_ecp_unlock(void)
{
    LOS_MuxPost(g_mutexes.ecp);
}

void mbedtls_aes_lock(void)
{
    LOS_MuxPend(g_mutexes.aes, LOS_WAIT_FOREVER);
}

void mbedtls_aes_unlock(void)
{
    LOS_MuxPost(g_mutexes.aes);
}

static void mbedtls_multithread_init(void)
{
    UINT32 res;
    res = LOS_MuxCreate(&g_mutexes.entropy);
    if (res != LOS_OK) {
        printf("LOS_MuxCreate(&g_mutexes.entropy) returned %x\r\n", res);
    }

    res = LOS_MuxCreate(&g_mutexes.ecp);
    if (res != LOS_OK) {
        printf("LOS_MuxCreate(&g_mutexes.ecp) returned %x\r\n", res);
    }

    res = LOS_MuxCreate(&g_mutexes.aes);
    if (res != LOS_OK) {
        printf("LOS_MuxCreate(&g_mutexes.aes) returned %x\r\n", res);
    }
}

SYS_RUN_PRI(mbedtls_multithread_init, 0);
