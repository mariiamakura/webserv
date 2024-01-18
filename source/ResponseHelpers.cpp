#include "Webserv.hpp"

void Response::jsonFileResponse()
{

	if (this->path.find("Status:") != std::string::npos)
	{
		this->status_message = "Internal Server Error";
		this->status_code = 500;
	}
	else
	{
		this->status_code = 200;
		this->status_message = "OK";
	}
	size_t jsonPosition = this->path.find("json");
	this->body = this->path.substr(jsonPosition + 4);
	this->path.clear();
	return;
	// std::cout << "Body: " << http_response->body << std::endl;
}

void Response::downloadFileResponse()
{
	this->status_code = 200;
	this->status_message = "OK";

	std::string httpResponseText = this->path;
	size_t doubleNewlinePos = httpResponseText.find("\r\n\r\n");
	if (doubleNewlinePos != std::string::npos)
	{
		// Extract headers
		std::string headersStr = httpResponseText.substr(0, doubleNewlinePos);
		std::istringstream headersStream(headersStr);
		std::string line;

		while (std::getline(headersStream, line, '\n'))
		{
			size_t colonPos = line.find(':');
			if (colonPos != std::string::npos)
			{
				std::string headerName = line.substr(0, colonPos);
				std::string headerValue = line.substr(colonPos + 2); // Skip ': ' after the header name
				this->headers[headerName] = headerValue;
			}
		}
		// Extract body
		this->body = httpResponseText.substr(doubleNewlinePos + 5); // Skip '\r\n\r\n' after headers
		// std::cout << "body size: " << this->body.size() << std::endl;
		// std::cout << "Content Size: " << this->headers["Content-Length"] << std::endl;
	}
}

Response *Webserv::create_http_response(void)
{
	// std::ostringstream sstream;
	// std::cout << "create response start\n";
	// std::cout << http_request->http_version << std::endl;
	http_response->http_version = http_request->http_version;

	if (http_response->status_code == 200)
	{
		http_response->status_message = "OK";
	}
	else if (http_response->status_code == 403)
	{
		http_response->status_message = "Forbidden";
		http_response->path = "./over42/403.html";
	}
	else if (http_response->status_code == 404)
	{
		http_response->status_message = "Not Found";
		http_response->path = "./over42/404.html";
	}
	else if (http_response->status_code == 405)
	{
		http_response->status_message = "Method Not Allowed";
		http_response->path = "./over42/405.html";
	}
	else if (http_response->status_code == 413)
	{
		http_response->status_message = "Request Entity Too Large";
		http_response->path = "./over42/413.html";
	}
	else if (http_response->status_code == 201)
	{
		http_response->body += "<html><body>";
		http_response->body += "Your data received by us";
		http_response->body += "</body></html>";
		http_response->status_message = "OK";
	}
	else if (http_response->status_code == 400)
	{
		http_response->status_message = "Bad Request";
	}
	else if (http_response->status_code == 206)
	{
		http_response->status_message = "Partial Content";
	}
	else if (http_response->status_code == 202)
	{
		http_response->body += "File deleted successfully";
		http_response->status_message = "OK";
	}
	else if (http_response->status_code == 500)
	{
		http_response->status_message = "Internal Server Error";
		http_response->path = "./over42/500.html";
	}
	else if (http_response->status_code == 999)
	{ // php cgi files list
		http_response->jsonFileResponse();
		return http_response;
	}

	else if (http_response->status_code == 777)
	{
		http_response->downloadFileResponse();
		return http_response;
	}

    if (!isCookies) {
        //std::cout << "THERE IS NO ID\n";

        std::srand(static_cast<unsigned int>(std::time(0)));
        int randomNum = std::rand();

        std::stringstream  ss;
        ss << randomNum;
        // id=2145060342
        http_response->headers["Set-Cookie"] += "id=" + ss.str();
    }

	// std::cout << "status code checked\n";

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

	//if int the config file error_page is set to a path, then set the path to that
	 

	// std::cout << "response PATH : " << http_response->path << std::endl;

	if (http_response->status_code == 200 || http_response->status_code == 404 || http_response->status_code == 403
	|| http_response->status_code == 405 || http_response->status_code == 413)
	{
		if (http_response->status_code != 200 && !http_response->isFile) // when autoindex is on to display page
			http_response->isFile = true;
		if (http_response->isFile)
		{
			std::cout << "is a file\n";
			std::ifstream file(http_response->path.c_str(), std::ios::binary);
			std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
			// std::cout << "str: " << str << std::endl;
			http_response->body = str;

			http_response->headers["Content-Length"] = int_to_string(http_response->body.size());
		}
		else
		{
			std::cout << "not a file\n";
			http_response->body = http_response->path;
		}
	}
	//std::cout << "response body: " << http_response->body;
	return http_response;
}

std::string Webserv::autoindex(const std::string &path)
{
	// Open the directory
	DIR *dir;
	struct dirent *ent;
	struct stat st;

	// Start building the HTML string
	std::ostringstream html;
	//html << "<html><body><ul>";
	html << "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>Download Folder Contents</title><base href=\"/over42/\"><link rel=\"stylesheet\" type=\"text/css\" href=\"style/style.css\"></head>";

	if ((dir = opendir(path.c_str())) != NULL)
	{
		// Loop through the directory entries
		while ((ent = readdir(dir)) != NULL)
		{
			// Get the file name and full path
			std::string file_name = ent->d_name;
			std::string full_path = path + "/" + file_name;

			// std::cout << "PATH: " << path << std::endl;
			//  Get file information
			if (stat(full_path.c_str(), &st) == 0)
			{
				// Check if it's a directory or a file
				if (S_ISDIR(st.st_mode))
				{
					// Add a list item with a link for directories
					html << "<li><a href=\"" << file_name << "/\">" << file_name << "/</a></li>";
				}
				else
				{
					if (file_name.find("html") != std::string::npos || file_name.find("txt") != std::string::npos)
					{
						html << "<li><a href=\"" << file_name << "\">" << file_name << "</a></li>";
					}
					else
					{
						// Add a list item with a download link for files
						std::cout << "download files\n";
						html << "<li><a href=\"" << file_name << "\" download>" << file_name << "</a></li>";
					}
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

void Webserv::deleteResponse(int i)
{
	std::map<int, Response *>::iterator it = out_response.find(i);

	if (it != out_response.end())
	{
		Response *http_delete = it->second;

		// Erase the entry from the map
		out_response.erase(it);

		// Delete the object (free the memory)
		delete http_delete;
	}
}


Config Webserv::checkConfig()
{
	// http_request->headers["Port"]
	std::vector<Config *> serverConfigs = this->getConfig();
	// std::cout << "PORT: " << http_request->headers["Port"] << std::endl;
	std::istringstream iss(http_request->headers["Host"]);
	std::string host;
	std::getline(iss, host, ':'); // Extract the part before the colon
	std::string port;
	std::getline(iss, port); // Extract the part after the colon

	int num;

	if (!port.empty())
	{
		std::istringstream portStream(port);
		if (portStream >> num)
		{
			// Conversion successful
			std::cout << "Parsed port number: " << num << std::endl;
		}
		else
		{
			// Conversion failed
			std::cout << "Invalid port number" << std::endl;
		}
	}
	else
	{
		// No port number found
		std::cout << "No port number found" << std::endl;
	}

	for (std::vector<Config *>::iterator itz = serverConfigs.begin(); itz != serverConfigs.end(); ++itz)
	{
		if (num == (*itz)->getPorts())
		{
			return *(*itz);
		}
	}
	
	return *(*serverConfigs.begin());
}

std::string Webserv::checkPath(std::string path)
{
	// http_request->headers["Port"]
	// std::vector<Config *> serverConfigs = this->getConfig();
	// std::cout << "PORT: " << http_request->headers["Port"] << std::endl;
	std::istringstream iss(http_request->headers["Host"]);
	std::string host;
	std::getline(iss, host, ':'); // Extract the part before the colon
	std::string port;
	std::getline(iss, port); // Extract the part after the colon

	int num;

	if (!port.empty())
	{
		std::istringstream portStream(port);
		if (portStream >> num)
		{
			// Conversion successful
			std::cout << "Parsed port number: " << num << std::endl;
		}
		else
		{
			// Conversion failed
			std::cout << "Invalid port number" << std::endl;
		}
	}
	else
	{
		// No port number found
		std::cout << "No port number found" << std::endl;
	}

	for (std::vector<Config *>::iterator itz = serverConfigs.begin(); itz != serverConfigs.end(); ++itz)
	{
		if (num == (*itz)->getPorts())
		{
			std::cout << "PORTS MATCH" << std::endl;
			const std::map<std::string, Location *> &locations = (*itz)->getLocation();
			
			
			
			
			
			std::cout << "PATH before: " << path << std::endl;

			// implement check if path is only /
			if (path[path.length() - 1] == '/' && path.length() > 1)
			{
				path = path.substr(0, path.length() - 1);
			}
			for (std::map<std::string, Location *>::const_iterator it = locations.begin(); it != locations.end(); ++it)
			{
				std::cout << it->first << " => " << it->second << '\n';
				if (path.find(it->first) != std::string::npos) {
					std::cout << "found path in location!" << std::endl;
					currentLocation = it->second;
                    isSameLocation = true;
				}
				if (path == it->first)
				{
                    // currentLocation = it->second;
                    // isSameLocation = true;
					//std::cout << "PATH MATCH" << std::endl;
					if (path[path.length() - 1] != '/')
					{
						path += "/";
					}
					path += it->second->getIndex();
				
					// path = path + it->second->getIndex();
					std::cout << "PATH after: " << path << std::endl;
					return path;
				}
			}
		}
	}
	return path;
}
