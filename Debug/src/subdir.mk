################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/connection_metadata.cpp \
../src/sio_packet.cpp \
../src/sio_packet_manager.cpp \
../src/websocket_endpoint.cpp 

OBJS += \
./src/connection_metadata.o \
./src/sio_packet.o \
./src/sio_packet_manager.o \
./src/websocket_endpoint.o 

CPP_DEPS += \
./src/connection_metadata.d \
./src/sio_packet.d \
./src/sio_packet_manager.d \
./src/websocket_endpoint.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++1y -I"/home/mikel/workspace/websocket_endpoint/include" -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


