#include "Webserv.hpp"

void Webserv::getMethod(size_t i) {
    logging("GET request", DEBUG);
    char *tmp = string_to_chararray(http_request->path);

    if (access(tmp, F_OK) == 0)
    {

        struct stat path_stat;
        stat(tmp, &path_stat);
        bool is_directory = S_ISDIR(path_stat.st_mode);

        http_response.status_code = 200;
        http_response.status_message = "OK";
        if (ft_strcmp(tmp, "./cgi-bin/index.py") == 0 && access("./cgi-bin/index.py", F_OK) == 0)
        {
            std::cout << "cgi-bin/index.py exists" << std::endl;
            if (access("./cgi-bin/index.py", X_OK) == 0)
            {

                std::cout << "cgi-bin/index.py exists" << std::endl;
                int pipefd[2];
                pipe(pipefd);
                if (fork() == 0)
                {
                    close(pipefd[0]);
                    dup2(pipefd[1], STDOUT_FILENO);
                    std::string tmp2 = "/cgi-bin/index.py";
                    delete[] tmp;
                    tmp = string_to_chararray(tmp2);
                    http_request->path = tmp2;
                    char *const args[] = {tmp, NULL};
                    execve(tmp, args, env);
                }
                else
                {
                    close(pipefd[1]);
                    char buffer[1024];
                    std::string scriptOutput;
                    ssize_t bytesRead;
                    while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0)
                    {
                        buffer[bytesRead] = '\0';
                        scriptOutput += buffer;
                    }
                    // std::cout << "scriptOutput: " << scriptOutput << std::endl;
                    http_request->path = scriptOutput;
                    // std::cout << "scripted http_request.path: " << http_request.path << std::endl;
                    waitpid(-1, NULL, WUNTRACED);

                }
            }
        }
        else if (!is_directory)
        {

            if (access(tmp, R_OK) == 0)
            {
                std::cout << "file exists" << std::endl;
                http_request->path = tmp;
            }
            else
            {
                http_response.status_code = 403;
                http_response.status_message = "Forbidden";
                http_request->path = "/403.html";
                // out_response[poll_fd[i].fd] = create_http_response();
                std::cout << "file doesn't exist" << std::endl;
            }

        }
        else if (is_directory)
        {
            std::string tmp2 = "." + http_request->path;
            http_request->path = autoindex(tmp2);
            // std::cout << "autoindex http_request.path: " << http_request.path << std::endl;
        }
        out_response[poll_fd[i].fd] = create_http_response();

    }
    else
    {
        http_response.status_code = 404;
        http_response.status_message = "Not Found";
        http_request->path = "./404.html";
        out_response[poll_fd[i].fd] = create_http_response();
        std::cout << "file doesn't exist" << std::endl;
    }
    delete[] tmp;
}