#include "utils.hpp"

int Common_utils::create_server(int port, const char* ip_addr, int type) {
    int server = socket(AF_INET, type, 0);

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip_addr);

    if (bind(server, (struct sockaddr*)&server_addr, sizeof(server_addr))) {
        throw std::runtime_error(fmt::format("Something went wrong with bind(server = {}, port = {}, ip_addr = {}).\n Errno status: {}.\n", 
                                            server, port, ip_addr, std::strerror(errno)));
    }

    return server;
}

int Common_utils::create_client(int type) {
    return socket(AF_INET, type, 0); 
}

void Common_utils::stop_client(int socket) {
    if (close(socket)) {
        throw std::runtime_error(fmt::format("Something went wrong with close(socket = {}).\n Errno status: {}.\n", 
                                            socket, std::strerror(errno)));
    }
}

std::string Common_utils::get_user_message() {
    std::string text(Common_utils::MaxBufferSize, 0);
    std::cin.getline(text.data(), Common_utils::MaxBufferSize - 1);

    return text;
}