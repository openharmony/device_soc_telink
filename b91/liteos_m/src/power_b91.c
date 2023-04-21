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

#include <los_arch_timer.h>
#include <los_compiler.h>
#include <los_pm.h>
#include <los_reg.h>
#include <los_task.h>
#include <los_timer.h>

#include <B91/clock.h>
#include <B91/stimer.h>
#include <B91/sys.h>

#include <B91/ext_driver/ext_pm.h>

#include <stack/ble/ble.h>

#include <inttypes.h>

#define SYSTICKS_MAX_SLEEP     (0xFFFFFFFF >> 2)
#define MTICKS_MIN_SLEEP       (80)
#define MTICKS_CORRECTION_TIME (0)
#define MTICKS_RESERVE_TIME    (1 + MTICKS_CORRECTION_TIME)

bool B91_system_suspend(UINT32 wake_stimer_tick);

static inline UINT64 MticksToSysticks(UINT64 mticks)
{
    return (mticks * SYSTEM_TIMER_TICK_1S) / OS_SYS_CLOCK;
}

static inline UINT32 SysticksToMticks(UINT32 sticks)
{
    return (UINT32)((UINT64)sticks * OS_SYS_CLOCK / SYSTEM_TIMER_TICK_1S);
}

static UINT32 B91Suspend(VOID);

static LosPmSysctrl g_sysctrl = {
    .normalSuspend = B91Suspend,
};

static inline void SetMtime(UINT64 time)
{
    HalIrqDisable(RISCV_MACH_TIMER_IRQ);
    WRITE_UINT32(U32_MAX, MTIMER + MTIMER_HI_OFFSET);
    WRITE_UINT32((UINT32)time, MTIMER);
    WRITE_UINT32((UINT32)(time >> SHIFT_32_BIT), MTIMER + MTIMER_HI_OFFSET);
    HalIrqEnable(RISCV_MACH_TIMER_IRQ);
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
 * @brief      	This function is used instead of the default sleep function
 * ArchEnterSleep()
 * @param[in]  	none.
 * @return     	none.
 */
_attribute_ram_code_ static UINT32 B91Suspend(VOID)
{
    UINT64 mcompare = GetMtimeCompare();
    UINT64 mtick = GetMtime();
    if ((mcompare - mtick) < (MTICKS_MIN_SLEEP + MTICKS_RESERVE_TIME)) {
        return 0;
    }
    UINT32 intSave = LOS_IntLock();
    mcompare = GetMtimeCompare();
    mtick = GetMtime();
    if ((mcompare - mtick) < MTICKS_MIN_SLEEP + MTICKS_RESERVE_TIME) {
        LOS_IntRestore(intSave);
        return 0;
    }

    UINT64 systicksSleepTimeout = MticksToSysticks(mcompare - mtick);
    if (systicksSleepTimeout > SYSTICKS_MAX_SLEEP) {
        systicksSleepTimeout = SYSTICKS_MAX_SLEEP;
    }
    blc_pm_setWakeupSource(PM_WAKEUP_PAD);

    UINT32 sleepTick = stimer_get_tick();

    if (B91_system_suspend(sleepTick + systicksSleepTimeout - MTICKS_RESERVE_TIME)) {
        UINT32 span = SysticksToMticks(stimer_get_tick() - sleepTick) + MTICKS_CORRECTION_TIME;
        mtick += span;
        SetMtime(mtick);
        uart_clr_tx_index(UART0);
        uart_clr_tx_index(UART1);
        uart_clr_rx_index(UART0);
        uart_clr_rx_index(UART1);
    }
    LOS_IntRestore(intSave);
    return 0;
}

VOID B91SuspendSleepInit(VOID)
{
    UINT32 ret = LOS_PmRegister(LOS_PM_TYPE_SYSCTRL, &g_sysctrl);
    if (ret != LOS_OK) {
        printf("Ret of PMRegister = %#x\r\n", ret);
    } else {
        printf("\r\n B91_SLEEP_init\r\n");
    }
}

_attribute_ram_code_ bool B91_system_suspend(UINT32 wake_stimer_tick)
{
    bool result = false;

    extern bool blc_ll_isBleTaskIdle(void);
    if (!blc_ll_isBleTaskIdle()) {
        blc_pm_setAppWakeupLowPower(wake_stimer_tick, 1);
        if (!blc_pm_handler()) {
            result = true;
        }
        blc_pm_setAppWakeupLowPower(0, 0);
    } else {
        cpu_sleep_wakeup_32k_rc(SUSPEND_MODE, PM_WAKEUP_TIMER | PM_WAKEUP_PAD, wake_stimer_tick);
        result = true;
    }
    return result;
}