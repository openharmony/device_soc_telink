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
#ifndef STACK_BLE_DEBUG_H_
#define STACK_BLE_DEBUG_H_

#ifndef DEBUG_PAIRING_ENCRYPTION
#define DEBUG_PAIRING_ENCRYPTION 0
#endif

/* BLE rf irq timing && log enable */
#ifndef BLE_IRQ_DBG_EN
#define BLE_IRQ_DBG_EN 0
#endif

/* BLE Tx fifo log enable */
#ifndef TX_FIFO_DBG_EN
#define TX_FIFO_DBG_EN 0
#endif

/* BLE smp trans.. log enable */
#ifndef SMP_DBG_EN
#define SMP_DBG_EN 0
#endif

#ifndef TX_PUSH_DATA_LOG
#define TX_PUSH_DATA_LOG 0
#endif

#ifndef RX_L2CAP_DATA_LOG
#define RX_L2CAP_DATA_LOG 0
#endif

#ifndef SCHE_DEBUG_DUMP_EN
#define SCHE_DEBUG_DUMP_EN 0
#endif

#ifndef SCHE_TIMING_DEBUG_EN
#define SCHE_TIMING_DEBUG_EN 0
#endif

#ifndef DBG_BOUNDARY_RX
#define DBG_BOUNDARY_RX 0
#endif

#ifndef DBG_LL_CTRL_LOG_EN
#define DBG_LL_CTRL_LOG_EN 0
#endif

#ifndef DBG_DLE_DUMP_EN
#define DBG_DLE_DUMP_EN 0
#endif

#ifndef DBG_SCHE_TIMING_EN
#define DBG_SCHE_TIMING_EN 0
#endif

#ifndef DBG_IAL_EN
#define DBG_IAL_EN 0
#endif

#ifndef DEB_CIG_MST_EN
#define DEB_CIG_MST_EN 0
#endif

#ifndef DEB_CIG_SLV_EN
#define DEB_CIG_SLV_EN 0
#endif

#ifndef DEB_BIG_BCST_EN
#define DEB_BIG_BCST_EN 0
#endif

#ifndef DEB_BIG_SYNC_EN
#define DEB_BIG_SYNC_EN 0
#endif

#ifndef DEB_ISO_TEST_EN
#define DEG_ISO_TEST_EN 1
#endif

#ifndef DEB_STRUCT_BUFFER_SIZE_CHECK
#define DEB_STRUCT_BUFFER_SIZE_CHECK 1
#endif

#if (DBG_SLAVE_CONN_UPDATE)
#define SLET_upt_cmd_1  10
#define SLET_upt_cmd_2  11
#define SLET_upt_cmd_3  12
#define SLET_upt_cmd_4  13
#define SLET_upt_sync_1 20
#define SLET_upt_sync_2 21
#define SLET_upt_sync_3 22
#define SLET_upt_sync_4 23
#endif

#endif /* STACK_BLE_DEBUG_H_ */
