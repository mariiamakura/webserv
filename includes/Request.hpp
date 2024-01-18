#pragma once

#include "Webserv.hpp"

class Webserv;

struct MetaData {
    std::string filename;
    std::string location;
    std::string content_type;
    std::string fullPath;
};

class Request {
private:
        std::string http_version;
        std::string method;
        std::string path;
        std::map<std::string, std::string> headers;
        std::string boundary;
        std::vector<uint8_t> body;
        std::vector<uint8_t> content;
        std::vector<uint8_t> contentHead;
        MetaData MetaD;

    public:
        Request() {};
        ~Request() {};
        friend class Webserv; //do we need it?

    //REQUEST HTTP METHODS
    void postContentProcess();
    void parseBodyReq();
    void setMetaData();
    int deleteMethod();

    //Cookie
    void parseCookies();// Parse and handle incoming cookies
};

bool canSeparate(const std::vector<uint8_t> &request, size_t &delimIndex);
bool isDelim(const std::vector<uint8_t>& request, std::size_t position, const std::vector<uint8_t>& delimiter);