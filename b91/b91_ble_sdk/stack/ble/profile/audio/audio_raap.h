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
#ifndef _AUDIO_RAAP_H_
#define _AUDIO_RAAP_H_

#if (BLC_AUDIO_RAAP_ENABLE)

int blc_audio_raapAttRead(u16 connHandle, void *p);
int blc_audio_raapAttWrite(u16 connHandle, void *p);

static int blt_audio_raapInit(blt_audio_handle_t *pHandle);
static int blt_audio_raapGattIn(blt_audio_handle_t *pHandle, u8 *pPkt);
static int blt_audio_raapSetServ(blt_audio_handle_t *pHandle, attribute_t *pService);
static void blt_audio_raapSetConn(blt_audio_handle_t *pHandle, BLC_AUDIO_STATE_ENUM state);
static void blt_audio_raapProcess(blt_audio_handle_t *pHandle);

#endif  // #if (BLC_AUDIO_RAAP_ENABLE)

#endif  // _AUDIO_RAAP_H_
