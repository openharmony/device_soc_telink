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
#ifndef ZB_BUFPOOL_H
#define ZB_BUFPOOL_H 1

#include "tl_common.h"

/*! \addtogroup buf */
/*! @{ */

/**
   Packet buffer header.
 */

typedef struct {
    u8 id;  // primitive id
    u8 handle;
    s8 rssi;
    u8 used : 1;
    u8 macTxFifo : 1;
    u8 mgmtLeave : 1;
    u8 active : 1;   // only for mac command buffer
    u8 pending : 1;  // only for endDev, if parent has pending data for me
    u8 resvHdr : 3;
} zb_buf_hdr_t;  // 4

/**
   Packet buffer
 */
#define ZB_BUFFER_DEBUG 0

#define TL_RXPRIMITIVEHDR 32  // To use the in buffer as RX buffer, needs to preallocate primitive fields

#define ZB_BUF_SIZE (160 + TL_RXPRIMITIVEHDR)

#ifndef ZB_BUF_POOL_NUM
#if defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
#define ZB_BUF_POOL_NUM 36
#else
#ifdef CHIP_8269
#define ZB_BUF_POOL_NUM 36
#else
#define ZB_BUF_POOL_NUM 12
#endif
#endif
#endif

typedef struct {
    u16 allocLine;
    u16 freeLine;
    u8 handler;
    u8 nlmeStatus;
    u8 id;
    u8 resv;
} zb_buf_allocInfo_t;

#if ZB_BUFFER_DEBUG
#define ZB_BUFF_DBG_NUM 16
#endif

typedef struct zb_buf_s {
    u8 buf[ZB_BUF_SIZE];
    zb_buf_hdr_t hdr;
    struct zb_buf_s *next;
    u32 allocCnt;
    u32 freeCnt;
#if ZB_BUFFER_DEBUG
    zb_buf_allocInfo_t allocInfo[ZB_BUFF_DBG_NUM];
#endif
} zb_buf_t;

typedef struct {
    zb_buf_t *head;
    u32 usedNum;
#if ZB_BUFFER_DEBUG
    u32 rsv_fill[2];
#endif
    zb_buf_t pool[ZB_BUF_POOL_NUM];  // shall be allocated at the last field in the structure of the zb_buf_pool_t
} zb_buf_pool_t;

extern u8 ZB_BUF_POOL_SIZE;
extern zb_buf_pool_t g_mPool;

void *tl_bufInitalloc(zb_buf_t *p, u8 size);
#define TL_BUF_INITIAL_ALLOC(p, size, ptr, type) (ptr) = (type)tl_bufInitalloc((p), (size))

#define TL_COPY_BUF(dst, src)                                                                                         \
    do {                                                                                                              \
        memcpy((dst), (src), ZB_BUF_SIZE + sizeof(zb_buf_hdr_t) - 1);                                                 \
    } while (0)

void zb_buf_clear(zb_buf_t *p);

bool is_zb_buf(void *p);

/*
 * buffer(only used in zigbee stack) initialization
 *
 * */
extern void tl_zbBufferInit(void);

extern u8 *tl_phyRxBufTozbBuf(u8 *p);

extern u8 *tl_zbBufToPhyRxBuf(u8 *p);

#define TL_RXBUF_TO_INBUF(p) tl_phyRxBufTozbBuf(p)
#define TL_INBUF_TO_RXBUF(p) tl_zbBufToPhyRxBuf(p)

u8 *tl_getRxBuf(void);

#if ZB_BUFFER_DEBUG
#define zb_buf_allocate(void) my_zb_buf_allocate(__LINE__)
#else
zb_buf_t *zb_buf_allocate(void);
#endif

#if ZB_BUFFER_DEBUG
#define zb_buf_free(x) my_zb_buf_free(x, __LINE__)
#else
u8 zb_buf_free(zb_buf_t *buf);
#endif

#define ZB_BUF_FROM_REF(ref) (&g_mPool.pool[ref])

#define ZB_REF_FROM_BUF(p) ((p) - &g_mPool.pool[0])

extern u8 RX_ZBBUF_OFFSET;
/*! @} */

#endif /* ZB_BUFPOOL_H */
