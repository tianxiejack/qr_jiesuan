################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/UART/UartCanMessage.cpp \
../src/UART/UartMessage.cpp \
../src/UART/record_log.cpp \
../src/UART/spiH.cpp 

OBJS += \
./src/UART/UartCanMessage.o \
./src/UART/UartMessage.o \
./src/UART/record_log.o \
./src/UART/spiH.o 

CPP_DEPS += \
./src/UART/UartCanMessage.d \
./src/UART/UartMessage.d \
./src/UART/record_log.d \
./src/UART/spiH.d 


# Each subdirectory must supply rules for building sources it contributes
src/UART/%.o: ../src/UART/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: NVCC Compiler'
	/usr/local/cuda-8.0/bin/nvcc -I/usr/include/opencv -I/usr/include/opencv2 -I/usr/include/GL -I../include -I../include/APP -I../include/dxutc -I../include/PRI -I../src/OSA_CAP/inc -G -g -O0 -Xcompiler -fopenmp -ccbin aarch64-linux-gnu-g++ -gencode arch=compute_53,code=sm_53 -m64 -odir "src/UART" -M -o "$(@:%.o=%.d)" "$<"
	/usr/local/cuda-8.0/bin/nvcc -I/usr/include/opencv -I/usr/include/opencv2 -I/usr/include/GL -I../include -I../include/APP -I../include/dxutc -I../include/PRI -I../src/OSA_CAP/inc -G -g -O0 -Xcompiler -fopenmp --compile -m64 -ccbin aarch64-linux-gnu-g++  -x c++ -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


