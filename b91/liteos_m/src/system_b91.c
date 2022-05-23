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

#include <target_config.h>

#include <los_compiler.h>

#include <B91/clock.h>
#include <B91/sys.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Define 48 MHz and 96 MHz CCLK clock options (not present in HAL) */
#define CCLK_64M_HCLK_32M_PCLK_16M \
  clock_init(PLL_CLK_192M, PAD_PLL_DIV, PLL_DIV3_TO_CCLK, \
             CCLK_DIV2_TO_HCLK, HCLK_DIV2_TO_PCLK, PLL_DIV4_TO_MSPI_CLK)

#define CCLK_96M_HCLK_48M_PCLK_24M \
  clock_init(PLL_CLK_192M, PAD_PLL_DIV, PLL_DIV2_TO_CCLK, \
             CCLK_DIV2_TO_HCLK, HCLK_DIV2_TO_PCLK, PLL_DIV4_TO_MSPI_CLK)

#if LOSCFG_TELINK_B91_CPU_FREQ == 16000000
#  define CLOCK_INIT CCLK_16M_HCLK_16M_PCLK_16M
#elif LOSCFG_TELINK_B91_CPU_FREQ == 24000000
#  define CLOCK_INIT CCLK_24M_HCLK_24M_PCLK_24M
#elif LOSCFG_TELINK_B91_CPU_FREQ == 32000000
#  define CLOCK_INIT CCLK_32M_HCLK_32M_PCLK_16M
#elif LOSCFG_TELINK_B91_CPU_FREQ == 48000000
#  define CLOCK_INIT CCLK_48M_HCLK_48M_PCLK_24M
#elif LOSCFG_TELINK_B91_CPU_FREQ == 64000000
#  define CLOCK_INIT CCLK_64M_HCLK_32M_PCLK_16M
#elif LOSCFG_TELINK_B91_CPU_FREQ == 96000000
#  define CLOCK_INIT CCLK_96M_HCLK_48M_PCLK_24M
#else /* LOSCFG_TELINK_B91_CPU_FREQ == 16000000 */
#  error Unsupported clock frequency. See LOSCFG_TELINK_B91_CPU_FREQ
#endif /* LOSCFG_TELINK_B91_CPU_FREQ == 16000000 */

/****************************************************************************
 * Public Functions
 ****************************************************************************/

#if defined(LOSCFG_POWER_MODE_LDO_1P4_LDO_1P8)
#  define POWER_MODE LDO_1P4_LDO_1P8
#elif defined(LOSCFG_POWER_MODE_DCDC_1P4_LDO_1P8)
#  define POWER_MODE DCDC_1P4_LDO_1P8
#elif defined(LOSCFG_POWER_MODE_DCDC_1P4_DCDC_1P8)
#  define POWER_MODE DCDC_1P4_DCDC_1P8
#else /* defined(LOSCFG_POWER_MODE_LDO_1P4_LDO_1P8) */
#  error Unsupported Power mode
#endif /* defined(LOSCFG_POWER_MODE_LDO_1P4_LDO_1P8) */

#if defined(LOSCFG_VBAT_TYPE_MAX_VALUE_GREATER_THAN_3V6)
#  define VBAT_TYPE VBAT_MAX_VALUE_GREATER_THAN_3V6
#elif defined(LOSCFG_VBAT_TYPE_MAX_VALUE_LESS_THAN_3V6)
#  define VBAT_TYPE VBAT_MAX_VALUE_LESS_THAN_3V6
#else /* defined(LOSCFG_VBAT_TYPE_MAX_VALUE_GREATER_THAN_3V6) */
#  error Unsupported VBat type
#endif /* defined(LOSCFG_VBAT_TYPE_MAX_VALUE_GREATER_THAN_3V6) */

VOID SystemInit(VOID)
{
    sys_init(POWER_MODE, VBAT_TYPE);
    CLOCK_INIT;

    clock_32k_init(CLK_32K_RC);
    clock_cal_32k_rc();
}
