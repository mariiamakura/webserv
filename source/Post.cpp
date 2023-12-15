#include "Webserv.hpp"

void Webserv::postMethod(int i) {
    if (http_requests.count(poll_fd[i].fd) == 0) {
        http_requests[poll_fd[i].fd] = http_request;

        std::cout << "START CONTENT" << std::endl;
        //std::cout << "CONTENT : " << http_request.content << std::endl;

    }
    const char *ContLen = http_request.headers["Content-Length"].c_str();
    size_t content_length = static_cast<size_t>(std::atoi(ContLen));
    if (http_request.content.size() == content_length + 1) //LONG TEXT 1 BYTE ADDED WRONG ON MY SIDE !!!!!!!!!!!!!!!
        content_length += 1;
    if (http_request.content.size() == content_length) {
        http_requests.erase(poll_fd[i].fd);
        std::cout << "FINISH CONTENT" << std::endl;
        out_response[poll_fd[i].fd] = post_getdata();
    }
    else if (http_request.content.size() > content_length || http_request.content.size() > content_length) {
        std::cout << "SUPPOSED content_length : " << content_length << std::endl;
        std::cout << "content size " << http_request.content.size() << std::endl;
        http_requests.erase(poll_fd[i].fd);
        out_response[poll_fd[i].fd] = "HTTP/1.1 400 Bad Request\r\n";

        std::cout << "CORRUPTED CONTENT" << std::endl;
    } else {
        std::cout << "PARTIAL CONTENT " << http_request.content.size() << " of " << content_length << std::endl;
        out_response[poll_fd[i].fd] = "HTTP/1.1 200 OK\r\n";

    }
}

void Webserv::processForm(const HttpRequest &http_request, int i) {
    std::map<std::string, std::string> headers = http_request.headers;
    std::string boundaryValue;
    std::string boundaryStart;
    std::string boundaryEnd;
    for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it) {
        if (it->first == "Content-Type") {
            size_t boundaryPtr = it->second.find("boundary=");
            if (boundaryPtr != std::string::npos) {
                boundaryPtr += 9;
                size_t boundaryLen = it->second.size() - boundaryPtr;
                boundaryValue = it->second.substr(boundaryPtr, boundaryLen);
                boundaryStart = "--" + boundaryValue;
                boundaryEnd = boundaryStart + "--";
                //std::cout << "boundaryStart in for loop: " << boundaryStart << std::endl;
                break;
            }
        }
    }
    std::string linePost;
    std::istringstream iss(in_request[poll_fd[i].fd]);
    bool foundBoundary = false;
    std::cout << "boundaryStart: " << boundaryStart << std::endl;

    std::cout << "BODY OF REQUEST\n" << iss << std::endl;
    while (getline(iss, linePost)) {
        if (foundBoundary) {
            // Process or save the line as needed
            std::cout << "foundBoundary TRUE: " << linePost << std::endl;
        }

        size_t boundaryStBody;
        //std::cout << "Boundary empty? " << boundaryStart.empty() << std::endl;
        if (!boundaryStart.empty()) {
            boundaryStBody = linePost.find(boundaryStart);
            //std::cout << boundaryStBody << std::endl;
        }
        if (!boundaryStart.empty() && boundaryStBody != std::string::npos) {
            foundBoundary = true; // Set the flag to start saving lines after finding the boundary
            //std::cout << "Boundary found!" << std::endl;
        }
    }
}

// Helper function to convert integer to string
std::string intToString(int value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

std::string Webserv::post_getdata() {
    std::string response;
    if (http_request.path == "/cgi-bin/index.py" || http_request.path == "/over42/upload" || http_request.path == "/submit/") {
        // std::cout << "in here\n";

        http_response.status_code = 200;

        // Set the content type to HTML
        http_response.headers["Content-Type"] = "text/html";

        // Build the response body
        response += "<html><body>";
        response += http_request.content;
        response += "</body></html>";

        // Set the content length in the headers
        std::ostringstream oss;
        oss << response.size();
        http_response.headers["Content-Length"] = oss.str();
        std::string full_response = "HTTP/1.1 " + intToString(http_response.status_code) + " OK\r\n";

        // Iterate over headers
        for (std::map<std::string, std::string>::const_iterator it = http_response.headers.begin(); it != http_response.headers.end(); ++it) {
            full_response += it->first + ": " + it->second + "\r\n";
        }

        full_response += "\r\n" + response;
        return full_response;
    }

    // for form
    // if (http_request.path == "/over42/upload") {
    //     processForm(http_request, i);
    //     response = "HTTP/1.1 303 See Other\r\n";
    //     response += "Location: http://localhost:9999/over42/upload.html\r\n\r\n";
    //     return response;
    // }

    return response;
}

