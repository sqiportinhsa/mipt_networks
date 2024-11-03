#include "../common/utils.hpp"
#include "TCP.hpp"
#include "TCP_utils.hpp"

#include <fmt/printf.h>
#include <iostream>

void TCP::run_server(int argc, char* argv[]) {
    if (argc != 3) {
        throw std::invalid_argument(fmt::format("Invalid number of input parameters."));
    }

    int server = Common_utils::create_server(atoi(argv[1]), argv[2], SOCK_STREAM);

    while (true) {
       
        std::cout << fmt::format("Server {} is waiting for client...\n", server);
        TCP_utils::socket_listen_wrapper(server);

        int client = TCP_utils::socket_accept_wrapper(server);
        std::cout << fmt::format("Server {} created a connection with th client {}.\n", server, client);

        while(true) {
            std::cout << "Waiting for client's message...\n";
            std::string request = TCP_utils::request_message(client);
            
            if (request.size() == 0){
                Common_utils::stop_client(client);
                std::cout << fmt::format("Connection with the client {} was closed.\n", client);
                break;
            }
            
            std::cout << "Client message is: " << request << "\n";
            std::cout << "Waiting for your answer...\n";

            std::string reply = Common_utils::get_user_message();
            TCP_utils::send_message(client, reply.c_str(), reply.size());
        }
    }
}