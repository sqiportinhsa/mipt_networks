#include "../common/utils.hpp"
#include "TCP.hpp"
#include "TCP_utils.hpp"

#include "../tls/tls.hpp"

#include <fmt/printf.h>
#include <iostream>
#include <openssl/ssl.h>
#include <stdexcept>

const char* default_crt = "../cert/server.crt";
const char* default_key = "../cert/server.key";

void TCP::run_server(int argc, char* argv[]) {
    if (argc != 5 && argc != 3) {
        throw std::invalid_argument(fmt::format("Invalid number of input parameters."));
    }

    const char* crt = default_crt;
    const char* key = default_key;

    if (argc > 3) {
        crt = argv[3];
        key = argv[4];
    }

    int server = Common_utils::create_server(atoi(argv[1]), argv[2], SOCK_STREAM);
    SSL_CTX* ctx = TLS::create_server_ctx(crt, key);

    while (true) {
       
        std::cout << fmt::format("Server {} is waiting for client...\n", server);
        TCP_utils::socket_listen_wrapper(server);

        int client = TCP_utils::socket_accept_wrapper(server);

        SSL* ssl = SSL_new(ctx);
        SSL_set_fd(ssl, client);
    
        if (SSL_accept(ssl) <= 0) {
            throw std::runtime_error("No ssl accept");
        }

        std::cout << fmt::format("Server {} created a connection with th client {}.\n", server, client);

        while(true) {
            std::cout << "Waiting for client's message...\n";
            std::string request = TLS::get_message(ssl);
            std::cout << "Recv\n";
            
            if (request.size() == 0){
                Common_utils::stop_client(client);
                std::cout << fmt::format("Connection with the client {} was closed.\n", client);
                break;
            }
            
            std::cout << "Client message is: " << request << "\n";
            std::cout << "Waiting for your answer...\n";

            std::string reply = Common_utils::get_user_message();
            SSL_write(ssl, reply.data(), reply.size());
        }

        SSL_shutdown(ssl);
        SSL_free(ssl);
    }

    SSL_CTX_free(ctx);
}