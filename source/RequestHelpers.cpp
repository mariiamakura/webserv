#include "Webserv.hpp"

void Webserv::newOrAppendRequest(int i) {
    int clientFD = poll_fd[i].fd;

    if (http_requests.count(clientFD) > 0) {
        http_request = http_requests[clientFD]; //this all should be copy of the pointer in the end for optimization
        std::cout << "Content size before append: " << http_request.content.size() <<std::endl;


        http_request.content.insert(http_request.content.end(), in_request[clientFD].begin(),
                                    in_request[clientFD].end());
//
//        adding content left bytes
//        size_t currentSize = http_request.content.size();
//        size_t contentSize = in_request[clientFD].size();

//        http_request.content.resize(currentSize + contentSize);
//        for (size_t j = 0; j < contentSize; ++j) {
//            http_request.content[currentSize + j] = in_request[clientFD][j];
//        }

        http_requests[clientFD] = http_request;
        std::cout << "APPEND REQUEST" << std::endl;
        std::cout << "Content size after append: " << http_request.content.size() <<std::endl;
    } else {
        //std::cout << "NEW HTTP PARSE" << std::endl;
        http_request = parse_http_request(in_request[clientFD]);
        std::cout << "NEW REQUEST" << std::endl;
    }
}