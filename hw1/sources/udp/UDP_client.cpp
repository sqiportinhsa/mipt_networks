#include <iostream>

#include "UDP.hpp"
#include "UDP_utils.hpp"
#include "../common/utils.hpp"

void UDP::run_client(int argc, char* argv[]) {

    if (argc != 3) {
        throw std::invalid_argument("Invalid number of input parameters.");
    }

    int client = Common_utils::create_client(SOCK_DGRAM);

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET; 
	server_addr.sin_port = htons(atoi(argv[1])); 
	server_addr.sin_addr.s_addr = inet_addr(argv[2]); 

    std::cout << "Hi there! Here is a list of avaible commands^^:"
              << "Avaible commands:\n"
              << "send - send a message to server,\n" 
              << "exit - close a connection,\n"
              << "help - show this message again.\n";

    while (true) {

       std::cout << "Enter your command please:";
        
        std::string command;
        getline(std::cin, command);

        if (command == "send"){
            std::string request = Common_utils::get_user_message();

            socklen_t len = sizeof(server_addr); 

            UDP_utils::send_message(client, request.c_str(), request.size(), &server_addr, len);
            std::cout << "Waiting for an answer...";

            std::string reply = UDP_utils::request_message(client, &server_addr, &len);
            std::cout << "Server's reply is: " << reply << "\n";

        } else if (command == "exit"){
            Common_utils::stop_client(client);
            std::cout << "Connection is closed. I'll be glad to see you the next time^^.\n";
            
            break;

        } else if (command == "help") {
            std::cout << "Avaible commands:\n"
              << "    send - send a message to server,\n" 
              << "    exit - stop client,\n"
              << "    help - show this message again.\n";

        } else {
            std::cout << "Uhh, it's an incorrect command. Try again or use help. ^.^\n";
        }
    }
}