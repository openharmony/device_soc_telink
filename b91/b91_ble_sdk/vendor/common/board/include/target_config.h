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

/**@defgroup los_config System configuration items
 * @ingroup kernel
 */

#ifndef _TARGET_CONFIG_H
#define _TARGET_CONFIG_H

#include "soc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/*=============================================================================
                                        System clock module configuration
=============================================================================*/
#define OS_SYS_CLOCK                                        (32000)
#define LOSCFG_BASE_CORE_TICK_PER_SECOND                    (1000UL)
#define LOSCFG_BASE_CORE_TICK_HW_TIME                       0
#define LOSCFG_BASE_CORE_TICK_WTIMER                        1
/*=============================================================================
                                        Hardware interrupt module configuration
=============================================================================*/
#define LOSCFG_PLATFORM_HWI                                 1
#define LOSCFG_USE_SYSTEM_DEFINED_INTERRUPT                 0
#define LOSCFG_PLATFORM_HWI_LIMIT                           25
#define LOSCFG_PLATFORM_HWI_WITH_ARG                        1
/*=============================================================================
                                       Task module configuration
=============================================================================*/
#define LOSCFG_BASE_CORE_TSK_LIMIT                          12
#define LOSCFG_BASE_CORE_TSK_IDLE_STACK_SIZE                (2048)
#define LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE             (4096)
#define LOSCFG_BASE_CORE_TSK_MIN_STACK_SIZE                 (1024)
#define LOSCFG_BASE_CORE_TIMESLICE                          1
#define LOSCFG_BASE_CORE_TIMESLICE_TIMEOUT                  100000
#define LOSCFG_BASE_CORE_TICK_RESPONSE_MAX                  0xFFFFFFFFFFFFFFF
/*=============================================================================
                                       Semaphore module configuration
=============================================================================*/
#define LOSCFG_BASE_IPC_SEM                                 1
#define LOSCFG_BASE_IPC_SEM_LIMIT                           48
/*=============================================================================
                                       Mutex module configuration
=============================================================================*/
#define LOSCFG_BASE_IPC_MUX                                 1
#define LOSCFG_BASE_IPC_MUX_LIMIT                           32
/*=============================================================================
                                       Queue module configuration
=============================================================================*/
#define LOSCFG_BASE_IPC_QUEUE                               1
#define LOSCFG_BASE_IPC_QUEUE_LIMIT                         16
/*=============================================================================
                                       Software timer module configuration
=============================================================================*/
#define LOSCFG_BASE_CORE_SWTMR                              1
#define LOSCFG_BASE_CORE_SWTMR_ALIGN                        1
#define LOSCFG_BASE_CORE_SWTMR_LIMIT                        6
/*=============================================================================
                                       Memory module configuration
=============================================================================*/
#define LOSCFG_SYS_EXTERNAL_HEAP                            1

#define LOSCFG_SYS_HEAP_SIZE                                OS_SYS_MEM_SIZE
#define OS_SYS_MEM_SIZE                                     64*1024

#if LOSCFG_SYS_EXTERNAL_HEAP
#include "memmap_config.h"
#define LOSCFG_SYS_HEAP_ADDR                                LOS_MEM_ADDR
#else
#define LOSCFG_SYS_HEAP_ADDR                                (VOID *)(__los_heap_addr_start__)
#endif

#define LOSCFG_BASE_MEM_NODE_INTEGRITY_CHECK                0
#define LOSCFG_BASE_MEM_NODE_SIZE_CHECK                     1
#define LOSCFG_MEM_MUL_POOL                                 0
#define OS_SYS_MEM_NUM                                      20
#define LOSCFG_KERNEL_MEM_SLAB                              0
/*=============================================================================
                                       Exception module configuration
=============================================================================*/
#define LOSCFG_PLATFORM_EXC                                 0
/* =============================================================================
                                       printf module configuration
============================================================================= */
#define LOSCFG_KERNEL_PRINTF                                1
/* =============================================================================
                                       enable backtrace
============================================================================= */
#define LOSCFG_BACKTRACE_TYPE                               0
#define LOSCFG_BACKTRACE_DEPTH                              15

#define LOSCFG_FAMILY_B91                                   1

#define LOSCFG_TELINK_B91_CPU_FREQ                          16000000
#define LOSCFG_POWER_MODE_DCDC_1P4_DCDC_1P8                 1
#define LOSCFG_VBAT_TYPE_MAX_VALUE_GREATER_THAN_3V6         1
#define LOSCFG_CLOCK32KHZ_EXTERNAL				            0

#define LOSCFG_KERNEL_PM 									1

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */


#endif /* _TARGET_CONFIG_H */
