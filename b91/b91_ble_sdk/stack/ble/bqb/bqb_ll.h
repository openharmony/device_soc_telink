/********************************************************************************************************
 * @file	bqb_ll.h
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
#ifndef BQB_LL_H_
#define BQB_LL_H_


typedef struct {

	u16	 	testAclConnEvent_cnt;
	u16 	testAclConnEvent_numMax;

	u16 	testCisConnEvent_cnt;
	u16 	testCisConnEvent_numMax;

	bool    testAclConnPktMaxEnFlg;
	bool    testCisConnPktMaxEnFlg;



} st_bqb_ll_t;

extern st_bqb_ll_t		bltBQB;

extern u64				bqbIrqEvt_mask;






void tp_set_acl_conn_event_max(u16 m);
void tp_set_cis_conn_event_max(u16 m);










void test_case_function_register (char * testcase, callback_testcase_t func);
int  exe_lower_tester_command (u8 *cmd);


void blc_bqb_ll_main_loop(void);















void 	tp_set_adv_pkt_max (u16 m);
void 	tp_enable_advData_inrease(u8 en, s8 step);

void 	tp_AdvAddress_companyId_exchange(void);
void 	tp_AdvAddress_companyAssignId_exchange(void);
void 	tp_AdvAddress_companyId_companyAssignId_mismatch(void);
//void 	tp_ScanRspAddress_companyId_exchange(void);
//void 		tp_ScanRspAddress_companyAssignId_exchange(void);
//void tp_ScanRspAddress_companyId_companyAssignId_mismatch(void);


void 	tp_enable_adv_scanRsp_Bad_CRC (u8 advBadCrc, u8 scanRspBadCrc );

void 	tp_set_conn_pkt_max (u32 m);

void	tp_flip_access_address (int n);
void	tp_set_test_data (u8 type, u8 len, u8 d, int pkts);
void	tp_set_manual_timeout (u32 timeout);
void 	tp_disable_more_data(u8 disable);
void 	tp_enbale_print_rcvd_data_connect(u8 en);
void 	tp_enbale_rcvd_l2cap_data_callback(u8 en);
void 	tp_disable_data_len_exchange(u8 en);
void 	tp_set_brx_missing_time(u32 time_ms);
void 	tp_set_brx_missing_delay_intervals(int interval_num);
void 	tp_enable_conn_pkt_Bad_CRC (u8 connBadCrc );



void 	tp_set_adv_pkt_max (u16 m);
void 	tp_disable_more_data(u8 disable);
void 	tp_set_conn_pkt_max(u32 m);
void 	tp_phy_req_col(u8 en);
void 	tp_phy_req_skip(u8 en);
void 	tp_phy_req_nochange(u8 en);
void 	tp_phy_no_common(u8 en);
void 	tp_set_test_data (u8 type, u8 len, u8 d, int pkts);
void 	tp_enbale_rcvd_l2cap_data_callback(u8 en);




void 	tp_set_test_pkt(u8 num);
void 	tp_SetPhyUpdate_Same(u8 en);
void 	tp_set_PhyUpdate_InsInPast(u8 en);
void 	tp_set_PhyReq_TO(u8 en);
void 	tp_set_DiffProc_ChnMap(u8 en);
void 	tp_set_DiffProc_ConUp(u8 en);
void 	tp_setRecv_no(u8 num);
void 	tp_set_PhyInd_TO(u8 en);
void 	tp_set_NoPhyReqChan(u8 en);
void 	tp_set_NoPhyRspChan(u8 en);





#endif /* LL_BQB_H_ */
