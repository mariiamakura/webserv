#include "Webserv.hpp"

bool Webserv::post_getdata(int i) {
    std::cout << "\npost_getdata function" << std::endl;
    if (in_request[poll_fd[i].fd].find("\r\n\r\n") != std::string::npos) {
        http_request = parse_http_request(in_request[poll_fd[i].fd]);
        logging("request :\n" + in_request[poll_fd[i].fd] + "\n", DEBUG);

        if (http_request.method == "POST") {
            logging("response \n got POST request. data to be saved", DEBUG);
            std::string linePost;
            std::istringstream iss(in_request[poll_fd[i].fd]);
            while (getline(iss, linePost)) {
                size_t usernamePtr = linePost.find("username");
                if (linePost.find("username") != std::string::npos) {
                        usernamePtr += 9;
                        size_t userNameLen = linePost.size() - usernamePtr;
                        std::string usernameValue = linePost.substr(usernamePtr, userNameLen);
                        std::cout << usernameValue << std::endl;

                    std::string response = "HTTP/1.1 200 OK\r\n";
                    response += "Content-Type: text/html\r\n\r\n";
                    response += "<html><head>";
                    response += "<style>";
                    response += "body { font-family: 'Arial', sans-serif; background-color: #f4f4f4; text-align: center; padding: 50px; }";
                    response += "h1 { color: #3498db; }";
                    response += "</style>";
                    response += "</head><body>";
                    response += "<h1>Hello, " + usernameValue + "!</h1>";
                    response += "</body></html>";
                    send(poll_fd[i].fd, response.c_str(), response.size(), 0);


                }
            }

            return true; //for break
        }
    }
    return false;
}