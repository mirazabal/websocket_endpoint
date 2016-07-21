cmake_minimum_required (VERSION 2.6)
project (websocket_endpoint)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CMAKE_CXX_COMPILER "/home/mikel/rpi/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian/bin/arm-linux-gnueabihf-g++")


# add the binary tree to the search path for include files
include_directories(${websocket_endpoint_SOURCE_DIR}/include)



file(GLOB WEB_SRC
    "src/*.h"
    "src/*.cpp"
)

#add_library(websocket_endpoint SHARED src/websocket_endpoint.cpp src/connection_metadata.cpp src/sio_packet.cpp src/sio_packet_manager.cpp)
add_library(websocket_endpoint SHARED ${WEB_SRC})


find_library(BOOST_SYSTEM_LIBRARY boost_system HINTS /home/mikel/workspace/websocket_endpoint_raspi/lib/Boost_ARM_lib)


TARGET_LINK_LIBRARIES(websocket_endpoint PUBLIC ${BOOST_SYSTEM_LIBRARY} pthread)



