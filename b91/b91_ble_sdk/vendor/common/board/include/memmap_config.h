/******************************************************************************
 * Copyright (c) 2021 Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 * All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *****************************************************************************/

#ifndef _MEMMAP_CONFIG_H
#define _MEMMAP_CONFIG_H

#include "soc.h"
#include "los_compiler.h"
#include "target_config.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#if (LOSCFG_SYS_EXTERNAL_HEAP == 0)

extern UINT8 __los_heap_addr_start__[];
extern UINT8 __los_heap_addr_end__[];
extern UINT8 __los_heap_size__[];

#else

extern UINT8 heap_reserved[LOSCFG_SYS_HEAP_SIZE];

#define LOS_MEM_ADDR        (VOID *)&heap_reserved[0]

#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _MEMMAP_CONFIG_H */
