#pragma once

#include "UDP.hpp"

namespace UDP_utils {

    std::string request_message(int dst_socket, struct sockaddr_in* src_adrr, socklen_t* src_len);

    int send_message(int src_socket, const char* buffer, size_t len, struct sockaddr_in* dst_adrr, socklen_t dst_len);

}