/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sung-hle <sung-hle@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/20 08:53:31 by fhassoun          #+#    #+#             */
/*   Updated: 2024/01/22 13:33:32 by sung-hle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"

Webserv::Webserv()
{
	server.push_back(Server());
	poll_fd.push_back(pollfd());
	autoindexBool = true;
    isSameLocation = false;
    isCookies = false;
}

Webserv::~Webserv()
{
	// if (http_request)
	//     delete http_request;
	// if (http_response)
	//     delete http_response;
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
	// check if current fd is one of the server fds
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

std::map<std::string, std::string> Webserv::parse_form_data(const std::string &formData)
{
	std::map<std::string, std::string> result;
	std::istringstream ss(formData);
	std::string field;

	while (std::getline(ss, field, '&'))
	{
		size_t equalPos = field.find('=');
		if (equalPos != std::string::npos)
		{
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
	std::vector<Config *> serverConfigs = this->getConfig();
	for (std::vector<Config *>::iterator itz = serverConfigs.begin(); itz != serverConfigs.end(); ++itz)
	{
		ports.push_back((*itz)->getPort());
		// std::cout << "port: " << (*itz)->getPorts() << std::endl;
	}
	// ports.push_back(DEF_PORT);
	std::cout << "Number of server configurations: " << serverConfigs.size() << std::endl;
	std::cout << "Number of server ports: " << ports.size() << std::endl;
	// ports.push_back(8888);
	// ports.push_back(7777);

	std::vector<Server>::size_type size = server.size();
	// std::cout << "size: " << size << std::endl;
	for (std::vector<Server>::size_type i = 0; i < size; i++)
	{
		// instead of ports[i] we need to loop through config files and pass port and backlog
		std::cout << "init server: " << ports[i] << std::endl;
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

int Webserv::handle_pollin(int i)
{
	// in_request.clear(); //do we need it?
	int clientID = poll_fd[i].fd;
	if (poll_fd[i].events & POLLIN) // reading data from client
	{
		rc = recv(clientID, buffer, sizeof(buffer) - 1, 0);
		logging("POLLIN fd" + int_to_string(clientID) + ": added " + int_to_string(rc) + " bytes into buffer", DEBUG);
		if (rc > 0)
		{
			size_t currentSize = in_request[clientID].size();
			in_request[clientID].resize(currentSize + rc);
			for (int i = 0; i < rc; ++i)
			{
				in_request[clientID][currentSize + i] = static_cast<uint8_t>(buffer[i]);
			}
		}
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
			if (sig_end_server == true)
			{
				logging("Server shutdown", INFO);
				
				break;
			}
			logging("Error: poll() failed", ERROR);
			break;
		}
		if (rc == 0)
		{
			logging("Error: poll() timed out. End program.", ERROR);
			break;
		}

		// loop through all the descriptors to see which ones are ready
		size = poll_fd.size();
		// std::cout << "size: " << size << std::endl;
		for (std::vector<pollfd>::size_type i = 0; i < size; i++)
		{
			// Loop through to find the descriptors that returned POLLIN
			if (poll_fd[i].revents == 0)
				continue;
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
				// logging("Descriptor " + int_to_string(poll_fd[i].fd) + " is readable", DEBUG);
				close_conn = FALSE;

				do
				{
					if (handle_pollin(i) != 0)
						break;
					if (poll_fd[i].events | POLLOUT)
					{
						std::vector<uint8_t> &requestData = in_request[poll_fd[i].fd];
						std::string requestString(requestData.begin(), requestData.end());

						logging("request :\n" + requestString + "\n", DEBUG);
						logging(" ---- request: " + int_to_string(in_request[poll_fd[i].fd].size()) + " bytes received  ----", DEBUG);

						http_response = new Response();
						newOrAppendRequest(i);

                        Config serverConfig = checkConfig(); //works

                        std::string tmp_path = checkPath(http_request->path);
                        if (tmp_path != "")
                            http_request->path = tmp_path;

                        if (http_request->headers.find("Cookie") != http_request->headers.end()) {
                            std::string cookieValue = http_request->headers["Cookie"];
                            if (cookieValue.find("id") != std::string::npos) {
                                isCookies = true;
                            }
                        }
                        if (http_request->method == "GET")
						{
                            if (isSameLocation && !isMethodAllowed("GET")) {
                                http_response->status_code = 405;
                            } else {
                                http_response->status_code = getMethod();
                            }
                            out_response[poll_fd[i].fd] = create_http_response();
							deleteRequest(poll_fd[i].fd);
							close_conn = TRUE;
						}
						else if (http_request->method == "POST")
						{
                            if (isSameLocation && !isMethodAllowed("POST")) {
                                http_response->status_code = 405;
                            } else {
                                http_response->status_code = postMethod(i, serverConfig.getClientBodyBufferSize());
                            }
							// create a response
							out_response[poll_fd[i].fd] = create_http_response();
							if (http_response->status_code == 201 || http_response->status_code == 413 || http_response->status_code == 405
							|| http_response->status_code == 413) {
                                deleteRequest(poll_fd[i].fd);
                            }
						}
						else if (http_request->method == "DELETE")
						{
                            if (isSameLocation && !isMethodAllowed("DELETE")) {
                                http_response->status_code = 405;
                            } else {
                                http_response->status_code = http_request->deleteMethod();
                            }
							out_response[poll_fd[i].fd] = create_http_response();
							deleteRequest(poll_fd[i].fd);
							close_conn = TRUE;
						}
						else
						{
							logging("Unknown request", DEBUG);
							// set the response for internal server error
							http_response->status_code = 500;
							out_response[poll_fd[i].fd] = create_http_response();
							deleteRequest(poll_fd[i].fd);
							close_conn = TRUE;
							// std::cout << "Unknown request" << std::endl;
						}

						size_t res_size = out_response[poll_fd[i].fd]->toString().size();
						const std::string responseStr = out_response[poll_fd[i].fd]->toString();
						const char *resStr = responseStr.c_str();

						rc = send(poll_fd[i].fd, resStr, res_size, 0);
						logging(" ---- response: " + int_to_string(rc) + " bytes sent  ----", DEBUG);
						logging("response :\n" + out_response[poll_fd[i].fd]->toString() + "\n", DEBUG);

						deleteResponse(poll_fd[i].fd);
                        isSameLocation = false;
                        isCookies = false;
						break;
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
				} while (!close_conn);
				// in_request[poll_fd[i].fd] = "";
				in_request[poll_fd[i].fd].clear();

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
		
	// } while (end_server == FALSE && sig_end_server == false);
	} while (end_server == FALSE );

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

void Webserv::setConfig(std::vector<Config *> _config)
{
	config = _config;
}

std::vector<Config *> Webserv::getConfig() const
{
	return config;
}

bool Webserv::isMethodAllowed(std::string method) {
    std::set <std::string> allow_meth = currentLocation->getAllowedMethods();
    std::set<std::string>::iterator it = allow_meth.find(method);

    // Check if the element was found
    if (it != allow_meth.end()) {
        return true;
    } else {
        return false;
    }
}

int Webserv::parseConfig(std::string path)
{
	std::ifstream configFile(path.c_str());

	if (!configFile.is_open())
	{
		std::cerr << "Error: Unable to open the server.conf file." << std::endl;
		return 1;
	}
	// std::vector<Config *> serverConfigs;

	int parseReturn;
	while (!configFile.eof())
	{
		// std::cout << "parsing" << std::endl;
		// Config *serverConfig = new Config();
		serverConfig = new Config();
		parseReturn = serverConfig->parse(configFile);
		// std::cout << "parseReturn: " << parseReturn << std::endl;
		if ( parseReturn == 0)
		{
			// std::cout << "pushing" << std::endl;
			// std::cout << serverConfig->getListen() << std::endl;
			serverConfigs.push_back(serverConfig);
		}
		else {
			// std::cout << "deleting" << std::endl;
			delete serverConfig;
			break;
		}
		
	}
	// std::cout << serverConfigs.size() << std::endl;
	setConfig(serverConfigs);

	configFile.close();
	
	
	// std::cout << parseReturn << std::endl;
	if (parseReturn > 1 ||
		serverConfigs[0]->getListen() == "" ||
		serverConfigs[0]->getHost().empty() ||
		serverConfigs[0]->getLocation().find("/") == serverConfigs[0]->getLocation().end())
	{
		std::cout << "Invalid configuration." << std::endl;
		std::cout << "Min requirements are Port, Host and Root-Location." << std::endl;
		for (std::vector<Config *>::iterator itz = serverConfigs.begin(); itz != serverConfigs.end(); ++itz)
		{
			delete *itz;
		}
		return 2;
	}
	
	// // // Clean up the serverConfigs vector to avoid memory leaks
	// for (std::vector<Config*>::iterator it = serverConfigs.begin(); it != serverConfigs.end(); ++it)
	// {
	// 	delete *it;
	// }
	// serverConfigs.clear();
	
	return 0;
}

// std::string Webserv::getCookie(const std::string& key) const {
//     auto it = cookies.find(key);
//     if (it != cookies.end()) {
//         return it->second;
//     }
//     return "";
// }

// void Webserv::setCookie(const std::string& key, const std::string& value) {
//     cookies[key] = value;
// }