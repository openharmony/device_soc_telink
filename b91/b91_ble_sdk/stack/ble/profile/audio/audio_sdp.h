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
#ifndef _AUDIO_SDP_H_
#define _AUDIO_SDP_H_

#if (BLC_AUDIO_SDP_ENABLE)

typedef enum {
    BLT_AUDIO_SDP_FLAG_NONE = 0x00,
    BLT_AUDIO_SDP_FLAG_WAIT = 0x80,
    BLT_AUDIO_SDP_FLAG_SERACH_SERVICE = 0x01,
    BLT_AUDIO_SDP_FLAG_INIT_SERVICE = 0x02,
    BLT_AUDIO_SDP_FLAG_LOOP_SERVICE = 0x04,

    BLT_AUDIO_SDP_FLAG_DEFAULT = BLT_AUDIO_SDP_FLAG_SERACH_SERVICE | BLT_AUDIO_SDP_FLAG_INIT_SERVICE,
} BLT_AUDIO_SDP_FLAGS_ENUM;
typedef enum {
    BLT_AUDIO_SRV_FLAG_NONE = 0x00,
    BLT_AUDIO_SRV_FLAG_WAIT = 0x80,
    BLT_AUDIO_SRV_FLAG_AUTO = 0x40,  // Auto do: SERACH_INCLUDE,SERACH_CHARACT,FIND_CHARACT,AUTO_ENABLE_CCC
    BLT_AUDIO_SRV_FLAG_OTHR = 0x20,
    BLT_AUDIO_SRV_FLAG_SERACH_INCLUDE = 0x01,
    BLT_AUDIO_SRV_FLAG_SERACH_CHARACT = 0x02,
    BLT_AUDIO_SRV_FLAG_FIND_CHARACT = 0x04,
    BLT_AUDIO_SRV_FLAG_ENABLE_CCC = 0x08,

    BLT_AUDIO_SRV_FLAG_DEFAULT = BLT_AUDIO_SRV_FLAG_SERACH_CHARACT | BLT_AUDIO_SRV_FLAG_FIND_CHARACT,
} BLT_AUDIO_SRV_FLAG_ENUM;

typedef enum {
    BLT_AUDIO_CHAR_MODE_IDLE = 0x00,  // Fill Character if idle
    BLT_AUDIO_CHAR_MODE_UUID = 0x01,  // Find Character by charUUID
} BLT_AUDIO_CHAR_MODE_ENUM;

int blt_audio_sdpRegServ(blt_audio_sdp_t *pSdp, blt_audio_srv_t *pServ);
int blt_audio_sdpClrServ(blt_audio_sdp_t *pSdp);

bool blt_audio_sdpStart(blt_audio_sdp_t *pSdp, u16 connHandle);
void blt_audio_sdpStop(blt_audio_sdp_t *pSdp);

void blt_audio_sdpHandler(blt_audio_sdp_t *pSdp);
int blt_audio_sdpGattHandler(blt_audio_sdp_t *pSdp, u8 *pkt);

#endif  // #if (BLC_AUDIO_SDP_ENABLE)

#endif  // _AUDIO_SDP_H_
