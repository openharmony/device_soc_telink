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

#include <target_config.h>

#include <los_compiler.h>
#include <los_timer.h>
#include <los_task.h>

#include <B91/clock.h>
#include <B91/sys.h>
#include <B91/stimer.h>

#include <B91/ext_driver/ext_pm.h>
#include "los_pm.h"
#include <stack/ble/ble.h>

#include <board_config.h>
#include <gpio_if.h>

#define RISCV_MTIME_BASE             0xE6000000
#define RISCV_MTIMECMP_BASE          0xE6000008

#define SYSTICKS_MAX_SLEEP           0xE0000000

#define mticks_to_systicks(mticks)      ((((UINT64)(mticks) * 2 * SYSTEM_TIMER_TICK_1S) / OS_SYS_CLOCK +1) / 2)
#define systicks_to_mticks(sticks)      ((((UINT64)(sticks) * 1024 * OS_SYS_CLOCK) / SYSTEM_TIMER_TICK_1S + 512) / 1024)
#define systicks_to_ms(sticks) ((UINT64)(sticks)  / SYSTEM_TIMER_TICK_1MS)

static void B91Suspend(VOID);

static LosPmSysctrl g_sysctrl = {
    .normalSuspend = B91Suspend,
    .normalResume = NULL,
    .lightSuspend = NULL,
    .lightResume = NULL,
};

void debug_led_on(int led)
{
         if(led == 0) { led=0x01; }
    else if(led == 1) { led=0x02; }
    else if(led == 2) { led=0x04; }
    else if(led == 3) { led=0x08; }
    else if(led == 4) { led=0x10; }
    else if(led == 5) { led=0x20; }
    else if(led == 6) { led=0x40; }
    else if(led == 7) { led=0x80; }
	uint8_t *gpio_reg1 = (uint8_t *)0x8014030a;
	uint8_t *gpio_reg2 = (uint8_t *)0x8014030b;
	*gpio_reg1 &= ~led;
    *gpio_reg2 |= led;
}
void debug_led_off(int led)
{
         if(led == 0) { led=0x01; }
    else if(led == 1) { led=0x02; }
    else if(led == 2) { led=0x04; }
    else if(led == 3) { led=0x08; }
    else if(led == 4) { led=0x10; }
    else if(led == 5) { led=0x20; }
    else if(led == 6) { led=0x40; }
    else if(led == 7) { led=0x80; }
	uint8_t *gpio_reg1 = (uint8_t *)0x8014030a;
	uint8_t *gpio_reg2 = (uint8_t *)0x8014030b;
	*gpio_reg1 &= ~led;
	*gpio_reg2 &= ~led;
}

static UINT32 tl_sleep_tick = 0;

static void set_mtime(UINT64 time)
{
	volatile UINT32 *const rl = (volatile UINT32 *const)RISCV_MTIME_BASE;
	volatile UINT32 *const rh = (volatile UINT32 *const)(RISCV_MTIME_BASE + sizeof(UINT32));

	*rl = 0;
	*rh = (UINT32)(time >> 32);
	*rl = (UINT32)time;
}

void tl_sleep_save_tick(void)
{
	tl_sleep_tick = stimer_get_tick();
}

UINT32 tl_sleep_get_diff(void)
{
	return stimer_get_tick() - tl_sleep_tick;
}

static UINT64 get_mtime_compare(void)
{
    return *(volatile UINT64 *)(RISCV_MTIMECMP_BASE);
}

static UINT64 get_mtime(void)
{
    const volatile UINT32 *const rl = (const volatile UINT32 *const)RISCV_MTIME_BASE;
    const volatile UINT32 *const rh = (const volatile UINT32 *const)(RISCV_MTIME_BASE + sizeof(UINT32));
    UINT32 mtime_l, mtime_h;
    do{
        mtime_h = *rh;
        mtime_l = *rl;
    } while(mtime_h != *rh);
    return (((UINT64)mtime_h) << 32) | mtime_l;
}

static void B91Suspend(VOID)
{
    UINT64 mcompare = get_mtime_compare();
    UINT64 mtick = get_mtime();
	UINT64 systicks_sleep_timeout = mticks_to_systicks(mcompare - mtick);
    if (systicks_sleep_timeout > SYSTICKS_MAX_SLEEP) 
    {
		systicks_sleep_timeout = SYSTICKS_MAX_SLEEP;
	}
    tl_sleep_save_tick();
    debug_led_on(4);
    cpu_sleep_wakeup_32k_rc(SUSPEND_MODE, PM_WAKEUP_TIMER, (tl_sleep_tick + systicks_sleep_timeout));
    debug_led_off(4);
    mtick += systicks_to_mticks(tl_sleep_get_diff());
    set_mtime(mtick);
}

int b91_sleep_callback(SleepMode_TypeDef sleep_mode,  SleepWakeupSrc_TypeDef wakeup_src, unsigned int  wakeup_tick)
{
    UINT32 time_ms = systicks_to_ms(wakeup_tick - stimer_get_tick());
    if (time_ms > 3) { 
        LOS_Msleep(time_ms - 1);
    }
    while(stimer_get_tick() < wakeup_tick) {}
}

void user_sleep_init(void) 
{
    printf("\r\n B91_SLEEP_init \r\n");
    UINT32 ret = LOS_PmRegister(LOS_PM_TYPE_SYSCTRL, &g_sysctrl);
    if (ret != LOS_OK) {
        printf("Ret of PMRegister = %#x", ret);
    }

    ret = LOS_PmModeSet(LOS_SYS_LIGHT_SLEEP);
    if (ret != LOS_OK) {
        printf("Ret of PMModeSet = %#x", ret);
    }

    cpu_sleep_wakeup = b91_sleep_callback;
    blc_ll_initPowerManagement_module();
    bls_pm_setSuspendMask (SUSPEND_ADV | SUSPEND_CONN);
    //gpio_shutdown(GPIO_ALL); 
}