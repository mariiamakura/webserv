#include "Webserv.hpp"

//void Webserv::post_getdata(int i) {
//    std::cout << "\nIM IN POLLIN" <<std::endl;
//    if (in_request[poll_fd[i].fd].find("\r\n\r\n") != std::string::npos ||
//            in_request[poll_fd[i].fd].find("\r\n") != std::string::npos)
//    {
//        std::cout << "\nTHE END OF REQUEST" <<std::endl;
//        http_request = parse_http_request(in_request[poll_fd[i].fd]);
//        logging("request :\n" + in_request[poll_fd[i].fd] + "\n", DEBUG);
//
//        if (http_request.method == "POST")
//        {
//            std::cout << "POST GOT DATA" << std::endl;
////            std::string linePost;
////            std::istringstream iss(in_request[poll_fd[i].fd]);
////            getline(iss, linePost);
//            //std::cout << linePost <<std::endl;
////                                while (getline(iss, linePost)) {
////                                    size_t usernamePtr = linePost.find("username");
////                                    std::cout << usernamePtr << std::endl;
////                                    if (usernamePtr) {
////                                        while (linePost[usernamePtr]) {
////                                            std::cout << linePost[usernamePtr];
////                                            usernamePtr++;
////                                        }
////                                        std::cout << "\n";
////                                    }
////                                }
//        }
//    }
//}

bool Webserv::post_getdata(int i) {
    std::cout << "\nIM IN POLLIN" << std::endl;
    if (in_request[poll_fd[i].fd].find("\r\n\r\n") != std::string::npos) {
        std::cout << "\nTHE END OF REQUEST" << std::endl;
        http_request = parse_http_request(in_request[poll_fd[i].fd]);
        logging("request :\n" + in_request[poll_fd[i].fd] + "\n", DEBUG);

        if (http_request.method == "POST") {
            logging("response \n got POST request. data to be saved", DEBUG);
            std::string requestBody;
            //char buffer[1024];
            //ssize_t bytesRead;
            std::cout << "BEFORE WHILE" << std::endl;
            return true;
        }
    }
    return false;
}