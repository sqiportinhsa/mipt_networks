#include "TCP.hpp"
#include "TCP_utils.hpp"
#include "../common/utils.hpp"

#include <stdexcept>


void TCP::run_client(int argc, char* argv[]) {

    if (argc != 3) {
        throw std::invalid_argument(fmt::format("Invalid number of input parameters."));
    }

    std::cout << "Hi there^^! Connectinig to the server...\n";

    int client = Common_utils::create_client(SOCK_STREAM);
    std::cout << client << "\n";
    TCP_utils::socket_connect_wrapper(client, atoi(argv[1]), argv[2]);

    SSL_CTX* ctx = TLS::create_client_ctx();
    SSL* ssl = SSL_new(ctx);
    SSL_set_fd(ssl, client);

    if (SSL_connect(ssl) <= 0) {
        throw std::runtime_error("Cannot connect to server");
    }

    std::cout << "Connected succesfully!\n";

    std::cout << "Here is a list of avaible commands^^:\n"
              << "send - send a message to server,\n" 
              << "exit - close a connection,\n"
              << "help - show this message again.\n";

    while (true) {
        std::cout << "Enter your command please: ";
        
        std::string command;
        getline(std::cin, command);

        if (command == "send") {
            std::string request = Common_utils::get_user_message();
            std::cout << "OK\n";
            std::cout << request.data() << " " << request.size() << "\n";
            SSL_write(ssl, request.data(), request.size());
            std::cout << "Waiting for an answer...\n";

            std::string reply = TLS::get_message(ssl);
            std::cout << "Server reply: \n" << reply << std::endl;

        } else if (command == "exit") {
            TCP_utils::socket_send_wrapper(client, nullptr, 0, 0);

            Common_utils::stop_client(client);
            std::cout << "Connection is closed. Thanks for using our messenger^^!\n";
            
            break;
        } else if (command == "help") {
            std::cout << "Avaible commands:\n"
              << "    send - send a message to server,\n" 
              << "    exit - close a connection,\n"
              << "    help - show this message again.\n";

        } else {
            std::cout << "Uhh, it's an incorrect command. Try again or use help^^.\n";
        }
    }

    SSL_free(ssl);
    SSL_CTX_free(ctx);
}