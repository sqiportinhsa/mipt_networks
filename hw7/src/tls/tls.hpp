#pragma once

#include "../common/utils.hpp"

#include <cstddef>
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/types.h>

namespace TLS {
    static const size_t Buf_size = 1 << 10;

    SSL_CTX* create_server_ctx(const char* crt_path_, const char* key_path_);
    SSL_CTX* create_client_ctx();

    SSL* create_ssl(bool is_server, SSL_CTX* ctx);

    std::string get_message(SSL *ssl);

    void SLLKeyLogCallback(const SSL* sll, const char* line);
}