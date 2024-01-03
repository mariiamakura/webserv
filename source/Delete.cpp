#include "Webserv.hpp"
#include "Request.hpp"

int Request::deleteMethod() {
    std::string filename =  this->path.substr(10, this->path.length() - 10);

    std::string fullPath = "./download/" + filename;
    const char *deleteObj = fullPath.c_str();
    if (std::remove(deleteObj) != 0) {
        return 204;
    }
    return 202;
}