################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/JPEG/yuyv2Jpeg.cpp 

OBJS += \
./src/JPEG/yuyv2Jpeg.o 

CPP_DEPS += \
./src/JPEG/yuyv2Jpeg.d 


# Each subdirectory must supply rules for building sources it contributes
src/JPEG/%.o: ../src/JPEG/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: NVCC Compiler'
	/usr/local/cuda-8.0/bin/nvcc -I/usr/include/opencv -I/usr/include/opencv2 -I/usr/include/GL -I../include -I../include/APP -I../include/dxutc -I../include/PRI -I../src/OSA_CAP/inc -O3 -Xcompiler -fopenmp -gencode arch=compute_53,code=sm_53  -odir "src/JPEG" -M -o "$(@:%.o=%.d)" "$<"
	/usr/local/cuda-8.0/bin/nvcc -I/usr/include/opencv -I/usr/include/opencv2 -I/usr/include/GL -I../include -I../include/APP -I../include/dxutc -I../include/PRI -I../src/OSA_CAP/inc -O3 -Xcompiler -fopenmp --compile  -x c++ -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


