cmake_minimum_required(VERSION 3.0.0)

project(my-sm2-sample VERSION 0.1.0)

include_directories(${PROJECT_SOURCE_DIR}/../include)# -I
include_directories(AFTER ${PROJECT_SOURCE_DIR}/include)
link_directories(${PROJECT_SOURCE_DIR}/../lib)# -L

file(GLOB SRC_FILES
    ${PROJECT_SOURCE_DIR}/../src/*.cpp 
    ${PROJECT_SOURCE_DIR}/../src/*.c
    ${PROJECT_SOURCE_DIR}/src/*.cpp)#搜索src中所有可编译文件

add_executable(TOTPclient OTPclient.cpp ${SRC_FILES})#totp客户端成品

add_executable(certify-client certify_client.cpp ${SRC_FILES})
add_executable(certify-server certify_server.cpp ${SRC_FILES})

target_link_libraries(certify-client miracl)
target_link_libraries(certify-server miracl)
