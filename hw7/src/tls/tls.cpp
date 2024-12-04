#include "tls.hpp"
#include <asm-generic/ioctls.h>
#include <cstddef>
#include <cstdio>
#include <openssl/ssl.h>
#include <string>
#include <sys/ioctl.h>

SSL_CTX* TLS::create_client_ctx() {

    SSL_CTX* ctx_ = SSL_CTX_new(TLS_method());
    if (!ctx_) {
        throw std::runtime_error("Cannot create tls context");
    }

    SSL_CTX_set_options(ctx_, SSL_OP_ALL|SSL_OP_NO_SSLv2|SSL_OP_NO_SSLv3);

    return ctx_;
}

SSL_CTX* TLS::create_server_ctx(const char* crt_path_, const char* key_path_) {
    if (crt_path_ == nullptr || key_path_ == nullptr) {
        throw std::runtime_error("Nullptr in tls init");
    }

    SSL_CTX* ssl_ctx_ = SSL_CTX_new(TLS_method());
    if (!ssl_ctx_) {
        throw std::runtime_error("Cannot create tls context");
    }


    if (SSL_CTX_use_certificate_file(ssl_ctx_, crt_path_, SSL_FILETYPE_PEM) != 1) {
        throw std::runtime_error("Cannot use certificate");
    }
    if (SSL_CTX_use_PrivateKey_file(ssl_ctx_, key_path_, SSL_FILETYPE_PEM) != 1) {
        throw std::runtime_error("Cannot use private key");
    } 

    if (SSL_CTX_check_private_key(ssl_ctx_) != 1) {
        throw std::runtime_error("Cannot check private key");
    }

    SSL_CTX_set_options(ssl_ctx_, SSL_OP_ALL|SSL_OP_NO_SSLv2|SSL_OP_NO_SSLv3);

    SSL_CTX_set_keylog_callback(ssl_ctx_, TLS::SLLKeyLogCallback);

    return ssl_ctx_;
}

std::string TLS::get_message(SSL *ssl) {
    char buf[TLS::Buf_size];
    std::string msg;

    // while (size > 0) {
    //     int num = SSL_read(ssl, buf, TLS::Buf_size);
    //     if (num < 0) {

    //     }
    //     msg.append(buf, num);
    //     if (ioctl(SSL_get_fd(ssl), FIONREAD, &size) < 0) {
    //         perror("Ioctl: ");
    //     }
    // }

    int num = SSL_read(ssl, buf, TLS::Buf_size);
    msg.append(buf, num);

    return msg;
}

void TLS::SLLKeyLogCallback(const SSL* sll, const char* line) {
    char* env_p = getenv("SSLKEYLOGFILE");
    FILE* fp = nullptr;
    if (env_p) {
        if (fp = fopen(env_p, "a+")) {
            fprintf(fp, "%s\n", line);
        }
    }
    fclose(fp);
}