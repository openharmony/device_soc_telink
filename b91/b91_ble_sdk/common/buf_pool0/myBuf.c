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
/*************************************************************************************************/
/*!
 *  \file   my_buf.c
 *
 *  \brief  Buffer pool service.
 *
 *  Copyright (c) 2009-2018 Arm Ltd. All Rights Reserved.
 *
 *  Copyright (c) 2019-2020 Packetcraft, Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
/*************************************************************************************************/

#include "application\print\printf.h"
#include "assert.h"
#include "common\compiler.h"
#include "types.h"
#include "utility.h"
#include <buf_pool0/myBuf.h>
#include <buf_pool0/myHeap.h>

/**************************************************************************************************
  Global Variables
**************************************************************************************************/

/* ! \brief  Critical section nesting level. */
_attribute_data_retention_ static u8 myCsNesting = 0;

/* !
 *  \brief  Enter a critical section.
 */
void myCsEnter(void)
{
    if (myCsNesting == 0) {
        irq_disable();
    }
    myCsNesting++;
}

/* !
 *  \brief  Exit a critical section.
 */
void myCsExit(void)
{
    assert(myCsNesting != 0);

    myCsNesting--;
    if (myCsNesting == 0) {
        irq_enable();
    }
}

/**************************************************************************************************
  Macros
**************************************************************************************************/
#define DEBUG_INFO  // printf // debug use

/* Magic number used to check for free buffer. */
#define MY_BUF_FREE_NUM 0xFAABD00D

/**************************************************************************************************
  Data Types
**************************************************************************************************/

/* Unit of memory storage-- a structure containing a pointer. */
typedef struct myBufMem_tag {
    struct myBufMem_tag *pNext;
#if MY_BUF_FREE_CHECK_ASSERT == TRUE
    u32 free;
#endif
} myBufMem_t;

/* Internal buffer pool. */
typedef struct {
    myBufPoolDesc_t desc; /* Number of buffers and length. */
    myBufMem_t *pStart;   /* Start of pool. */
    myBufMem_t *pFree;    /* First free buffer in pool. */
#if MY_BUF_STATS == TRUE
    u8 numAlloc;   /* Number of buffers currently allocated from pool. */
    u8 maxAlloc;   /* Maximum buffers ever allocated from pool. */
    u16 maxReqLen; /* Maximum request length from pool. */
#endif
} myBufPool_t;

/**************************************************************************************************
  Global Variables
**************************************************************************************************/

/* Number of pools. */
_attribute_data_retention_ u8 myBufNumPools;

/* Memory used for pools. */
_attribute_data_retention_ myBufMem_t *myBufMem = NULL;

/* Currently use for debugging only. */
_attribute_data_retention_ u32 myBufMemLen;

#if MY_BUF_STATS_HIST == TRUE
/* Buffer allocation counter. */
_attribute_data_retention_ u8 myBufAllocCount[MY_BUF_STATS_MAX_LEN];

/* Pool Overflow counter. */
_attribute_data_retention_ u8 myPoolOverFlowCount[MY_BUF_STATS_MAX_POOL];
#endif

#if MY_OS_DIAG == TRUE
/* MY buffer diagnostic callback function. */
_attribute_data_retention_ static myBufDiagCback_t myBufDiagCback = NULL;
#endif

/*!
 *  \brief  Calculate size required by the buffer pool.
 *
 *  \param  numPools  Number of buffer pools.
 *  \param  pDesc     Array of buffer pool descriptors, one for each pool.
 *
 *  \return Amount of pBufMem used.
 */
u32 myBufCalcSize(u8 numPools, myBufPoolDesc_t *pDesc)
{
    u32 len;
    u32 descLen;
    myBufPool_t *pPool;
    myBufMem_t *pStart;
    u8 i;

    myBufMem = (myBufMem_t *)0;
    pPool = (myBufPool_t *)myBufMem;

    /* Buffer storage starts after the pool structs. */
    pStart = (myBufMem_t *)(pPool + numPools);

    /* Create each pool; see loop exit condition below. */
    while (TRUE) {
        /* Exit loop after verification check. */
        if (numPools-- == 0) {
            break;
        }

        /* Adjust pool lengths for minimum size and alignment. */
        if (pDesc->len < sizeof(myBufMem_t)) {
            descLen = sizeof(myBufMem_t);
        } else if ((pDesc->len % sizeof(myBufMem_t)) != 0) {
            descLen = pDesc->len + sizeof(myBufMem_t) - (pDesc->len % sizeof(myBufMem_t));
        } else {
            descLen = pDesc->len;
        }

        len = descLen / sizeof(myBufMem_t);
        for (i = pDesc->num; i > 0; i--) {
            /* Pointer to the next free buffer is stored in the buffer itself. */
            pStart += len;
        }
        pDesc++;
    }

    return (u8 *)pStart - (u8 *)myBufMem;
}

/*!
 *  \brief  Initialize the buffer pool service.  This function should only be called once
 *          upon system initialization.
 *
 *  \param  numPools  Number of buffer pools.
 *  \param  pDesc     Array of buffer pool descriptors, one for each pool.
 *
 *  \return Amount of pBufMem used or 0 for failures.
 */
u32 myBufInit(u8 numPools, myBufPoolDesc_t *pDesc)
{
    myBufPool_t *pPool;
    myBufMem_t *pStart;
    u16 len;
    u8 i;

    myBufMem = (myBufMem_t *)myHeapGetFreeStartAddress();
    pPool = (myBufPool_t *)myBufMem;

    /* Buffer storage starts after the pool structs. */
    pStart = (myBufMem_t *)(pPool + numPools);

    myBufNumPools = numPools;

    /* Create each pool; see loop exit condition below. */
    while (TRUE) {
        /* Verify we didn't overrun memory; if we did, abort. */
        if (pStart > &myBufMem[myHeapCountAvailable() / sizeof(myBufMem_t)]) {
            assert(FALSE);
            return 0;
        }

        /* Exit loop after verification check. */
        if (numPools-- == 0) {
            break;
        }

        /* Adjust pool lengths for minimum size and alignment. */
        if (pDesc->len < sizeof(myBufMem_t)) {
            pPool->desc.len = sizeof(myBufMem_t);
        } else if ((pDesc->len % sizeof(myBufMem_t)) != 0) {
            pPool->desc.len = pDesc->len + sizeof(myBufMem_t) - (pDesc->len % sizeof(myBufMem_t));
        } else {
            pPool->desc.len = pDesc->len;
        }

        pPool->desc.num = pDesc->num;
        pDesc++;

        pPool->pStart = pStart;
        pPool->pFree = pStart;
#if MY_BUF_STATS == TRUE
        pPool->numAlloc = 0;
        pPool->maxAlloc = 0;
        pPool->maxReqLen = 0;
#endif

        /* Initialize free list. */
        len = pPool->desc.len / sizeof(myBufMem_t);
        for (i = pPool->desc.num; i > 1; i--) {
            /* Verify we didn't overrun memory; if we did, abort. */
            if (pStart > &myBufMem[myHeapCountAvailable() / sizeof(myBufMem_t)]) {
                assert(FALSE);
                return 0;
            }
            /* Pointer to the next free buffer is stored in the buffer itself. */
            pStart->pNext = pStart + len;
            pStart += len;
        }

        /* Verify we didn't overrun memory; if we did, abort. */
        if (pStart > &myBufMem[myHeapCountAvailable() / sizeof(myBufMem_t)]) {
            assert(FALSE);
            return 0;
        }
        /* Last one in list points to NULL. */
        pStart->pNext = NULL;
        pStart += len;

        /* Next pool. */
        pPool++;
    }

    myBufMemLen = (u8 *)pStart - (u8 *)myBufMem;

    return myBufMemLen;
}

/*!
 *  \brief  Allocate a buffer.
 *
 *  \param  len     Length of buffer to allocate.
 *
 *  \return Pointer to allocated buffer or NULL if allocation fails.
 */
void *myBufAlloc(u16 len)
{
    myBufPool_t *pPool;
    myBufMem_t *pBuf;
    u8 i;

    assert(len > 0);

    pPool = (myBufPool_t *)myBufMem;

    for (i = myBufNumPools; i > 0; i--, pPool++) {
        /* Check if buffer is big enough. */
        if (len <= pPool->desc.len) {
            /* Enter critical section. */
            myCsEnter();

            /* Check if buffers are available. */
            if (pPool->pFree != NULL) {
                /* Allocation succeeded. */
                pBuf = pPool->pFree;

                /* Next free buffer is stored inside current free buffer. */
                pPool->pFree = pBuf->pNext;

#if MY_BUF_FREE_CHECK_ASSERT == TRUE
                pBuf->free = 0;
#endif
#if MY_BUF_STATS_HIST == TRUE
                /* Increment count for buffers of this length. */
                if (len < MY_BUF_STATS_MAX_LEN) {
                    myBufAllocCount[len]++;
                } else {
                    myBufAllocCount[0]++;
                }
#endif
#if MY_BUF_STATS == TRUE
                if (++pPool->numAlloc > pPool->maxAlloc) {
                    pPool->maxAlloc = pPool->numAlloc;
                }
                pPool->maxReqLen = max2(pPool->maxReqLen, len);
#endif
                /* Exit critical section. */
                myCsExit();
                ;

                return pBuf;
            } else {
#if MY_BUF_STATS_HIST == TRUE
                /* Pool overflow: increment count of overflow for current pool. */
                myPoolOverFlowCount[myBufNumPools - i]++;
#endif
            }
            /* Exit critical section. */
            myCsExit();
            ;

#if MY_BUF_ALLOC_BEST_FIT_FAIL_ASSERT == TRUE
            assert(FALSE);
#endif
        }
    }

    /* Allocation failed. */
#if MY_OS_DIAG == TRUE
    if (myBufDiagCback != NULL) {
        myBufDiag_t info;

        info.type = MY_BUF_ALLOC_FAILED;
        info.param.alloc.taskId = MY_OS_GET_ACTIVE_HANDLER_ID();
        info.param.alloc.len = len;

        myBufDiagCback(&info);
    } else {
    }
#else

#endif

#if MY_BUF_ALLOC_FAIL_ASSERT == TRUE
    assert(FALSE);
#endif

    return NULL;
}

/*!
 *  \brief  Free a buffer.
 *
 *  \param  pBuf    Buffer to free.
 */
void myBufFree(void *pBuf)
{
    myBufPool_t *pPool;
    myBufMem_t *p = pBuf;

    /* Verify pointer is within range. */
#if MY_BUF_FREE_CHECK_ASSERT == TRUE
    assert(p >= ((myBufPool_t *)myBufMem)->pStart);
    assert(p < (myBufMem_t *)(((u8 *)myBufMem) + myBufMemLen));
#endif

    /* Iterate over pools starting from last pool. */
    pPool = (myBufPool_t *)myBufMem + (myBufNumPools - 1);
    while (pPool >= (myBufPool_t *)myBufMem) {
        /* Check if the buffer memory is located inside this pool. */
        if (p >= pPool->pStart) {
            /* Enter critical section. */
            myCsEnter();

#if MY_BUF_FREE_CHECK_ASSERT == TRUE
            assert(p->free != MY_BUF_FREE_NUM);
            p->free = MY_BUF_FREE_NUM;
#endif
#if MY_BUF_STATS == TRUE
            pPool->numAlloc--;
#endif

            /* Pool found; put buffer back in free list. */
            p->pNext = pPool->pFree;
            pPool->pFree = p;

            /* Exit critical section. */
            myCsExit();
            ;

            return;
        }

        /* Next pool. */
        pPool--;
    }

    /* Should never get here. */
    assert(FALSE);

    return;
}

/*!
 *  \brief  Diagnostic function to get the buffer allocation statistics.
 *
 *  \return Buffer allocation statistics array.
 */
u8 *myBufGetAllocStats(void)
{
#if MY_BUF_STATS_HIST == TRUE
    return myBufAllocCount;
#else
    return NULL;
#endif
}

/*!
 *  \brief  Diagnostic function to get the number of overflow times for each pool.
 *
 *  \return Overflow times statistics array
 */
u8 *myBufGetPoolOverFlowStats(void)
{
#if MY_BUF_STATS_HIST == TRUE
    return myPoolOverFlowCount;
#else
    return NULL;
#endif
}

/*!
 *  \brief  Get number of pools.
 *
 *  \return Number of pools.
 */
u8 myBufGetNumPool(void)
{
    return myBufNumPools;
}

/*!
 *  \brief  Get statistics for each pool.
 *
 *  \param  pBuf    Buffer to store the statistics.
 *  \param  poolId  Pool ID.
 */
void myBufGetPoolStats(myBufPoolStat_t *pStat, u8 poolId)
{
    myBufPool_t *pPool;

    if (poolId >= myBufNumPools) {
        pStat->bufSize = 0;
        return;
    }

    myCsEnter();

    pPool = (myBufPool_t *)myBufMem;

    pStat->bufSize = pPool[poolId].desc.len;
    pStat->numBuf = pPool[poolId].desc.num;
#if MY_BUF_STATS == TRUE
    pStat->numAlloc = pPool[poolId].numAlloc;
    pStat->maxAlloc = pPool[poolId].maxAlloc;
    pStat->maxReqLen = pPool[poolId].maxReqLen;
#else
    pStat->numAlloc = 0;
    pStat->maxAlloc = 0;
    pStat->maxReqLen = 0;
#endif

    /* Exit critical section. */
    myCsExit();
    ;
}

/* !
 *  \brief  Called to register the buffer diagnostics callback function.
 *
 *  \param  pCallback   Pointer to the callback function.
 */
void myBufDiagRegister(myBufDiagCback_t callback)
{
#if MY_OS_DIAG == TRUE
    myBufDiagCback = callback;
#else
    /* Unused parameter */
    (void)callback;
#endif
}