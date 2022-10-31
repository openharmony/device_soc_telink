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
#include <sys/mount.h>

#include <device_resource_if.h>
#include <hdf_device_desc.h>
#include <hdf_log.h>
#include <lfs_adapter.h>
#include <los_config.h>

#include <los_fs.h>

#include <B91/flash.h>

#define LITTLEFS_PHYS_ADDR (1024 * 1024)
#define LITTLEFS_PHYS_SIZE (128 * 1024)

#define READ_SIZE      16
#define PROG_SIZE      16
#define BLOCK_SIZE     4096
#define BLOCK_COUNT    32
#define CACHE_SIZE     512
#define LOOKAHEAD_SIZE 64
#define BLOCK_CYCLES   500

struct fs_cfg {
    char *mount_point;
    struct lfs_config lfs_cfg;
};

static struct fs_cfg fs[LOSCFG_LFS_MAX_MOUNT_SIZE] = {0};

static int readFunc(int partition, UINT32 *offset, unsigned char *buf, UINT32 size)
{
    flash_read_page(LITTLEFS_PHYS_ADDR + *offset, size, buf);
    return LFS_ERR_OK;
}

/* partition low-level write func */
static int writeFunc(int partition, UINT32 *offset, const unsigned char *buf, UINT32 size)
{
    flash_write_page(LITTLEFS_PHYS_ADDR + *offset, size, (unsigned char *)buf);
    return LFS_ERR_OK;
}

/* partition low-level erase func */
static int eraseFunc(int partition, UINT32 offset, UINT32 size)
{
    flash_erase_sector(LITTLEFS_PHYS_ADDR + offset);
    return LFS_ERR_OK;
}

static uint32_t FsGetResource(struct fs_cfg *fs, const struct DeviceResourceNode *resourceNode)
{
    struct DeviceResourceIface *resource = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (resource == NULL) {
        HDF_LOGE("Invalid DeviceResourceIface");
        return HDF_FAILURE;
    }
    int32_t num = resource->GetElemNum(resourceNode, "mount_points");
    if (num < 0 || num > LOSCFG_LFS_MAX_MOUNT_SIZE) {
        HDF_LOGE("%s: invalid mount_points num %d", __func__, num);
        return HDF_FAILURE;
    }
    for (int32_t i = 0; i < num; i++) {
        if (resource->GetStringArrayElem(resourceNode, "mount_points", i, &fs[i].mount_point, NULL) != HDF_SUCCESS) {
            HDF_LOGE("%s: failed to get mount_points", __func__);
            return HDF_FAILURE;
        }
        if (resource->GetUint32ArrayElem(resourceNode, "partitions", i, (uint32_t *)&fs[i].lfs_cfg.context, 0) !=
            HDF_SUCCESS) {
            HDF_LOGE("%s: failed to get partitions", __func__);
            return HDF_FAILURE;
        }
        if (resource->GetUint32ArrayElem(resourceNode, "block_size", i, &fs[i].lfs_cfg.block_size, 0) !=
            HDF_SUCCESS) {
            HDF_LOGE("%s: failed to get block_size", __func__);
            return HDF_FAILURE;
        }
        if (resource->GetUint32ArrayElem(resourceNode, "block_count", i, &fs[i].lfs_cfg.block_count, 0) !=
            HDF_SUCCESS) {
            HDF_LOGE("%s: failed to get block_count", __func__);
            return HDF_FAILURE;
        }
        HDF_LOGD("%s: fs[%d] mount_point=%s, partition=%u, block_size=%u, block_count=%u", __func__, i,
            fs[i].mount_point, (uint32_t)fs[i].lfs_cfg.context, fs[i].lfs_cfg.block_size, fs[i].lfs_cfg.block_count);
    }
    return HDF_SUCCESS;
}

void PartitionsInit(void)
{

    int lengthArray[LOSCFG_LFS_MAX_MOUNT_SIZE];
    int addrArray[LOSCFG_LFS_MAX_MOUNT_SIZE];
    int nextAddr = 0;

    for (int i = 0; i < sizeof(fs) / sizeof(fs[0]); i++) {
        lengthArray[i] = fs[i].lfs_cfg.block_count * fs[i].lfs_cfg.block_size;
        addrArray[i] = nextAddr;
        nextAddr += lengthArray[i];
    }

    (VOID) LOS_DiskPartition("flash0", "littlefs", lengthArray, addrArray, LOSCFG_LFS_MAX_MOUNT_SIZE);
}

int32_t FsDriverInit(struct HdfDeviceObject *object)
{
    struct FileOpInfo *fileOpInfo = NULL;

    if (object == NULL) {
        return HDF_FAILURE;
    }
    if (object->property) {
        if (FsGetResource(fs, object->property) != HDF_SUCCESS) {
            HDF_LOGE("%s: FsGetResource failed", __func__);
            return HDF_FAILURE;
        }
    }

    struct PartitionCfg partCfg = {.readSize = READ_SIZE,
        .writeSize = PROG_SIZE,
        .cacheSize = CACHE_SIZE,
        .partNo = 0,
        .blockCycles = BLOCK_CYCLES,
        .lookaheadSize = LOOKAHEAD_SIZE,
        .readFunc = readFunc,
        .writeFunc = writeFunc,
        .eraseFunc = eraseFunc};

    for (int i = 0; i < sizeof(fs) / sizeof(fs[0]); i++) {
        if (fs[i].mount_point == NULL) {
            continue;
        }

        partCfg.blockSize = fs[i].lfs_cfg.block_size;
        partCfg.blockCount = fs[i].lfs_cfg.block_count;

        int ret = mount(NULL, fs[i].mount_point, "littlefs", 0, &partCfg);
        HDF_LOGI("%s: mount fs on '%s' %s\n", __func__, fs[i].mount_point, (ret == 0) ? "succeed" : "failed");
        if (ret == 0) {
            ret = mkdir(fs[i].mount_point, S_IRWXU | S_IRWXG | S_IRWXO);
            if (ret == 0) {
                HDF_LOGI("create root dir success.");
            } else if (errno == EEXIST) {
                HDF_LOGI("root dir exist.");
            } else {
                HDF_LOGI("create root dir failed.");
            }
        }
    }

    return HDF_SUCCESS;
}

static int32_t FsDriverBind(struct HdfDeviceObject *device)
{
    (void)device;
    return HDF_SUCCESS;
}

static void FsDriverRelease(struct HdfDeviceObject *device)
{
    (void)device;
}

static struct HdfDriverEntry g_fsDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_FS_LITTLEFS",
    .Bind = FsDriverBind,
    .Init = FsDriverInit,
    .Release = FsDriverRelease,
};

HDF_INIT(g_fsDriverEntry);
