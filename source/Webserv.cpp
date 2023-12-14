/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhassoun <fhassoun@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/20 08:53:31 by fhassoun          #+#    #+#             */
/*   Updated: 2023/12/11 11:56:01 by fhassoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"

Webserv::Webserv()
{
	server.push_back(Server());
	poll_fd.push_back(pollfd());
}

Webserv::~Webserv()
{
}

Webserv::Webserv(Webserv const &src)
{
	*this = src;
}

Webserv &Webserv::operator=(Webserv const &src)
{
	if (this == &src)
		return (*this);
	return (*this);
}

void Webserv::logging(std::string str, int status)
{
	std::string color1 = WHITE;
	std::string color2 = WHITE;
	switch (status)
	{
	case DEBUG:
		color1 = CYAN;
		color2 = PURPLE;
		break;
	case INFO:
		color1 = CYAN;
		color2 = GREEN;
		break;
	case WARNING:
		color1 = YELLOW;
		color2 = ORANGE;
		break;
	case ERROR:
		color1 = RED;
		color2 = LILA;
		break;
	default:
		break;
	}
	// get time
	time_t rawtime;
	struct tm *timeinfo;
	char buffer[80];
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buffer, sizeof(buffer), "[%d/%m/%Y %H:%M:%S] ", timeinfo);
	std::string time(buffer);
	std::cout << color1 << time << color2 << " " << str << RESET << std::endl;
}

int Webserv::check_sockfds(std::vector<int> sockfds, int i)
{
	//check if current fd is one of the server fds
	std::vector<int>::iterator it;
	for (it = sockfds.begin(); it != sockfds.end(); it++)
	{
		if (poll_fd[i].fd == *it)
			return (1);
	}
	return (0);
}

// setters
void Webserv::setEnv(char **env)
{
	this->env = env;
}

// getters
char **Webserv::getEnv()
{
	return (this->env);
}

// methods

std::string Webserv::autoindex(const std::string& path) 
{
    DIR* dir;
    struct dirent* ent;
    struct stat st;

    std::ostringstream html;
    html << "<html><body><ul>";

    if ((dir = opendir(path.c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            std::string file_name = ent->d_name;
            std::string full_path = path + "/" + file_name;

            if (stat(full_path.c_str(), &st) == 0) {
                if (S_ISDIR(st.st_mode)) {
                    html << "<li><a href=\"" << file_name << "/\">" << file_name << "/</a></li>";
                } else {
                    html << "<li><a href=\"" << file_name << "\">" << file_name << "</a></li>";
                }
            }
        }
        closedir(dir);
    }

    html << "</ul></body></html>";
    return html.str();
}

std::map<std::string, std::string> Webserv::parse_form_data(const std::string &formData)
{
    std::map<std::string, std::string> result;
    std::istringstream ss(formData);
    std::string field;

    while (std::getline(ss, field, '&')) {
        size_t equalPos = field.find('=');
        if (equalPos != std::string::npos) {
            std::string name = field.substr(0, equalPos);
            std::string value = field.substr(equalPos + 1);
            result[name] = value;
        }
    }

    return result;
}

void Webserv::init_servers()
{
	std::vector<int> ports;
	ports.push_back(DEF_PORT);
	// ports.push_back(8888);
	// ports.push_back(7777);

	std::vector<Server>::size_type size = server.size();
	//std::cout << "size: " << size << std::endl;
	for (std::vector<Server>::size_type i = 0; i < size; i++)
	{
		// instead of ports[i] we need to loop through config files and pass port and backlog
		server[i].init_server(ports[i], DEF_BACKLOG);
		poll_fd[i].fd = server[i].getSockfd();
		poll_fd[i].events = POLLIN;
		if (poll_fd.size() < ports.size())
		{
			server.push_back(Server());
			poll_fd.push_back(pollfd());
			size++;
		}
	}

}

std::string Webserv::create_http_response(void)
{
	std::ostringstream sstream;
	http_response.http_version = http_request.http_version;

	// std::cout << "http_request.path: " << http_request.path << std::endl;
	if (http_request.path.find(".html") != std::string::npos)
	{
		http_response.headers["Content-Type"] = "text/html";
		// http_request.path = "." + http_request.path;
	}
	// else if it is a css file, set it to text/css
	else if (http_request.path.find(".css") != std::string::npos)
	{
		http_response.headers["Content-Type"] = "text/css";
		// http_request.path = "." + http_request.path;
	}
	else if (http_request.path.find(".jpg") != std::string::npos)
	{
		http_response.headers["Content-Type"] = "image/jpeg";
		// http_request.path = "." + http_request.path;
	}
	// else set it to text/plain
	else
	{
		http_response.headers["Content-Type"] = "text/html";
		// http_request.path =  http_request.path;
	}

		
	
	// http_response.headers["Content-Type"] = "text/html";

	// http_request.path = "." + http_request.path;
	//Check if it is a file (static website), if not it's a cgi script
	if (access(http_request.path.c_str(), F_OK) != 0)
		http_response.body = http_request.path ;
	else
	{
		std::ifstream file(http_request.path.c_str());
		std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		// std::cout << "str: " << str << std::endl;
		http_response.body = str;
		
	}
	http_response.headers["Content-Length"] = int_to_string(http_response.body.size());

	// The status line format is: HTTP/VERSION STATUS_CODE STATUS_MESSAGE
	sstream << http_response.http_version << " " << http_response.status_code << " " << http_response.status_message << "\r\n";

	// Write each header line
	for (std::map<std::string, std::string>::const_iterator it = http_response.headers.begin(); it != http_response.headers.end(); ++it)
	{
		sstream << it->first << ": " << it->second << "\r\n";
	}

	// Write the body
	sstream << "\r\n"
			<< http_response.body;

	return sstream.str();
}
int Webserv::handle_pollin(int i)
{
    // check events on fd
    // if (poll_fd[i].events & POLLIN)
    // if (poll_fd[i].events == POLLIN)






    if (poll_fd[i].events & POLLIN) //reading data from client
    {
        rc = recv(poll_fd[i].fd, buffer, sizeof(buffer) - 1, 0);
        logging("POLLIN fd" + int_to_string(poll_fd[i].fd) + ": added " + int_to_string(rc) + " bytes into buffer", DEBUG);
        //http_request = parse_http_request(in_request[poll_fd[i].fd]);
        //std::cout << http_request.method << std::endl;
        //std::cout << in_request[poll_fd[i].fd] << std::endl; //this works with POST
        if (rc > 0)
        {
            buffer[rc] = '\0';
            in_request[poll_fd[i].fd] += buffer;
        }
        // logging("buffer:\n" + std::string(buffer), DEBUG);
    }
    else if (poll_fd[i].events == POLLHUP)
    {
        logging("POLLHUP", DEBUG);
        close_conn = TRUE;
        return (1);
    }
    else if (poll_fd[i].events & POLLERR)
    {
        logging("POLLERR", DEBUG);
        close_conn = TRUE;
        return (1);
    }
    else
    {
        logging("Error: Unknown event", DEBUG);
        close_conn = TRUE;
        return (1);
    }
    if (rc < 0)
    {
        //  check if this needs to be handled differently !!!!
        if (poll_fd[i].events & POLLIN)
        {
            // Socket buffer is full, continue with other tasks or wait
            return (1);
        }


        // if (errno == EWOULDBLOCK)
        // {
        // 	// No data available, continue with other tasks or wait
        // 	return (1);
        // }
        // else
        {
            // Handle other errors
            logging("Error: recv() failed", ERROR);
            close_conn = TRUE;
            return (1);
        }
    }
    // Check to see if the connection has been closed by the client
    if (rc == 0)
    {
        logging("Connection closed", DEBUG);
        close_conn = TRUE;
        return (1);
    }
    return (0);
}



void Webserv::run()
{

	end_server = FALSE;
	close_conn = FALSE;
	std::vector<int> sockfds;
	std::vector<pollfd>::size_type size = poll_fd.size();
	for (std::vector<pollfd>::size_type i = 0; i < size; i++)
		sockfds.push_back(poll_fd[i].fd);

	in_request.clear();
	// logging("Listening socket is " + int_to_string(sockfd), DEBUG);
	do
	{
		logging("Waiting on poll()...", INFO);
		rc = poll(&poll_fd[0], poll_fd.size(), -1);
		if (rc < 0)
		{
			logging("Error: poll() failed", ERROR);
			break;
		}
		if (rc == 0)
		{
			logging("Error: poll() timed out. End program.", ERROR);
			break;
		}

		// loop through all the descriptors to see which ones are ready
		std::vector<pollfd>::size_type size = poll_fd.size();
		// std::cout << "size: " << size << std::endl;
		for (std::vector<pollfd>::size_type i = 0; i < size; i++)
		{
			// Loop through to find the descriptors that returned POLLIN
			if (poll_fd[i].revents == 0)
				continue;
			// if revents is not POLLIN than it's an unexpected result
			//  if (poll_fd[i].revents != POLLIN)
			// {
			// 	logging("Error: revents = " + int_to_string(poll_fd[i].revents) + " from " + int_to_string(poll_fd[i].fd), ERROR);
			// 	// std::cout << "Error: revents = " << poll_fd[i].revents << std::endl;
			// 	end_server = TRUE;
			// 	break;
			// }

			// if (poll_fd[i].fd == sockfd || poll_fd[i].fd == sockfd2 || poll_fd[i].fd == sockfd3)
			if (check_sockfds(sockfds, i) == 1)
			{
				logging("Listening socket is readable", DEBUG);
				// Accept all incoming connections that are queued up on the listening socket before we loop back and call poll again
				do
				{
					// Accept each incoming connection
					//  new_sd = accept(sockfd, NULL, NULL);
					new_sd = accept(poll_fd[i].fd, NULL, NULL);
					if (new_sd < 0)
					{
						if (errno != EWOULDBLOCK)
						{
							logging("Error: accept() failed", ERROR);
							end_server = TRUE;
						}
						break;
					}
					poll_fd.push_back(pollfd());
					poll_fd.back().fd = new_sd;
					poll_fd.back().events = POLLIN;
					size++;
				} while (new_sd != -1);
			}
			else
			{
				logging("Descriptor " + int_to_string(poll_fd[i].fd) + " is readable", DEBUG);
				close_conn = FALSE;

				do
				{
					if (handle_pollin(i) != 0)
						break;
					if (poll_fd[i].events | POLLOUT)
					{
//                        std::cout << "request before\n " << in_request[poll_fd[i].fd] << std::endl;
//                        if (in_request[poll_fd[i].fd].find("\r\n\r\n") != std::string::npos)
//						{
//							logging(" ---- request: " + int_to_string(in_request[poll_fd[i].fd].size()) + " bytes received  ----", DEBUG);
//							http_request = parse_http_request(in_request[poll_fd[i].fd]);
//							logging("request :\n" + in_request[poll_fd[i].fd] + "\n", DEBUG);
                            logging(" ---- request: " + int_to_string(in_request[poll_fd[i].fd].size()) + " bytes received  ----", DEBUG);
                            if (http_requests.count(poll_fd[i].fd) > 0) {
                                http_request = http_requests[poll_fd[i].fd];
                                http_request.content += in_request[poll_fd[i].fd];
                                std::cout << "APPEND REQUEST" << std::endl;
                            } else {
                                http_request = parse_http_request(in_request[poll_fd[i].fd]);
                                std::cout << "NEW REQUEST" << std::endl;
                            }
							 if (http_request.method == "GET")
							{
								logging("GET request", DEBUG);
								char *tmp = string_to_chararray(http_request.path);
								
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
												http_request.path = tmp2;
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
												http_request.path = scriptOutput;
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
											http_request.path = tmp;
										}
										else
										{
											http_response.status_code = 403;
											http_response.status_message = "Forbidden";
											http_request.path = "/403.html";
											// out_response[poll_fd[i].fd] = create_http_response();
											std::cout << "file doesn't exist" << std::endl;
										}
										
									}
									else if (is_directory)
									{
										 std::string tmp2 = "." + http_request.path;
										http_request.path = autoindex(tmp2);
										// std::cout << "autoindex http_request.path: " << http_request.path << std::endl;
									}
									out_response[poll_fd[i].fd] = create_http_response();

								}
								else
								{
									http_response.status_code = 404;
									http_response.status_message = "Not Found";
									http_request.path = "./404.html";
									out_response[poll_fd[i].fd] = create_http_response();
									std::cout << "file doesn't exist" << std::endl;
								}
								delete[] tmp;
							}
                             else if  (http_request.method == "POST") {
                                 if (http_requests.count(poll_fd[i].fd) == 0) {
                                     http_requests[poll_fd[i].fd] = http_request;

                                     std::cout << "START CONTENT" << std::endl;
                                 }
                                 size_t content_length = std::stoi(http_request.headers["Content-Length"]);
                                 if (http_request.content.size() == content_length) {
                                     http_requests.erase(poll_fd[i].fd);
                                     std::cout << "FINISH CONTENT" << std::endl;
                                     out_response[poll_fd[i].fd] = post_getdata(i);
                                 }
                                 else if (http_request.content.size() > content_length) {
                                     http_requests.erase(poll_fd[i].fd);

                                     std::cout << "CORRUPTED CONTENT" << std::endl;
                                 } else {
                                     std::cout << "PARTIAL CONTENT " << http_request.content.size() << " of " << content_length << std::endl;
                                 }

                             }
							else if (http_request.method == "DELETE")
							{
								logging("DELETE request", DEBUG);
								// std::cout << "DELETE request" << std::endl;
							}
							else
							{
								logging("Unknown request", DEBUG);
								// std::cout << "Unknown request" << std::endl;
							}

							// -------------------------------------------------------------------------------------------------------------
							// for debugging, print out what is in buffer
							logging("request :\n" + in_request[poll_fd[i].fd] + "\n", DEBUG);
							// -------------------------------------------------------------------------------------------------------------

							rc = send(poll_fd[i].fd, out_response[poll_fd[i].fd].c_str(), out_response[poll_fd[i].fd].size(), 0);
							logging(" ---- response: " + int_to_string(rc) + " bytes sent  ----", DEBUG);
							logging("response :\n" + out_response[poll_fd[i].fd] + "\n", DEBUG);
                            out_response.erase(poll_fd[i].fd);
							break;
						//}
						// rc = send(p_iter.fd, buffer, len, 0);
					}
					else if (poll_fd[i].events | POLLHUP)
					{
						// std::cout << "POLLHUP" << std::endl;
						logging("POLLHUP", DEBUG);
						logging("Connection closed by the client", DEBUG);
						// std::cout << "Connection closed by the client" << std::endl;
						close_conn = TRUE;
						break;
					}
					else if (poll_fd[i].events | POLLERR)
					{
						// std::cout << "POLLERR" << std::endl;
						logging("POLLERR", DEBUG);
						close_conn = TRUE;
						break;
					}
					else
					{
						// std::cout << "Error: Unknown event" << std::endl;
						logging("Error: Unknown event", DEBUG);
						close_conn = TRUE;
						break;
					}
					if (rc < 0)
					{
						if (errno == EWOULDBLOCK)
						{
							// Socket buffer is full, continue with other tasks or wait
							break;
						}
						else
						{
							// Handle other errors
							logging("Error: send() failed", ERROR);
							// std::cout << "Error: send() failed" << std::endl;
							close_conn = TRUE;
							break;
						}
					}

					ft_memset(buffer, 0, sizeof(buffer));
					// } while (TRUE);
				
				} while (!close_conn);
				in_request[poll_fd[i].fd] = "";

				// If the close_conn flag was turned on, we need to clean up this active connection. This clean up process includes removing the descriptor
				if (close_conn)
				{
					close(poll_fd[i].fd);

					poll_fd[i].fd = -1;
					logging("close_conn for: " + int_to_string(poll_fd[i].fd), DEBUG);
					// std::cout << "close_conn for: " << poll_fd[i].fd << std::endl;
					poll_fd.erase(poll_fd.begin() + i);
					size--;
				}
			}
		}

	} while (end_server == FALSE);

	// Close the connections
	p_iter = poll_fd.begin();
	while (p_iter != poll_fd.end())
	{
		// std::cout << "close_conn for: " << p_iter->fd << std::endl;
		logging("close_conn after server shutdown for: " + int_to_string(p_iter->fd), DEBUG);
		if (p_iter->fd >= 0)
			close(p_iter->fd);
		p_iter++;
	}
	return;
}
