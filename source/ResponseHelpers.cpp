#include "Webserv.hpp"

Response *Webserv::create_http_response(void)
{
    //std::ostringstream sstream;
    //std::cout << "create response start\n";
    //std::cout << http_request->http_version << std::endl;
    http_response->http_version = http_request->http_version;

    if (http_response->status_code == 200) {
        http_response->status_message = "OK";
    }
    else if (http_response->status_code == 403) {
        http_response->status_message = "Forbidden";
        http_response->path = "./over42/404.html";
    } else if (http_response->status_code == 404) {
        http_response->status_message = "Not Found";
        http_response->path =  "./over42/404.html";
    } else if (http_response->status_code == 201) {
        http_response->body += "<html><body>";
        http_response->body += "Your data received by us";
        http_response->body += "</body></html>";
        http_response->status_message = "OK";

    } else if (http_response->status_code == 400) {
        http_response->status_message = "Bad Request";
    } else if (http_response->status_code == 206) {
        http_response->status_message = "Partial Content";
    } else if (http_response->status_code == 204) {
        http_response->status_message = "No Content";
    } else if (http_response->status_code == 202) {
        http_response->body += "File deleted";
        http_response->status_message = "OK";
    } else if (http_response->status_code == 777) {
        http_response->status_code = 200;
        http_response->status_message = "OK";

        std::string httpResponseText = http_response->path;
        size_t doubleNewlinePos = httpResponseText.find("\r\n\r\n");
        if (doubleNewlinePos != std::string::npos) {
            // Extract headers
            std::string headersStr = httpResponseText.substr(0, doubleNewlinePos);
            std::istringstream headersStream(headersStr);
            std::string line;

            while (std::getline(headersStream, line, '\n')) {
                size_t colonPos = line.find(':');
                if (colonPos != std::string::npos) {
                    std::string headerName = line.substr(0, colonPos);
                    std::string headerValue = line.substr(colonPos + 2); // Skip ': ' after the header name
                    http_response->headers[headerName] = headerValue;
                }
            }

            // Extract body
            http_response->body = httpResponseText.substr(doubleNewlinePos + 4); // Skip '\r\n\r\n' after headers
            std::cout << "body size: " << http_response->body.size() << std::endl;
            std::cout << "Content Size: " << http_response->headers["Content-Length"] << std::endl;
            return http_response;
        }
    }


    //std::cout << "status code checked\n";

    if (http_request->path.find(".html") != std::string::npos)
    {
        http_response->headers["Content-Type"] = "text/html";
        // http_request.path = "." + http_request.path;
    }
        // else if it is a css file, set it to text/css
    else if (http_request->path.find(".css") != std::string::npos)
    {
        http_response->headers["Content-Type"] = "text/css";
        // http_request.path = "." + http_request.path;
    }

    // http_request.path = "." + http_request.path;
    //Check if it is a file (static website), if not it's a cgi script
    //std::cout << "PATH: " << http_response->path << std::endl;
    if (http_response->path.find("json") != std::string::npos)
    {

        size_t jsonPosition = http_response->path.find("json");
        http_response->body = http_response->path.substr(jsonPosition + 4);
        http_response->path.clear();
        //std::cout << "Body: " << http_response->body << std::endl;
        return http_response;
    }
    if (http_response->status_code == 200 || http_response->status_code == 404 || http_response->status_code == 403) {
        if (access(http_response->path.c_str(), F_OK) != 0) {
            std::cout << "ACCES OK\n";
            http_response->body = http_response->path;
        }
        else {
            std::cout << "ACCES ELSE\n";
            std::ifstream file(http_response->path.c_str(), std::ios::binary);
            std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            // std::cout << "str: " << str << std::endl;
            http_response->body = str;

            http_response->headers["Content-Length"] = int_to_string(http_response->body.size());
            if (http_response->path.find("jpg") != std::string::npos || http_response->path.find("png") != std::string::npos) {
                std::cout << "Dispostition added\n";
                http_response->headers["Content-Disposition"] = "attachment; filename=\"" + http_response->path + "\"";
            }
        }
    }

    //std::cout << "response body: " << http_response->body;
    return http_response;
}


std::string Webserv::autoindex(const std::string& path)
{
    // Open the directory
    DIR* dir;
    struct dirent* ent;
    struct stat st;

    // Start building the HTML string
    std::ostringstream html;
    html << "<html><body><ul>";

    if ((dir = opendir(path.c_str())) != NULL) {
        // Loop through the directory entries
        while ((ent = readdir(dir)) != NULL) {
            // Get the file name and full path
            std::string file_name = ent->d_name;
            std::string full_path = path + "/" + file_name;

            //std::cout << "PATH: " << path << std::endl;
            // Get file information
            if (stat(full_path.c_str(), &st) == 0) {
                // Check if it's a directory or a file
                if (S_ISDIR(st.st_mode)) {
                    // Add a list item with a link for directories
                    html << "<li><a href=\"" << file_name << "/\">" << file_name << "/</a></li>";
                } else if (file_name.find("jpg") != std::string::npos || file_name.find("png") != std::string::npos ||
                file_name.find("gif") != std::string::npos){
                    // Add a list item with a link for files
                    std::cout << "download files\n";
                    html << "<li><a href=\"/download/" << file_name << "\" download>" << file_name << "</a></li>";
                }
                else {
                    //other files
                    html << "<li><a href=\"" << file_name << "\">" << file_name << "</a></li>";
                }
            }
        }
        closedir(dir);
    }

    // Finish building the HTML string
    html << "</ul></body></html>";

    // Convert the HTML stringstream to a string and return
    return html.str();
}


void Webserv::deleteResponse(int i) {
    std::map<int, Response*>::iterator it = out_response.find(i);

    if (it != out_response.end()) {
        Response *http_delete = it->second;

        // Erase the entry from the map
        out_response.erase(it);

        // Delete the object (free the memory)
        delete http_delete;
    }
}
