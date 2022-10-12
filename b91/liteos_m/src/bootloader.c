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

#include <B91/flash.h>

#include <bootloader.h>

#define FLASH_START 0x20000000u

#define META_DATA_ADDR  (1024 * 1024 - PAGE_SIZE)

typedef struct {
    UINT32 run_addr;
} ota_meta_data_t;

#define META_DATA ((ota_meta_data_t *)(META_DATA_ADDR + FLASH_START))

void bl_main(void)
{
    UINT32 run_addr = META_DATA->run_addr;

    if (run_addr == 0xFFFFFFFFu) {
        return;
    }

    unsigned int pc;
    __asm__("auipc t0, 0\n"
            "mv %0, t0" : "=r"(pc));

    unsigned cur_part = (pc - FLASH_START) / OTA_PARTITION_SIZE;
    unsigned needed_part = run_addr / OTA_PARTITION_SIZE;

    if (cur_part == needed_part) {
        return;
    }

    __attribute__((noreturn)) void (*f)(void) = (void *)(run_addr + FLASH_START);
    f();

    __builtin_unreachable();
}

_attribute_ram_code_sec_ void bl_save_run_address(UINT32 addr)
{
    ota_meta_data_t data;
    data.run_addr = addr;
    if (*(volatile unsigned int *)(FLASH_START + META_DATA_ADDR) != 0xFFFFFFFFu) {
        flash_erase_page_ram(META_DATA_ADDR);
    }
    flash_write_page_ram(META_DATA_ADDR, sizeof(data), (unsigned char *)&data);
}
