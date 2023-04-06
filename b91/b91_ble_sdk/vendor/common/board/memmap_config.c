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
#include "soc.h"
#include "los_compiler.h"
#include "target_config.h"

#if (RTOS_SELECT == RTOS_LITEOS &&  (LOSCFG_SYS_EXTERNAL_HEAP == 1))
__attribute((__section__(".rtos_heap")))  __attribute((__aligned__(256))) __attribute((__used__)) 
UINT8 heap_reserved[LOSCFG_SYS_HEAP_SIZE];
#endif


