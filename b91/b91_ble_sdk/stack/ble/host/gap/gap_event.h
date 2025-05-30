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
/*
 * gap_event.h
 *
 *  Created on: 2018-12-5
 *      Author: Administrator
 */

#ifndef GAP_EVENT_H_
#define GAP_EVENT_H_

/*****************************************  SMP message sequence and event chart  *************************************

																							  GAP_SMP EVENT
Situation 1:   SMP Standard Pair																    |
	Phase 1: Pairing Feature Exchange																|
	Phase 2: Encryption( STK/LTK Generation )														|
	Phase 3: Key Distribution																		|
																									|
  	     Master														  Slave							|
			|															|							|
			|														    |							|
    ________|___________________________________________________________|_______					|
	| 																			|					|
	|						   Establish LL connection							|					|
	|___________________________________________________________________________|					|
			|															|							|
			|															|							|
	        |              Phase 1: Pairing Feature Exchange				|							|
	 _______|___________________________________________________________|_______					|
	|		|													        |	    |					|
	|		|			        (Optional)Security_Request		    	|	    |					|
	|		|<----------------------------------------------------------|	    |					|
	|		|						Pairing_Req							|	    |
	| 		|---------------------------------------------------------->|=======|=>>> GAP_EVT_SMP_PAIRING_BEGIN
	|		|						Pairing_Rsp					    	|	    |
	|  		|<----------------------------------------------------------|       |					|
	|		|						....... 							|	    |					|
	|_______|___________________________________________________________|_______|					|
			|															|							|
			|															|							|
	        |                  Phase 2: Encryption					    |							|
	 _______|___________________________________________________________|_______					|
	|		|															|	    |					|
	|		|					    LL_ENC_REQ				            |	    |					|
	|		|---------------------------------------------------------->|	    |					|
	|		|					    LL_ENC_RSP						    |	    |					|
	| 		|<----------------------------------------------------------|	    |					|
	|		|				     LL_START_ENC_REQ						|	    |					|
	|  		|<----------------------------------------------------------|       |					|
	|		|					 LL_START_ENC_RSP						|	    |					|
	|		|---------------------------------------------------------->|	    |					|
	|       |				     LL_START_ENC_RSP	 					|	    |
	|		|<----------------------------------------------------------|=======|=>>> GAP_EVT_SMP_CONN_ENCRYPTION_DONE
	|_______|___________________________________________________________|_______|
			|															|							|
			|															|							|
	        |              Phase 3: Key Distribution				    |							|
	 _______|___________________________________________________________|_______					|
	|		|															|	    |					|
	|		|					  Key Distribution	 			        |	    |					|
	|		|<----------------------------------------------------------|	    |					|
	|		|					  Key Distribution						|	    |					|
	| 		|<----------------------------------------------------------|	    |					|
	|		|				           ....... 					        |	    |					|
	|  		|															|       |					|
	|		|					  Key Distribution				        |	    |					|
	|		|---------------------------------------------------------->|	    |					|
	|		|					  Key Distribution						|	    |					|
	| 		|---------------------------------------------------------->|	    |					|
	|		|				           ....... 					        |	    |					|
	|  		|															|       |
	|		|				 All Key Distribution Finish				|=======|=>>> GAP_EVT_SMP_PAIRING_SUCCESS
	|		|				 											|=======|=>>> GAP_EVT_SMP_SECURITY_PROCESS_DONE
	|_______|___________________________________________________________|_______|
																									|
																									|
																									|
																									|
Situation 2:   SMP Fast Connect																		|
	Only 1 Phase: Encryption																	    |
																									|
  	     Master														  Slave							|
			|															|							|
			|														    |							|
    ________|___________________________________________________________|_______					|
	| 																			|					|
	|						  Establish LL connection							|					|
	|___________________________________________________________________________|					|
			|															|							|
			|															|							|
	        |                  Phase 2: Encryption					    |							|
	 _______|___________________________________________________________|_______					|
	|		|															|	    |					|
	|		|					    LL_ENC_REQ				            |	    |					|
	|		|---------------------------------------------------------->|	    |					|
	|		|					    LL_ENC_RSP						    |	    |					|
	| 		|<----------------------------------------------------------|	    |					|
	|		|				     LL_START_ENC_REQ						|	    |					|
	|  		|<----------------------------------------------------------|       |					|
	|		|					 LL_START_ENC_RSP						|	    |					|
	|		|---------------------------------------------------------->|	    |					|
	|       |				     LL_START_ENC_RSP	 					|	    |					|
	|		|<----------------------------------------------------------|=======|=>>> GAP_EVT_SMP_CONN_ENCRYPTION_DONE
	|		|				 											|=======|=>>> GAP_EVT_SMP_SECURITY_PROCESS_DONE
	|_______|___________________________________________________________|_______|


 *********************************************************************************************************************/

/**
 * @brief      GAP event type
 */
#define GAP_EVT_SMP_PAIRING_BEGIN         0  // Refer to SMP message sequence and event chart above
#define GAP_EVT_SMP_PAIRING_SUCCESS       1  // Refer to SMP message sequence and event chart above
#define GAP_EVT_SMP_PAIRING_FAIL          2
#define GAP_EVT_SMP_CONN_ENCRYPTION_DONE  3  // Refer to SMP message sequence and event chart above
#define GAP_EVT_SMP_SECURITY_PROCESS_DONE 4  // Refer to SMP message sequence and event chart above

#define GAP_EVT_SMP_TK_DISPALY         8
#define GAP_EVT_SMP_TK_REQUEST_PASSKEY 9
#define GAP_EVT_SMP_TK_REQUEST_OOB     10
#define GAP_EVT_SMP_TK_NUMERIC_COMPARE 11
#define GAP_EVT_SMP_BONDING_INFO_FULL  12

#define GAP_EVT_ATT_EXCHANGE_MTU          16
#define GAP_EVT_GATT_HANDLE_VLAUE_CONFIRM 17

#define GAP_EVT_L2CAP_CONN_PARAM_UPDATE_REQ 20
#define GAP_EVT_L2CAP_CONN_PARAM_UPDATE_RSP 21

#if (L2CAP_CREDIT_BASED_FLOW_CONTROL_MODE_EN)
#define GAP_EVT_L2CAP_LE_CREDIT_BASED_CONNECT 22
#define GAP_EVT_L2CAP_CREDIT_BASED_CONNECT    23
#define GAP_EVT_L2CAP_CREDIT_BASED_RECONFIG   24
#define GAP_EVT_L2CAP_FLOW_CONTROL_CREDIT     25
#define GAP_EVT_L2CAP_DISCONNECT              26
#define GAP_EVT_L2CAP_COC_DATA                27
#endif

/**
 * @brief      GAP event mask
 */
#define GAP_EVT_MASK_NONE                      0x00000000
#define GAP_EVT_MASK_SMP_PAIRING_BEGIN         (1 << GAP_EVT_SMP_PAIRING_BEGIN)
#define GAP_EVT_MASK_SMP_PAIRING_SUCCESS       (1 << GAP_EVT_SMP_PAIRING_SUCCESS)
#define GAP_EVT_MASK_SMP_PAIRING_FAIL          (1 << GAP_EVT_SMP_PAIRING_FAIL)
#define GAP_EVT_MASK_SMP_CONN_ENCRYPTION_DONE  (1 << GAP_EVT_SMP_CONN_ENCRYPTION_DONE)
#define GAP_EVT_MASK_SMP_SECURITY_PROCESS_DONE (1 << GAP_EVT_SMP_SECURITY_PROCESS_DONE)

#define GAP_EVT_MASK_SMP_TK_DISPALY         (1 << GAP_EVT_SMP_TK_DISPALY)
#define GAP_EVT_MASK_SMP_TK_REQUEST_PASSKEY (1 << GAP_EVT_SMP_TK_REQUEST_PASSKEY)
#define GAP_EVT_MASK_SMP_TK_REQUEST_OOB     (1 << GAP_EVT_SMP_TK_REQUEST_OOB)
#define GAP_EVT_MASK_SMP_TK_NUMERIC_COMPARE (1 << GAP_EVT_SMP_TK_NUMERIC_COMPARE)
#define GAP_EVT_MASK_SMP_BONDING_INFO_FULL  (1 << GAP_EVT_SMP_BONDING_INFO_FULL)

#define GAP_EVT_MASK_ATT_EXCHANGE_MTU          (1 << GAP_EVT_ATT_EXCHANGE_MTU)
#define GAP_EVT_MASK_GATT_HANDLE_VLAUE_CONFIRM (1 << GAP_EVT_GATT_HANDLE_VLAUE_CONFIRM)

#define GAP_EVT_MASK_L2CAP_CONN_PARAM_UPDATE_REQ (1 << GAP_EVT_L2CAP_CONN_PARAM_UPDATE_REQ)
#define GAP_EVT_MASK_L2CAP_CONN_PARAM_UPDATE_RSP (1 << GAP_EVT_L2CAP_CONN_PARAM_UPDATE_RSP)

#if (L2CAP_CREDIT_BASED_FLOW_CONTROL_MODE_EN)
#define GAP_EVT_MASK_L2CAP_LE_CREDIT_BASED_CONNECT (1 << GAP_EVT_L2CAP_LE_CREDIT_BASED_CONNECT)
#define GAP_EVT_MASK_L2CAP_CREDIT_BASED_CONNECT    (1 << GAP_EVT_L2CAP_CREDIT_BASED_CONNECT)
#define GAP_EVT_MASK_L2CAP_CREDIT_BASED_RECONFIG   (1 << GAP_EVT_L2CAP_CREDIT_BASED_RECONFIG)
#define GAP_EVT_MASK_L2CAP_FLOW_CONTROL_CREDIT     (1 << GAP_EVT_L2CAP_FLOW_CONTROL_CREDIT)
#define GAP_EVT_MASK_L2CAP_DISCONNECT              (1 << GAP_EVT_L2CAP_DISCONNECT)
#define GAP_EVT_MASK_L2CAP_COC_DATA                (1 << GAP_EVT_L2CAP_COC_DATA)
#endif

#define GAP_EVT_MASK_DEFAULT                                                                                          \
    (GAP_EVT_MASK_SMP_TK_DISPALY | GAP_EVT_MASK_SMP_TK_REQUEST_PASSKEY | GAP_EVT_MASK_SMP_TK_REQUEST_OOB |            \
     GAP_EVT_MASK_ATT_EXCHANGE_MTU)

/**
 *  @brief  Event Parameters for "GAP_EVT_SMP_PAIRING_BEGIN"
 */
typedef struct {
    u16 connHandle;
    u8 secure_conn;
    u8 tk_method;
} gap_smp_pairingBeginEvt_t;

/**
 *  @brief  Event Parameters for "GAP_EVT_SMP_PAIRING_SUCCESS"
 */
typedef struct {
    u16 connHandle;
    u8 bonding;
    u8 bonding_result;
} gap_smp_pairingSuccessEvt_t;

/**
 *  @brief  Event Parameters for "GAP_EVT_SMP_PAIRING_FAIL"
 */
typedef struct {
    u16 connHandle;
    u8 reason;
} gap_smp_pairingFailEvt_t;

/**
 *  @brief  Event Parameters for "GAP_EVT_SMP_CONN_ENCRYPTION_DONE"
 */
typedef struct {
    u16 connHandle;
    u8 re_connect;  // 1: re_connect, encrypt with previous distributed LTK;   0: pairing , encrypt with STK
} gap_smp_connEncDoneEvt_t;

/**
 *  @brief  Event Parameters for "GAP_EVT_SMP_SECURITY_PROCESS_DONE"
 */
typedef struct {
    u16 connHandle;
    u8 re_connect;  // 1: re_connect, encrypt with previous distributed LTK;   0: pairing , encrypt with STK
} gap_smp_securityProcessDoneEvt_t;

/**
 *  @brief  Event Parameters for "GAP_EVT_SMP_TK_DISPALY"
 */
typedef struct {
    u16 connHandle;
    u32 tk_pincode;
} gap_smp_TkDisplayEvt_t;

/**
 *  @brief  Event Parameters for "GAP_EVT_SMP_TK_REQUEST_PASSKEY"
 */
typedef struct {
    u16 connHandle;
} gap_smp_TkReqPassKeyEvt_t;

/**
 *  @brief  Event Parameters for "GAP_EVT_ATT_EXCHANGE_MTU"
 */
typedef struct {
    u16 connHandle;
    u16 peer_MTU;
    u16 effective_MTU;
} gap_gatt_mtuSizeExchangeEvt_t;

/**
 *  @brief  Event Parameters for "GAP_EVT_L2CAP_CONN_PARAM_UPDATE_REQ"
 */
typedef struct {
    u16 connHandle;
    u8 id;
    u16 min_interval;
    u16 max_interval;
    u16 latency;
    u16 timeout;
} gap_l2cap_connParamUpdateReqEvt_t;

/**
 *  @brief  Event Parameters for "GAP_EVT_L2CAP_CONN_PARAM_UPDATE_RSP"
 */
typedef struct {
    u16 connHandle;
    u8 id;
    u16 result;
} gap_l2cap_connParamUpdateRspEvt_t;

#if (L2CAP_CREDIT_BASED_FLOW_CONTROL_MODE_EN)
typedef struct {
    u16 connHandle;
    u16 result;
    u16 reason;
    u16 local_mtu;
    u16 local_mps;
    u16 local_credit;
    u16 peer_mtu;
    u16 peer_mps;
    u16 peer_credit;
    u8 srv_num;
    u16 scid;
    u16 dcid;
} gap_l2cap_leCreditBasedConnectEvt_t;
typedef struct {
    u16 connHandle;
    u16 result;
    u16 reason;
    u16 local_mtu;
    u16 local_mps;
    u16 local_credit;
    u16 peer_mtu;
    u16 peer_mps;
    u16 peer_credit;
    u8 srv_num;
    u8 cid_count;
    u16 scid[5];
    u16 dcid[5];
} gap_l2cap_creditBasedConnectEvt_t;
typedef struct {
    u16 connHandle;
    u16 local_credit;
    u16 peer_credit;
    u16 scid;
    u16 dcid;
} gap_l2cap_flowControlCreditEvt_t;
typedef struct {
    u16 connHandle;
    u16 spsm;
    u16 scid;
    u16 dcid;
} gap_l2cap_disconnectEvt_t;
typedef struct {
    u16 connHandle;
    u16 result;
    u16 local_mtu;
    u16 local_mps;
    u16 local_credit;
    u16 peer_mtu;
    u16 peer_mps;
    u16 peer_credit;
    u8 srv_num;
    u8 cid_count;
    u16 scid[5];
    u16 dcid[5];
} gap_l2cap_creditBasedReconfigEvt_t;
typedef struct {
    u16 connHandle;
    u16 spsm;
    u16 scid;
    u16 dcid;
    u16 dataLen;
    u8 *pData;
} gap_l2cap_cocData_t;
#endif  // #if (L2CAP_CREDIT_BASED_FLOW_CONTROL_MODE_EN)

/**
 * @brief     GAP event callback function declaration
 */
typedef int (*gap_event_handler_t)(u32 h, u8 *para, int n);

/**
 * @brief      set event mask for specific gap event
 * @param[in]  evtMask - event mask
 * @return     none
 */
void blc_gap_setEventMask(u32 evtMask);

/**
 * @brief      register public enter for all gap event callback
 * @param[in]  handler - public enter function
 * @return     none
 */
void blc_gap_registerHostEventHandler(gap_event_handler_t handler);

#endif /* GAP_EVENT_H_ */
