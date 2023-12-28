#include "Webserv.hpp"

void Webserv::postMethod(size_t i) {
    int clientFD = poll_fd[i].fd;
    if (http_requests.count(clientFD) == 0) {
        http_requests[clientFD] = http_request;

        std::cout << "START CONTENT" << std::endl;
        //std::cout << "CONTENT : " << http_request.content << std::endl;

    }
    const char *ContLen = http_request->headers["Content-Length"].c_str();
    size_t content_length = static_cast<size_t>(std::atoi(ContLen));
    if (http_request->content.size() == content_length) {

        //http_requests.erase(clientFD);
        std::cout << "FINISH CONTENT" << std::endl;
        http_request->postContentProcess();
        out_response[clientFD] = formPostResponse();

        deleteRequest(clientFD);

        //file formed to temporary location
        //business logic decide to safe it to permanent folder while saving check if all requirements are meet
    } else if (http_request->content.size() > content_length || http_request->content.size() > content_length) {
        std::cout << "SUPPOSED content_length : " << content_length << std::endl;
        std::cout << "content size " << http_request->content.size() << std::endl;
        deleteRequest(clientFD);

        http_response->http_version = http_request->http_version;
        http_response->status_code = 400;
        http_response->status_message = "Bad Request";
        out_response[clientFD] =  http_response;

        std::cout << "CORRUPTED CONTENT" << std::endl;
    } else {
        std::cout << "PARTIAL CONTENT " << http_request->content.size() << " of " << content_length << std::endl;

        http_response->http_version = http_request->http_version;
        http_response->status_code = 200;
        http_response->status_message = "OK";
        out_response[clientFD] =  http_response;

    }
}

void Request::setMetaData() {
    std::istringstream lineStream(std::string(this->contentHead.begin(),this->contentHead.end()));
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
                this->MetaD.filename = line.substr(fileNameStartOffset, fileNameLength);
            }
        }
        size_t typePtr = line.find("Content-Type: ");
        if (typePtr != std::string::npos) {
            typePtr += 14;
            size_t typeLen = line.length() - typePtr;
            this->MetaD.content_type = line.substr(typePtr, typeLen);
        }
    }
    this->MetaD.location = "download/";
    this->MetaD.fullPath = this->MetaD.location + this->MetaD.filename;
}

void Request::postContentProcess() {
    if (this->content.empty())
        return;
    size_t delimIndex = 0;
    if (!canSeparate(this->content, delimIndex))
        return;
    this->contentHead.assign(this->content.begin(), this->content.begin() + delimIndex + 4);
    setMetaData();
    //erase header from content and last boundary
    this->content.erase(this->content.begin(), this->content.begin() + delimIndex + 4);
    //+4 for "--" in the beginning and end + 1013 after the content
    this->content.erase(this->content.end() - (this->boundary.size() + 6), this->content.end());

    std::ofstream outputFile;
    outputFile.open(this->MetaD.fullPath.c_str(), std::ios::binary);
    if (outputFile.is_open()) {

        outputFile.write(reinterpret_cast<const char *>(&this->content[0]), this->content.size());

        // Close the file when done
        outputFile.close();
        std::cout << "File created successfully: " << this->MetaD.fullPath << std::endl;
    } else {
        std::cerr << "Failed to create file: " << this->MetaD.fullPath << std::endl;
    }

}

//// Helper function to convert integer to string
//static std::string intToString(int value) {
//    std::ostringstream oss;
//    oss << value;
//    return oss.str();
//}

Response *Webserv::formPostResponse() {
    http_response->status_code = 200;
    http_response->status_message = "OK";
    http_response->http_version = http_request->http_version;

    // Set the content type to HTML
    http_response->headers["Content-Type"] = "text/html";

    // Build the response body
    http_response->body += "<html><body>";
    http_response->body += "Your data received by us";
    http_response->body += "</body></html>";

    // Set the content length in the headers
    std::ostringstream oss;
    oss << http_response->body.size();
    http_response->headers["Content-Length"] = oss.str();

    return http_response;
}

void Webserv::deleteRequest(int i) {
    std::map<int, Request*>::iterator it = http_requests.find(i);

    if (it != http_requests.end()) {
        Request *http_request = it->second;

        // Erase the entry from the map
        http_requests.erase(it);

        // Delete the object (free the memory)
        delete http_request;

    } else {
        std::cout << "I DONT DELETE REQUEST\n";
    }
}

