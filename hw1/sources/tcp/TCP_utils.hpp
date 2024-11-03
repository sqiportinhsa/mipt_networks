#pragma once

#include "TCP.hpp"


namespace TCP_utils {

    const int MaxRequests = 1;

    int socket_recv_wrapper(int socket, void* buffer, size_t len, int flag);

    int socket_send_wrapper(int socket, const void* buffer, size_t len, int flag);

    void socket_listen_wrapper(int server);

    int socket_accept_wrapper(int server);

    void socket_connect_wrapper(int client, int port, const char* ip_addr);

    std::string request_message(int socket);

    int send_message(int socket, const char* buffer, size_t len);

}