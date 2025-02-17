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
#pragma once

#include "stack/ble/ble_format.h"

#define AES_BLOCK_SIZE 16

enum {
    AES_SUCC = SUCCESS,
    AES_NO_BUF,
    AES_FAIL,
};

typedef struct {
    u32 pkt;
    u8 dir;
    u8 iv[8];
} ble_cyrpt_nonce_t;

typedef struct {
    u64 enc_pno;
    u64 dec_pno;
    u8 sk[16];  // session key
    ble_cyrpt_nonce_t nonce;
    u8 st;
    u8 enable;  // 1: slave enable; 2: master enable
    u8 mic_fail;
} ble_crypt_para_t;

struct CCM_FLAGS_TAG {
    union {
        struct {
            u8 L : 3;
            u8 M : 3;
            u8 aData : 1;
            u8 reserved : 1;
        } bf;
        u8 val;
    };
};

typedef struct CCM_FLAGS_TAG ccm_flags_t;

typedef struct {
    union {
        u8 A[AES_BLOCK_SIZE];
        u8 B[AES_BLOCK_SIZE];
    } bf;

    u8 tmpResult[AES_BLOCK_SIZE];
    u8 newAstr[AES_BLOCK_SIZE];
} aes_enc_t;

enum {
    CRYPT_NONCE_TYPE_ACL = 0,
    CRYPT_NONCE_TYPE_CIS = 1,
    CRYPT_NONCE_TYPE_BIS = 2,
};

/**
 * @brief   	this function is used to encrypt the plaintext
 * @param[in]	*key - aes key: 128 bit key for the encryption of the data, little--endian.
 * @param[in]	*plaintext - 128 bit data block that is requested to be encrypted, little--endian.
 * @param[out]	*result - 128 bit encrypted data block, little--endian.
 * @return  	none.
 * @Note		Input data requires strict Word alignment
 */
void aes_ll_encryption(u8 *key, u8 *plaintext, u8 *encrypted_data);

/**
 * @brief   	this function is used to initialize the aes_ccm initial value
 * @param[in]   ltk - encryption key, LTK
 * @param[in]   skdm -
 * @param[in]   skds -
 * @param[in]   ivm -
 * @param[in]   ivs -
 * @param[in]   pd - Reference structure ble_crypt_para_t
 * @return  	none
 */
void aes_ll_ccm_encryption_init(u8 *ltk, u8 *skdm, u8 *skds, u8 *ivm, u8 *ivs, ble_crypt_para_t *pd);

/**
 * @brief   	this function is used to encrypt the aes_ccm value, version2
 * @param[in]   pd - Reference structure leCryptCtrl_t
 * @return  	none
 */
void aes_ll_ccm_encryption(llPhysChnPdu_t *pllPhysChnPdu, u8 role, u8 ll_type, ble_crypt_para_t *pd);

/**
 * @brief   	this function is used to decrypt the aes_ccm value, version2
 * @param[in]   pd - Reference structure leCryptCtrl_t
 * @return  	0: decryption succeeded; 1: decryption failed
 */
int aes_ll_ccm_decryption(llPhysChnPdu_t *pllPhysChnPdu, u8 role, u8 ll_type, ble_crypt_para_t *pd);
