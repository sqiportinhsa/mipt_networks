#include "UDP.hpp"
#include "UDP_utils.hpp"
#include "../common/utils.hpp"

std::string UDP_utils::request_message(int dst_socket, struct sockaddr_in* src_adrr, socklen_t* src_len) {
    
    std::string message(Common_utils::MaxBufferSize, 0);

    int result = recvfrom(dst_socket, message.data(), Common_utils::MaxBufferSize, MSG_WAITALL, (struct sockaddr*)src_adrr, src_len);
    if (result < 0) {
        throw std::runtime_error(fmt::format("Error in recvfrom(socket = {}, buffer = {}, len = {}). Errno status: {}.\n", 
                                            dst_socket, message.data(), Common_utils::MaxBufferSize, std::strerror(errno)));
    }
        
    return message;
}

int UDP_utils::send_message(int src_socket, const char* buffer, size_t len, struct sockaddr_in* dst_adrr, socklen_t dst_len) { 
    int result = sendto(src_socket, buffer, len, MSG_CONFIRM, (const struct sockaddr*)dst_adrr, dst_len);

    if (result < 0){
        throw std::runtime_error(fmt::format("Error in sendto(socket = {}, buffer = {}, len = {}). Errno status: {}.\n", 
                                            src_socket, buffer, len, std::strerror(errno)));
    }

    return result;
}