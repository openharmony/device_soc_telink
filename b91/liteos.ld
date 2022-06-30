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

MEMORY
{
  FLASH   (rxai!w) : ORIGIN = 0x20000000, LENGTH = 1M
  RAM_ILM (wxa)    : ORIGIN = 0x00000000, LENGTH = 128K
  RAM_DLM (wxa)    : ORIGIN = 0x00080000, LENGTH = 128K
}

ENTRY(reset_vector)
EXTERN(_b91_inject_start)

SECTIONS
{
  PROVIDE (BIN_BEGIN = ORIGIN(FLASH));

  PROVIDE(__text_start = .);
  .entry.text : ALIGN(8)
  {
    KEEP(*(.entry.text))
  } > FLASH

  ._b91_inject_start ORIGIN(FLASH) + 0x18 :
  {
    KEEP(*(._b91_inject_start))
  } > FLASH

  .retention_data : ALIGN(8)
  {
    KEEP(*(.retention_data ))
    . = .;
  } > RAM_ILM AT > FLASH

  PROVIDE (_RETENTION_DATA_VMA_START = ADDR(.retention_data));
  PROVIDE (_RETENTION_DATA_LMA_START = LOADADDR(.retention_data));
  PROVIDE (_RETENTION_DATA_VMA_END = .);

  .ram_code : ALIGN(8)
  {
    KEEP(*(.ram_code ))
    . = .;
  } > RAM_ILM AT > FLASH

  PROVIDE (_RAMCODE_VMA_START = ADDR(.ram_code));
  PROVIDE (_RAMCODE_LMA_START = LOADADDR(.ram_code));
  PROVIDE (_RAMCODE_VMA_END = .);

  .text : ALIGN(8)
  {
    *(.text .stub .text.* .gnu.linkonce.t.* )
    *(.interrupt.text )
    *(.interrupt.text.* )
    *(.interrupt.HalTrapVector.text )
    KEEP(*(.text.*personality* ))
    *(.text.unlikely .text.unlikely.*)
    KEEP(*(.itcm.text))
    PROVIDE(__rodata_start = .);
    *(.gnu.warning )
    *(.rodata .rodata.* .gnu.linkonce.r.* )
    PROVIDE(__rodata_end = .);
    *(.gcc_except_table .gcc_except_table.*)
    *(.eh_frame_hdr )
    . = ALIGN(0x20);
    KEEP(*(.eh_frame ))
    KEEP(*(.exec.itable))

    KEEP (*(.init))
    KEEP (*(.fini))

    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array))
    PROVIDE_HIDDEN (__preinit_array_end = .);

    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))
    KEEP (*(.init_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .ctors))
    KEEP (*crtbegin.o(.ctors))
    KEEP (*crtbegin?.o(.ctors))
    KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*(.ctors))
    PROVIDE_HIDDEN (__init_array_end = .);

    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT_BY_INIT_PRIORITY(.fini_array.*) SORT_BY_INIT_PRIORITY(.dtors.*)))
    KEEP (*(.fini_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .dtors))
    PROVIDE_HIDDEN (__fini_array_end = .);

    PROVIDE(_hdf_drivers_start = .);
    KEEP(*(.hdf.driver))
    PROVIDE(_hdf_drivers_end = .);

    KEEP (*crtbegin.o(.dtors))
    KEEP (*crtbegin?.o(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))

  } > FLASH AT > FLASH

  /* zInit code and data - will be freed after init */
  .zInit :  ALIGN(8)
  {
    __zinitcall_bsp_start = .;
    KEEP (*(.zinitcall.bsp0.init))
    KEEP (*(.zinitcall.bsp1.init))
    KEEP (*(.zinitcall.bsp2.init))
    KEEP (*(.zinitcall.bsp3.init))
    KEEP (*(.zinitcall.bsp4.init))
    __zinitcall_bsp_end = .;
    __zinitcall_device_start = .;
    KEEP (*(.zinitcall.device0.init))
    KEEP (*(.zinitcall.device1.init))
    KEEP (*(.zinitcall.device2.init))
    KEEP (*(.zinitcall.device3.init))
    KEEP (*(.zinitcall.device4.init))
    __zinitcall_device_end = .;
    __zinitcall_core_start = .;
    KEEP (*(.zinitcall.core0.init))
    KEEP (*(.zinitcall.core1.init))
    KEEP (*(.zinitcall.core2.init))
    KEEP (*(.zinitcall.core3.init))
    KEEP (*(.zinitcall.core4.init))
    __zinitcall_core_end = .;
    __zinitcall_sys_service_start = .;
    KEEP (*(.zinitcall.sys.service0.init))
    KEEP (*(.zinitcall.sys.service1.init))
    KEEP (*(.zinitcall.sys.service2.init))
    KEEP (*(.zinitcall.sys.service3.init))
    KEEP (*(.zinitcall.sys.service4.init))
    __zinitcall_sys_service_end = .;
    __zinitcall_sys_feature_start = .;
    KEEP (*(.zinitcall.sys.feature0.init))
    KEEP (*(.zinitcall.sys.feature1.init))
    KEEP (*(.zinitcall.sys.feature2.init))
    KEEP (*(.zinitcall.sys.feature3.init))
    KEEP (*(.zinitcall.sys.feature4.init))
    __zinitcall_sys_feature_end = .;
    __zinitcall_run_start = .;
    KEEP (*(.zinitcall.run0.init))
    KEEP (*(.zinitcall.run1.init))
    KEEP (*(.zinitcall.run2.init))
    KEEP (*(.zinitcall.run3.init))
    KEEP (*(.zinitcall.run4.init))
    __zinitcall_run_end = .;
    __zinitcall_app_service_start = .;
    KEEP (*(.zinitcall.app.service0.init))
    KEEP (*(.zinitcall.app.service1.init))
    KEEP (*(.zinitcall.app.service2.init))
    KEEP (*(.zinitcall.app.service3.init))
    KEEP (*(.zinitcall.app.service4.init))
    __zinitcall_app_service_end = .;
    __zinitcall_app_feature_start = .;
    KEEP (*(.zinitcall.app.feature0.init))
    KEEP (*(.zinitcall.app.feature1.init))
    KEEP (*(.zinitcall.app.feature2.init))
    KEEP (*(.zinitcall.app.feature3.init))
    KEEP (*(.zinitcall.app.feature4.init))
    __zinitcall_app_feature_end = .;
    __zinitcall_test_start = .;
    KEEP (*(.zinitcall.test0.init))
    KEEP (*(.zinitcall.test1.init))
    KEEP (*(.zinitcall.test2.init))
    KEEP (*(.zinitcall.test3.init))
    KEEP (*(.zinitcall.test4.init))
    __zinitcall_test_end = .;
    __zinitcall_exit_start = .;
    KEEP (*(.zinitcall.exit0.init))
    KEEP (*(.zinitcall.exit1.init))
    KEEP (*(.zinitcall.exit2.init))
    KEEP (*(.zinitcall.exit3.init))
    KEEP (*(.zinitcall.exit4.init))
    __zinitcall_exit_end = .;
  } > FLASH AT > FLASH

  PROVIDE(__text_end = .);

  .data : ALIGN(8)
  {
    PROVIDE (__global_pointer$ = . + (4K / 2));
    . += 8; /* Workaround because linker don't use global pointer (GP) on extreme edge */
    *(.data .data.* .gnu.linkonce.d.* )
    KEEP(*( SORT (.liteos.table.*)));
    KEEP(*(.gnu.linkonce.d.*personality* ))
    SORT(CONSTRUCTORS)
    *(.srodata.cst16 )
    *(.srodata.cst8 )
    *(.srodata.cst4 )
    *(.srodata.cst2 )
    *(.srodata .srodata.* )
    *(.sdata .sdata.* .gnu.linkonce.s.* )
    *(.sdata2 .sdata2.* .gnu.linkonce.s.* )
  } > RAM_DLM AT > FLASH

  PROVIDE(_DATA_VMA_START = ADDR(.data));
  PROVIDE(_DATA_LMA_START = LOADADDR(.data));
  PROVIDE(_DATA_VMA_END = .);
  PROVIDE(__ram_data_start = _DATA_VMA_START);
  PROVIDE(__ram_data_end = _DATA_VMA_END);

  .bss (NOLOAD) : ALIGN(8)
  {
    *(.dynsbss )
    *(.sbss .sbss.* .gnu.linkonce.sb.* )
    *(.scommon .scommon.* )
    *(.dynbss )
    *(.bss .bss.* .gnu.linkonce.b.* )
    *(COMMON )
  } > RAM_DLM

  PROVIDE (_BSS_VMA_START = ADDR(.bss));
  PROVIDE (_BSS_VMA_END = .);
  PROVIDE(__bss_start = _BSS_VMA_START);
  PROVIDE(__bss_end = _BSS_VMA_END);

  .noinit (NOLOAD) : ALIGN(16)
  {
    *(.noinit)
  } > RAM_DLM

  .int_stack (NOLOAD) : ALIGN(0x1000)
  {
    __int_stack_start = .;
    *(.int_stack);
    __int_stack_end = .;
  } > RAM_DLM

  . = ALIGN(8);
  /* end is the starting address of the heap, the heap grows upward */
  _end = .;
  _heap_end = ORIGIN(RAM_DLM) + LENGTH(RAM_DLM) - 1;

  .heap : ALIGN(1024)
  {
  PROVIDE (__los_heap_addr_start__ = .);
  . = ORIGIN(RAM_ILM) + LENGTH(RAM_ILM);
  PROVIDE (__los_heap_addr_end__ = . - 1);
  } > RAM_ILM
  PROVIDE (__los_heap_size__ = __los_heap_addr_end__ - __los_heap_addr_start__ + 1);

/*  PROVIDE (__los_heap_addr_start__ = .);
  . = ORIGIN(RAM_DLM) + LENGTH(RAM_DLM);
  PROVIDE (__los_heap_addr_end__ = . - 1);
  PROVIDE (__los_heap_size__ = __los_heap_addr_end__ - __los_heap_addr_start__ + 1);*/

  .BIN_END :
  {
    BIN_END = .;
  } > FLASH
  PROVIDE (BIN_SIZE = BIN_END - BIN_BEGIN);

  /* Remove information from the standard libraries 0x400 */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
    *(.dummy)
  }
}
