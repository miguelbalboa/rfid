################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/adc_18xx_43xx.c \
../src/aes_18xx_43xx.c \
../src/atimer_18xx_43xx.c \
../src/ccan_18xx_43xx.c \
../src/chip_18xx_43xx.c \
../src/clock_18xx_43xx.c \
../src/dac_18xx_43xx.c \
../src/eeprom_18xx_43xx.c \
../src/emc_18xx_43xx.c \
../src/enet_18xx_43xx.c \
../src/evrt_18xx_43xx.c \
../src/fpu_init.c \
../src/gpdma_18xx_43xx.c \
../src/gpio_18xx_43xx.c \
../src/gpiogroup_18xx_43xx.c \
../src/hsadc_18xx_43xx.c \
../src/i2c_18xx_43xx.c \
../src/i2cm_18xx_43xx.c \
../src/i2s_18xx_43xx.c \
../src/iap_18xx_43xx.c \
../src/lcd_18xx_43xx.c \
../src/otp_18xx_43xx.c \
../src/pinint_18xx_43xx.c \
../src/pmc_18xx_43xx.c \
../src/ring_buffer.c \
../src/ritimer_18xx_43xx.c \
../src/rtc_18xx_43xx.c \
../src/sct_18xx_43xx.c \
../src/sct_pwm_18xx_43xx.c \
../src/sdif_18xx_43xx.c \
../src/sdio_18xx_43xx.c \
../src/sdmmc_18xx_43xx.c \
../src/spi_18xx_43xx.c \
../src/ssp_18xx_43xx.c \
../src/stopwatch_18xx_43xx.c \
../src/sysinit_18xx_43xx.c \
../src/timer_18xx_43xx.c \
../src/uart_18xx_43xx.c \
../src/wwdt_18xx_43xx.c 

OBJS += \
./src/adc_18xx_43xx.o \
./src/aes_18xx_43xx.o \
./src/atimer_18xx_43xx.o \
./src/ccan_18xx_43xx.o \
./src/chip_18xx_43xx.o \
./src/clock_18xx_43xx.o \
./src/dac_18xx_43xx.o \
./src/eeprom_18xx_43xx.o \
./src/emc_18xx_43xx.o \
./src/enet_18xx_43xx.o \
./src/evrt_18xx_43xx.o \
./src/fpu_init.o \
./src/gpdma_18xx_43xx.o \
./src/gpio_18xx_43xx.o \
./src/gpiogroup_18xx_43xx.o \
./src/hsadc_18xx_43xx.o \
./src/i2c_18xx_43xx.o \
./src/i2cm_18xx_43xx.o \
./src/i2s_18xx_43xx.o \
./src/iap_18xx_43xx.o \
./src/lcd_18xx_43xx.o \
./src/otp_18xx_43xx.o \
./src/pinint_18xx_43xx.o \
./src/pmc_18xx_43xx.o \
./src/ring_buffer.o \
./src/ritimer_18xx_43xx.o \
./src/rtc_18xx_43xx.o \
./src/sct_18xx_43xx.o \
./src/sct_pwm_18xx_43xx.o \
./src/sdif_18xx_43xx.o \
./src/sdio_18xx_43xx.o \
./src/sdmmc_18xx_43xx.o \
./src/spi_18xx_43xx.o \
./src/ssp_18xx_43xx.o \
./src/stopwatch_18xx_43xx.o \
./src/sysinit_18xx_43xx.o \
./src/timer_18xx_43xx.o \
./src/uart_18xx_43xx.o \
./src/wwdt_18xx_43xx.o 

C_DEPS += \
./src/adc_18xx_43xx.d \
./src/aes_18xx_43xx.d \
./src/atimer_18xx_43xx.d \
./src/ccan_18xx_43xx.d \
./src/chip_18xx_43xx.d \
./src/clock_18xx_43xx.d \
./src/dac_18xx_43xx.d \
./src/eeprom_18xx_43xx.d \
./src/emc_18xx_43xx.d \
./src/enet_18xx_43xx.d \
./src/evrt_18xx_43xx.d \
./src/fpu_init.d \
./src/gpdma_18xx_43xx.d \
./src/gpio_18xx_43xx.d \
./src/gpiogroup_18xx_43xx.d \
./src/hsadc_18xx_43xx.d \
./src/i2c_18xx_43xx.d \
./src/i2cm_18xx_43xx.d \
./src/i2s_18xx_43xx.d \
./src/iap_18xx_43xx.d \
./src/lcd_18xx_43xx.d \
./src/otp_18xx_43xx.d \
./src/pinint_18xx_43xx.d \
./src/pmc_18xx_43xx.d \
./src/ring_buffer.d \
./src/ritimer_18xx_43xx.d \
./src/rtc_18xx_43xx.d \
./src/sct_18xx_43xx.d \
./src/sct_pwm_18xx_43xx.d \
./src/sdif_18xx_43xx.d \
./src/sdio_18xx_43xx.d \
./src/sdmmc_18xx_43xx.d \
./src/spi_18xx_43xx.d \
./src/ssp_18xx_43xx.d \
./src/stopwatch_18xx_43xx.d \
./src/sysinit_18xx_43xx.d \
./src/timer_18xx_43xx.d \
./src/uart_18xx_43xx.d \
./src/wwdt_18xx_43xx.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DDEBUG -D__CODE_RED -D__USE_LPCOPEN -DCORE_M4 -I"F:\GIT\SmartFare\SmartFare_Embedded\lpc_chip_43xx\inc" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fsingle-precision-constant -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mthumb -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


