#include "Webserv.hpp"

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
            if (http_request.headers["Content-Type"].find("application/x-www-form-urlencoded") != std::string::npos) {
                line.erase(0, line.find_first_not_of(" \r"));
                line.erase(line.find_last_not_of(" \r") + 1);
                http_request.content += line;
            }
            else if (http_request.headers["Content-Type"].find("multipart/form-data") != std::string::npos) {
                while (std::getline(sstream, line)) {
                    http_request.content += line + '\n';
                    std::cout << "content line: " << line << std::endl;
                }
            }
        }


    }
    std::cout << "\n\nCONTENT PARSE: ";
    std::cout << http_request.content << "the end ofCONTENT PARSE\n"<< std::endl;
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
