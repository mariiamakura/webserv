#include "Webserv.hpp"

static std::string usernameResponse() {
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

std::string Webserv::post_getdata(int i) {
    logging("response \n got POST request. data to be saved", DEBUG);
    std::string linePost;
    std::istringstream iss(in_request[poll_fd[i].fd]);
    std::string response = usernameResponse();
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

    return response;
}
