# CMake version
cmake_minimum_required(
    VERSION 3.2
)

# Project information
project(
    keti-redfish 
    LANGUAGES CXX
    VERSION 1.0.0
)

# Directory setting
set(ROOT_DIR /home/keti/rk3399pro/linuxsdk-friendlyelec/buildroot)
# set(ROOT_DIR /root/CMM_SDK)
set(PROJECT_SOURCE_DIR ${ROOT_DIR}/source/keti-redfish)
set(Buildroot_INCLUDE_DIR ${ROOT_DIR}/output/rockchip_rk3399/host/aarch64-buildroot-linux-gnu/sysroot/usr/include)
set(Buildroot_LIBRARY_DIR ${ROOT_DIR}/output/rockchip_rk3399/host/aarch64-buildroot-linux-gnu/sysroot/usr/lib)

# Compiler setting
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CROSS_COMPILE ${ROOT_DIR}/output/rockchip_rk3399/host/bin/aarch64-linux-)
set(GCC ${CROSS_COMPILE}gcc)
set(GPP ${CROSS_COMPILE}g++)
set(CMAKE_C_COMPILER ${GCC})
set(CMAKE_CXX_COMPILER ${GPP})
set(CMAKE_CXX_FLAGS "-O1 -Wall -Wfatal-errors -fpermissive -w -DBOOST_LOG_DYN_LINK -Wno-psabi -std=c++11") 
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g") 
message(STATUS "Compiler")
message(STATUS " - ID       \t: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS " - Version  \t: ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS " - Path     \t: ${CMAKE_CXX_COMPILER}")


# Directory setting
set(Boost_INCLUDE_DIR ${ROOT_DIR}/output/rockchip_rk3399/build/boost-1.66.0)
set(Boost_LIBRARY_DIR ${ROOT_DIR}/output/rockchip_rk3399/build/boost-1.66.0/stage/lib)
set(CPPREST_INCLUDE_DIR ${ROOT_DIR}/output/rockchip_rk3399/build/libcpprestsdk-v2.10.1/Release/include)
set(CPPREST_LIBRARY_DIR ${ROOT_DIR}/output/rockchip_rk3399/build/libcpprestsdk-v2.10.1/Release/Binaries)
# Linkinking
## Header
include_directories(
    ${Buildroot_INCLUDE_DIR}
    ${Boost_INCLUDE_DIR} 
    ${CPPREST_INCLUDE_DIR}
    ${PROJECT_SOURCE_DIR}/include/
)
## Source
link_directories(
    ${Buildroot_LIBRARY_DIR}
    ${Boost_LIBRARY_DIR}
    ${CPPREST_LIBRARY_DIR}
)


# Target setting
add_subdirectory(src)

