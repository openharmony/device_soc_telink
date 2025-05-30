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
#ifndef BIS_BCST_H_
#define BIS_BCST_H_

// Note: user can't modify this value,and this value must 4 byte aligned
#define BIG_BCST_PARAM_LENGTH (672)  // 4B aligned

/**
 * @brief      This function is used to initialize BIG broadcast module.
 * @param	   none
 * @return     none
 */
void blc_ll_initBigBroadcast_module(void);

/**
 * @brief
 * @param	   none
 * @return     none
 */
ble_sts_t blc_ll_initBigBcstParameters(u8 *pBigBcstPara, u8 bigBcstNum);

/**
 * @brief
 * @param	   none
 * @return     none
 */
ble_sts_t blc_hci_le_createBigParams(hci_le_createBigParams_t *pCmdParam);

/**
 * @brief
 * @param	   none
 * @return     none
 */
ble_sts_t blc_hci_le_createBigParamsTest(hci_le_createBigParamsTest_t *pCmdParam);

/**
 * @brief
 * @param	   none
 * @return     none
 */
ble_sts_t
blc_hci_le_terminateBig(hci_le_terminateBigParams_t *pCmdParam);

/**
 * @brief      Used to enable private ExtADV to send BIGINFO
 * @param	   ExtADV handle
 * @param	   If the BIS encrypted
 * @return     none
 */
void blc_ll_enPrivExtAdvForBigBcst(u8 extAdvHdl);

#endif /* BIS_BCST_H_ */
