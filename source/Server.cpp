/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhassoun <fhassoun@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/13 11:43:52 by fhassoun          #+#    #+#             */
/*   Updated: 2023/11/24 14:16:32 by fhassoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server()
{

	
	
}

Server::~Server()
{
}

Server::Server(Server const &src)
{
	*this = src;
}

Server &Server::operator=(Server const &src)
{
	if (this == &src)
		return (*this);
	return (*this);
}


//getter
// void Server::setConfig(std::vector<Config> config)
// {
// 	this->_config = config;
// }

std::map <int, std::string> Server::getInRequest()
{
	return (in_request);
}

std::map <int, std::string> Server::getOutResponse()
{
	return (out_response);
}

int Server::getSockfd()
{
	return (sockfd);
}





//setter
// void Server::setConfig(std::vector<Config> config)
// {
// 	this->_config = config;
// }

void Server::setInRequest(std::map <int, std::string> in_request)
{
	this->in_request = in_request;
}

void Server::setOutResponse(std::map <int, std::string> out_response)
{
	this->out_response = out_response;
}


void Server::parseConfig(char *path)
{
	//open the file
	std::fstream conf;
	std::string str;
	Config tmpConf;
	

	// memset(&tmpConf, 0, sizeof(tmpConf));
	std::vector<Config>::iterator iter = this->_config.begin();

	conf.open(path, std::ifstream::in);
	if (!conf.is_open())
	{
		std::cout << "Error : could not open config-file.\nUsing Default!" << std::endl;
	}

	// if (std::getline(conf, str).eof())
	// {
	// 	std::cout << "Error : File empty or no data in.\nUsing Default!" << std::endl;
	// }

	while (!std::getline(conf, str).eof())
	{
		if (str.compare("server {") == 0)
		{

			//tmpConf = new Config();
			
			std::cout << "server start" << std::endl;
		
		}
		else if (str.compare( "}") == 0)
		{
			
			this->_config.push_back(tmpConf);

			// std::cout << "after push_back start this: " << this->_config.begin()->getListen() << std::endl;
			//delete tmpConf;
			iter++;
			std::cout << "server end" << std::endl;
			
		}
		else 
		{
			// std::cout << str << std::endl;
			// if (str.find("listen") != std::string::npos)
			// {
			// 	// std::cout << "listen" << std::endl;
			// 	tmpConf.setListen(str);
			// 	std::cout << "parsed port: " << tmpConf.getListen() << std::endl;
			// }
			
			

			
		}
	}
	
	

	
	//read the file
	//parse the file
	//create a config object
	//add the config object to the vector
	//repeat for the next config

	std::cout << "config parsed :)" << std::endl;
	conf.close();
}



void Server::init_server(int port, int backlog)
{

	// int sockfd;
	int rc, on;
	pollfd fds[200];

	rc = 0;
	on = 1;
	sockaddr_in sockaddr;
	std::vector<Client> clients;
	
	// Create a socket (IPv4, TCP)
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	std::cout << "sockfd: " << sockfd << std::endl;
	if (sockfd == -1)
	{
		std::cout << "Failed to create socket. errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}
	
	//Allow socket descriptor to be reuseable  
	rc = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
	if (rc < 0)
	{
		std::cout << "setsockopt() failed" << std::endl;
		close(sockfd);
		exit(EXIT_FAILURE);
	}
	rc = fcntl(sockfd, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	if (rc < 0)
	{
		std::cout << "fcntl() failed" << std::endl;
		close(sockfd);
		exit(EXIT_FAILURE);
	}
	
	
	// Listen to port  on any address
	ft_memset(&sockaddr, 0, sizeof(sockaddr));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = INADDR_ANY;
	ft_memcpy(&sockaddr.sin_addr, &sockaddr.sin_addr.s_addr, sizeof(sockaddr.sin_addr.s_addr));
	
	sockaddr.sin_port = htons(port); // htons is necessary to convert a number to
									// network byte order

	// Bind the socket to the specified IP address
	rc = bind(sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr));
	if (rc < 0)
	{
		std::cout << "Failed to bind to port " << port << ". errno: " << errno << std::endl;
		close(sockfd);
		exit(EXIT_FAILURE);
	}
	// Start listening. Hold at most 100 connections in the queue
	rc = listen(sockfd, backlog);
	if (rc < 0)
	{
		std::cout << "Failed to listen on socket. errno: " << errno << std::endl;
		close(sockfd);
		exit(EXIT_FAILURE);
	}

	//setup poll structure
	ft_memset(fds, 0 , sizeof(fds));
	this->sockfd = sockfd;
	fds[0].fd = sockfd;
	fds[0].events = POLLIN;

	//initialize timeout to 3 minutes
	// int timeout = (3 * 60 * 1000);
	

// 	do
// 	{
// 		std::cout << "Waiting on poll()..." << std::endl;
// 		rc = poll(fds, nfds, timeout);
		
// 		if (rc < 0)
// 		{
// 			std::cout << "Error: poll() failed" << std::endl;
// 			break;
// 		}
// 		if (rc == 0)
// 		{
// 			std::cout << "Error: poll() timed out. End program." << std::endl;
// 			break;	
// 		}
// 		current_size = nfds;
// 		for (i = 0; i < current_size; i++)
// 		{
// 			//Loop through to find the descriptors that returned POLLIN
// 			if (fds[i].revents == 0)
// 				continue;
			
// 			//if revents is not POLLIN than it's an unexpected result
// 			if (fds[i].revents != POLLIN)
// 			{
// 				std::cout << "Error: revents = " << fds[i].revents << std::endl;
// 				end_server = TRUE;
// 				break;
// 			}

// 			if (fds[i].fd == sockfd)
// 			{
// 				std::cout << "Listening socket is readable" << std::endl;
// 				//Accept all incoming connections that are queued up on the listening socket before we loop back and call poll again
// 				do
// 				{
// 					//Accept each incoming connection
// 					new_sd = accept(sockfd, NULL, NULL);
// 					if (new_sd < 0)
// 					{
// 						if (errno != EWOULDBLOCK)
// 						{
// 							std::cout << "Error: accept() failed" << std::endl;
// 							end_server = TRUE;
// 						}
// 						break;
// 					}
// 					std::cout << "New incoming connection - " << new_sd << std::endl;
// 					fds[nfds].fd = new_sd;
// 					fds[nfds].events = POLLIN;
// 					nfds++;
// 				} while (new_sd != -1);
// 			}
// 			else
// 			{
// 				std::cout << "Descriptor " << fds[i].fd << " is readable" << std::endl;
// 				close_conn = FALSE;

// 				do
// 				{
// 					// Receive all incoming data on this socket before we loop back and call poll again
					

// 					if (fds[i].events | POLLIN)
// 					{
// 						std::cout << "POLLIN" << std::endl;
// 						rc = recv(fds[i].fd, buffer, sizeof(buffer), 0);
						
// 						in_request[fds[i].fd] += buffer;
						
// 						// std::cout << "\nin_buffer :\n" << in_request[fds[i].fd] << "\n" << std::endl;
// 						// std::cout << "buffer: " << buffer << std::endl;
// 					}
// 					else if (fds[i].events & POLLHUP)
// 					{
// 						std::cout << "POLLHUP" << std::endl;
// 						close_conn = TRUE;
// 						break;
// 					}
// 					else if (fds[i].events & POLLERR)
// 					{
// 						std::cout << "POLLERR" << std::endl;
// 						close_conn = TRUE;
// 						break;
// 					}
// 					else
// 					{
// 						std::cout << "Error: Unknown event" << std::endl;
// 						close_conn = TRUE;
// 						break;
// 					}


					

// 					// if the last 4 bytes of buffer are \r\n\r\n
					
					
					
					
					
				
// 					// while (rc != 0)
// 					// {
// 					// 	//check if the last 4 bytes are \r\n\r\n
// 					// 	rc = recv(fds[i].fd, buffer, sizeof(buffer), 0);
// 					// 	if (buffer[rc - 1] == '\n' && buffer[rc - 2] == '\r' && buffer[rc - 3] == '\n' && buffer[rc - 4] == '\r')
// 					// 	{
// 					// 		std::cout << " ---- request received  ----"  << std::endl;
// 					// 		break ;
// 					// 	}
// 					// }
// 					if (rc < 0)
// 					{
// 						if (errno == EWOULDBLOCK) {
// 							// No data available, continue with other tasks or wait
// 							break;
// 						} else {
// 							// Handle other errors
// 							std::cout << "Error: recv() failed" << std::endl;
// 							close_conn = TRUE;
// 							break;
// 						}
// 					}

					
// 					//Check to see if the connection has been closed by the client
// 					if (rc == 0)
// 					{
// 						std::cout << "Connection closed" << std::endl;
// 						close_conn = TRUE;
// 						break;
// 					}
// 					// std::cout << "buffer: " << buffer << std::endl;
// 					//Data was received
// 					len = rc;
// 					std::cout << len << " bytes received" << std::endl;
					
// 					//Echo the data back to the client
// 					// rc = send(fds[i].fd, buffer, len, 0);
// 					// std::string response = "HTTP/1.1 200 OK \nContent-Type: text/html\n\n<html><body><h1>Hello, World!</h1></body></html>\n";
// 					//  std::string response = "HTTP/1.1 200 OK /n/r/n/r Content-Type: text/html /n/r/n/r <html><body><h1>Hello, World!</h1></body></html> /n/r/n/r";

// 					// std::string response = "hello 2";
// 					// std::string rresponse = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " + int_to_string(response.size()) + "\n\n" + response;
// 					std::string response = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 23\n\nHello, World from fd " + int_to_string(fds[i].fd) + "\n";
				
// 					if (fds[i].events | POLLOUT)
// 					{
// 						if (buffer[rc - 1] == '\n' && buffer[rc - 2] == '\r' && buffer[rc - 3] == '\n' && buffer[rc - 4] == '\r')
// 						{
// 							std::cout << " ---- request received  ----"  << std::endl;
// 							std::cout << "POLLOUT" << std::endl;
						

// // -------------------------------------------------------------------------------------------------------------
// 							std::cout << "\nin_buffer :\n" << in_request[fds[i].fd] << "\n" << std::endl;
// 							// getting html files
// 							// if (strncmp(buffer, "GET /index.html", 15))
// 							if (in_request[fds[i].fd].find("GET /index2.html") != std::string::npos)
// 							{
// 								std::cout << "GET index2.html" << std::endl;
// 								std::ifstream file("index2.html");
// 								std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
// 								response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " + int_to_string(str.size()) + "\n\n" + str;
								
// 								rc = send(fds[i].fd, response.c_str(), response.size(), 0);
// 							}
							
// 							if (in_request[fds[i].fd].find("GET /index.html") != std::string::npos)
// 							{
// 								std::cout << "GET index.html" << std::endl;
// 								std::ifstream file("index.html");
// 								std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
// 								response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " + int_to_string(str.size()) + "\n\n" + str;
								
// 								rc = send(fds[i].fd, response.c_str(), response.size(), 0);
// 							}
							

// 							// getting simple responses 
// 							// else if (strncmp(buffer, "GET /", 5))
// 							else if (in_request[fds[i].fd].find("GET /") != std::string::npos)
// 							{
// 								std::cout << "GET" << std::endl;
// 								rc = send(fds[i].fd, response.c_str(), response.size(), 0);
// 							}
// // -------------------------------------------------------------------------------------------------------------
							
// 							std::cout << rc << " bytes sent" << std::endl;
// 							break ;
// 						}
// 						// rc = send(fds[i].fd, buffer, len, 0);
// 					}
// 					else if (fds[i].events | POLLHUP)
// 					{
// 						std::cout << "POLLHUP" << std::endl;
// 						std::cout << "Connection closed by the client" << std::endl;
// 						close_conn = TRUE;
// 						break;
// 					}
// 					else if (fds[i].events | POLLERR)
// 					{
// 						std::cout << "POLLERR" << std::endl;
// 						close_conn = TRUE;
// 						break;
// 					}
// 					else
// 					{
// 						std::cout << "Error: Unknown event" << std::endl;
// 						close_conn = TRUE;
// 						break;
// 					}
// 					if (rc < 0)
// 					{
// 						if (errno == EWOULDBLOCK) {
// 							// Socket buffer is full, continue with other tasks or wait
// 							break;
// 						} else {
// 							// Handle other errors
// 							std::cout << "Error: send() failed" << std::endl;
// 							close_conn = TRUE;
// 							break;
// 						}
// 					}
					
// 					// if (rc < 0) {
// 					// 	std::cout << "Error: send() failed" << std::endl;
// 					// 	close_conn = TRUE;
// 					// 	break;
// 					// }
				
// 					// if (rc < 0)
// 					// {
// 					// 	std::cout << "Error: send() failed" << std::endl;
// 					// 	close_conn = TRUE;
// 					// 	break;
// 					// }
// 					ft_memset(buffer, 0, sizeof(buffer));
// 				} while (TRUE);
// 				in_request[fds[i].fd] = "";	
				
// 				//If the close_conn flag was turned on, we need to clean up this active connection. This clean up process includes removing the descriptor
// 				if (close_conn)
// 				{
// 					close(fds[i].fd);
// 					fds[i].fd = -1;
// 					compress_array = TRUE;
// 				}
// 			}
			
// 		}
// 		if (compress_array)
// 		{
// 			compress_array = FALSE;
// 			for ( i = 0; i < nfds; i++)
// 			{
// 				if (fds[i].fd == -1)
// 				{
// 				for( j = i; j < nfds; j++)
// 				{
// 					fds[j].fd = fds[j+1].fd;
// 				}
// 				i--;
// 				nfds--;
// 				}
// 			}
// 		}
// 	} while (end_server == FALSE);
	
	

	
// 	// Close the connections
// 	for (int i = 0; i < nfds; i++)
// 	{
// 		if (fds[i].fd >= 0)
// 			close(fds[i].fd);
// 	}
	
// 	// if (bind(sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0)
// 	// {
// 	// 	std::cout << "Failed to bind to port " << port << ". errno: " << errno << std::endl;
// 	// 	exit(EXIT_FAILURE);
// 	// }

// 	// // Start listening. Hold at most 10 connections in the queue
// 	// if (listen(sockfd, backlog) < 0)
// 	// {
// 	// 	std::cout << "Failed to listen on socket. errno: " << errno << std::endl;
// 	// 	exit(EXIT_FAILURE);
// 	// }

// 	return ;
}

// int accept_new_connection(int sockfd)
// {
// 	sockaddr_in sockaddr;
// 	// Grab a connection from the queue
// 	int addrlen = sizeof(sockaddr);
// 	int connection = accept(sockfd, (struct sockaddr *)&sockaddr, (socklen_t *)&addrlen);
// 	if (connection < 0)
// 	{
// 		std::cout << "Failed to grab connection. errno: " << errno << std::endl;
// 		exit(EXIT_FAILURE);
// 	}
// 	return (connection);
// }

// void Server::run()
// {

	
// 	 init_server(DEF_PORT, DEF_BACKLOG);


		
	
// }