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

void Webserv::processForm(const HttpRequest &http_request) {
    std::map<std::string, std::string> headers = http_request.headers;

}

//make return reference to the main function in the end - use new()
std::string Webserv::post_getdata(int i) {
    std::string response;
    http_request = parse_http_request(in_request[poll_fd[i].fd]);
    if(http_request.path == "/cgi-bin/index.py" || http_request.path == "/submit") {
        response = usernamePostRequest(i);
        return (response);
    }
    if (http_request.path == "/over42/upload") {
        //processForm(http_request);
        response = "HTTP/1.1 303 See Other\r\n";
        response += "Location: http://localhost:9999/over42/upload.html\r\n\r\n";
        return response;

    }
    return response;
}
