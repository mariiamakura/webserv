#include "Webserv.hpp"

Response *Webserv::create_http_response(void)
{
    //std::ostringstream sstream;
    //std::cout << "create response start\n";
    //std::cout << http_request->http_version << std::endl;
    http_response->http_version = http_request->http_version;

    if (http_response->status_code == 200) {
        http_response->status_message = "OK";
    }
    else if (http_response->status_code == 403) {
        http_response->status_message = "Forbidden";
        http_response->path = "/403.html";
    } else if (http_response->status_code == 404) {
        http_response->status_message = "Not Found";
        http_response->path = "./404.html";
    } else if (http_response->status_code == 201) {
        http_response->body += "<html><body>";
        http_response->body += "Your data received by us";
        http_response->body += "</body></html>";
        http_response->status_message = "OK";
    } else if (http_response->status_code == 400) {
    http_response->status_message = "Bad Request";
    } else if (http_response->status_code == 206) {
        http_response->status_message = "Partial Content";
    }
    //std::cout << "status code checked\n";

    if (http_request->path.find(".html") != std::string::npos)
    {
        http_response->headers["Content-Type"] = "text/html";
        // http_request.path = "." + http_request.path;
    }
        // else if it is a css file, set it to text/css
    else if (http_request->path.find(".css") != std::string::npos)
    {
        http_response->headers["Content-Type"] = "text/css";
        // http_request.path = "." + http_request.path;
    }
    // http_request.path = "." + http_request.path;
    //Check if it is a file (static website), if not it's a cgi script
    if (http_response->status_code == 200 || http_response->status_code == 404 || http_response->status_code == 403) {
        if (access(http_response->path.c_str(), F_OK) != 0)
            http_response->body = http_response->path;
        else {
            std::ifstream file(http_response->path.c_str(), std::ios::binary);
            std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            // std::cout << "str: " << str << std::endl;
            http_response->body = str;

            http_response->headers["Content-Length"] = int_to_string(http_response->body.size());
            if (http_response->path.find("download") != std::string::npos) {
                std::cout << "Dispostition added\n";
                    http_response->headers["Content-Disposition"] = "attachment; filename=\"" + http_response->path + "\"";
            }
        }
    }

    //std::cout << "create response finish\n";
    return http_response;
}

void Webserv::deleteResponse(int i) {
    std::map<int, Response*>::iterator it = out_response.find(i);

    if (it != out_response.end()) {
        Response *http_delete = it->second;

        // Erase the entry from the map
        out_response.erase(it);

        // Delete the object (free the memory)
        delete http_delete;
    }
}
