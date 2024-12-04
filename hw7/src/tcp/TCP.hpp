#pragma once

#include <cstring>
#include <exception>
#include <fmt/printf.h>
#include <fstream>
#include <iostream>
#include <string>

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "../tls/tls.hpp"

namespace TCP {
    void run_client(int argc, char* argv[]);
    void run_server(int argc, char* argv[]);
}
