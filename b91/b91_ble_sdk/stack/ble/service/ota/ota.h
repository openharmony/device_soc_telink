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
#ifndef OTA_H_
#define OTA_H_

#include <types.h>

/**
 * @brief 	Legacy OTA command
 */
#define CMD_OTA_VERSION 0xFF00  // client -> server
#define CMD_OTA_START   0xFF01  // client -> server
#define CMD_OTA_END     0xFF02  // client -> server

/**
 * @brief 	Extended OTA command, optional
 */
#define CMD_OTA_START_EXT      0xFF03  // client -> server
#define CMD_OTA_FW_VERSION_REQ 0xFF04  // client -> server
#define CMD_OTA_FW_VERSION_RSP 0xFF05  // server -> client
#define CMD_OTA_RESULT         0xFF06  // server -> client

/**
 * @brief 	Multiple boot address enumeration
 */
typedef enum {
    MULTI_BOOT_ADDR_0x20000 = 0x20000,  // 128 K
    MULTI_BOOT_ADDR_0x40000 = 0x40000,  // 256 K
    MULTI_BOOT_ADDR_0x80000 = 0x80000,  // 512 K
} multi_boot_addr_e;

/**
 * @brief	OTA result
 */
enum {
    // 0x00
    OTA_SUCCESS = 0,          // success
    OTA_DATA_PACKET_SEQ_ERR,  // OTA data packet sequence number error: repeated OTA PDU or lost some OTA PDU
    OTA_PACKET_INVALID,       // invalid OTA packet:
                              // 1. invalid OTA command
                              // 2. addr_index out of range
                              // 3.not standard OTA PDU length
    OTA_DATA_CRC_ERR,         // packet PDU CRC err

    // 0x04
    OTA_WRITE_FLASH_ERR,  // write OTA data to flash ERR
    OTA_DATA_UNCOMPLETE,  // lost last one or more OTA PDU
    OTA_FLOW_ERR,         // peer device send OTA command or OTA data not in correct flow
    OTA_FW_CHECK_ERR,     // firmware CRC check error

    // 0x08
    OTA_VERSION_COMPARE_ERR,  // the version number to be update is lower than the current version
    OTA_PDU_LEN_ERR,  // PDU length error: not 16*n, or not equal to the value it declare in "CMD_OTA_START_EXT" packet
    OTA_FIRMWARE_MARK_ERR,  // firmware mark error: not generated by telink's BLE SDK
    OTA_FW_SIZE_ERR,        // firmware size error: no firmware_size; firmware size too small or too big

    // 0x0C
    OTA_DATA_PACKET_TIMEOUT,  // time interval between two consequent packet exceed a value(user can adjust this value)
    OTA_TIMEOUT,              // OTA flow total timeout
    OTA_FAIL_DUE_TO_CONNECTION_TERMIANTE,  // OTA fail due to current connection terminate
                                           // (maybe connection timeout or local/peer device terminate connection)
};

/**
 *  @brief data structure of OTA command "CMD_OTA_START"
 */
typedef struct {
    u16 ota_cmd;
} ota_start_t;

/**
 *  @brief data structure of OTA command "CMD_OTA_START_EXT"
 */
typedef struct {
    u16 ota_cmd;
    u8 pdu_length;       // must be: 16*n(n is in range of 1 ~ 15); pdu_length: 16,32,48,...240
    u8 version_compare;  // 0: no version compare; 1: only higher version can replace lower version
} ota_startExt_t;

/**
 *  @brief data structure of OTA command "CMD_OTA_END"
 */
typedef struct {
    u16 ota_cmd;
    u16 adr_index_max;
    u16 adr_index_max_xor;
} ota_end_t;

/**
 *  @brief data structure of OTA command "CMD_OTA_RESULT"
 */
typedef struct {
    u16 ota_cmd;
    u8 result;
} ota_result_t;

/**
 *  @brief data structure of OTA command "CMD_OTA_FW_VERSION_REQ"
 */
typedef struct {
    u16 ota_cmd;
    u16 version_num;
    u8 version_compare;  // 1: only higher version can replace lower version
} ota_versionReq_t;

/**
 *  @brief data structure of OTA command "CMD_OTA_FW_VERSION_RSP"
 */
typedef struct {
    u16 ota_cmd;
    u16 version_num;
    u8 version_accept;  // 1: accept firmware update; 0: reject firmware update
                        // (version compare enable, and compare result: fail)
} ota_versionRsp_t;

typedef struct {
    u16 adr_index;
    u8 data[16];
    u16 crc_16;
} ota_pdu16_t;

unsigned long crc32_half_cal(unsigned long crc, unsigned char *input, unsigned long *table, int len);
unsigned long crc32_cal(unsigned long crc, unsigned char *input, unsigned long *table, int len);

#endif /* OTA_H_ */
