################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/board.c \
../src/board_sysinit.c \
../src/lcd_st7565s.c \
../src/lpc_phy_smsc87x0.c \
../src/wm8904.c 

OBJS += \
./src/board.o \
./src/board_sysinit.o \
./src/lcd_st7565s.o \
./src/lpc_phy_smsc87x0.o \
./src/wm8904.o 

C_DEPS += \
./src/board.d \
./src/board_sysinit.d \
./src/lcd_st7565s.d \
./src/lpc_phy_smsc87x0.d \
./src/wm8904.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DDEBUG -D__CODE_RED -D__USE_LPCOPEN -DCORE_M4 -I"F:\GIT\SmartFare\SmartFare_Embedded\lpc_chip_43xx\inc" -I"F:\GIT\SmartFare\SmartFare_Embedded\lpc_board_nxp_lpcxpresso_4337\inc" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fsingle-precision-constant -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mthumb -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


