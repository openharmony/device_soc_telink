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

.global _b91_inject_start
.section ._b91_inject_start, "a"
_b91_inject_start:
    .org 0x08
    .word ('T'<<24 | 'L'<<16 | 'N'<<8 | 'K')

    .org 0x0e
    .short (0x173B) /* DREAD:   cmd:1x, addr:1x, data:2x, dummy:8 */
    .align 2
