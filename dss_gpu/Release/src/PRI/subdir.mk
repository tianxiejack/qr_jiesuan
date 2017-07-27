################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/PRI/GrenadePort.cpp \
../src/PRI/LaserPort.cpp \
../src/PRI/MachGunPort.cpp \
../src/PRI/PositionPort.cpp \
../src/PRI/TurretPosPort.cpp \
../src/PRI/WeaponCtrl.cpp \
../src/PRI/osdCdc.cpp \
../src/PRI/osdPort.cpp \
../src/PRI/osdProcess.cpp \
../src/PRI/statCtrl.cpp \
../src/PRI/tasklist.cpp 

OBJS += \
./src/PRI/GrenadePort.o \
./src/PRI/LaserPort.o \
./src/PRI/MachGunPort.o \
./src/PRI/PositionPort.o \
./src/PRI/TurretPosPort.o \
./src/PRI/WeaponCtrl.o \
./src/PRI/osdCdc.o \
./src/PRI/osdPort.o \
./src/PRI/osdProcess.o \
./src/PRI/statCtrl.o \
./src/PRI/tasklist.o 

CPP_DEPS += \
./src/PRI/GrenadePort.d \
./src/PRI/LaserPort.d \
./src/PRI/MachGunPort.d \
./src/PRI/PositionPort.d \
./src/PRI/TurretPosPort.d \
./src/PRI/WeaponCtrl.d \
./src/PRI/osdCdc.d \
./src/PRI/osdPort.d \
./src/PRI/osdProcess.d \
./src/PRI/statCtrl.d \
./src/PRI/tasklist.d 


# Each subdirectory must supply rules for building sources it contributes
src/PRI/%.o: ../src/PRI/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: NVCC Compiler'
	/usr/local/cuda-8.0/bin/nvcc -I/usr/include/opencv -I/usr/include/opencv2 -I/usr/include/GL -I../include -I../include/APP -I../include/dxutc -I../src/OSA_CAP/inc -I../include/PRI -O3 -Xcompiler -fopenmp -ccbin aarch64-linux-gnu-g++ -gencode arch=compute_53,code=sm_53 -m64 -odir "src/PRI" -M -o "$(@:%.o=%.d)" "$<"
	/usr/local/cuda-8.0/bin/nvcc -I/usr/include/opencv -I/usr/include/opencv2 -I/usr/include/GL -I../include -I../include/APP -I../include/dxutc -I../src/OSA_CAP/inc -I../include/PRI -O3 -Xcompiler -fopenmp --compile -m64 -ccbin aarch64-linux-gnu-g++  -x c++ -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


