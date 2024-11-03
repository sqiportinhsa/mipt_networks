cmake_minimum_required(VERSION 3.6)
project(nethw)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

file(GLOB_RECURSE sources ${CMAKE_SOURCE_DIR}/sources/*.cpp ${CMAKE_SOURCE_DIR}/sources/*.hpp)


include(FetchContent)
FetchContent_Declare(fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG master
)
FetchContent_MakeAvailable(fmt)

# Build fully static
add_executable(tcp_client ${sources})
target_include_directories(tcp_client PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(tcp_client fmt::fmt)
target_compile_definitions(tcp_client PUBLIC TCP_CLIENT_MODE)

add_executable(tcp_server ${sources})
target_include_directories(tcp_server PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(tcp_server fmt::fmt)
target_compile_definitions(tcp_server PUBLIC TCP_SERVER_MODE)

add_executable(udp_client ${sources})
target_include_directories(udp_client PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(udp_client fmt::fmt)
target_compile_definitions(udp_client PUBLIC UDP_CLIENT_MODE)

add_executable(udp_server ${sources})
target_include_directories(udp_server PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(udp_server fmt::fmt)
target_compile_definitions(udp_server PUBLIC UDP_SERVER_MODE)