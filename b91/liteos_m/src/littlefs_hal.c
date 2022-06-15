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

#include <stdio.h>
#include <string.h>

#include <los_mux.h>

#include <lfs.h>

#include <B91/flash.h>

#define LITTLEFS_PATH "/littlefs/"

#define LITTLEFS_PHYS_ADDR (1024 * 1024)
#define LITTLEFS_PHYS_SIZE (16 * 1024)

#define READ_SIZE      16
#define PROG_SIZE      16
#define BLOCK_SIZE     4096
#define BLOCK_COUNT    32
#define CACHE_SIZE     512
#define LOOKAHEAD_SIZE 64
#define BLOCK_CYCLES   500

#if defined(LFS_THREADSAFE)
static uint32_t g_lfsMutex;
#endif /* LFS_THREADSAFE */

static int LittlefsRead(const struct lfs_config *cfg, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size)
{
    uint32_t addr = block * (cfg->block_size) + off;

    flash_read_page(LITTLEFS_PHYS_ADDR + addr, size, buffer);

    return LFS_ERR_OK;
}

static int LittlefsProg(const struct lfs_config *cfg, lfs_block_t block, lfs_off_t off, const void *buffer,
                        lfs_size_t size)
{
    uint32_t addr = block * (cfg->block_size) + off;

    flash_write_page(LITTLEFS_PHYS_ADDR + addr, size, (unsigned char *)buffer);

    return LFS_ERR_OK;
}

static int LittlefsErase(const struct lfs_config *cfg, lfs_block_t block)
{
    uint32_t addr = block * (cfg->block_size);

    flash_erase_sector(LITTLEFS_PHYS_ADDR + addr);

    return LFS_ERR_OK;
}

static int LittlefsSync(const struct lfs_config *cfg)
{
    return LFS_ERR_OK;
}

#if defined(LFS_THREADSAFE)
static int LittlefsLock(const struct lfs_config *conf)
{
    (void)conf;
    (void)LOS_MuxPend(g_lfsMutex, LOS_WAIT_FOREVER);
    return LFS_ERR_OK;
}

static int LittlefsUnlock(const struct lfs_config *conf)
{
    (void)conf;
    (void)LOS_MuxPost(g_lfsMutex);
    return LFS_ERR_OK;
}
#endif /* LFS_THREADSAFE */

static struct lfs_config g_lfsConfig = {
    // block device operations
    .context = NULL,
    .read = LittlefsRead,
    .prog = LittlefsProg,
    .erase = LittlefsErase,
    .sync = LittlefsSync,
#if defined(LFS_THREADSAFE)
    .lock = LittlefsLock,
    .unlock = LittlefsUnlock,
#endif /* LFS_THREADSAFE */
    // block device configuration
    .read_size = READ_SIZE,
    .prog_size = PROG_SIZE,
    .block_size = BLOCK_SIZE,
    .block_count = BLOCK_COUNT,
    .cache_size = CACHE_SIZE,
    .lookahead_size = LOOKAHEAD_SIZE,
    .block_cycles = BLOCK_CYCLES,
};

void LittlefsDriverInit(int needErase)
{
    (void)needErase;
}

struct lfs_config *LittlefsConfigGet(void)
{
#if defined(LFS_THREADSAFE)
    (void)LOS_MuxCreate(&g_lfsMutex);
#endif /* LFS_THREADSAFE */
    return &g_lfsConfig;
}
