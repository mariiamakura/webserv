#include "Webserv.hpp"

static std::string okResponsePost() {
    // Initialize response string outside the loop
    std::string response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: text/html\r\n\r\n";
    response += "<html><head>";
    response += "<style>";
    response += "body { font-family: 'Arial', sans-serif; background-color: #f4f4f4; text-align: center; padding: 50px; }";
    response += "h1 { color: #3498db; }";
    response += "</style>";
    response += "</head><body>";
    return (response);
}

//make return reference to the main function in the end - use new()
std::string  Webserv::usernamePostRequest(int i) {
    std::string linePost;
    std::istringstream iss(in_request[poll_fd[i].fd]);
    std::string response = okResponsePost();
    while (getline(iss, linePost)) {
        size_t usernamePtr = linePost.find("username");
        if (usernamePtr != std::string::npos) {
            usernamePtr += 9;
            size_t userNameLen = linePost.size() - usernamePtr;
            std::string usernameValue = linePost.substr(usernamePtr, userNameLen);
            std::cout << usernameValue << std::endl;
            response += "<h1>Hello, " + usernameValue + "!</h1>";
        }
    }
    response += "</body></html>";
    return (response);
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

//make return reference to the main function in the end - use new()
std::string Webserv::post_getdata(int i) {
    std::string response;
    if(http_request.path == "/cgi-bin/index.py" || http_request.path == "/over42/upload" || http_request.path == "submit/") {
        //std::cout << "in here\n";
        response = okResponsePost();
        response += "<h1>Hello, " + http_request.content + "!</h1>";
        response += "</body></html>";
        //std::cout << response << std::endl;
        //send(poll_fd[i].fd, response.c_str(), response.size(), 0);
        return (response);
    }
    std::cout << i << std::endl;
    //for form
//    if (http_request.path == "/over42/upload") {
//        processForm(http_request, i);
//        response = "HTTP/1.1 303 See Other\r\n";
//        response += "Location: http://localhost:9999/over42/upload.html\r\n\r\n";
//        return response;
//
//    }
    return response;
}
