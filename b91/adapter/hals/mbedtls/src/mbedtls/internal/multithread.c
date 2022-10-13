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

#include <stdbool.h>

#include <los_mux.h>
#include <ohos_init.h>

#include "common.h"
#include "multithread.h"

static struct {
    UINT32 entropy;
    UINT32 ecp;
    UINT32 aes;
} g_mutexes;

/****************************************************************
 * If RTOS is used and cryptography stuffs are used
 * from more than one thread implement exclusive access
 * for HW crypto units depending on used operation system.
 * See documentation for your RTOS.
 ****************************************************************/

void mbedtls_multithread_init(void)
{
    UINT32 res;
    if (LOS_OK != (res = LOS_MuxCreate(&g_mutexes.entropy))) {
        printf("LOS_MuxCreate(&g_mutexes.entropy) = %u\r\n");
        return;
    }
    if (LOS_OK != (res = LOS_MuxCreate(&g_mutexes.ecp))) {
        printf("LOS_MuxCreate(&g_mutexes.ecp) = %u\r\n");
        return;
    }
    if (LOS_OK != (res = LOS_MuxCreate(&g_mutexes.aes))) {
        printf("LOS_MuxCreate(&g_mutexes.aes) = %u\r\n");
        return;
    }
}

void mbedtls_entropy_lock( void )
{
    LOS_MuxPend(g_mutexes.entropy, LOS_WAIT_FOREVER);
}

void mbedtls_entropy_unlock( void )
{
    LOS_MuxPost(g_mutexes.entropy);
}

void mbedtls_ecp_lock( void )
{
    LOS_MuxPend(g_mutexes.ecp, LOS_WAIT_FOREVER);
}

void mbedtls_ecp_unlock( void )
{
    LOS_MuxPost(g_mutexes.ecp);
}

void mbedtls_aes_lock( void )
{
    LOS_MuxPend(g_mutexes.aes, LOS_WAIT_FOREVER);
}

void mbedtls_aes_unlock( void )
{
    LOS_MuxPost(g_mutexes.aes);
}

SYS_RUN_PRI(mbedtls_multithread_init, 0);
