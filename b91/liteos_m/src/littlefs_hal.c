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

#include <lfs.h>
#include <los_fs.h>

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

int littlefs_block_read(const struct lfs_config *cfg, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size)
{
    uint32_t addr = block * (cfg->block_size) + off;

    flash_read_page(LITTLEFS_PHYS_ADDR + addr, size, buffer);

    return LFS_ERR_OK;
}

int littlefs_block_write(const struct lfs_config *cfg, lfs_block_t block, lfs_off_t off, const void *buffer,
                        lfs_size_t size)
{
    uint32_t addr = block * (cfg->block_size) + off;

    flash_write_page(LITTLEFS_PHYS_ADDR + addr, size, (unsigned char *)buffer);

    return LFS_ERR_OK;
}

int littlefs_block_erase(const struct lfs_config *cfg, lfs_block_t block)
{
    uint32_t addr = block * (cfg->block_size);

    flash_erase_sector(LITTLEFS_PHYS_ADDR + addr);

    return LFS_ERR_OK;
}

int littlefs_block_sync(const struct lfs_config *cfg)
{
    return LFS_ERR_OK;
}

static struct PartitionCfg partCfg = {
    .readFunc = NULL,
    .writeFunc = NULL,
    .eraseFunc = NULL,
    .readSize = READ_SIZE,
    .writeSize = PROG_SIZE,
    .blockSize = BLOCK_SIZE,
    .blockCount = BLOCK_COUNT,
    .cacheSize = CACHE_SIZE,
    .partNo = 0,
    .lookaheadSize = LOOKAHEAD_SIZE,
    .blockCycles = BLOCK_CYCLES
};

struct PartitionCfg *LittlefsConfigGet(void)
{
    return &partCfg;
}
