#include "Webserv.hpp"

bool isDelim(const std::vector<uint8_t>& request, std::size_t position, const std::vector<uint8_t>& delimiter) {
    for (size_t i = 0; i < delimiter.size(); ++i) {
        if (position + i >= request.size() || request[position + i] != delimiter[i]) {
            return false;
        }
    }
    return true;
}

void Webserv::parseBodyReq(const std::vector<uint8_t> &body, HttpRequest &http_request) {
    std::istringstream sstream(std::string(body.begin(), body.end()));
    // The request line format is: METHOD PATH HTTP/VERSION
    sstream >> http_request.method >> http_request.path >> http_request.http_version;
    // Parse each header line
    std::string line;
    while (std::getline(sstream, line) && !line.empty())
    {

        //std::cout << "line len " << line.length() << "line itself : " << line << std::endl;
        size_t colonPos = line.find(": ");
        if (colonPos != std::string::npos)
        {
            // Extract key and value
            std::string key = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 2);

            // Remove leading and trailing whitespace from the value
            value.erase(0, value.find_first_not_of(" \t\r"));
            value.erase(value.find_last_not_of(" \t\r") + 1);

            http_request.headers[key] = value;
        }
    }
    if (http_request.headers.find("Content-Type") != http_request.headers.end()) {
        std::string boundaryPrefix = "boundary=";
        std::string contentType = http_request.headers["Content-Type"];
        size_t boundaryOffset = contentType.find(boundaryPrefix);
        if (boundaryOffset != std::string::npos) {
            boundaryOffset += boundaryPrefix.length();
            size_t boundaryLen = contentType.length() - boundaryOffset;
            http_request.boundary = contentType.substr(boundaryOffset, boundaryLen);
        }
    }
    std:cout << "Orig baoundary: ";
    for (size_t i = 0; i < http_request.boundary.size(); i++)
    {
        std::cout << http_request.boundary[i] << " ";
    }
    std:cout << "\n";
}

HttpRequest Webserv::parse_http_request(const std::vector<uint8_t> &request) {

    HttpRequest http_request;
    const uint8_t delimArray[] = {13, 10, 13, 10};
    const std::vector<uint8_t> delim(delimArray, delimArray + sizeof(delimArray) / sizeof(delimArray[0]));

    std::size_t delimIndex = 0;
    bool foundDelim = false;

    for (size_t i = 0; i < request.size(); ++i) {
        if (isDelim(request, i, delim)) {
            delimIndex = i;
            foundDelim = true;
            break;
        }
    }
    if (foundDelim) {
        http_request.body.assign(request.begin(), request.begin() + delimIndex + 4);
        http_request.content.assign(request.begin() + delimIndex + 4, request.end());
    }
    if (http_request.body.size() > 0) {
        parseBodyReq(http_request.body, http_request);
    }
    return http_request;
}

std::string Webserv::create_http_response(void)
{
    std::ostringstream sstream;
    http_response.http_version = http_request.http_version;

    // std::cout << "http_request.path: " << http_request.path << std::endl;
    if (http_request.path.find(".html") != std::string::npos)
    {
        http_response.headers["Content-Type"] = "text/html";
        // http_request.path = "." + http_request.path;
    }
        // else if it is a css file, set it to text/css
    else if (http_request.path.find(".css") != std::string::npos)
    {
        http_response.headers["Content-Type"] = "text/css";
        // http_request.path = "." + http_request.path;
    }
    else if (http_request.path.find(".jpg") != std::string::npos)
    {
        http_response.headers["Content-Type"] = "image/jpeg";
        // http_request.path = "." + http_request.path;
    }
        // else set it to text/plain
    else
    {
        http_response.headers["Content-Type"] = "text/html";
        // http_request.path =  http_request.path;
    }



    // http_response.headers["Content-Type"] = "text/html";

    // http_request.path = "." + http_request.path;
    //Check if it is a file (static website), if not it's a cgi script
    if (access(http_request.path.c_str(), F_OK) != 0)
        http_response.body = http_request.path ;
    else
    {
        std::ifstream file(http_request.path.c_str());
        std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        // std::cout << "str: " << str << std::endl;
        http_response.body = str;

    }
    http_response.headers["Content-Length"] = int_to_string(http_response.body.size());

    // The status line format is: HTTP/VERSION STATUS_CODE STATUS_MESSAGE
    sstream << http_response.http_version << " " << http_response.status_code << " " << http_response.status_message << "\r\n";

    // Write each header line
    for (std::map<std::string, std::string>::const_iterator it = http_response.headers.begin(); it != http_response.headers.end(); ++it)
    {
        sstream << it->first << ": " << it->second << "\r\n";
    }

    // Write the body
    sstream << "\r\n"
            << http_response.body;

    return sstream.str();
}
