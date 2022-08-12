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

#include <stdbool.h>
#include <stdio.h>

#include <securec.h>

#include <los_compiler.h>

#include "hal_hota_board.h"

#include <B91/flash.h>

#include <bootloader.h>

#define CONFIG_USE_BOOTLOADER 1

#define PAGE_MASK        (~(unsigned int)(PAGE_SIZE - 1))
#define BOOT_MARK_OFFSET 0x20

#define OTA_PARTITION_SIZE (1024 * 64 * 8)
#define OTA_PARTITONS_START OTA_PARTITION_SIZE

#define RUNNING_PARTITION_ADDR 0

static struct {
    size_t flash_size;
    size_t first_invalid_byte;
    int n_partition_started;
    size_t partition0_addr;
    size_t cur_partition_addr;
    UpdateMetaData metaData;
} g_ota_state;

static const char mypublic_pem[] = "-----BEGIN PUBLIC KEY-----\n"
                                   "MFwwDQYJKoZIhvcNAQEBBQADSwAwSAJBAL4tQSooTP+Bn2LDxnBzUehsoYERgsus\n"
                                   "mthynMi1HPEzzQJX3lsaFmGgMQMeUiMkfs8+e6YF4iKhNkpvf09WIWECAwEAAQ==\n"
                                   "-----END PUBLIC KEY-----";

int HotaHalInit(void)
{
    g_ota_state.n_partition_started = -1;

    g_ota_state.cur_partition_addr = (1024 * 128) * (reg_mspi_set_l & 0x07);

    if (g_ota_state.cur_partition_addr) {
        g_ota_state.partition0_addr = 0;
    } else {
        g_ota_state.partition0_addr = OTA_PARTITONS_START;
    }

    UINT8 buf[4];
    flash_read_mid(buf);
    g_ota_state.flash_size = 1 << buf[2];
    g_ota_state.first_invalid_byte =
        ((g_ota_state.flash_size - OTA_PARTITONS_START) / OTA_PARTITION_SIZE) * OTA_PARTITION_SIZE +
        OTA_PARTITONS_START;
    printf("flash_size: %uk first_invalid_byte: %u\r\n", g_ota_state.flash_size / 1024,
           g_ota_state.first_invalid_byte);

    return OHOS_SUCCESS;
}

int HotaHalGetUpdateIndex(unsigned int *index)
{
    printf(" === %s:%d\r\n", __func__, __LINE__);
    return OHOS_SUCCESS;
}

int HotaHalDeInit(void)
{
    if (g_ota_state.n_partition_started >= 0) {
        size_t addr = OTA_PARTITONS_START + OTA_PARTITION_SIZE * g_ota_state.n_partition_started;
        size_t next_part = addr + OTA_PARTITION_SIZE;
        while (addr < next_part) {
            flash_erase_64kblock(addr);
            addr += 64 * 1024;
        }
        g_ota_state.n_partition_started = -1;
        printf(" === %s:%d\r\n", __func__, __LINE__);
    } else {
        printf(" === %s:%d\r\n", __func__, __LINE__);
        return OHOS_FAILURE;
    }

    return OHOS_SUCCESS;
}

int HotaHalRead(int partition, unsigned int offset, unsigned int bufLen, unsigned char *buffer)
{
    if ((buffer == NULL) || (bufLen == 0)) {
        return OHOS_FAILURE;
    }

    unsigned int start =
        ((0 == partition) ? g_ota_state.partition0_addr : OTA_PARTITONS_START + OTA_PARTITION_SIZE * partition) +
        offset;
    unsigned int end = start + bufLen;
    if ((start > g_ota_state.first_invalid_byte) || (end > g_ota_state.first_invalid_byte)) {
        printf("ERROR: Exceed flash size start: %u end: %u\r\n", start, end);
        return OHOS_FAILURE;
    }

    memset(buffer, 0, bufLen);
    flash_read_page(start, bufLen, buffer);
    printf(" === %s:%d\r\n", __func__, __LINE__);

    return OHOS_SUCCESS;
}

_attribute_ram_code_sec_ void OtaPartitionClear(unsigned addr)
{
    for (unsigned int end = addr + OTA_PARTITION_SIZE; addr < end; addr += 64 * 1024) {
        flash_erase_64kblock_ram(addr);
    }
}

int HotaHalWrite(int partition, unsigned char *buffer, unsigned int offset, unsigned int bufLen)
{
    if (partition == PARTITION_INFO_COMP) {
        printf("partition == PARTITION_INFO_COMP, skip it.");
        return OHOS_SUCCESS;
    }

    unsigned int start =
        ((0 == partition) ? g_ota_state.partition0_addr : OTA_PARTITONS_START + OTA_PARTITION_SIZE * partition) +
        offset;

    if (-1 == g_ota_state.n_partition_started) {
        g_ota_state.n_partition_started = partition;
        OtaPartitionClear(start & (~(OTA_PARTITION_SIZE - 1)));
    }

    unsigned int end = start + bufLen;
    if ((start > g_ota_state.first_invalid_byte) || (end > g_ota_state.first_invalid_byte)) {
        printf("ERROR: Exceed flash size start: %u end: %u\r\n", start, end);
        return OHOS_FAILURE;
    }

    flash_write_page(start, bufLen, buffer);

    return OHOS_SUCCESS;
}
_attribute_ram_code_sec_ bool FlashCheckPageIsFree(unsigned int addr, void *workbuf)
{
    //    const UINT32 *page_end = (addr + PAGE_SIZE) & PAGE_MASK;
    //    size_t sz = (-addr) % PAGE_SIZE;
    addr &= PAGE_MASK;
    flash_read_page_ram(addr, PAGE_SIZE, workbuf);

    for (const UINT32 *p = workbuf, *page_end = p + (PAGE_SIZE / sizeof(*p)); p < page_end; ++p) {
        if (*p != 0xFFFFFFFFu) {
            return false;
        }
    }

    return true;
}

_attribute_ram_code_sec_ STATIC void FlashCopyPage(unsigned int dst, unsigned int src, void *workbuf)
{
    if (false == FlashCheckPageIsFree(dst, workbuf)) {
        flash_erase_page_ram(dst);
    }

    flash_read_page_ram(src, PAGE_SIZE, workbuf);
    flash_write_page_ram(dst, PAGE_SIZE, workbuf);
}

_attribute_ram_code_sec_ STATIC void OtaPartitionCopy(unsigned int dst, unsigned int src, void *workbuf)
{
    for (unsigned int addr = 0; addr < OTA_PARTITION_SIZE; addr += PAGE_SIZE) {
        FlashCopyPage(dst + addr, src + addr, workbuf);
    }
}

_attribute_ram_code_sec_ STATIC void flash_write_data(unsigned long addr, unsigned long len, const void *data,
                                                      unsigned char *buf)
{
    unsigned int page_addr = addr & PAGE_MASK;
    flash_read_page_ram(page_addr, PAGE_SIZE, buf);
    memcpy(&buf[(addr % PAGE_SIZE)], data, len);
    flash_erase_page_ram(page_addr);
    flash_write_page_ram(page_addr, PAGE_SIZE, buf);
}

_attribute_ram_code_sec_ STATIC int HotaBootNewImpl(void)
{
    void *buf = malloc(PAGE_SIZE);

    if (NULL == buf) {
        return OHOS_FAILURE;
    }

    core_interrupt_disable();

    __asm__ volatile("csrci mmisc_ctl, 8");  // disable BTB

    unsigned int new_partition_addr =
        ((0 == g_ota_state.n_partition_started)
             ? g_ota_state.partition0_addr
             : OTA_PARTITONS_START + OTA_PARTITION_SIZE * g_ota_state.n_partition_started);

    flash_write_data(new_partition_addr + BOOT_MARK_OFFSET, 4, "KNLT", buf);
    flash_write_data(g_ota_state.cur_partition_addr + BOOT_MARK_OFFSET, 4, "\0\0\0\0", buf);

    sys_reboot();

    while (1) {
        _ASM_NOP_;
    }

    __builtin_unreachable();
}

int HotaHalRestart(void)
{
    if (-1 == g_ota_state.n_partition_started) {
        return OHOS_FAILURE;
    }

    printf(" === %s:%d\r\n", __func__, __LINE__);

    return HotaBootNewImpl();
}

int HotaHalSetBootSettings(void)
{
    printf(" === %s:%d\r\n", __func__, __LINE__);

    return OHOS_SUCCESS;
}

_attribute_ram_code_sec_ int HotaBootRoollbackImpl(void)
{
    if (-1 != g_ota_state.n_partition_started) {
        return OHOS_FAILURE;
    }

    void *buf = malloc(PAGE_SIZE);

    if (NULL == buf) {
        return OHOS_FAILURE;
    }

    unsigned int r = core_interrupt_disable();

    __asm__ volatile("csrci mmisc_ctl, 8");  // disable BTB

    unsigned int rollback_partition_addr = g_ota_state.partition0_addr;

    if (true == FlashCheckPageIsFree(rollback_partition_addr, buf)) {
        __asm__ volatile("csrsi mmisc_ctl, 8");  // enable BTB
        core_restore_interrupt(r);
        free(buf);
        return OHOS_FAILURE;
    }

    flash_write_data(rollback_partition_addr + BOOT_MARK_OFFSET, 4, "KNLT", buf);
    flash_write_data(g_ota_state.cur_partition_addr + BOOT_MARK_OFFSET, 4, "\0\0\0\0", buf);

    sys_reboot();

    while (1) {
        _ASM_NOP_;
    }

    __builtin_unreachable();
}

int HotaHalRollback(void)
{
    printf(" === %s:%d\r\n", __func__, __LINE__);
    return HotaBootRoollbackImpl();
}

const ComponentTableInfo *HotaHalGetPartitionInfo()
{
    printf(" === %s:%d\r\n", __func__, __LINE__);
    return NULL;
}

unsigned char *HotaHalGetPubKey(unsigned int *length)
{
    if (length == NULL) {
        printf("OTA pub key is NULL.\r\n");
        return NULL;
    }

    printf(" === %s:%d\r\n", __func__, __LINE__);

    *length = sizeof(mypublic_pem);
    return mypublic_pem;
}

int HotaHalGetUpdateAbility(void)
{
    printf(" === %s:%d\r\n", __func__, __LINE__);
    return 0;
}

int HotaHalGetOtaPkgPath(char *path, int len)
{
    printf(" === %s:%d\r\n", __func__, __LINE__);
    return OHOS_FAILURE;
}

int HotaHalIsDeviceCanReboot(void)
{
    return 1;
}

int HotaHalGetMetaData(UpdateMetaData *metaData)
{
    *metaData = g_ota_state.metaData;
    return OHOS_SUCCESS;
}

int HotaHalSetMetaData(UpdateMetaData *metaData)
{
    g_ota_state.metaData = *metaData;
    return OHOS_SUCCESS;
}

int HotaHalRebootAndCleanUserData(void)
{
    return OHOS_SUCCESS;
}

int HotaHalRebootAndCleanCache(void)
{
    return OHOS_SUCCESS;
}

int HotaHalCheckVersionValid(const char *currentVersion, const char *pkgVersion, unsigned int pkgVersionLength)
{
    return (strncmp(currentVersion, pkgVersion, pkgVersionLength) == 0) ? 1 : 0;
}
