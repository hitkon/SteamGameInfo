cmake_minimum_required(VERSION 3.24)
project(SysInfo)

set(CMAKE_CXX_STANDARD 20)

set(INFOWARE_USE_D3D ON)
#set(INFOWARE_EXAMPLES ON)

add_subdirectory(infoware)
include_directories(infoware/include)

add_executable(SysInfo main.cpp)
target_link_libraries(SysInfo infoware)
