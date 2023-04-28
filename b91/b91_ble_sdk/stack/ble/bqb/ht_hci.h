/********************************************************************************************************
 * @file	ht_hci.h
 *
 * @brief	This is the header file for BLE SDK
 *
 * @author	BLE GROUP
 * @date	2020.06
 *
 * @par     Copyright (c) 2020, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
 *
 *          Redistribution and use in source and binary forms, with or without
 *          modification, are permitted provided that the following conditions are met:
 *
 *              1. Redistributions of source code must retain the above copyright
 *              notice, this list of conditions and the following disclaimer.
 *
 *              2. Unless for usage inside a TELINK integrated circuit, redistributions
 *              in binary form must reproduce the above copyright notice, this list of
 *              conditions and the following disclaimer in the documentation and/or other
 *              materials provided with the distribution.
 *
 *              3. Neither the name of TELINK, nor the names of its contributors may be
 *              used to endorse or promote products derived from this software without
 *              specific prior written permission.
 *
 *              4. This software, with or without modification, must only be used with a
 *              TELINK integrated circuit. All other usages are subject to written permission
 *              from TELINK and different commercial license may apply.
 *
 *              5. Licensee shall be solely responsible for any claim to the extent arising out of or
 *              relating to such deletion(s), modification(s) or alteration(s).
 *
 *          THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *          ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *          WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *          DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
 *          DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *          (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *          LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *          ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *          (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *          SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************************************/
#ifndef STACK_BLE_BQB_BT_HCI_H_
#define STACK_BLE_BQB_BT_HCI_H_

#include "bqb.h"

#include "stack/ble/hci/hci.h"
#include "stack/ble/hci/hci_cmd.h"
#include "stack/ble/hci/hci_const.h"
#include "stack/ble/hci/hci_event.h"

typedef enum {
    HT_ERR_EVT_TIMEOUT_START = 0x80,
    HT_ERR_EVT_TIMEOUT_LE_CONNECTION_COMPLETE = HT_ERR_EVT_TIMEOUT_START + HCI_SUB_EVT_LE_CONNECTION_COMPLETE,
    HT_ERR_EVT_TIMEOUT_LE_ADVERTISING_REPORT = HT_ERR_EVT_TIMEOUT_START + HCI_SUB_EVT_LE_ADVERTISING_REPORT,
    HT_ERR_EVT_TIMEOUT_LE_CONNECTION_UPDATE_COMPLETE =
        HT_ERR_EVT_TIMEOUT_START + HCI_SUB_EVT_LE_CONNECTION_UPDATE_COMPLETE,
    HT_ERR_EVT_TIMEOUT_LE_READ_REMOTE_USED_FEATURES_COMPLETE =
        HT_ERR_EVT_TIMEOUT_START + HCI_SUB_EVT_LE_READ_REMOTE_USED_FEATURES_COMPLETE,

    HT_ERR_EVT_TIMEOUT_LE_DATA_LENGTH_CHANGE = HT_ERR_EVT_TIMEOUT_START + HCI_SUB_EVT_LE_DATA_LENGTH_CHANGE,
    HT_ERR_EVT_TIMEOUT_LE_PHY_UPDATE_COMPLETE = HT_ERR_EVT_TIMEOUT_START + HCI_SUB_EVT_LE_PHY_UPDATE_COMPLETE,

    HT_ERR_EVT_TIMEOUT_DISCONNECTION_COMPLETE = HT_ERR_EVT_TIMEOUT_START + HCI_SUB_EVT_MAX + 0,
    HT_ERR_EVT_TIMEOUT_NUM_OF_COMPLETE_PACKETS = HT_ERR_EVT_TIMEOUT_START + HCI_SUB_EVT_MAX + 1,

    HT_ERR_COMPLETE_EVT_TIMEOUT = 0xF0,
    HT_ERR_STATUS_EVT_TIMEOUT = 0xF1,
    HT_ERR_PARAM_LEN_ERR = 0xF2,
    HT_ERR_OPCODE_ERR = 0xF3,
    HT_ERR_UNEXPECT_STS = 0xF4,

    HOST_RX_FIFO_OVERFLOW = 0xF8,
    CONTROLLER_RX_FIFO_OVERFLOW = 0xF9,
} ht_err_t;

extern hci_fifo_t host_hciTxfifo;
extern hci_fifo_t host_hciRxfifo;

typedef int (*loop_func_t)(void);

void ht_hci_init(void);

int blc_register_host_loop_func(void *p);
int blc_register_controller_loop_func(void *p);

int host_process_wait_hci_event(void);

int host_hci_handler(u8 *p, int n);

ble_sts_t host_initHciTxFifo(u8 *pTxbuf, int fifo_size, int fifo_number);
ble_sts_t host_initHciRxFifo(u8 *pRxbuf, int fifo_size, int fifo_number);

void host_register_hci_handler(void *prx, void *ptx);
int ht_hci_send_data(u32 h, u8 *para, int n);

void host_testcase_function_register(char *testcase, callback_testcase_t func);
int exe_upper_tester_command(u8 *cmd);

void ht_reset_stack(void);

int ht_set_test_result(int err_flag);

#endif /* STACK_BLE_BQB_BT_HCI_H_ */
