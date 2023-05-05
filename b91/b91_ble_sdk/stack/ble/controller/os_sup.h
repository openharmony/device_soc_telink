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

#ifndef _OS_SUP_H_
#define _OS_SUP_H_

#include "stack/ble/ble_config.h"

#define OS_SUP_LONG_SLEEP 0

typedef void (*os_give_sem_t)(void);

/* User API */
void blc_ll_registerGiveSemCb(os_give_sem_t func);
int blc_pm_handler(void);

#if OS_SUP_LONG_SLEEP
#if OS_SUP_EN
extern uint32_t os_wakeup_tick;
extern uint8_t os_wakeup_en;
#endif
void blc_ll_setOSWakeupTick(uint32_t wakeup_tick);
#endif

/* Stack API. !!! user can't use. */
void blt_ll_sem_give(void);

#endif /* _OS_SUP_H_ */
