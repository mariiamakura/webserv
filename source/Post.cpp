#include "Webserv.hpp"

void Webserv::postMethod(int i) {
    int clientFD = poll_fd[i].fd;
    if (http_requests.count(clientFD) == 0) {
        http_requests[clientFD] = http_request;

        std::cout << "START CONTENT" << std::endl;
        //std::cout << "CONTENT : " << http_request.content << std::endl;

    }
    const char *ContLen = http_request.headers["Content-Length"].c_str();
    size_t content_length = static_cast<size_t>(std::atoi(ContLen));
//    if (http_request.content.size() == content_length + 1) //LONG TEXT 1 BYTE ADDED WRONG ON MY SIDE !!!!!!!!!!!!!!!
//        content_length += 1;
    if (http_request.content.size() == content_length) {
        http_requests.erase(clientFD);
        std::cout << "FINISH CONTENT" << std::endl;
        out_response[clientFD] = post_getdata();
        //file formed to temporary location
        //business logic decide to safe it to permanent folder while saving check if all requirements are meet
    } else if (http_request.content.size() > content_length || http_request.content.size() > content_length) {
        std::cout << "SUPPOSED content_length : " << content_length << std::endl;
        std::cout << "content size " << http_request.content.size() << std::endl;
        http_requests.erase(clientFD);
        out_response[clientFD] = "HTTP/1.1 400 Bad Request\r\n";

        std::cout << "CORRUPTED CONTENT" << std::endl;
    } else {
        std::cout << "PARTIAL CONTENT " << http_request.content.size() << " of " << content_length << std::endl;
        out_response[clientFD] = "HTTP/1.1 200 OK\r\n";

    }
}

std::string Webserv::post_getdata() {
    if (http_request.path == "/cgi-bin/index.py" || http_request.path == "/submit/") {
        return formPostResponse();
    } else if (http_request.path == "/over42/upload") {
        postContentProcess();
    }
    return formPostResponse();
}

void Webserv::postContentProcess() {
    MetaData metaD;
    std::string boundaryLine;
    std::string contentHeader;
    size_t newLineCount = 0;

    for (std::vector<uint8_t>::const_iterator it = http_request.content.begin();
         it != http_request.content.end(); ++it) {
        contentHeader += static_cast<char>(*it);
        if (*it == '\n') {
            newLineCount++;
            if (newLineCount == 1) {
                boundaryLine = contentHeader;
            }
            if (newLineCount == 4)
                break;
        }
    }

    std::string boundarySurrounding = "--";
    size_t leadingBoundarySize = boundaryLine.size();
    size_t boundaryEndLineSize = leadingBoundarySize -
                                 boundarySurrounding.size() -
                                 http_request.boundary.size();
    size_t trailingBoundarySize = http_request.boundary.size() +
                                  2 * boundaryEndLineSize +
                                  2 * boundarySurrounding.size();

    std::cout << contentHeader << std::endl;

    std::istringstream lineStream(contentHeader);
    std::string line;
    while (std::getline(lineStream, line) && !line.empty()) {
        std::string fileNamePrefix = "filename=\"";
        std::string fileNameSuffix = "\"";
        size_t fileNameStartOffset = line.find(fileNamePrefix);
        if (fileNameStartOffset != std::string::npos) {
            fileNameStartOffset += fileNamePrefix.length();
            size_t fileNameEndOffset = line.find(fileNameSuffix, fileNameStartOffset);
            if (fileNameEndOffset != std::string::npos) {
                size_t fileNameLength = fileNameEndOffset - fileNameStartOffset;
                metaD.filename = line.substr(fileNameStartOffset, fileNameLength);
            }
        }
        size_t typePtr = line.find("Content-Type: ");
        if (typePtr != std::string::npos) {
            typePtr += 14;
            size_t typeLen = line.length() - typePtr;
            metaD.content_type = line.substr(typePtr, typeLen);
        }
    }
    metaD.location = "download/";
    metaD.fullPath = metaD.location + metaD.filename;

    std::ofstream outputFile(metaD.fullPath, std::ios::binary);
    if (outputFile.is_open()) {
        std::cout << "File created successfully: " << metaD.fullPath << std::endl;

        size_t contentSize = http_request.content.size() - trailingBoundarySize;
        for (size_t i = contentHeader.size(); i < contentSize; i++) {
            outputFile.write(reinterpret_cast<const char *>(&http_request.content[i]), 1);
        }

        // Close the file when done
        outputFile.close();

    } else {
        std::cerr << "Failed to create file: " << metaD.fullPath << std::endl;
    }

}


// Helper function to convert integer to string
static std::string intToString(int value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

std::string Webserv::formPostResponse() {
    std::string response;
    http_response.status_code = 200;

    // Set the content type to HTML
    http_response.headers["Content-Type"] = "text/html";

    // Build the response body
    response += "<html><body>";
    response += "Your data received by us";
    //response += http_request.content; //UNCPMENT
    response += "</body></html>";

    // Set the content length in the headers
    std::ostringstream oss;
    oss << response.size();
    http_response.headers["Content-Length"] = oss.str();
    std::string full_response = "HTTP/1.1 " + intToString(http_response.status_code) + " OK\r\n";

    // Iterate over headers
    for (std::map<std::string, std::string>::const_iterator it = http_response.headers.begin();
         it != http_response.headers.end(); ++it) {
        full_response += it->first + ": " + it->second + "\r\n";
    }

    full_response += "\r\n" + response;
    return full_response;
}


