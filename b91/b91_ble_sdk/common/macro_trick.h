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
#ifndef COMMON_MACRO_TRICK_H_
#define COMMON_MACRO_TRICK_H_

#pragma once

/////////////////  variadic macro  ////////////////////////

// a little more complex version that works with GCC and visual studio

///   http://stackoverflow.com/questions/9183993/msvc-variadic-macro-expansion
#define COUNT_ARGS_IMPL2(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20,   \
                         N, ...)                                                                                      \
    N
#define COUNT_ARGS_IMPL(args) COUNT_ARGS_IMPL2 args
#define COUNT_ARGS(...)                                                                                               \
    COUNT_ARGS_IMPL((__VA_ARGS__, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0))

#define MACRO_CHOOSE_HELPER2(base, count) base##count
#define MACRO_CHOOSE_HELPER1(base, count) MACRO_CHOOSE_HELPER2(base, count)
#define MACRO_CHOOSE_HELPER(base, count)  MACRO_CHOOSE_HELPER1(base, count)

#define MACRO_GLUE(x, y)  x y
#define VARARG(base, ...) MACRO_GLUE(MACRO_CHOOSE_HELPER(base, COUNT_ARGS(__VA_ARGS__)), (__VA_ARGS__))
// usage
/*
 *	#define  fun1(a)		xxxx
 *	#define  fun2(a, b)		xxxx
 *	#define  fun3(a, b, c)	xxxx
 *
 *	#define fun(...)		VARARG(fun,  __VA_ARGS__)
 *
 *	int main(){
 *		fun(1);			// calls fun1(1)
 *		fun(1, 2);			// calls fun2(1,2)
 *		fun(1, 2, 3);		// calls fun3(1,2,3)
 *	}
 *
 */

/////////////////  end of variadic macro  //////////////////////

#endif /* COMMON_MACRO_TRICK_H_ */
