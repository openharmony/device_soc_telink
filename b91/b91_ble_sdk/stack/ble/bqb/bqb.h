/********************************************************************************************************
 * @file	bqb.h
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

#ifndef STACK_BLE_BQB_BQB_H_
#define STACK_BLE_BQB_BQB_H_

typedef int (*callback_testcase_t)(int, void *);

typedef enum {
    TP_STATUS_ACL_CONN_PKT_MAX = 0,
    TP_STATUS_CIS_CONN_PKT_MAX,

    TP_STATUS_CONN_UNKNOWN_RSP,

    TP_STATUS_CONNECTION_DATA_SEND,

    TP_STATUS_ADV_PKT_MAX,
    TP_STATUS_CONN_ESTABLISH,
    TP_STATUS_CONN_TERMINATE,

    TP_STATUS_CONN_PARA_REQ,
    TP_STATUS_CONN_PARA_UPDATE,

    TP_STATUS_CONNECTION_TIMEOUT,
    TP_STATUS_CONNE_RCVD_L2CAP_DATA,

    TP_STATUS_CONN_MAP_REQ,
    TP_STATUS_CONN_MAP_UPDATE,
    TP_STATUS_CONN_DATA_LENGTH_EXCHANGE,

    TP_STATUS_CONN_BRX_MISS_END,

    TP_STATUS_L2CAP_DATA_IN_IRQ_RX,

    TP_STATUS_TERMINATE_IN_IRQ_RX,
    TP_STATUS_BRX_START,
    TP_STATUS_RECV_CERT_DATA,
    TP_STATUS_PUSH_DATA_CRC_ERR,
    TP_STATUS_PUSH_DATA_SN_ERR,
    TP_STATUS_CONN_IN_IRQ_RX,
    TP_STATUS_BRX_POST,
    TP_STATUS_BRX_TX,

    TP_STATUS_SCAN_HIT_MAX,
    TP_STATUS_SCAN_RCVD_RSP,
    TP_STATUS_SCAN_REQ_SENT,
    TP_STATUS_CONN_HIT_MAX,

    TP_STATUS_CONN_RCVD_DAT,
    TP_STATUS_CONN_REQ_SENT,
    TP_STATUS_TIMEOUT,
    TP_STATUS_CONNECTION_LOST,
    TP_STATUS_CONN_PKT_MISSING,
    TP_STATUS_CONNECTION_RETX_MAX,

    TP_STATUS_RCVD_NO_EMPTY_PKT,
    TP_STATUS_CHN_MAP_FULL_USED,

    TP_STATUS_CONN_PHY_UPDATE,

    TP_STATUS_CONNECTION_FEATURE_REQ,

    TP_STATUS_CONN_BTX_MIDDLE,

    TP_STATUS_HCI_EVT_DISCONNECT = 0x80,
    TP_STATUS_HCI_EVT_CONNECTION_COMPLETE,
    TP_EVT_NUM_OF_COMPLETE_PACKETS,
    TP_STATUS_HCI_EVT_LE_ADV_REPORT,
    TP_STATUS_HCI_EVT_LE_CONNECTION_UPDATE_COMPLETE,
    TP_STATUS_HCI_EVT_LE_READ_REMOTE_USED_FEATURES_COMPLET,
    TP_STATUS_HCI_EVT_LE_LONG_TERM_KEY_REQUESTED,
    TP_STATUS_HCI_EVT_LE_REMOTE_CONNECTION_PARAM_REQUEST,
    TP_STATUS_HCI_EVT_LE_DATA_LENGTH_CHANGE,
    TP_STATUS_HCI_EVT_LE_READ_LOCAL_P256_KEY_COMPLETE,
    TP_STATUS_HCI_EVT_LE_GENERATE_DHKEY_COMPLETE,
    TP_STATUS_HCI_EVT_LE_ENHANCED_CONNECTION_COMPLETE,
    // ...
    TP_STATUS_HCI_EVT_LE_PHY_UPDATE_COMPLETE,

    TP_STATUS_HCI_EVT_LE_EXTENDED_ADVERTISING_REPORT,
    TP_STATUS_HCI_EVT_LE_ADVERTISING_SET_TERMINATED,

    TP_STATUS_HCI_EVT_LE_CIS_ESTABLISHED,
    TP_STATUS_HCI_EVT_LE_CIS_REQUESTED,
    TP_STATUS_HCI_EVT_LE_CREATE_BIG_COMPLETE,
    TP_STATUS_HCI_EVT_LE_BIG_SYNC_ESTABLILSHED,
    TP_STATUS_HCI_EVT_LE_BIG_SYNC_LOST,
    TP_EVT_UNKNOWN_PACKETS,

    TP_STATUS_INIT = 0x8000,
} tp_status_t;

#endif /* STACK_BLE_BQB_BQB_H_ */
