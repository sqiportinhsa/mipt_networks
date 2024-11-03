#include "UDP.hpp"
#include "UDP_utils.hpp"
#include "../common/utils.hpp"
#include "fmt/base.h"

void UDP::run_server(int argc, char* argv[]) {

    if (argc != 3) {
        throw std::invalid_argument("Invalid number of input parameters.");
    }

    int server = Common_utils::create_server(atoi(argv[1]), argv[2], SOCK_DGRAM);

    while (true) {
        struct sockaddr_in client_addr; 
        socklen_t client_len = sizeof(client_addr);
       
        fmt::print("Server {} is waiting...\n", server);

        std::string request = UDP_utils::request_message(server, &client_addr, &client_len);
        std::cout << "Client message is: " << request << std::endl;
        std::cout << "Waiting for your answer...\n";

        std::string reply = Common_utils::get_user_message();
        UDP_utils::send_message(server, reply.c_str(), reply.size(), &client_addr, client_len);
    }
}