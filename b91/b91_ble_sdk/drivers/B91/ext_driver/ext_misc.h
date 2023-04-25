/********************************************************************************************************
 * @file	ext_misc.h
 *
 * @brief	This is the header file for B91
 *
 * @author	BLE Group
 * @date	2020
 *
 * @par     Copyright (c) 2020, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
 *
 *          Redistribution and use in source and binary forms, with or without
 *          modification, are permitted provided that the following conditions are met:
 *
 *              1. Redistributions of source code must retain the above copyright
 *              notice, this list of conditions and the following disclaimer.
 *
 *              2. Unless for usage inside a TELINK integrated circuit, redistributions
 *              in binary form must reproduce the above copyright notice, this list of
 *              conditions and the following disclaimer in the documentation and/or other
 *              materials provided with the distribution.
 *
 *              3. Neither the name of TELINK, nor the names of its contributors may be
 *              used to endorse or promote products derived from this software without
 *              specific prior written permission.
 *
 *              4. This software, with or without modification, must only be used with a
 *              TELINK integrated circuit. All other usages are subject to written permission
 *              from TELINK and different commercial license may apply.
 *
 *              5. Licensee shall be solely responsible for any claim to the extent arising out of or
 *              relating to such deletion(s), modification(s) or alteration(s).
 *
 *          THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *          ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *          WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *          DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
 *          DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *          (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *          LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *          ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *          (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *          SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************************************/

#ifndef DRIVERS_B91_EXT_MISC_H_
#define DRIVERS_B91_EXT_MISC_H_

#include "nds_intrinsic.h"

#include "../analog.h"
#include "../clock.h"
#include "../dma.h"
#include "../flash.h"
#include "../gpio.h"
#include "../mdec.h"
#include "../plic.h"
#include "../pm.h"
#include "../stimer.h"
#include "../sys.h"
#include "../timer.h"
#include "../trng.h"
#include "../uart.h"
#include "compiler.h"
#include "types.h"

/* for debug */
#define DBG_SRAM_ADDR 0x00014

/*
 * addr - only 0x00012 ~ 0x00021 can be used !!! */
#define write_dbg32(addr, value) write_sram32(addr, value)

#define write_log32(err_code) write_sram32(0x00014, err_code)

/******************************* stimer_start ******************************************************************/
#define SYSTICK_NUM_PER_US  16
#define reg_system_tick_irq reg_system_irq_level

typedef enum {
    STIMER_IRQ_MASK = BIT(0),
    STIMER_32K_CAL_IRQ_MASK = BIT(1),
} stimer_irq_mask_e;

typedef enum {
    FLD_IRQ_SYSTEM_TIMER = BIT(0),
} system_timer_irq_mask_e;

typedef enum {
    STIMER_IRQ_CLR = BIT(0),
    STIMER_32K_CAL_IRQ_CLR = BIT(1),
} stimer_irq_clr_e;

/**
 * @brief    This function serves to enable system timer interrupt.
 * @return  none
 */
static inline void systimer_irq_enable(void)
{
    reg_irq_src0 |= BIT(IRQ1_SYSTIMER);
    // plic_interrupt_enable(IRQ1_SYSTIMER);
}

/**
 * @brief    This function serves to disable system timer interrupt.
 * @return  none
 */
static inline void systimer_irq_disable(void)
{
    reg_irq_src0 &= ~BIT(IRQ1_SYSTIMER);
    // plic_interrupt_disable(IRQ1_SYSTIMER);
}

static inline void systimer_set_irq_mask(void)
{
    reg_system_irq_mask |= STIMER_IRQ_MASK;
}

static inline void systimer_clr_irq_mask(void)
{
    reg_system_irq_mask &= (~STIMER_IRQ_MASK);
}

static inline unsigned char systimer_get_irq_status(void)
{
    return reg_system_cal_irq & FLD_IRQ_SYSTEM_TIMER;
}

static inline void systimer_clr_irq_status(void)
{
    reg_system_cal_irq = STIMER_IRQ_CLR;
}

static inline void systimer_set_irq_capture(unsigned int tick)
{
    reg_system_irq_level = tick;
}

static inline unsigned int systimer_get_irq_capture(void)
{
    return reg_system_irq_level;
}

static inline int tick1_exceed_tick2(unsigned int tick1, unsigned int tick2)
{
    return (unsigned int)(tick1 - tick2) < BIT(30);
}
/******************************* stimer_end ********************************************************************/

/******************************* aes_start ******************************************************************/
extern unsigned int aes_data_buff[8];
/******************************* aes_end ********************************************************************/

/******************************* core_start ******************************************************************/
#define irq_disable     core_interrupt_disable
#define irq_enable      core_interrupt_enable
#define irq_restore(en) core_restore_interrupt(en)
/******************************* core_end ********************************************************************/

/******************************* analog_start ******************************************************************/
#define analog_write analog_write_reg8
#define analog_read  analog_read_reg8

/******************************* analog_end ********************************************************************/

/******************************* clock_start ******************************************************************/
typedef enum {
    SYSCLK_16M = 16,
    SYSCLK_24M = 24,
    SYSCLK_32M = 32,
    SYSCLK_48M = 48,
    SYSCLK_64M = 64,
} sys_clk_fre_t;

static inline unsigned char clock_get_system_clk()
{
    return sys_clk.cclk;
}
/******************************* clock_end ********************************************************************/

/******************************* trng_start ******************************************************************/
#define rand                  trng_rand
#define random_generator_init trng_init

/**
 * @brief     This function performs to generate a series of random numbers
 * @param[in]  len - data length
 * @param[out] data - data pointer
 * @return    none
 **/
void generateRandomNum(int len, unsigned char *data);

/******************************* trng_end ********************************************************************/

/******************************* sys_start ******************************************************************/
#define sleep_us(x) delay_us(x)
#define sleep_ms(x) delay_ms(x)

/******************************* sys_end ********************************************************************/

/******************************* dma_start ***************************************************************/

/**
 * @brief	ACL RX Data buffer length = maxRxOct + 21, then 16 Byte align
 *			maxRxOct + 21 = 4(DMA_len) + 2(BLE header) + maxRxOct + 4(MIC) + 3(CRC) + 8(ExtraInfor)
			RX buffer size must be be 16*n, due to MCU design
 */
#define CAL_LL_ACL_RX_FIFO_SIZE(maxRxOct) (((maxRxOct + 21) + 15) / 16 * 16)

/**
 * @brief	ACL TX Data buffer length = maxTxOct + 10, then 16 Byte align
 *			maxTxOct + 10 = 4(DMA_len) + 2(BLE header) + maxTxOct + 4(MIC)
			TX buffer size must be be 16*n, due to MCU design
 */
#define CAL_LL_ACL_TX_FIFO_SIZE(maxTxOct) (((maxTxOct + 10) + 15) / 16 * 16)

/* HCI TX RX buffer len = uart_fifo+ dma 4byte */
#define HCI_FIFO_SIZE(n) (((n + 2 + 4) + 15) / 16 * 16)

/*
 * @brief	ISO RX Data buffer length = ISORxOct + 21, then 16 Byte align
 *			ISORxOct + 21 = 4(DMA_len) + 2(BLE header) + ISORxOct + 4(MIC) + 3(CRC) + 8(ExtraInfor)
 *			RX buffer size must be be 16*n, due to MCU design
 */
#define CAL_LL_ISO_RX_FIFO_SIZE(n) (((n + 21) + 15) / 16 * 16)

/*
 * @brief	ISO TX Data buffer length = ISOTxOct + 10, then 16 Byte align
 * 			ISORxOct + 10 = 4(DMA_len) + 2(BLE header) + ISOTxOct + 4(MIC)
 *			TX buffer size must be be 16*n, due to MCU design
 */
#define CAL_LL_ISO_TX_FIFO_SIZE(n) (((n + 10) + 15) / 16 * 16)

/*
* DMA_LEN(4B)+Hdr(2B)+PLD(251B)+MIC(4B)+CRC(3B)+TLK_PKT_INFO(12B)
*             **use 2B enough**
*/
#define ISO_BIS_RX_PDU_SIZE_ALLIGN16(n) (((n + 25) + 15) / 16 * 16)  // 4+2+4+2+4+3+12

// 12 = 4(struct bis_rx_pdu_tag	*next) + 4(u32 payloadNum) + 4(u32 idealPldAnchorTick) in bis_rx_pdu_t
#define BIS_LL_RX_PDU_FIFO_SIZE(n) (CAL_LL_ISO_RX_FIFO_SIZE(n) + 12)

/******************************* dma_end ********************************************************************/

/******************************* plic_start ******************************************************************/
enum {  // todo
    FLD_IRQ_EXCEPTION_EN,
    FLD_IRQ_SYSTIMER_EN,
    FLD_IRQ_ALG_EN,
    FLD_IRQ_TIMER1_EN,
    FLD_IRQ_TIMER0_EN,
    FLD_IRQ_DMA_EN,
    FLD_IRQ_BMC_EN,
    FLD_IRQ_USB_CTRL_EP_SETUP_EN,
    FLD_IRQ_USB_CTRL_EP_DATA_EN,
    FLD_IRQ_USB_CTRL_EP_STATUS_EN,
    FLD_IRQ_USB_CTRL_EP_SETINF_EN,
    FLD_IRQ_USB_ENDPOINT_EN,
    FLD_IRQ_ZB_DM_EN,
    FLD_IRQ_ZB_BLE_EN,
    FLD_IRQ_ZB_BT_EN,
    FLD_IRQ_ZB_RT_EN,
    FLD_IRQ_PWM_EN,
    FLD_IRQ_PKE_EN,  // add
    FLD_IRQ_UART1_EN,
    FLD_IRQ_UART0_EN,
    FLD_IRQ_DFIFO_EN,
    FLD_IRQ_I2C_EN,
    FLD_IRQ_SPI_APB_EN,
    FLD_IRQ_USB_PWDN_EN,
    FLD_IRQ_EN,
    FLD_IRQ_GPIO2RISC0_EN,
    FLD_IRQ_GPIO2RISC1_EN,
    FLD_IRQ_SOFT_EN,

    FLD_IRQ_NPE_BUS0_EN,
    FLD_IRQ_NPE_BUS1_EN,
    FLD_IRQ_NPE_BUS2_EN,
    FLD_IRQ_NPE_BUS3_EN,
    FLD_IRQ_NPE_BUS4_EN,

    FLD_IRQ_USB_250US_EN,
    FLD_IRQ_USB_RESET_EN,
    FLD_IRQ_NPE_BUS7_EN,
    FLD_IRQ_NPE_BUS8_EN,

    FLD_IRQ_NPE_BUS13_EN = 42,
    FLD_IRQ_NPE_BUS14_EN,
    FLD_IRQ_NPE_BUS15_EN,

    FLD_IRQ_NPE_BUS17_EN = 46,

    FLD_IRQ_NPE_BUS21_EN = 50,
    FLD_IRQ_NPE_BUS22_EN,
    FLD_IRQ_NPE_BUS23_EN,
    FLD_IRQ_NPE_BUS24_EN,
    FLD_IRQ_NPE_BUS25_EN,
    FLD_IRQ_NPE_BUS26_EN,
    FLD_IRQ_NPE_BUS27_EN,
    FLD_IRQ_NPE_BUS28_EN,
    FLD_IRQ_NPE_BUS29_EN,
    FLD_IRQ_NPE_BUS30_EN,
    FLD_IRQ_NPE_BUS31_EN,

    FLD_IRQ_NPE_COMB_EN,
    FLD_IRQ_PM_TM_EN,
    FLD_IRQ_EOC_EN,

};

/******************************* plic_end ********************************************************************/

/******************************* flash_start *****************************************************************/
/**
 * @brief     flash capacity definition
 * Call flash_read_mid function to get the size of flash capacity.
 * Example is as follows:
 * unsigned char temp_buf[4];
 * flash_read_mid(temp_buf);
 * The value of temp_buf[2] reflects flash capacity.
 */
typedef enum {
    FLASH_CAPACITY_64K = 0x10,
    FLASH_CAPACITY_128K = 0x11,
    FLASH_CAPACITY_256K = 0x12,
    FLASH_CAPACITY_512K = 0x13,
    FLASH_CAPACITY_1M = 0x14,
    FLASH_CAPACITY_2M = 0x15,
    FLASH_CAPACITY_4M = 0x16,
    FLASH_CAPACITY_8M = 0x17,
} Flash_CapacityDef;
void flash_set_capacity(Flash_CapacityDef flash_cap);
Flash_CapacityDef flash_get_capacity(void);

/******************************* flash_end *******************************************************************/

/******************************* usb_end *********************************************************************/
#define reg_usb_irq REG_ADDR8(0x100839)
/******************************* usb_end *********************************************************************/

/******************************* core_start ******************************************************************/
#define SUPPORT_PFT_ARCH 1
/******************************* core_end ********************************************************************/

/******************************* uart_start ******************************************************************/
_attribute_ram_code_ void uart_receive_dma_set(dma_chn_e chn, unsigned char *addr, unsigned int rev_size);

void uart0_init(unsigned int baudrate);
/******************************* uart_end ********************************************************************/

#endif /* DRIVERS_B91_EXT_MISC_H_ */
