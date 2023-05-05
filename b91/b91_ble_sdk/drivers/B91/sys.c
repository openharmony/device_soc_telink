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
#include "sys.h"
#include "analog.h"
#include "compiler.h"
#include "core.h"
#include "gpio.h"
#include "mspi.h"
#include "pm.h"
#include "stimer.h"

unsigned int g_chip_version = 0;

extern void pm_update_status_info(void);

/**
 * @brief      This function performs a series of operations of writing digital or analog registers
 *             according to a command table
 * @param[in]  pt - pointer to a command table containing several writing commands
 * @param[in]  size  - number of commands in the table
 * @return     number of commands are carried out
 */

int write_reg_table(const tbl_cmd_set_t *pt, int size)
{
    int l = 0;

    while (l < size) {
        unsigned int cadr = ((unsigned int)0x80000000) | pt[l].adr;
        unsigned char cdat = pt[l].dat;
        unsigned char ccmd = pt[l].cmd;
        unsigned char cvld = (ccmd & TCMD_UNDER_WR);
        ccmd &= TCMD_MASK;
        if (cvld) {
            if (ccmd == TCMD_WRITE) {
                write_reg8(cadr, cdat);
            } else if (ccmd == TCMD_WAREG) {
                analog_write_reg8(cadr, cdat);
            } else if (ccmd == TCMD_WAIT) {
                delay_us(pt[l].adr * 256 + cdat);
            }
        }
        l++;
    }
    return size;
}

/**********************************************************************************************************************
 *                    						local function implementation                                             *
 *********************************************************************************************************************/
