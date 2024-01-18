#pragma once

#include "Webserv.hpp"

class Response {
    private:
        std::string http_version;
        int status_code;
        std::string status_message;
        std::map<std::string, std::string> headers;
        std::string body;
        std::string path;
        bool isFile;
public:
    Response() {
        isFile = false;
    };
    ~Response() {};
    friend class Webserv; //do we need it?

    std::string toString() const {
        std::stringstream ss;

        if (!http_version.empty() && !status_message.empty()) {
            ss << http_version << " ";
            ss << status_code << " ";
            ss << status_message << "\r\n";
        }

        if (!headers.empty()) {
            for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
                ss << it->first << ": " << it->second << "\r\n";
            }
        }

        if (!body.empty()) {
            ss << "\r\n" << body;
        }

        return ss.str();
    }

    void jsonFileResponse();
    void downloadFileResponse();

    //for Cookies
    // void setCookie(const std::string& name, const std::string& value);  // Set outgoing cookies
};