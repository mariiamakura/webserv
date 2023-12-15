#include "Webserv.hpp"

void Webserv::newOrAppendRequest(int i) {
    int clientFD = poll_fd[i].fd;
    if (http_requests.count(clientFD) > 0) {
        http_request = http_requests[clientFD]; //this all should be copy of the pointer in the end for optimization
        std::cout << "Content size before append: " << http_request.content.size() <<std::endl;
        http_request.content += in_request[clientFD];
        http_requests[clientFD] = http_request;
        std::cout << "APPEND REQUEST" << std::endl;
        std::cout << "Content size after append: " << http_request.content.size() <<std::endl;
    } else {
        http_request = parse_http_request(in_request[clientFD]);
        std::cout << "NEW REQUEST" << std::endl;
    }
}