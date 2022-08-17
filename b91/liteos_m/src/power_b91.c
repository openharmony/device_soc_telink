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
#include <los_pm.h>
#include <los_task.h>
#include <los_timer.h>

#include <B91/clock.h>
#include <B91/stimer.h>
#include <B91/sys.h>

#include <B91/ext_driver/ext_pm.h>

#include <stack/ble/ble.h>

#define SLEEP_TIME_CORRECTION      ((UINT64)(3 * SYSTEM_TIMER_TICK_1S) / OS_SYS_CLOCK)
#define SYSTICKS_MAX_SLEEP         0xE0000000
#define SYSTICKS_MIN_SLEEP         (18352 + SLEEP_TIME_CORRECTION)
#define RESERVE_WAKEUP_TIME        1

static inline UINT64 MticksToSysticks(UINT64 mticks)
{
    return (mticks * SYSTEM_TIMER_TICK_1S) / OS_SYS_CLOCK;
}

static inline UINT32 SysticksToMticks(UINT32 sticks)
{
    return (UINT32)((UINT64)sticks * OS_SYS_CLOCK / SYSTEM_TIMER_TICK_1S);
}

static void B91Suspend(VOID);

static LosPmSysctrl g_sysctrl = {
    .normalSuspend = B91Suspend,
};

static inline void SetMtime(UINT64 time)
{
    volatile UINT32 *const rl = (volatile UINT32 *const)MTIMER;
    volatile UINT32 *const rh = (volatile UINT32 *const)(MTIMER + sizeof(UINT32));

    *rl = 0;
    *rh = (UINT32)(time >> SHIFT_32_BIT);
    *rl = (UINT32)time;
}

static inline UINT64 GetMtimeCompare(void)
{
    return *(volatile UINT64 *)(MTIMERCMP);
}

static inline UINT64 GetMtime(void)
{
    const volatile UINT32 *const rl = (const volatile UINT32 *const)MTIMER;
    const volatile UINT32 *const rh = (const volatile UINT32 *const)(MTIMER + sizeof(UINT32));
    UINT32 mtimeL, mtimeH;
    do {
        mtimeH = *rh;
        mtimeL = *rl;
    } while (mtimeH != *rh);
    return (((UINT64)mtimeH) << SHIFT_32_BIT) | mtimeL;
}

/**
 * @brief      	This function is used instead of the default sleep function ArchEnterSleep()
 * @param[in]  	none.
 * @return     	none.
*/
static void B91Suspend(VOID)
{
    UINT64 mcompare = GetMtimeCompare();
    UINT64 mtick = GetMtime();
    while (mtick == GetMtime()) {
    }
    mtick++;
    if (mtick > mcompare) {
        return;
    }

    UINT64 systicksSleepTimeout = MticksToSysticks(mcompare - mtick);

    if (systicksSleepTimeout >= SYSTICKS_MIN_SLEEP) {
        if (systicksSleepTimeout > SYSTICKS_MAX_SLEEP) {
            systicksSleepTimeout = SYSTICKS_MAX_SLEEP;
        } else {
            systicksSleepTimeout -= SLEEP_TIME_CORRECTION;
        }
        UINT32 sleepTick = stimer_get_tick();
        cpu_sleep_wakeup_32k_rc(SUSPEND_MODE, PM_WAKEUP_TIMER, sleepTick + systicksSleepTimeout);
        uart_clr_tx_index(UART0);
        uart_clr_tx_index(UART1);
        uart_clr_rx_index(UART0);
        uart_clr_rx_index(UART1);
        mtick += SysticksToMticks(stimer_get_tick() - sleepTick + SLEEP_TIME_CORRECTION);
        SetMtime(mtick);
        systicksSleepTimeout = MticksToSysticks(mcompare - mtick);
        if (systicksSleepTimeout < SYSTICKS_MIN_SLEEP) {
            ArchEnterSleep();
        }
    } else {
        ArchEnterSleep();
    }
}

/**
 * @brief   This callback function is used instead of the cpu_sleep_wakeup_32k_rc() sleep function of the BLE stack,
 *          set in the blc_pm_select_internal_32k_crystal() function.
*/
static int B91SleepCallback(SleepMode_TypeDef sleep_mode, SleepWakeupSrc_TypeDef wakeup_src, unsigned int wakeup_tick)
{
    int ret = 0;
    UINT32 timeSleepMs = (wakeup_tick - stimer_get_tick()) / SYSTEM_TIMER_TICK_1MS;
    systimer_irq_enable();
    if (timeSleepMs > RESERVE_WAKEUP_TIME) {
        LOS_Msleep(timeSleepMs - RESERVE_WAKEUP_TIME);
    }
    if (pm_get_wakeup_src() & WAKEUP_STATUS_TIMER) {
        ret = WAKEUP_STATUS_TIMER | STATUS_ENTER_SUSPEND;
    }
    return ret;
}

VOID B91SuspendSleepInit(VOID)
{
    UINT32 ret = LOS_PmRegister(LOS_PM_TYPE_SYSCTRL, &g_sysctrl);
    if (ret != LOS_OK) {
        printf("Ret of PMRegister = %#x\r\n", ret);
    } else {
        printf("\r\n B91_SLEEP_init\r\n");
    }

    cpu_sleep_wakeup = B91SleepCallback;
    blc_ll_initPowerManagement_module();
    bls_pm_setSuspendMask(SUSPEND_ADV | SUSPEND_CONN);
}