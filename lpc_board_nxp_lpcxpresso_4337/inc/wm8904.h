/*
 * @brief WM894 Audio Codec header
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2012
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#ifndef _WM8904_H
#define _WM8904_H

/** @defgroup BOARD_COMMON_WM8904 BOARD: WM8904 Audio codec interface module
 * @ingroup BOARD_Common
 * WM8904 Audio codec interface module, the module registers are accessed
 * using I2C. The board which uses this module must define WM8904_I2C_BUS to I2C0,
 * I2C1, etc, based on which I2C bus is connected to WM8904. All the
 * functions in this modules assumes that the I2C interrupt for WM8904_I2C_BUS
 * is enabled and Chip_I2C_MasterStateHandler(WM8904_I2C_BUS) is called from the
 * ISR. If the functions are to be used in polling mode the caller must replace
 * the event handler to Chip_I2C_EventHandlerPolling(), by using API
 * Chip_I2C_SetMasterEventHandler(). A macro I2CDEV_WM8904_ADDR must be defined
 * to the appropriate slave address of WM8904 audio codec.
 * @{
 */

#define WM8904_CLK_MCLK                         1
#define WM8904_CLK_FLL                          2

#define WM8904_FLL_MCLK                         1
#define WM8904_FLL_BCLK                         2
#define WM8904_FLL_LRCLK                        3
#define WM8904_FLL_FREE_RUNNING                 4

/*
 * Register values.
 */
#define WM8904_SW_RESET_AND_ID                  0x00
#define WM8904_REVISION                         0x01
#define WM8904_BIAS_CONTROL_0                   0x04
#define WM8904_VMID_CONTROL_0                   0x05
#define WM8904_MIC_BIAS_CONTROL_0               0x06
#define WM8904_MIC_BIAS_CONTROL_1               0x07
#define WM8904_ANALOGUE_DAC_0                   0x08
#define WM8904_MIC_FILTER_CONTROL               0x09
#define WM8904_ANALOGUE_ADC_0                   0x0A
#define WM8904_POWER_MANAGEMENT_0               0x0C
#define WM8904_POWER_MANAGEMENT_2               0x0E
#define WM8904_POWER_MANAGEMENT_3               0x0F
#define WM8904_POWER_MANAGEMENT_6               0x12
#define WM8904_CLOCK_RATES_0                    0x14
#define WM8904_CLOCK_RATES_1                    0x15
#define WM8904_CLOCK_RATES_2                    0x16
#define WM8904_AUDIO_INTERFACE_0                0x18
#define WM8904_AUDIO_INTERFACE_1                0x19
#define WM8904_AUDIO_INTERFACE_2                0x1A
#define WM8904_AUDIO_INTERFACE_3                0x1B
#define WM8904_DAC_DIGITAL_VOLUME_LEFT          0x1E
#define WM8904_DAC_DIGITAL_VOLUME_RIGHT         0x1F
#define WM8904_DAC_DIGITAL_0                    0x20
#define WM8904_DAC_DIGITAL_1                    0x21
#define WM8904_ADC_DIGITAL_VOLUME_LEFT          0x24
#define WM8904_ADC_DIGITAL_VOLUME_RIGHT         0x25
#define WM8904_ADC_DIGITAL_0                    0x26
#define WM8904_DIGITAL_MICROPHONE_0             0x27
#define WM8904_DRC_0                            0x28
#define WM8904_DRC_1                            0x29
#define WM8904_DRC_2                            0x2A
#define WM8904_DRC_3                            0x2B
#define WM8904_ANALOGUE_LEFT_INPUT_0            0x2C
#define WM8904_ANALOGUE_RIGHT_INPUT_0           0x2D
#define WM8904_ANALOGUE_LEFT_INPUT_1            0x2E
#define WM8904_ANALOGUE_RIGHT_INPUT_1           0x2F
#define WM8904_ANALOGUE_OUT1_LEFT               0x39
#define WM8904_ANALOGUE_OUT1_RIGHT              0x3A
#define WM8904_ANALOGUE_OUT2_LEFT               0x3B
#define WM8904_ANALOGUE_OUT2_RIGHT              0x3C
#define WM8904_ANALOGUE_OUT12_ZC                0x3D
#define WM8904_DC_SERVO_0                       0x43
#define WM8904_DC_SERVO_1                       0x44
#define WM8904_DC_SERVO_2                       0x45
#define WM8904_DC_SERVO_4                       0x47
#define WM8904_DC_SERVO_5                       0x48
#define WM8904_DC_SERVO_6                       0x49
#define WM8904_DC_SERVO_7                       0x4A
#define WM8904_DC_SERVO_8                       0x4B
#define WM8904_DC_SERVO_9                       0x4C
#define WM8904_DC_SERVO_READBACK_0              0x4D
#define WM8904_ANALOGUE_HP_0                    0x5A
#define WM8904_ANALOGUE_LINEOUT_0               0x5E
#define WM8904_CHARGE_PUMP_0                    0x62
#define WM8904_CLASS_W_0                        0x68
#define WM8904_WRITE_SEQUENCER_0                0x6C
#define WM8904_WRITE_SEQUENCER_1                0x6D
#define WM8904_WRITE_SEQUENCER_2                0x6E
#define WM8904_WRITE_SEQUENCER_3                0x6F
#define WM8904_WRITE_SEQUENCER_4                0x70
#define WM8904_FLL_CONTROL_1                    0x74
#define WM8904_FLL_CONTROL_2                    0x75
#define WM8904_FLL_CONTROL_3                    0x76
#define WM8904_FLL_CONTROL_4                    0x77
#define WM8904_FLL_CONTROL_5                    0x78
#define WM8904_GPIO_CONTROL_1                   0x79
#define WM8904_GPIO_CONTROL_2                   0x7A
#define WM8904_GPIO_CONTROL_3                   0x7B
#define WM8904_GPIO_CONTROL_4                   0x7C
#define WM8904_DIGITAL_PULLS                    0x7E
#define WM8904_INTERRUPT_STATUS                 0x7F
#define WM8904_INTERRUPT_STATUS_MASK            0x80
#define WM8904_INTERRUPT_POLARITY               0x81
#define WM8904_INTERRUPT_DEBOUNCE               0x82
#define WM8904_EQ1                              0x86
#define WM8904_EQ2                              0x87
#define WM8904_EQ3                              0x88
#define WM8904_EQ4                              0x89
#define WM8904_EQ5                              0x8A
#define WM8904_EQ6                              0x8B
#define WM8904_EQ7                              0x8C
#define WM8904_EQ8                              0x8D
#define WM8904_EQ9                              0x8E
#define WM8904_EQ10                             0x8F
#define WM8904_EQ11                             0x90
#define WM8904_EQ12                             0x91
#define WM8904_EQ13                             0x92
#define WM8904_EQ14                             0x93
#define WM8904_EQ15                             0x94
#define WM8904_EQ16                             0x95
#define WM8904_EQ17                             0x96
#define WM8904_EQ18                             0x97
#define WM8904_EQ19                             0x98
#define WM8904_EQ20                             0x99
#define WM8904_EQ21                             0x9A
#define WM8904_EQ22                             0x9B
#define WM8904_EQ23                             0x9C
#define WM8904_EQ24                             0x9D
#define WM8904_CONTROL_INTERFACE_TEST_1         0xA1
#define WM8904_ADC_TEST_0                       0xC6
#define WM8904_ANALOGUE_OUTPUT_BIAS_0           0xCC
#define WM8904_FLL_NCO_TEST_0                   0xF7
#define WM8904_FLL_NCO_TEST_1                   0xF8

#define WM8904_REGISTER_COUNT                   101
#define WM8904_MAX_REGISTER                     0xF8


/* register setting constants */
#define WM8904_FLL_1288MHZ_K                    (0x3127)
#define WM8904_FLL_1288MHZ_N                    (0x8 << 5)
#define WM8904_FLL_1128MHZ_K                    (0x86C2)
#define WM8904_FLL_1128MHZ_N                    (0x7 << 5)

#define WM8904_BDIV_256FS_2_64FS                (0x0004)
#define WM8904_BDIV_256FS_2_32FS                (0x0008)
#define WM8904_BDIV_384FS_2_64FS                (0x0007)
#define WM8904_BDIV_384FS_2_32FS                (0x000B)
#define WM8904_LRDIV_64FS                       (0x0840)
#define WM8904_LRDIV_32FS                       (0x0820)

#define WM8904_AIF123_256FS_2_32FS              { 0x4042, WM8904_BDIV_256FS_2_32FS, WM8904_LRDIV_32FS}
#define WM8904_AIF123_256FS_2_64FS              { 0x404A, WM8904_BDIV_256FS_2_64FS, WM8904_LRDIV_64FS}
#define WM8904_AIF123_384FS_2_32FS              { 0x4042, WM8904_BDIV_384FS_2_32FS, WM8904_LRDIV_32FS}
#define WM8904_AIF123_384FS_2_64FS              { 0x404A, WM8904_BDIV_384FS_2_64FS, WM8904_LRDIV_64FS}

#define WM8904_DAC_DIGITAL_1_VALUE              0x0240

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @def WM8904_U8(val)
 * Convert a 16 bit register value to 2 x 8 bit values that could be written
 * to the I2C bus in an efficient way.
 */
#define WM8904_U8(val)        (((val) >> 8) & 0xFF), ((val) & 0xFF)

/**
 * @brief	Write a 16-bit value to Codec Register
 * @param	reg		: Register to which @a val be written
 * @param	val		: 16-Bit value to be written
 * @return	Nothing
 */
void WM8904_REG_Write(uint8_t reg, uint16_t val);

/**
 * @brief	Read a 16-bit value from WM8904 codec register
 * @param	reg		: Register from which the value to be read
 * @return	Returns the value read from the register
 */
uint16_t WM8904_REG_Read(uint8_t reg);

/**
 * @brief	Writes a value to a UDA register, read back and verify the value
 * @param	reg		: Register to which the value be written
 * @param	val		: Value to be written
 * @return	1 On success, 0 on failure
 */
int WM8904_REG_WriteVerify(uint8_t reg, uint16_t val);

/**
 * @brief	Write multiple value to WM8904 registers
 * @param	buff	: Pointer to buffer (See note section)
 * @param	len		: Number of bytes in buff
 * @return	1 on Success, 0 on failure
 * @note	buff[0] must be the address of the register to which
 * the first data i.e, buff[1], buff[2] be written, the next bytes
 * buff[3], buff[4] be written to register buff[0]+1 and so on.
 */
int WM8904_REG_WriteMult(const uint8_t *buff, int len);

/**
 * @brief	Verify values in multiple WM8904 registers
 * @param	reg		: Starting register from which data be read
 * @param	value	: Pointer to memory which contains values to be compared
 * @param	buff	: Pointer to memory to which data be read
 * @param	len		: Length of bytes in value @a buff
 * @return	1 on Success & Data is valid, 0 on Failure
 */
int WM8904_REG_VerifyMult(uint8_t reg, const uint8_t *value, uint8_t *buff, int len);

/**
 * @brief	Initialize WM8904 to its default state
 * @param	input	: Audio input source (Must be one of  WM8904_LINE_IN
 *                    or WM8904_MIC_IN_L or WM8904_MIC_IN_LR)
 * @return	1 on Success and 0 on failure
 */
int WM8904_Init(int input);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* _WM8904__ */

