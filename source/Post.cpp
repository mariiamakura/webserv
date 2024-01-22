#include "Webserv.hpp"

int Webserv::postMethod(size_t i, size_t client_body_size) {
    int clientFD = poll_fd[i].fd;
    const char *ContLen = http_request->headers["Content-Length"].c_str();
    size_t content_length = static_cast<size_t>(std::atoi(ContLen));
    if (http_requests.count(clientFD) == 0) {
        http_requests[clientFD] = http_request;

        std::cout << "START CONTENT" << std::endl;
    }

    if (http_request->content.size() == content_length) {
        std::cout << "FINISH CONTENT" << std::endl;
        http_request->postContentProcess();
        close_conn = TRUE;
		// std::vector<uint8_t>::iterator it = http_request->content.begin();
		// while (it != http_request->content.end())
		// {
		// 	std::cout << "body: " << *it << std::endl;
		// 	it++;

		// }
        return 201;
    }
    else if (http_request->content.size() > content_length) {
        std::cout << "SUPPOSED content_length : " << content_length << std::endl;
        std::cout << "Allowed client body size : " << client_body_size << std::endl;
        std::cout << "content size " << http_request->content.size() << std::endl;

        std::cout << "CORRUPTED CONTENT" << std::endl;
        close_conn = TRUE;
        return 500;
    } else {
        std::cout << "PARTIAL CONTENT " << http_request->content.size() << " of " << content_length << std::endl;
        std::cout << "request content lengh: " << content_length << " client body size: " << client_body_size << std::endl;
        if (content_length > client_body_size) {
            close_conn = TRUE;
            return 413;
        }
        return 206;

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
    struct stat info;
    if (!(stat("download", &info) == 0 && S_ISDIR(info.st_mode))) {
        if(mkdir("download",  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0)
            exit(EXIT_FAILURE);
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

