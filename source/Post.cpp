#include "Webserv.hpp"

//static std::string okResponsePost() {
//    // Initialize response string outside the loop
//    std::string response = "HTTP/1.1 200 OK\r\n";
//    response += "Content-Type: text/html\r\n\r\n";
//    response += "<html><head>";
//    response += "<style>";
//    response += "body { font-family: 'Arial', sans-serif; background-color: #f4f4f4; text-align: center; padding: 50px; }";
//    response += "h1 { color: #3498db; }";
//    response += "</style>";
//    response += "</head><body>";
//    return (response);
//}


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
    if (http_request.path == "/cgi-bin/index.py" || http_request.path == "/over42/upload" || http_request.path == "/over42/submit/") {
        // std::cout << "in here\n";

        http_response.status_code = 200;

        // Set the content type to HTML
        http_response.headers["Content-Type"] = "text/html";

        // Build the response body
        response += "<html><body>";
        response += "<h1>Hello, " + http_request.content + "!</h1>";
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

