#include "Webserv.hpp"

HttpRequest Webserv::parse_http_request(const std::vector<uint8_t> &request)
{

    std::istringstream sstream(std::string(request.begin(), request.end()));
    HttpRequest http_request;
    // The request line format is: METHOD PATH HTTP/VERSION
    sstream >> http_request.method >> http_request.path >> http_request.http_version;
    size_t http_reqSize = sstream.tellg();
    // Parse each header line
    std::string line;
    while (std::getline(sstream, line) && !line.empty())
    {
        // Check if the line contains ": "
        http_reqSize += line.length() + 1; //for right size calculation
        //std::cout << "line len " << line.length() << "line itself : " << line << std::endl;
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
    }
    std::cout << "http_reqSize " << http_reqSize << std::endl;
    http_request.content.insert(http_request.content.end(), request.begin() + http_reqSize, request.end());

    if (http_request.headers.find("Content-Type") != http_request.headers.end()) {
        std::string boundaryPrefix = "boundary=";
        std::string contentType = http_request.headers["Content-Type"];
        size_t boundaryOffset = contentType.find(boundaryPrefix);
        if (boundaryOffset != std::string::npos) {
            boundaryOffset += boundaryPrefix.length();
            size_t boundaryLen = contentType.length() - boundaryOffset;
            std::string boundarySelf = contentType.substr(boundaryOffset, boundaryLen);
            http_request.boundBeg = "--" + boundarySelf;
            http_request.boundEnd = "--" + boundarySelf + "--";

            std::cout << "AAAAAAAAAAAAAA" << std::endl;
            std::cout << boundarySelf << std::endl;
            std::cout << "size=" << boundarySelf.size() << std::endl;
            std::cout << "length=" << boundarySelf.length() << std::endl;
            std::cout << http_request.boundBeg << std::endl;
            std::cout << "size=" << http_request.boundBeg.size() << std::endl;
            std::cout << "length=" << http_request.boundBeg.length() << std::endl;
            std::cout << http_request.boundEnd << std::endl;
            std::cout << "size=" << http_request.boundEnd.size() << std::endl;
            std::cout << "length=" << http_request.boundEnd.length() << std::endl;
            std::cout << "BBBBBBBBBBBBBB" << std::endl;
        }
    }
//    std::cout << "CONTENT PARSE\n"<< std::endl;
//    for (std::vector<uint8_t>::iterator it = http_request.content.begin(); it != http_request.content.end(); ++it) {
//        std::cout << *it;
//    }
//    std::cout << "\nthe end of CONTENT PARSE\n" << std::endl;

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
