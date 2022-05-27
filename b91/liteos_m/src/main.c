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

#include <unistd.h>
#include <fcntl.h>
#include <sys/mount.h>
#include <sys/stat.h>

#include <los_task.h>

#include <devmgr_service_start.h>
#include <gpio_if.h>
#include <hiview_log.h>
#include <hiview_output_log.h>
#include <utils_file.h>

#include <board_config.h>

#include <b91_irq.h>
#include <system_b91.h>

#include <B91/clock.h>
#include <B91/gpio.h>
#include <B91/uart.h>

#include <B91/gpio_default.h>

#include <../vendor/common/blt_common.h>

#define DEBUG_UART_PORT        UART0
#define DEBUG_UART_PIN_TX      UART0_TX_PB2
#define DEBUG_UART_PIN_RX      UART0_RX_PB3
#define DEBUG_UART_PARITY      UART_PARITY_NONE
#define DEBUG_UART_STOP_BITS   UART_STOP_BIT_ONE
#define DEBUG_UART_BAUDRATE    921600

#define B91_SYSTEM_INIT_TASK_STACKSIZE     (1024 * 32)
#define B91_SYSTEM_INIT_TASK_PRIO          7
#define B91_SYSTEM_INIT_TASK_NAME          "B91SystemInit"

extern UserErrFunc g_userErrFunc;

void OHOS_SystemInit(void);
struct lfs_config *LittlefsConfigGet(void);

VOID HardwareInit(VOID)
{
    SystemInit();
}

STATIC VOID LittlefsInit(VOID)
{
#define DIR_DATA         "/data"
#define PAR_DATA         0

    int res;

    printf("LittleFS_Init\r\n");

    struct lfs_config *cfg = LittlefsConfigGet();

    res = mount(PAR_DATA , DIR_DATA, "littlefs", 0, cfg);
    printf("mount = %d\r\n", res);

    res = mkdir(DIR_DATA, 0777);
    printf("mkdir = %d\r\n", res);
}

STATIC VOID LittlefsTest(VOID)
{
#define DIR_TEST         "/data"
#define PAR_TEST         0
#define TEST_FILE        "/data/rw.sys.version"

    int res;
    ssize_t fs_ret;

    printf("Littlefs Driver Test\n");
    struct lfs_config *cfg = LittlefsConfigGet();
    res = mount(PAR_TEST , DIR_TEST, "littlefs", 0, cfg);
    printf("mount = %d\n", res);

    res = mkdir(DIR_TEST, 0777);
    printf("mkdir = %d\n", res);

    uint32_t count = 0;
    int fd = open(TEST_FILE, O_RDWR | O_CREAT);
    printf("open = %d\n", fd);

    fs_ret = read(fd, &count, sizeof(count));
    printf("read = %ld\n", fs_ret);
    printf("count = %lu\n", count);
    count += 1;

    fs_ret = lseek(fd, 0, SEEK_SET);
    printf("lseek = %ld\n", fs_ret);

    fs_ret = write(fd, &count, sizeof(count));
    printf("write = %ld\n", fs_ret);

    close(fd);
    printf("close\n");
}

VOID IoTWatchDogKick(VOID)
{
}

STATIC VOID HdfGpioDriverTest(VOID)
{
    int32_t ret;

    ret = GpioSetDir(LED_BLUE_HDF, GPIO_DIR_OUT);
    printf("GpioSetDir = %d\r\n", ret);

    while(1) {
        GpioWrite(LED_BLUE_HDF, GPIO_VAL_LOW);
        LOS_TaskDelay(500);

        GpioWrite(LED_BLUE_HDF, GPIO_VAL_HIGH);
        LOS_TaskDelay(500);
    }
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
    TSK_INIT_PARAM_S task_ohos = { 0 };

    task_ohos.pfnTaskEntry = (TSK_ENTRY_FUNC)B91SystemInit;
    task_ohos.uwStackSize  = B91_SYSTEM_INIT_TASK_STACKSIZE;
    task_ohos.pcName       = B91_SYSTEM_INIT_TASK_NAME;
    task_ohos.usTaskPrio   = B91_SYSTEM_INIT_TASK_PRIO;
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
    uart_cal_div_and_bwpc(DEBUG_UART_BAUDRATE, sys_clk.pclk * 1000 * 1000, &div, &bwpc);
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

__attribute__((noreturn))
void __assert_func(const char *file, int line, const char *func, const char *expr)
{
       printf("Assertion failed: %s (%s: %s: %d)\r\n", expr, file, func, line);
       fflush(NULL);
       abort();
}

STATIC VOID UserErrFuncImpl(CHAR *fileName, UINT32 lineNo, UINT32 errorNo,
                            UINT32 paraLen, VOID *para)
{
    printf("ERROR: \"/%s\" <<< line: %x err: %x para[%u]: ", fileName, lineNo, errorNo, paraLen);

    const u8 *pc = (const u8 *)para;
    for (UINT32 i=0; i<paraLen; ++i) {
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

    LOS_Start();

START_FAILED:
    while (1) {
        __asm__ volatile("wfi");
    }

    return 0;
}

LITE_OS_SEC_TEXT_INIT VOID ArchStackGuardInit(VOID);

#pragma GCC push_options
#pragma GCC optimize ("-fno-stack-protector")
INT32 main(VOID)
{
#ifdef __GNUC__
    ArchStackGuardInit();
#endif

    return SafeMain();
}
#pragma GCC pop_options
