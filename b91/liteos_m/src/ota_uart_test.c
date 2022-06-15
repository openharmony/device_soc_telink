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

#include <ctype.h>
#include <inttypes.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>

#include <los_task.h>

#include <ohos_types.h>
#include <hota_updater.h>

#include <app_sha256.h>

#include <B91/uart.h>
#include <B91/reg_include/uart_reg.h>

#include "serial.h"

#include "ota_uart_test.h"

#define OTA_UART_TASK_PRIO          7

#define SOH  0x01
#define STX  0x02
#define EOT  0x04
#define ACK  0x06
#define NAK  0x15
#define CAN  0x18
#define CTRLZ 0x1A

#define DLY_1S 1000
#define MAXRETRANS 25

typedef enum {
    COMMAND_TYPE_ERROR_TIMEOUT = -1,
    COMMAND_TYPE_ERROR_PREAMBLE = -2,
    COMMAND_TYPE_ERROR_WRONG_COMMAND = -3,
    COMMAND_TYPE_ERROR_WRONG_ARGS = -4,

    COMMAND_TYPE_UPLOAD = 0,
    COMMAND_TYPE_PRINT = 1,
    COMMAND_TYPE_CANCEL = 2,
    COMMAND_TYPE_ERASE = 3,
    COMMAND_TYPE_HASH_SHA256 = 4,
    COMMAND_TYPE_RESTART = 5,
    COMMAND_TYPE_ROLLBACK = 6,
} CommandType;

unsigned short crc16_ccitt( const unsigned char *buf, int len )
{
    unsigned short crc = 0;
    while( len-- ) {
        int i;
        crc ^= *(char *)buf++ << 8;
        for( i = 0; i < 8; ++i ) {
            if( crc & 0x8000 )
                crc = (crc << 1) ^ 0x1021;
            else
                crc = crc << 1;
        }
    }
    return crc;
}

static int check(bool is_crc, const unsigned char *buf, int sz)
{
    if (is_crc) {
        unsigned short crc = crc16_ccitt(buf, sz);
        unsigned short tcrc = (buf[sz]<<8)+buf[sz+1];
        if (crc == tcrc)
            return 1;
    }
    else {
        int i;
        unsigned char cks = 0;
        for (i = 0; i < sz; ++i) {
            cks += buf[i];
        }
        if (cks == buf[sz])
        return 1;
    }

    return 0;
}

static int _inbyte(unsigned short timeout) // msec timeout
{
    int c = uart_read(LOS_MS2Tick(timeout));
//    if (c > 0) {
//        printf("[>%02x]", c);
//    }
    return c;
}

STATIC int read_with_echo(UINT32 timeout_ms)
{
    int c = uart_read(LOS_MS2Tick(timeout_ms));
    if (c > 0) {
//        printf("[>%02x]", c);
        if (0x7f == c) {
            uart_send_byte(UART1, 0x08);
            uart_send_byte(UART1, ' ');
            c = 0x08;
        }
        uart_send_byte(UART1, (unsigned char)c);
    }
    return c;
}

static void _outbyte(int c)
{
//    printf("[<%02x]", c);
    uart_send_byte(UART1, (unsigned char)c);
}

static void flushinput(void)
{
    while (_inbyte(((DLY_1S)*3)>>1) >= 0) {
    }
}

typedef void (*XModemReceiveCB)(uint8_t *buf, size_t addr, size_t len, void *arg);

int xmodemReceive(unsigned char *xbuff, int destsz, XModemReceiveCB cb, void *arg)
{
    unsigned char *p;
    int bufsz, crc = 0;
    unsigned char trychar = 'C';
    unsigned char packetno = 1;
    int c, len = 0;
    int retry, retrans = MAXRETRANS;

    for(;;) {
        for( retry = 0; retry < 16; ++retry) {
            if (trychar) _outbyte(trychar);
            if ((c = _inbyte((DLY_1S)<<1)) >= 0) {
                switch (c) {
                case SOH:
                    bufsz = 128;
                    goto start_recv;
                case STX:
                    bufsz = 1024;
                    goto start_recv;
                case EOT:
                    flushinput();
                    _outbyte(ACK);
                    return len; /* normal end */
                case CAN:
                    if ((c = _inbyte(DLY_1S)) == CAN) {
                        flushinput();
                        _outbyte(ACK);
                        return -1; /* canceled by remote */
                    }
                    break;
                default:
                    break;
                }
            }
        }
        if (trychar == 'C') { trychar = NAK; continue; }
        flushinput();
        _outbyte(CAN);
        _outbyte(CAN);
        _outbyte(CAN);
        return -2; /* sync error */

    start_recv:
        if (trychar == 'C') crc = 1;
        trychar = 0;
        p = xbuff;
        *p++ = c;

        INT32 to_read = (bufsz+(crc?1:0)+3);
        INT32 nread = uart_read_buf(LOS_MS2Tick(DLY_1S*10), p, to_read);
        if (nread < to_read) {
            printf(" === %s:%d to_read: %d nread: %d\r\n", __func__, __LINE__, to_read, nread);
            goto reject;
        }

        if (xbuff[1] == (unsigned char)(~xbuff[2]) &&
            (xbuff[1] == packetno || xbuff[1] == (unsigned char)packetno-1) &&
            check(crc, &xbuff[3], bufsz)) {
            if (xbuff[1] == packetno)	{
                int count = destsz - len;
                if (count > bufsz) count = bufsz;
                if (count > 0) {
                    cb(&xbuff[3], len, count, arg);
                    len += count;
                }
                ++packetno;
                retrans = MAXRETRANS+1;
            }
            if (--retrans <= 0) {
                flushinput();
                _outbyte(CAN);
                _outbyte(CAN);
                _outbyte(CAN);
                return -3; /* too many retry error */
            }
            _outbyte(ACK);
            continue;
        }
    reject:
        flushinput();
        _outbyte(NAK);
    }
}

STATIC CommandType CommandProcess(UINT32 *arg1, UINT32 *arg2)
{
    int c;
    if ((c = read_with_echo(LOS_WAIT_FOREVER)) < 0) {
        return COMMAND_TYPE_ERROR_TIMEOUT;
    }
    if ('{' != c) {
        return COMMAND_TYPE_ERROR_PREAMBLE;
    }

    if ((c = read_with_echo(5000)) < 0) {
        return COMMAND_TYPE_ERROR_TIMEOUT;
    }

    CommandType cmd;

    switch (c) {
        case 'u': {
            cmd = COMMAND_TYPE_UPLOAD;
            break;
        }
        case 'p': {
            cmd = COMMAND_TYPE_PRINT;
            break;
        }
        case 'c': {
            cmd = COMMAND_TYPE_CANCEL;
            break;
        }
        case 'h': {
            cmd = COMMAND_TYPE_HASH_SHA256;
            break;
        }
        case 'r': {
            cmd = COMMAND_TYPE_RESTART;
            break;
        }
        case 'b': {
            cmd = COMMAND_TYPE_ROLLBACK;
            break;
        }
        default: {
            return COMMAND_TYPE_ERROR_WRONG_COMMAND;
        }
    }

    char buf[32] = {0};
    size_t i = 0;

    while (1) {
        if ((c = read_with_echo(5000)) < 0) {
            return COMMAND_TYPE_ERROR_TIMEOUT;
        }

        if ('\r' == c) {
            continue;
        }
        if ('\n' == c) {
            break;
        }
        if ((0x08 == c)) {
            if (i > 0) {
                buf[--i] = '\0';
            }
            continue;
        }
        buf[i++] = c;
    }

    *arg1 = UINT32_MAX;
    *arg2 = UINT32_MAX;

    if ((COMMAND_TYPE_UPLOAD == cmd) || (COMMAND_TYPE_PRINT == cmd) || (COMMAND_TYPE_HASH_SHA256 == cmd)) {
        char *p;
        errno = ENOERR;
        *arg1 = strtoul(buf, &p, 10);
        if (((0 == *arg1) || (ULONG_MAX ==  *arg1)) && (ENOERR != errno)) {
            printf(" === %s:%d errno: %d\r\n", __func__, __LINE__, errno);
            return COMMAND_TYPE_ERROR_WRONG_ARGS;
        }
        *arg2 = strtoul(p, &p, 10);
        if (((0 == *arg2) || (ULONG_MAX ==  *arg2)) && (ENOERR != errno)) {
            printf(" === %s:%d errno: %d\r\n", __func__, __LINE__, errno);
            *arg2 = UINT32_MAX;
        }
    } else if (COMMAND_TYPE_CANCEL == cmd) {
        if (0 != strcmp("ancel", buf)) {
            return COMMAND_TYPE_ERROR_WRONG_COMMAND;
        }
    } else if (COMMAND_TYPE_RESTART == cmd) {
        if (0 != strcmp("estart", buf)) {
            return COMMAND_TYPE_ERROR_WRONG_COMMAND;
        }
    } else if (COMMAND_TYPE_ROLLBACK == cmd) {
        if (0 != strcmp("ack", buf)) {
            return COMMAND_TYPE_ERROR_WRONG_COMMAND;
        }
    }

    return cmd;
}

void OtaErrorCallBack(HotaErrorCode errorCode)
{
    printf(" === %s:%d errorCode: %d\r\n", __func__, __LINE__, errorCode);
}

void OtaStatusCallBack(HotaStatus status)
{
    printf(" === %s:%d status: %d\r\n", __func__, __LINE__, status);
}

#define BUF_SIZE 1030

typedef struct {
    int sz, start;

    AppSha256Context sha256;
} xmodemReceiveCBArg;

void xmodemReceiveCB(uint8_t *buf, size_t addr, size_t len, void *_arg)
{
    xmodemReceiveCBArg *arg = _arg;
//    int to_print = arg->sz < res ? arg->sz : res;
    int to_print = len;
    if (to_print > 0) {
        printf("Received[%d %d %u %d]: \"", arg->sz, arg->start, addr, to_print);
//        printf("%.*s", to_print, buf);
//            for (size_t i=0; i < to_print; ++i) {
//                printf("%02x", buf[i]);
//            }
        printf("\"\r\n");
        AppSha256Update(&arg->sha256, buf, len);
        unsigned int offset = (unsigned)(arg->start > 0 ? arg->start : 0) + addr;
        int ret = HotaWrite(buf, offset, len);
        //printf("HotaWrite() =  %d\r\n", ret);
    }
}

STATIC VOID OTA_TestThread(VOID)
{
    printf(" === %s:%d\r\n", __func__, __LINE__);

    SerialInit();

    unsigned char *buf = malloc(BUF_SIZE);

    while (1) {
        int sz, sz2;
        CommandType cmd = CommandProcess((UINT32 *)&sz, (UINT32 *)&sz2);
        printf(" === %s:%d cmd: %d sz: %d sz2: %d\r\n", __func__, __LINE__, cmd, sz, sz2);

        int res = 0;
        if (COMMAND_TYPE_UPLOAD == cmd) {
            xmodemReceiveCBArg arg = {sz, sz2, {0}};
            AppSha256Init(&arg.sha256);
            res = xmodemReceive(buf, sz, xmodemReceiveCB, &arg);
            uint8_t hash[32];
            AppSha256Finish(&arg.sha256, hash);
            printf("sha256: ");
            for (size_t i=0; i<sizeof(hash); ++i) {
                printf("%02x", hash[i]);
            }
            printf("\r\n");
        } else if (COMMAND_TYPE_PRINT == cmd) {
            unsigned start = sz; // & ~0xFF; //(sz / 256) * 256;
            unsigned bufLen = (unsigned)(sz2 >= 0 ? sz2 : 256);

            for (unsigned offset = 0, next = 1024; next < bufLen; offset = next, next += 1024) {
                if (OHOS_SUCCESS == HotaRead(start+offset, 1024, buf)) {}
                printf("Read[%u %u %u %u]: {", start, start+offset, 1024u, bufLen);
                for (size_t i=0; i<1024; ++i) {
                     printf("%02x", buf[i]);
                }
                printf("}\r\n");
            }

            unsigned rem = bufLen % 1024;

            if (0 != rem) {
                if (OHOS_SUCCESS == HotaRead(start+bufLen-rem, rem, buf)) {}
                printf("Read[%u %u %u %u]: {", start, start+bufLen-rem, rem, bufLen);
                for (size_t i=0; i<rem; ++i) {
                     printf("%02x", buf[i]);
                }
                printf("}\r\n");
            }
        } else if (COMMAND_TYPE_HASH_SHA256 == cmd) {
            unsigned start = sz; // & ~0xFF; //(sz / 256) * 256;
            unsigned bufLen = (unsigned)(sz2 >= 0 ? sz2 : 256);

            AppSha256Context sha256;
            AppSha256Init(&sha256);

            for (unsigned offset = 0, next = 1024; next < bufLen; offset = next, next += 1024) {
                if (OHOS_SUCCESS == HotaRead(start+offset, 1024, buf)) {}
                AppSha256Update(&sha256, buf, 1024);
            }

            unsigned rem = bufLen % 1024;

            if (0 != rem) {
                if (OHOS_SUCCESS == HotaRead(start+bufLen-rem, rem, buf)) {}
                AppSha256Update(&sha256, buf, rem);
            }

            uint8_t hash[32];
            AppSha256Finish(&sha256, hash);
            printf("sha256: ");
            for (size_t i=0; i<sizeof(hash); ++i) {
                printf("%02x", hash[i]);
            }
            printf("\r\n");
        } else if (COMMAND_TYPE_CANCEL == cmd) {
            HotaCancel();
            (VOID)HotaInit(OtaErrorCallBack, OtaStatusCallBack);
            (VOID)HotaSetPackageType(NOT_USE_DEFAULT_PKG);
        } else if (COMMAND_TYPE_RESTART == cmd) {
            int ret = HotaRestart();
            printf("HotaRestart() =  %d\r\n", ret);
        } else if (COMMAND_TYPE_ROLLBACK == cmd) {
            int HotaHalRollback(void);
            int ret = HotaHalRollback();
            printf("HotaHalRollback() =  %d\r\n", ret);
        }
        printf(" === %s:%d res: %d\r\n", __func__, __LINE__, res);
    }

    free(buf);
}

VOID OtaUartTestInit(VOID)
{
    (VOID)HotaInit(OtaErrorCallBack, OtaStatusCallBack);
    (VOID)HotaSetPackageType(NOT_USE_DEFAULT_PKG);

    UINT32 ret = LOS_OK;

    unsigned int task_id;
    TSK_INIT_PARAM_S task_param = { 0 };

    task_param.pfnTaskEntry = (TSK_ENTRY_FUNC)OTA_TestThread;
    task_param.uwStackSize  = 1024 * 16;
    task_param.pcName = "OTA_TestThread";
    task_param.usTaskPrio   = OTA_UART_TASK_PRIO;
    ret = LOS_TaskCreate(&task_id, &task_param);
    if (ret != LOS_OK) {
        printf("Create Task failed! ERROR: 0x%x\r\n", ret);
    }
}
