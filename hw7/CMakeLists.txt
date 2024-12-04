cmake_minimum_required(VERSION 3.6)
project(nethw)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

file(GLOB_RECURSE sources ${CMAKE_SOURCE_DIR}/src/*.cpp ${CMAKE_SOURCE_DIR}/src/*.hpp)

find_package(OpenSSL REQUIRED)
if(OPENSSL_FOUND)
    MESSAGE(STATUS "Using OpenSSL: ${OPENSSL_VERSION} from: ${OPENSSL_LIBRARIES}")
    include_directories(${OPENSSL_INCLUDE_DIRS})
    link_directories(${OPENSSL_LIBRARIES})
    # target_link_libraries(protocol ${OPENSSL_LIBRARIES})
    # target_include_directories(protocol PUBLIC ${OPENSSL_INCLUDE_DIR})
    # target_link_libraries(${BINARY}_lib ${OPENSSL_LIBRARIES})
    # target_include_directories(${BINARY}_lib PUBLIC ${OPENSSL_INCLUDE_DIR})
endif()

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
target_link_libraries(tcp_client /usr/lib/x86_64-linux-gnu/libssl.so /usr/lib/x86_64-linux-gnu/libcrypto.so)
target_compile_definitions(tcp_client PUBLIC TCP_CLIENT_MODE)

add_executable(tcp_server ${sources})
target_include_directories(tcp_server PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(tcp_server fmt::fmt)
target_link_libraries(tcp_server /usr/lib/x86_64-linux-gnu/libssl.so /usr/lib/x86_64-linux-gnu/libcrypto.so)
target_compile_definitions(tcp_server PUBLIC TCP_SERVER_MODE)
