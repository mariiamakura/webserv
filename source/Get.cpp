#include "Webserv.hpp"


std::string readFile(FILE* file) {
    const size_t bufferSize = 1024;
    char buffer[bufferSize];
    std::ostringstream contentStream;

    while (!feof(file) && !ferror(file)) {
        size_t bytesRead = fread(buffer, 1, bufferSize, file);
        if (bytesRead > 0) {
            contentStream.write(buffer, bytesRead);
        }
    }

    return contentStream.str();
}

int Webserv::getMethod() {
    logging("GET request", DEBUG);

    //std::cout << "PATH: " << http_request->path << std::endl;
    char *tmp = string_to_chararray(http_request->path);
    //std::cout << "PATH IN REQUEST: " << tmp << std::endl;

     if (http_request->path.find("download.php") != std::string::npos && access("./over42/download.php", F_OK) == 0) {
         std::cout << "./over42/download.php exists" << std::endl;

         std::string phpScriptPath = "./over42/download.php";

         // Replace "your_filename.txt" with the actual filename you want to pass as a parameter
         size_t filenamePTR = http_request->path.find("file=");
         std::string filename = http_request->path.substr(filenamePTR + 5);

        //std::cout << "filename: " << filename << std::endl;

         //const char* filename = "text.txt";
         std::string phpCommand = "php-cgi " + phpScriptPath + " file=" + filename;
         //std::string phpCommand = "php-cgi " + http_request->path;
         FILE *phpScript = popen(phpCommand.c_str(), "r");
         if (phpScript) {
             // Read the entire content of the file
             std::string fileContent = readFile(phpScript);

             // Assign the content to http_response->path
             http_response->path = fileContent;

             std::cout << "DOWNLOAD PATH SIZE: " << http_response->path.size() << std::endl;
             pclose(phpScript);
         }
         return 777;
     }

    if (access(tmp, F_OK) == 0) {

        struct stat path_stat;
        stat(tmp, &path_stat);
        bool is_directory = S_ISDIR(path_stat.st_mode);

        if (ft_strcmp(tmp, "./cgi-bin/index.py") == 0 && access("./cgi-bin/index.py", F_OK) == 0) {
            std::cout << "cgi-bin/index.py exists" << std::endl;
            if (access("./cgi-bin/index.py", X_OK) == 0) {

                std::cout << "cgi-bin/index.py exists" << std::endl;
                int pipefd[2];
                pipe(pipefd);
                if (fork() == 0) {
                    close(pipefd[0]);
                    dup2(pipefd[1], STDOUT_FILENO);
                    std::string tmp2 = "/cgi-bin/index.py";
                    delete[] tmp;
                    tmp = string_to_chararray(tmp2);
                    http_response->path = tmp2;
                    char *const args[] = {tmp, NULL};
                    //std::cout << "tmp: " << tmp << " args: " << args[0] << std::endl;
                    execve(tmp, args, env);
                } else {
                    close(pipefd[1]);
                    char buffer[1024];
                    std::string scriptOutput;
                    ssize_t bytesRead;
                    while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
                        buffer[bytesRead] = '\0';
                        scriptOutput += buffer;
                    }
                    // std::cout << "scriptOutput: " << scriptOutput << std::endl;
                    http_response->path = scriptOutput;
                    // std::cout << "scripted http_request.path: " << http_request.path << std::endl;
                    waitpid(-1, NULL, WUNTRACED);

                }
            }
        }
        else if (ft_strcmp(tmp, "./over42/get_files.php") == 0 && access("./over42/get_files.php", F_OK) == 0) {
            std::cout << "/over42/get_files.php exists" << std::endl;

            FILE *phpScript = popen("php-cgi ./over42/get_files.php", "r");

            if (phpScript) {
                char buffer[1024];
                std::string scriptOutput;
                while (fgets(buffer, sizeof(buffer), phpScript) != NULL) {
                    scriptOutput += buffer;
                }
                http_response->path = scriptOutput;
                //std::cout << "PATH: " << http_response->path << std::endl;
                pclose(phpScript);
            }
        }

        else if (!is_directory)
        {

            if (access(tmp, R_OK) == 0)
            {
                std::cout << "file exists" << std::endl;
                http_response->path = tmp;
            }
            else
            {
                std::cout << "file doesn't exist" << std::endl;
                delete[] tmp;
                return 403;
            }

        }
        else if (is_directory)
        {
            std::string tmp2 = "." + http_request->path;
            http_response->path = autoindex(tmp2); //AUTOINDEX HERE
            // std::cout << "autoindex http_request.path: " << http_request.path << std::endl;
        }
        delete[] tmp;
        //std::cout << "PATH IN REQUEST END: " << http_response->path << std::endl;
        return 200;

    }
    else
    {
        std::cout << "file doesn't exist" << std::endl;
    }
    delete[] tmp;
    return 404;
}