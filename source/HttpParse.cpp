#include "Webserv.hpp"

bool isDelim(const std::vector<uint8_t>& request, std::size_t position, const std::vector<uint8_t>& delimiter) {
    for (size_t i = 0; i < delimiter.size(); ++i) {
        if (position + i >= request.size() || request[position + i] != delimiter[i]) {
            return false;
        }
    }
    return true;
}

void Request::parseBodyReq() {
    std::istringstream sstream(std::string(this->body.begin(), this->body.end()));
    // The request line format is: METHOD PATH HTTP/VERSION
    sstream >> this->method >> this->path >> this->http_version;
    // Parse each header line
    std::string line;
    while (std::getline(sstream, line) && !line.empty())
    {

        size_t colonPos = line.find(": ");
        if (colonPos != std::string::npos)
        {
            // Extract key and value
            std::string key = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 2);

            // Remove leading and trailing whitespace from the value
            value.erase(0, value.find_first_not_of(" \t\r"));
            value.erase(value.find_last_not_of(" \t\r") + 1);

            this->headers[key] = value;
        }
    }
    if (this->headers.find("Content-Type") != this->headers.end()) {
        std::string boundaryPrefix = "boundary=";
        std::string contentType = this->headers["Content-Type"];
        size_t boundaryOffset = contentType.find(boundaryPrefix);
        if (boundaryOffset != std::string::npos) {
            boundaryOffset += boundaryPrefix.length();
            size_t boundaryLen = contentType.length() - boundaryOffset;
            this->boundary = contentType.substr(boundaryOffset, boundaryLen);
            this->boundary += "\r\n";
        }
    }
}

bool canSeparate(const std::vector<uint8_t> &request, size_t &delimIndex) {
    const uint8_t delimArray[] = {13, 10, 13, 10};
    const std::vector<uint8_t> delim(delimArray, delimArray + sizeof(delimArray) / sizeof(delimArray[0]));

    for (size_t i = 0; i < request.size(); ++i) {
        if (isDelim(request, i, delim)) {
            delimIndex = i;
            return true;
        }
    }
    return false;
}

Request *Webserv::parse_http_request(const std::vector<uint8_t> &request) {

    http_request = new Request();

    size_t delimIndex = 0;

    if (canSeparate(request, delimIndex)) {
        http_request->body.assign(request.begin(), request.begin() + delimIndex + 4);
        http_request->content.assign(request.begin() + delimIndex + 4, request.end());
    }
    if (!http_request->body.empty()) {
        http_request->parseBodyReq();
    }
    return http_request;
}

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
