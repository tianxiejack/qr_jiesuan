################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/APP/app_hardctrl.cpp \
../src/APP/app_main.cpp \
../src/APP/app_user.cpp 

OBJS += \
./src/APP/app_hardctrl.o \
./src/APP/app_main.o \
./src/APP/app_user.o 

CPP_DEPS += \
./src/APP/app_hardctrl.d \
./src/APP/app_main.d \
./src/APP/app_user.d 


# Each subdirectory must supply rules for building sources it contributes
src/APP/%.o: ../src/APP/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: NVCC Compiler'
	/usr/local/cuda-8.0/bin/nvcc -I/usr/include/opencv -I/usr/include/opencv2 -I/usr/include/GL -I../include -I../include/APP -I../include/PRI -I../include/dxutc -I../src/OSA_CAP/inc -I../src/osa_cap/inc -O3 -Xcompiler -fopenmp -ccbin aarch64-linux-gnu-g++ -gencode arch=compute_53,code=sm_53 -m64 -odir "src/APP" -M -o "$(@:%.o=%.d)" "$<"
	/usr/local/cuda-8.0/bin/nvcc -I/usr/include/opencv -I/usr/include/opencv2 -I/usr/include/GL -I../include -I../include/APP -I../include/PRI -I../include/dxutc -I../src/OSA_CAP/inc -I../src/osa_cap/inc -O3 -Xcompiler -fopenmp --compile -m64 -ccbin aarch64-linux-gnu-g++  -x c++ -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


