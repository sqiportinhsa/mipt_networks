#pragma once

#include <cstring>
#include <exception>
#include <fmt/core.h>
#include <fstream>
#include <iostream>
#include <string>

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>


namespace Common_utils {

    const size_t MaxBufferSize = 1 << 10;

    int create_server(int port, const char* ip_addr, int type);

    int create_client(int type);

    void stop_client(int socket);

    std::string get_user_message();
}