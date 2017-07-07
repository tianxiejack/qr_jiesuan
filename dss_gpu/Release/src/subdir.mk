################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Displayer.cpp \
../src/Gpio026.cpp \
../src/MultiChVideo.cpp \
../src/VideoProcess.cpp \
../src/app_ctrl.cpp \
../src/app_project_xgs026.cpp \
../src/cuProcess.cpp \
../src/cuda_mem.cpp \
../src/process021.cpp \
../src/test_videoProcess.cpp \
../src/v4l2camera.cpp 

CU_SRCS += \
../src/cuda.cu 

CU_DEPS += \
./src/cuda.d 

OBJS += \
./src/Displayer.o \
./src/Gpio026.o \
./src/MultiChVideo.o \
./src/VideoProcess.o \
./src/app_ctrl.o \
./src/app_project_xgs026.o \
./src/cuProcess.o \
./src/cuda.o \
./src/cuda_mem.o \
./src/process021.o \
./src/test_videoProcess.o \
./src/v4l2camera.o 

CPP_DEPS += \
./src/Displayer.d \
./src/Gpio026.d \
./src/MultiChVideo.d \
./src/VideoProcess.d \
./src/app_ctrl.d \
./src/app_project_xgs026.d \
./src/cuProcess.d \
./src/cuda_mem.d \
./src/process021.d \
./src/test_videoProcess.d \
./src/v4l2camera.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: NVCC Compiler'
	/usr/local/cuda-8.0/bin/nvcc -I/usr/include/opencv -I/usr/include/opencv2 -I/usr/include/GL -I../include -I../include/APP -I../include/dxutc -I../src/OSA_CAP/inc -O3 -Xcompiler -fopenmp -ccbin aarch64-linux-gnu-g++ -gencode arch=compute_53,code=sm_53 -m64 -odir "src" -M -o "$(@:%.o=%.d)" "$<"
	/usr/local/cuda-8.0/bin/nvcc -I/usr/include/opencv -I/usr/include/opencv2 -I/usr/include/GL -I../include -I../include/APP -I../include/dxutc -I../src/OSA_CAP/inc -O3 -Xcompiler -fopenmp --compile -m64 -ccbin aarch64-linux-gnu-g++  -x c++ -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.cu
	@echo 'Building file: $<'
	@echo 'Invoking: NVCC Compiler'
	/usr/local/cuda-8.0/bin/nvcc -I/usr/include/opencv -I/usr/include/opencv2 -I/usr/include/GL -I../include -I../include/APP -I../include/dxutc -I../src/OSA_CAP/inc -O3 -Xcompiler -fopenmp -ccbin aarch64-linux-gnu-g++ -gencode arch=compute_53,code=sm_53 -m64 -odir "src" -M -o "$(@:%.o=%.d)" "$<"
	/usr/local/cuda-8.0/bin/nvcc -I/usr/include/opencv -I/usr/include/opencv2 -I/usr/include/GL -I../include -I../include/APP -I../include/dxutc -I../src/OSA_CAP/inc -O3 -Xcompiler -fopenmp --compile --relocatable-device-code=false -gencode arch=compute_53,code=compute_53 -gencode arch=compute_53,code=sm_53 -m64 -ccbin aarch64-linux-gnu-g++  -x cu -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


