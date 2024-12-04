#include "tcp/TCP.hpp"

int main(int argc, char* argv[]) {

    OPENSSL_init_ssl(0, nullptr);
    
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    #if OPENSSL_VERSION_MAJOR < 3
    ERR_load_BIO_strings(); // deprecated since OpenSSL 3.0
    #endif
    ERR_load_crypto_strings();


    #ifdef TCP_CLIENT_MODE
        TCP::run_client(argc, argv);
    #else 
    #ifdef TCP_SERVER_MODE
        TCP::run_server(argc, argv);
    #endif
    #endif

    return 0;
}