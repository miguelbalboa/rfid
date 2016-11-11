/*
 * @brief WM8904 Audio codec interface file
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

#include "board.h"
#include "wm8904.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

#define WM8904_STATE_OFF            0
#define WM8904_STATE_ON             1

#define WM8904_LOCK_TIMEOUT         10

typedef struct __WM8904_Init_Seq {
    uint16_t reg_adr;
    uint16_t reg_val;
} WM8904_Init_Seq_t;

/* codec constants */
typedef struct __Codec_Cfg
{
    uint16_t fll_k;
    uint16_t fll_n;
} Codec_Cfg_t;

const Codec_Cfg_t g_CodecCfgs[2] = {
    { WM8904_FLL_1288MHZ_K, WM8904_FLL_1288MHZ_N}, /*I2S_48000*/
    { WM8904_FLL_1128MHZ_K, WM8904_FLL_1128MHZ_N}, /*I2S_44100*/
};

#define WM8904_INIT_STEPS               31               
const WM8904_Init_Seq_t g_wm8904[] = {

    { 0x00, 0x0000}, // SW Reset and ID(00H):    0000  SW_RST_DEV_ID1=0000_0000_0000_0000
    { 0x16, 0x0006 | 0x8}, /* MCLK_INV=0, SYSCLK_SRC=0, MCLK_SRC=0, TOCLK_RATE=0, ADC_DIV=000, DAC_DIV=000, OPCLK_ENA=0, CLK_SYS_ENA=1, CLK_DSP_ENA=1, TOCLK_ENA=0 */
    /* execute default start=up sequence */
    { 0x6C, 0x0100}, // Write Sequencer 0(6CH):  0100  WSEQ_ENA=1, WSEQ_WRITE_INDEX=0_0000
    { 0x6F, 0x0100}, // Write Sequencer 3(6FH):  0100  WSEQ_ABORT=0, WSEQ_START=1, WSEQ_START_INDEX=00_0000
    { 0xFF, 300}, // insert_delay_ms 500 needs to pause for write sequencer to finish before further writes

    { 0x21, WM8904_DAC_DIGITAL_1_VALUE}, // DAC Digital 1(21H):      0000  DAC_MONO=0, DAC_SB_FILT=0, DAC_MUTERATE=0, DAC_UNMUTE_RAMP=1, DAC_OSR128=1, DAC_MUTE=0, DEEMPH=00
    { 0x68, 0x0005}, // Class W 0(68H):          0005  CP_DYN_PWR=1

#if 0 // I2S Slave
    { 0x19, 0x0002}, // AIFDAC_TDM=0, AIFDAC_TDM_CHAN=0, AIFADC_TDM=0, AIFADC_TDM_CHAN=0, AIF_TRIS=0, AIF_BCLK_INV=0, BCLK_DIR=0, AIF_LRCLK_INV=0, AIF_WL=00, AIF_FMT=10
    { 0x1B, 0x0020}, /* LRCLK_DIR=0, LRCLK_RATE=000_0100_0000 */
#endif
#if 1 // I2S Master
    { 0x19, 0x0042}, // AIFDAC_TDM=0, AIFDAC_TDM_CHAN=0, AIFADC_TDM=0, AIFADC_TDM_CHAN=0, AIF_TRIS=0, AIF_BCLK_INV=0, BCLK_DIR=0, AIF_LRCLK_INV=0, AIF_WL=00, AIF_FMT=10
    { 0x1B, 0x0820}, /* LRCLK_DIR=0, LRCLK_RATE=000_0100_0000 */
#endif
    { 0x1A, 0x000F}, /* BCLK_DIV= 8 for 64FS 16bit stereo*/
    { 0x15, 0x1802}, // CLK_SYS_RAT=0110 SAMPLE_RATE=010
                
    { 0x7F, 0x0004}, // clear FLL Lock interrupt status if set
    { 0x80, 0xFFFB}, // enable FLL lock interrupt 

    { WM8904_FLL_CONTROL_1, 0x0000}, /* FLL_FRACN_ENA=1, FLL_OSC_ENA=0, FLL_ENA=0 */
#if 1 // Ref clk = 12 MHz MCLK
    { WM8904_FLL_CONTROL_2, 0x0700}, /* FLL_OUTDIV=00_0111, FLL_CTRL_RATE=000, FLL_FRATIO=000 */
    { WM8904_FLL_CONTROL_3, WM8904_FLL_1288MHZ_K}, /* FLL_K = 0 */
    { WM8904_FLL_CONTROL_4, WM8904_FLL_1288MHZ_N}, /* FLL_N = 96 */
    { WM8904_FLL_CONTROL_5, 0x0000}, /* FLL Clock src = 0 (MCLK) */
#endif

#if 0 // Ref clk = 0.512 MHz Bit Clk
    { WM8904_FLL_CONTROL_2, 0x0701}, /* FLL_OUTDIV=00_0111, FLL_CTRL_RATE=000, FLL_FRATIO=001 */
    { WM8904_FLL_CONTROL_3, 0}, /* FLL_K = 0 */
    { WM8904_FLL_CONTROL_4, 0x600}, /* FLL_N = 96 */
    { WM8904_FLL_CONTROL_5, 0x0001}, /* FLL Clock src = 0 (MCLK) */
#endif

{ WM8904_FLL_CONTROL_1, 0x0005}, /* FLL_FRACN_ENA=1, FLL_OSC_ENA=0, FLL_ENA=1 */

    { 0x39, 0x003F}, // Analogue OUT1 Left(39H): 0039  HPOUTL_MUTE=0, HPOUT_VU=0, HPOUTLZC=0, HPOUTL_VOL=11_1001
    { 0x3A, 0x003F}, // Analogue OUT1 Right(3AH): 00B9  HPOUTR_MUTE=0, HPOUT_VU=1, HPOUTRZC=0, HPOUTR_VOL=11_1001
//     /* enable ADC for line-in capture */
    { 0x0C, 0x0003}, // INL_ENA=1, INR_ENA=1
    { 0x12, 0x000F}, // DACL_ENA=1, DACR_ENA=1, ADCL_ENA=1, ADCR_ENA=1
    { 0x26, 0x0003}, // ADC_HPF_CU=01, ADC_HPF = 1
    { 0x2C, 0x001C}, // LINMUTE=0, LIN_VOL=0_0101
    { 0x2D, 0x001C}, // RINMUTE=0, LIN_VOL=0_0101
    { 0x2E, 0x0054}, // INL_CM_ENA=1, L_IP_SEL_N=IN2L, L_IP_SEL_P=IN2L, L_MODE=Single-Ended
    { 0x2F, 0x0054}, // INR_CM_ENA=1, R_IP_SEL_N=IN2R, R_IP_SEL_P=IN2R, R_MODE=Single-Ended
    { 0x06, 0x0001}, // MICDET_THR=0.070mA, MICSHORT_THR=0.520mA, MICDET_ENA=0, MIC_BIAS_ENA=1
    { 0x07, 0x0001}, // MICBIAS_SEL=2.0V
#if 0 // Use Digital MIC
    /* Output Digital MIC clock */
    { WM8904_GPIO_CONTROL_1, 0x8},
    { 0x27, 0x1000}, // Digital MIC DMCDAT1
    { 0x24, 0x00C0}, // ADC Digital Volume Left
    { 0x25, 0x01C0}, // ADC Digital Volume Right
#endif
};

#define WM8904_INPUT_INIT_STEPS                 4
const WM8904_Init_Seq_t g_wm8904_input_init[] = {
    /* enable ADC for line-in capture */
    { 0x0C, 0x0003}, // INL_ENA=1, INR_ENA=1
    //{ 0x0A, 0x0001}, // ADC_OSR128=1
    //{ 0xC6, 0x0005}, // ADC_128_OSR_TST_MODE=1, ADC_BIASX1P5=1
    { 0x12, 0x000F}, // DACL_ENA=1, DACR_ENA=1, ADCL_ENA=1, ADCR_ENA=1
    { 0x2C, 0x0005}, // LINMUTE=0, LIN_VOL=0_0101
    { 0x2D, 0x0005}, // RINMUTE=0, LIN_VOL=0_0101

};

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/
int WM8904_MultiRegWrite(const WM8904_Init_Seq_t* seq, uint32_t cnt);

/*****************************************************************************
 * Private functions
 ****************************************************************************/
/* Set the default values to the codec registers */
static int Audio_Codec_SetDefaultValues(const uint8_t *values, int sz)
{
	int ret;
  uint16_t rd_val = 1;

  /* Set System register's default values */
	ret = WM8904_MultiRegWrite((const WM8904_Init_Seq_t *)values, sz);
	rd_val = WM8904_REG_Read(0x7F);
	while ( (rd_val & 0x4) != 0x4 )
	{
		rd_val = WM8904_REG_Read(0x7F);
	}
  WM8904_REG_Write(0x16, 0x4006 | 0x8);	

	return ret;
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Read data from UDA register */
uint16_t WM8904_REG_Read(uint8_t reg) {
	uint8_t rx_data[2];
	if (Chip_I2C_MasterCmdRead(WM8904_I2C_BUS, I2CDEV_WM8904_ADDR, reg, rx_data, 2) == 2) {
		return (rx_data[0] << 8) | rx_data[1];
	}
	return 0;
}

/* Write data to Codec register */
void WM8904_REG_Write(uint8_t reg, uint16_t val)
{
	uint8_t dat[3];
	dat[0] = reg; dat[1] = val >> 8; dat[2] = val & 0xFF;
	Chip_I2C_MasterSend(WM8904_I2C_BUS, I2CDEV_WM8904_ADDR, dat, sizeof(dat));
}

/* Write data to codec register and verify the value by reading it back */
int WM8904_REG_WriteVerify(uint8_t reg, uint16_t val)
{
	uint16_t ret;
	WM8904_REG_Write(reg, val);
	ret = WM8904_REG_Read(reg);
	return ret == val;
}

/* Multiple value verification function */
int WM8904_REG_VerifyMult(uint8_t reg, const uint8_t *value, uint8_t *buff, int len)
{
	int i;
	if (Chip_I2C_MasterCmdRead(WM8904_I2C_BUS, I2CDEV_WM8904_ADDR, reg, buff, len) != len) {
		return 0;	/* Partial read */

	}
	/* Compare the values */
	for (i = 0; i < len; i++) {
		if (value[i] != buff[i]) {
			break;
		}
	}

	return i == len;
}

/* WM8904 initialize function */
int WM8904_Init(int input)
{
	I2C_EVENTHANDLER_T old = Chip_I2C_GetMasterEventHandler(WM8904_I2C_BUS);
	int ret;

	/* Initialize I2C */
	Board_I2C_Init(WM8904_I2C_BUS);
	Chip_I2C_Init(WM8904_I2C_BUS);
	Chip_I2C_SetClockRate(WM8904_I2C_BUS, 100000);
	Chip_I2C_SetMasterEventHandler(WM8904_I2C_BUS, Chip_I2C_EventHandlerPolling);

	/* Initialize the default values */
	ret = Audio_Codec_SetDefaultValues((void *)&g_wm8904[0], sizeof(g_wm8904)/sizeof(WM8904_Init_Seq_t));
	

#if 0
	if (ret) {
		ret = Audio_Codec_SetDefaultValues(UDA_interfil_regs_dat, sizeof(UDA_interfil_regs_dat));
	}

	if (ret) {
		ret = Audio_Codec_SetDefaultValues(UDA_decimator_regs_dat, sizeof(UDA_decimator_regs_dat));
	}

	if (ret && input) {
		/* Disable Power On for ADCR, PGAR, PGAL to get mic sound more clearly */
		ret = UDA1380_REG_WriteVerify(UDA_POWER_CTRL,
									  UDA1380_REG_PWRCTRL_DEFAULT_VALUE & (~(0x0B)));

		if (ret) {
			ret = UDA1380_REG_WriteVerify(UDA_ADC_CTRL,
										  UDA1380_REG_ADC_DEFAULT_VALUE | input);
		}
	}
#endif	
	Chip_I2C_SetMasterEventHandler(WM8904_I2C_BUS, old);

	return ret;
}

static void delay(uint32_t i) {
	while (i--) {}
}

/* Write multiple registers in one go */
int WM8904_MultiRegWrite(const WM8904_Init_Seq_t* seq, uint32_t cnt)
{
    uint16_t rd_val = 1;
    uint32_t i;
    
    for (i = 0; i < cnt; i++) {
        /* check if it is delay register */
        if (seq[i].reg_adr == 0xFF) {
            
            delay(seq[i].reg_val * 1000);
						do { 
				       rd_val = WM8904_REG_Read(0x70);
            } while (rd_val & 1);
        } else {
            WM8904_REG_Write(seq[i].reg_adr, seq[i].reg_val);
						rd_val = WM8904_REG_Read(seq[i].reg_adr);
        }
    }
    return (SUCCESS);
}

/**
 * @}
 */
