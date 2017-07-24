################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/DXUTC/config_menu.cpp \
../src/DXUTC/data_port.cpp \
../src/DXUTC/dx.cpp \
../src/DXUTC/dxTimer.cpp \
../src/DXUTC/ipc_port.cpp \
../src/DXUTC/mctrl.cpp \
../src/DXUTC/port.cpp \
../src/DXUTC/port_tsk.cpp \
../src/DXUTC/script.cpp \
../src/DXUTC/sys_config.cpp \
../src/DXUTC/uart_port.cpp \
../src/DXUTC/udp_port.cpp \
../src/DXUTC/wtd_tsk.cpp 

OBJS += \
./src/DXUTC/config_menu.o \
./src/DXUTC/data_port.o \
./src/DXUTC/dx.o \
./src/DXUTC/dxTimer.o \
./src/DXUTC/ipc_port.o \
./src/DXUTC/mctrl.o \
./src/DXUTC/port.o \
./src/DXUTC/port_tsk.o \
./src/DXUTC/script.o \
./src/DXUTC/sys_config.o \
./src/DXUTC/uart_port.o \
./src/DXUTC/udp_port.o \
./src/DXUTC/wtd_tsk.o 

CPP_DEPS += \
./src/DXUTC/config_menu.d \
./src/DXUTC/data_port.d \
./src/DXUTC/dx.d \
./src/DXUTC/dxTimer.d \
./src/DXUTC/ipc_port.d \
./src/DXUTC/mctrl.d \
./src/DXUTC/port.d \
./src/DXUTC/port_tsk.d \
./src/DXUTC/script.d \
./src/DXUTC/sys_config.d \
./src/DXUTC/uart_port.d \
./src/DXUTC/udp_port.d \
./src/DXUTC/wtd_tsk.d 


# Each subdirectory must supply rules for building sources it contributes
src/DXUTC/%.o: ../src/DXUTC/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: NVCC Compiler'
	/usr/local/cuda-8.0/bin/nvcc -I/usr/include/opencv -I/usr/include/opencv2 -I/usr/include/GL -I../include -I../include/APP -I../include/dxutc -I../src/OSA_CAP/inc -I../include/PRI -G -g -O0 -Xcompiler -fopenmp -ccbin aarch64-linux-gnu-g++ -gencode arch=compute_53,code=sm_53 -m64 -odir "src/DXUTC" -M -o "$(@:%.o=%.d)" "$<"
	/usr/local/cuda-8.0/bin/nvcc -I/usr/include/opencv -I/usr/include/opencv2 -I/usr/include/GL -I../include -I../include/APP -I../include/dxutc -I../src/OSA_CAP/inc -I../include/PRI -G -g -O0 -Xcompiler -fopenmp --compile -m64 -ccbin aarch64-linux-gnu-g++  -x c++ -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


