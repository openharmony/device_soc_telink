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
#ifndef _AUDIO_OTP_H_
#define _AUDIO_OTP_H_

#if (BLC_AUDIO_OTP_ENABLE)

typedef enum {
    BLT_AUDIO_OTP_FLAG_NONE = 0x0000,

    BLT_AUDIO_OTP_FLAG_OACP_IND = 0x0001,
    BLT_AUDIO_OTP_FLAG_OLCP_IND = 0x0002,
    BLT_AUDIO_OTP_FLAG_OBJECT_CHANGED = 0x0004,
} BLT_AUDIO_OTP_FLAGS_ENUM;

typedef enum {
    BLT_AUDIO_OTP_ERRCODE_WRITE_REJECTED = 0x80,  // Write Request Rejected: An attempt was made to write a value
    // that is invalid or not supported by this Server for a reason other than the attribute permissions.
    BLT_AUDIO_OTP_ERRCODE_OBJECT_NOTSEL = 0x81,  // Object Not Selected: An attempt was made to read or write to
    // an Object Metadata characteristic while the Current Object was an Invalid Object
    BLT_AUDIO_OTP_ERRCODE_LIMIT_EXCEEDED = 0x82,  // Concurrency Limit Exceeded: The Server is unable to service
    // the Read Request or Write Request because it exceeds the concurrency limit of the service.
    BLT_AUDIO_OTP_ERRCODE_NAME_EXISTS =
        0x83,  // Object Name Already Exists: The requested object name was rejected because
               // the name was already in use by an existing object on the Server.
} BLT_AUDIO_OTP_ERRCODE_ENUM;

typedef enum {
    BLT_AUDIO_OTP_OACP_SUCCESS = 0x01,                       // Response for successful operation.
    BLT_AUDIO_OTP_OACP_ERRCODE_OPCODE_NOT_SUPPORTED = 0x02,  // Response if unsupported Op Code is received
    BLT_AUDIO_OTP_OACP_ERRCODE_INVALID_PARAMETER =
        0x03,  // Response if Parameter received does not meet the requirements of the service.
    BLT_AUDIO_OTP_OACP_ERRCODE_INSUFFICIENT_RESOURCES = 0x04,  //
    BLT_AUDIO_OTP_OACP_ERRCODE_INVALID_OBJECT =
        0x05,  // Response if the requested OACP procedure cannot be performed because
               // the Current Object is an Invalid Object.
    BLT_AUDIO_OTP_OACP_ERRCODE_CHANNEL_UNAVAILABLE =
        0x06,  // Response if the requested procedure could not be performed because
               // an Object Transfer Channel was not available for use.
    BLT_AUDIO_OTP_OACP_ERRCODE_UNSUPPORTED_TYPE =
        0x07,  // Response if the object type specified in the OACP procedure Type parameter
               // is not supported by the Server.
    BLT_AUDIO_OTP_OACP_ERRCODE_PROCEDURE_NOT_PERMITTED =
        0x08,  // Response if the requested procedure is not permitted according to the properties
               // of the Current Object
    BLT_AUDIO_OTP_OACP_ERRCODE_OBJECT_LOCKED =
        0x09,  // Response if the Current Object is temporarily locked by the Server.
    BLT_AUDIO_OTP_OACP_ERRCODE_OPERATION_FAILED =
        0x0A,  // Response if the requested procedure failed for any reason other than those enumerated in this table.
} BLT_AUDIO_OTP_OACP_ERRCODE_ENUM;
typedef enum {
    BLT_AUDIO_OTP_OLCP_SUCCESS = 0x01,                       // Response for successful operation.
    BLT_AUDIO_OTP_OLCP_ERRCODE_OPCODE_NOT_SUPPORTED = 0x02,  // Response if unsupported Op Code is received.
    BLT_AUDIO_OTP_OLCP_ERRCODE_INVALID_PARAMETER =
        0x03,  // Response if Parameter received does not meet the requirements of the service.
    BLT_AUDIO_OTP_OLCP_ERRCODE_OPERATION_FAILED =
        0x04,  // Response if the requested procedure failed for a reason other than those enumerated below.
    BLT_AUDIO_OTP_OLCP_ERRCODE_OUT_OF_BOUNDS =
        0x05,  //  Response if the requested procedure attempted to select an object beyond
               // the first object or beyond the last object in the current list.
    BLT_AUDIO_OTP_OLCP_ERRCODE_TOO_MANY_OBJECS =
        0x06,  // Response if the requested procedure failed due to too many objects in the current list.
    BLT_AUDIO_OTP_OLCP_ERRCODE_NO_OBJECT =
        0x07,  // Response if the requested procedure failed due to there being zero objects in the current list.
    BLT_AUDIO_OTP_OLCP_ERRCODE_OBJID_NOT_FOUND =
        0x08,  // Response if the requested procedure failed due to there being no object with the requested Object ID.
} BLT_AUDIO_OTP_OLCP_ERRCODE_ENUM;

int blc_audio_otpAttRead(u16 connHandle, void *p);
int blc_audio_otpAttWrite(u16 connHandle, void *p);

int blc_audio_otpSetObjectName(u16 connHandle, u8 *pName, u16 nameLen);
int blc_audio_otpSendActionCtrl(u16 connHandle, u8 opcode, u8 *pData, u8 dataLen);
int blc_audio_otpSendListCtrl(u16 connHandle, u8 opcode, u8 *pData, u8 dataLen);

static int blt_audio_otpInit(blt_audio_handle_t *pHandle);
static int blt_audio_otpGattIn(blt_audio_handle_t *pHandle, u8 *pPkt);
static int blt_audio_otpSetServ(blt_audio_handle_t *pHandle, attribute_t *pService);
static void blt_audio_otpSetConn(blt_audio_handle_t *pHandle, BLC_AUDIO_STATE_ENUM state);
static void blt_audio_otpProcess(blt_audio_handle_t *pHandle);

#endif  // #if (BLC_AUDIO_OTP_ENABLE)

#endif  // _AUDIO_OTP_H_
