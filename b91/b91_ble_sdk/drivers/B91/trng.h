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
/**	@page TRNG
 *
 *	Introduction
 *	===============
 *	TLSRB91 supports hardware trng.
 *
 *	API Reference
 *	===============
 *	Header File: trng.h
 */
#ifndef TRNG_H_
#define TRNG_H_

#include "reg_include/register_b91.h"
#include "stimer.h"
#include "sys.h"

/**********************************************************************************************************************
 *                                         global constants                                                           *
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *                                           global macro                                                             *
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *                                         global data type                                                           *
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *                                     global variable declaration                                                    *
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *                                      global function prototype                                                     *
 *********************************************************************************************************************/

/**
 * @brief     This function performs to get one random number.If chip in suspend TRNG module should be close.
 *            else its current will be larger.
 * @return    none
 **/
void trng_init(void);

/**
 * @brief     This function performs to get one random number.
 * @return    the value of one random number
 **/
unsigned int trng_rand(void);

#endif
