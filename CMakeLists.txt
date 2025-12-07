cmake_minimum_required(VERSION 3.16)
project(HolePunchWebRTC)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Path to local libdatachannel build
set(LIBDC_BUILD "${CMAKE_SOURCE_DIR}/libdatachannel/build")

include_directories(
    ${LIBDC_BUILD}/include
    ${LIBDC_BUILD}/deps/json/include
    ${LIBDC_BUILD}/deps/plog/include
    ${LIBDC_BUILD}/deps/libsrtp/include
    ${LIBDC_BUILD}/deps/libjuice/include
    ${LIBDC_BUILD}/deps/usrsctp/include
)

add_executable(peer holePunchWebRTC.cpp)

# Link directly to the library file
target_link_libraries(peer
    ${LIBDC_BUILD}/libdatachannel.so
    ssl
    crypto
    pthread
)

