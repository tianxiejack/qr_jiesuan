################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/MMT/multitarget.cpp 

OBJS += \
./src/MMT/multitarget.o 

CPP_DEPS += \
./src/MMT/multitarget.d 


# Each subdirectory must supply rules for building sources it contributes
src/MMT/%.o: ../src/MMT/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: NVCC Compiler'
	/usr/local/cuda-8.0/bin/nvcc -I/usr/include/opencv -I/usr/include/opencv2 -I/usr/include/GL -I../include -I../include/APP -I../include/dxutc -I../include/PRI -I../src/OSA_CAP/inc -O3 -Xcompiler -fopenmp -gencode arch=compute_53,code=sm_53  -odir "src/MMT" -M -o "$(@:%.o=%.d)" "$<"
	/usr/local/cuda-8.0/bin/nvcc -I/usr/include/opencv -I/usr/include/opencv2 -I/usr/include/GL -I../include -I../include/APP -I../include/dxutc -I../include/PRI -I../src/OSA_CAP/inc -O3 -Xcompiler -fopenmp --compile  -x c++ -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

