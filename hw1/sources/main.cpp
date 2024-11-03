#include "tcp/TCP.hpp"
#include "udp/UDP.hpp"

int main(int argc, char* argv[]) {

    #ifdef TCP_CLIENT_MODE
        TCP::run_client(argc, argv);

    #else 
    #ifdef TCP_SERVER_MODE
        TCP::run_server(argc, argv);

    #else
    #ifdef UDP_CLIENT_MODE
        UDP::run_client(argc, argv);

    #else
    #ifdef UDP_SERVER_MODE
        UDP::run_server(argc, argv);

    #endif
    #endif
    #endif
    #endif

    return 0;
}