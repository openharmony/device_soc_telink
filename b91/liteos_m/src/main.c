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

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <unistd.h>

#include <los_task.h>
#include <los_memory.h>

#include <devmgr_service_start.h>
#include <gpio_if.h>
#include <hiview_log.h>
#include <hiview_output_log.h>
#include <utils_file.h>

#include <board_config.h>

#include <b91_irq.h>
#include <system_b91.h>
#include <power_b91.h>

#include <B91/clock.h>
#include <B91/gpio.h>
#include <B91/uart.h>

#include <B91/gpio_default.h>

#include <../vendor/common/blt_common.h>

#include "canary.h"

#define HZ_IN_MHZ (1000 * 1000)

#define DEBUG_UART_PORT      UART0
#define DEBUG_UART_PIN_TX    UART0_TX_PB2
#define DEBUG_UART_PIN_RX    UART0_RX_PB3
#define DEBUG_UART_PARITY    UART_PARITY_NONE
#define DEBUG_UART_STOP_BITS UART_STOP_BIT_ONE
#define DEBUG_UART_BAUDRATE  921600

#define B91_SYSTEM_INIT_TASK_STACKSIZE (1024 * 32)
#define B91_SYSTEM_INIT_TASK_PRIO      7
#define B91_SYSTEM_INIT_TASK_NAME      "B91SystemInit"

extern UserErrFunc g_userErrFunc;

void *__wrap_malloc(size_t s)
{
    if (s == 0) {
        return NULL;
    }
    return LOS_MemAlloc(OS_SYS_MEM_ADDR, s);
}

void __wrap_free(void *ptr)
{
    if (ptr == NULL) {
        return;
    }
    return LOS_MemFree(OS_SYS_MEM_ADDR, ptr);
}

void OHOS_SystemInit(void);
struct PartitionCfg *LittlefsConfigGet(void);

STATIC VOID LittlefsInit(VOID)
{
#define DIR_DATA "/data"
#define PAR_DATA 0
#define DIR_PERMISSIONS 0777

    int res;

    printf("LittleFS_Init \r\n");

    struct PartitionCfg *cfg = LittlefsConfigGet();

    res = mount(PAR_DATA, DIR_DATA, "littlefs", 0, cfg);
    printf("mount = %d\r\n", res);

    struct stat sinfo;
    if ( !(stat(DIR_DATA, &sinfo) == 0 && S_ISDIR(sinfo.st_mode))) {
        res = mkdir(DIR_DATA, DIR_PERMISSIONS);
        printf("mkdir = %d\r\n", res);
    }
}

VOID HardwareInit(VOID)
{
    SystemInit();
}

VOID IoTWatchDogKick(VOID)
{
}

STATIC VOID B91SystemInit(VOID)
{
    OHOS_SystemInit();
    LittlefsInit();
}

UINT32 LosAppInit(VOID)
{
    UINT32 ret = LOS_OK;

    B91IrqInit();

    unsigned int taskID_ohos;
    TSK_INIT_PARAM_S task_ohos = {0};

    task_ohos.pfnTaskEntry = (TSK_ENTRY_FUNC)B91SystemInit;
    task_ohos.uwStackSize = B91_SYSTEM_INIT_TASK_STACKSIZE;
    task_ohos.pcName = B91_SYSTEM_INIT_TASK_NAME;
    task_ohos.usTaskPrio = B91_SYSTEM_INIT_TASK_PRIO;
    ret = LOS_TaskCreate(&taskID_ohos, &task_ohos);
    if (ret != LOS_OK) {
        printf("Create Task failed! ERROR: 0x%x\r\n", ret);
    }

    return ret;
}

__attribute__((weak)) void *_sbrk(ptrdiff_t incr)
{
    extern char _end[];
    extern char _heap_end[];
    static char *curbrk = _end;

    if ((curbrk + incr < _end) || (curbrk + incr > _heap_end)) {
        return (void *)(-1);
    }

    curbrk += incr;
    return (void *)(curbrk - incr);
}

VOID UsartInit(VOID)
{
    unsigned short div;
    unsigned char bwpc;

    uart_set_pin(DEBUG_UART_PIN_TX, DEBUG_UART_PIN_RX);
    uart_reset(DEBUG_UART_PORT);
    uart_cal_div_and_bwpc(DEBUG_UART_BAUDRATE, sys_clk.pclk * HZ_IN_MHZ, &div, &bwpc);
    telink_b91_uart_init(DEBUG_UART_PORT, div, bwpc, DEBUG_UART_PARITY, DEBUG_UART_STOP_BITS);
    uart_rx_irq_trig_level(DEBUG_UART_PORT, 1);
}

int _write(int handle, char *data, int size)
{
    UNUSED(handle);
    UNUSED(data);
    UNUSED(size);

    int ret = 0;

    switch (handle) {
        case STDOUT_FILENO:
        case STDERR_FILENO: {
            uart_send(UART0, (unsigned char *)data, size);
            while (uart_tx_is_busy(UART0)) {}
            ret = size;
            break;
        }
        default: {
            break;
        }
    }

    return ret;
}

static boolean hilog(const HiLogContent *hilogContent, uint32 len)
{
    UNUSED(len);
    static char buf[256];
    int32 bytes = LogContentFmt(buf, sizeof(buf), (const uint8 *)hilogContent);
    _write(STDOUT_FILENO, buf, bytes);
    return TRUE;
}

__attribute__((noreturn)) void __assert_func(const char *file, int line, const char *func, const char *expr)
{
    printf("Assertion failed: %s (%s: %s: %d)\r\n", expr, file, func, line);
    fflush(NULL);
    abort();
}

STATIC VOID UserErrFuncImpl(CHAR *fileName, UINT32 lineNo, UINT32 errorNo, UINT32 paraLen, VOID *para)
{
    printf("ERROR: \"/%s\" <<< line: %x err: %x para[%u]: ", fileName, lineNo, errorNo, paraLen);

    const u8 *pc = (const u8 *)para;
    for (UINT32 i = 0; i < paraLen; ++i) {
        printf("%02x", pc[i]);
    }

    printf("\r\n");
}

/**
 * @brief Inner part of main function without disabled stack guard check
 * @return
 */
STATIC INT32 SafeMain(VOID)
{
    UINT32 ret;

    HardwareInit();
    UsartInit();

    printf("\r\n OHOS start \r\n");

    g_userErrFunc.pfnHook = UserErrFuncImpl;
    HiLogRegisterProc(hilog);

    ret = LOS_KernelInit();
    if (ret != LOS_OK) {
        printf("Liteos kernel init failed! ERROR: 0x%x\r\n", ret);
        goto START_FAILED;
    }

    if (DeviceManagerStart()) {
        printf("DeviceManagerStart failed!\r\n");
    }

    ret = LosAppInit();
    if (ret != LOS_OK) {
        printf("LosAppInit failed! ERROR: 0x%x\r\n", ret);
        goto START_FAILED;
    }

    B91SuspendSleepInit();
    LOS_Start();

START_FAILED:
    while (1) {
        __asm__ volatile("wfi");
    }

    return 0;
}

#pragma GCC push_options
#pragma GCC optimize("-fno-stack-protector")
INT32 main(VOID)
{
#ifdef __GNUC__
    ArchStackGuardInit();
#endif

    return SafeMain();
}
#pragma GCC pop_options
