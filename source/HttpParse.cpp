#include "Webserv.hpp"
//HttpRequest Webserv::parse_http_request(const std::string &request)
//{
//    std::istringstream sstream(request);
//    HttpRequest http_request;
//
//    // The request line format is: METHOD PATH HTTP/VERSION
//    sstream >> http_request.method >> http_request.path >> http_request.http_version;
//
//    // Parse each header line
//    std::string line;
//    while (std::getline(sstream, line) && line != "\r\n" && !line.empty())
//    {
//        std::istringstream ls(line);
//        std::string key, value;
//
//        // Split the line into key and value
//        std::getline(ls, key, ':');
//        std::getline(ls, value);
//
//        // Remove leading and trailing whitespace from the value
//        value.erase(0, value.find_first_not_of(" \t"));
//        value.erase(value.find_last_not_of(" \t") + 1);
//
//        http_request.headers[key] = value;
//    }
//
//
//    return http_request;
//}



HttpRequest Webserv::parse_http_request(const std::string &request)
{
    std::istringstream sstream(request);
    HttpRequest http_request;

    // The request line format is: METHOD PATH HTTP/VERSION
    sstream >> http_request.method >> http_request.path >> http_request.http_version;

    // Parse each header line
    std::string line;
    while (std::getline(sstream, line) && !line.empty())
    {
        // Check if the line contains ": "
        size_t colonPos = line.find(": ");
        if (colonPos != std::string::npos)
        {
            // Extract key and value
            std::string key = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 2);

            // Remove leading and trailing whitespace from the value
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);

            http_request.headers[key] = value;
        }
        else
        {
            // No ": " found, treat the line as part of the body
            line.erase(0, line.find_first_not_of(" \r"));
            line.erase(line.find_last_not_of(" \r") + 1);
            http_request.content += line;
        }


    }

    return http_request;
}
