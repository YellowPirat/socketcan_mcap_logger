cmake_minimum_required(VERSION 3.10)
project(MCL)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
#set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR} ${CMAKE_MODULE_PATH})
set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR} ${CMAKE_PREFIX_PATH})

find_package(mcap REQUIRED)
find_package(dbcppp REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(Boost REQUIRED)
find_package(protobuf REQUIRED)
find_package(gflags REQUIRED)
find_package(fmt REQUIRED)


file(GLOB SRC src/*.cpp)

add_executable(${PROJECT_NAME} ${SRC} )

target_include_directories(${PROJECT_NAME} PUBLIC inc/)

target_link_libraries(${PROJECT_NAME}   PUBLIC
                                        mcap::mcap 
                                        dbcppp::dbcppp
                                        protobuf::protobuf
                                        yaml-cpp::yaml-cpp
                                        Boost::headers
                                        gflags::gflags
                                        fmt::fmt)


target_compile_definitions(${PROJECT_NAME} PRIVATE _USE_MATH_DEFINES)
