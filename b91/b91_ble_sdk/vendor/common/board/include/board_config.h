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

#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

#include <B91/gpio.h>

#define LED_BLUE_HDF 0
#define LED_GREEN_HDF 1
#define LED_WHITE_HDF 2
#define LED_RED_HDF 3

#define SW1_2_GPIO GPIO_PC2
#define SW3_4_GPIO GPIO_PC0
#define SW1_3_GPIO GPIO_PC3
#define SW2_4_GPIO GPIO_PC1

#define SW1_2_GPIO_HDF 6
#define SW3_4_GPIO_HDF 4
#define SW1_3_GPIO_HDF 7
#define SW2_4_GPIO_HDF 5

#endif // BOARD_CONFIG_H
