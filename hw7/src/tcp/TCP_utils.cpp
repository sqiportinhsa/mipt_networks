#include "TCP.hpp"
#include "TCP_utils.hpp"
#include "../common/utils.hpp"

int TCP_utils::socket_recv_wrapper(int socket, void* buffer, size_t len, int flag) {
    int result = recv(socket, buffer, len, flag);
    if (result < 0){
        throw std::runtime_error(fmt::format("Something went wrong with recv(socket = {}, buffer = {}, len = {}, flag = {})."
                                            "\n Socket_fd = {}.\n Errno status: {}.\n", 
                                            socket, buffer, len, flag, socket, std::strerror(errno)));
    }

    return result;
}

int TCP_utils::socket_send_wrapper(int socket, const void* buffer, size_t len, int flag) {
    int result = send(socket, buffer, len, flag);
    if (result < 0){
        throw std::runtime_error(fmt::format("Something went wrong with send(socket = {}, buffer = {}, len = {}, flag = {})."
                                            "\n Socket_fd = {}.\n Errno status: {}.\n", 
                                            socket, buffer, len, flag, socket, std::strerror(errno)));
    }

    return result;
}

void TCP_utils::socket_listen_wrapper(int server) {
    if (listen(server, TCP_utils::MaxRequests)){
        throw std::runtime_error(fmt::format("Error in listen(server = {}, n = {}).\n Errno status: {}.\n", 
                                 server, TCP_utils::MaxRequests, std::strerror(errno)));
    }
}

int TCP_utils::socket_accept_wrapper(int server) {
    int client = accept(server, nullptr, nullptr);

    if (client < 0) {
        throw std::runtime_error(fmt::format("Error in accept(server = {}, addr = nullptr, addr_len = nullptr)."
                                            "\n Client_fd = {}.\n Errno status: {}.\n", 
                                            server, client, std::strerror(errno)));
    }

    return client;
}

void TCP_utils::socket_connect_wrapper(int client, int port, const char* ip_addr) {
    
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip_addr);

    if (connect(client, (struct sockaddr*)&server_addr, sizeof(server_addr))){
        throw std::runtime_error(fmt::format("Error in connect(client = {}, port = {}, ip_addr = {}).\n Errno status: {}.\n", 
                                            client, port, ip_addr, std::strerror(errno)));
    }
}

std::string TCP_utils::request_message(int socket) {
    size_t msg_len = 0;
    TCP_utils::socket_recv_wrapper(socket, &msg_len, sizeof(msg_len), 0);
    
    std::string buffer(msg_len, 0);

    size_t read_bytes = 0;
    
    do {

        int result = TCP_utils::socket_recv_wrapper(socket, buffer.data() + read_bytes, msg_len - read_bytes, 0);
        if (result == 0) {
            break;
        }

        read_bytes += result;

    } while (read_bytes < msg_len);
    
    return buffer;
}

int TCP_utils::send_message(int socket, const char* buffer, size_t len) { 
    TCP_utils::socket_send_wrapper(socket, &len, sizeof(len), 0);
    return TCP_utils::socket_send_wrapper(socket, buffer, len, 0);
}