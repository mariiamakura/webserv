/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhassoun <fhassoun@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/20 08:53:31 by fhassoun          #+#    #+#             */
/*   Updated: 2023/11/20 15:57:57 by fhassoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"



Webserv::Webserv()
{
	server.push_back( Server());
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

//setters

//getters

//methods
void Webserv::init_servers()
{
	this->s_iter = server.begin();
	this->p_iter = poll_fd.begin();
	while (s_iter != server.end())
	{
		// !!! need to implement looping through config files and passing port and backlog !!!
		s_iter->init_server(DEF_PORT, DEF_BACKLOG);
		
		std::cout << "socket: " << s_iter->getSockfd() << std::endl;
		std::cout << "poll_fd size: " << poll_fd.size() << std::endl;
		p_iter->fd = s_iter->getSockfd();
		p_iter->events = POLLIN;
		std::cout << "fd: " << p_iter->fd << std::endl;
		p_iter++;
		s_iter++;
	}
}

void Webserv::run()
{
	// int rc = poll(&poll_fd[0], poll_fd.size(), -1);
	// int rc = poll(&poll_fd[0], poll_fd.size(), -1);
	int  end_server = FALSE, close_conn = FALSE;
	int current_size = 0;
	char   buffer[80];
	int nfds = 1;
	int new_sd = 0;
	int len = 1;
	this->sockfd = server[0].getSockfd();
	

	std::cout << "Listening socket is " << sockfd << std::endl;
	// Set the timeout to 3 minutes
	int timeout = (3 * 60 * 1000);
	// int timeout = (30 * 1000);
	do
	{
		std::cout << "Waiting on poll()..." << std::endl;
		// rc = poll(fds, nfds, timeout);
		int rc = poll(&poll_fd[0], poll_fd.size(), timeout);
		
		if (rc < 0)
		{
			std::cout << "Error: poll() failed" << std::endl;
			break;
		}
		if (rc == 0)
		{
			std::cout << "Error: poll() timed out. End program." << std::endl;
			break;	
		}
		current_size = nfds;
		
		// for (i = 0; i < current_size; i++)
		// this->p_iter = poll_fd.begin();
		// while (p_iter != poll_fd.end())
		
		std::vector<pollfd>::size_type size = poll_fd.size();
		for (std::vector<pollfd>::size_type i = 0; i < size; i++)
		{
			//Loop through to find the descriptors that returned POLLIN
			if (poll_fd[i].revents == 0)
				continue;
			
			//if revents is not POLLIN than it's an unexpected result
			if (poll_fd[i].revents != POLLIN)
			{
				std::cout << "Error: revents = " << poll_fd[i].revents << std::endl;
				end_server = TRUE;
				break;
			}

			if (poll_fd[i].fd == sockfd)
			{
				std::cout << "Listening socket is readable" << std::endl;
				//Accept all incoming connections that are queued up on the listening socket before we loop back and call poll again
				do
				{
					//Accept each incoming connection
					new_sd = accept(sockfd, NULL, NULL);
					if (new_sd < 0)
					{
						if (errno != EWOULDBLOCK)
						{
							std::cout << "Error: accept() failed" << std::endl;
							end_server = TRUE;
						}
						break;
					}
					std::cout << "new_sd: " << new_sd << std::endl;
					std::cout << "New incoming connection - " << new_sd << std::endl;
					std::cout << "size: " << poll_fd.size() << "  last fd: " << poll_fd.back().fd << std::endl;
					poll_fd.push_back(pollfd());
					poll_fd.back().fd = new_sd;
					poll_fd.back().events = POLLIN;
					std::cout << "size: " << poll_fd.size() << "  last fd: " << poll_fd.back().fd << "\n" << std::endl;
					size++;
					
					
					// fds[nfds].fd = new_sd;
					// fds[nfds].events = POLLIN;
					// nfds++;
				} while (new_sd != -1);
			}
			else
			{
				std::cout << "Descriptor " << poll_fd[i].fd << " is readable" << std::endl;
				close_conn = FALSE;

				do
				{
					// Receive all incoming data on this socket before we loop back and call poll again
					

					if (poll_fd[i].events | POLLIN)
					{
						std::cout << "POLLIN" << std::endl;
						rc = recv(poll_fd[i].fd, buffer, sizeof(buffer), 0);
						
						in_request[poll_fd[i].fd] += buffer;
						
						// std::cout << "\nin_buffer :\n" << in_request[p_iter.fd] << "\n" << std::endl;
						// std::cout << "buffer: " << buffer << std::endl;
					}
					else if (poll_fd[i].events & POLLHUP)
					{
						std::cout << "POLLHUP" << std::endl;
						close_conn = TRUE;
						break;
					}
					else if (poll_fd[i].events & POLLERR)
					{
						std::cout << "POLLERR" << std::endl;
						close_conn = TRUE;
						break;
					}
					else
					{
						std::cout << "Error: Unknown event" << std::endl;
						close_conn = TRUE;
						break;
					}


					if (rc < 0)
					{
						if (errno == EWOULDBLOCK) {
							// No data available, continue with other tasks or wait
							break;
						} else {
							// Handle other errors
							std::cout << "Error: recv() failed" << std::endl;
							close_conn = TRUE;
							break;
						}
					}

					
					//Check to see if the connection has been closed by the client
					if (rc == 0)
					{
						std::cout << "Connection closed" << std::endl;
						close_conn = TRUE;
						break;
					}
					// std::cout << "buffer: " << buffer << std::endl;
					//Data was received
					len = rc;
					std::cout << len << " bytes received" << std::endl;
					
					std::string response = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 23\n\nHello, World from fd " + int_to_string(p_iter->fd) + "\n";
				
					if (poll_fd[i].events | POLLOUT)
					{
						if (buffer[rc - 1] == '\n' && buffer[rc - 2] == '\r' && buffer[rc - 3] == '\n' && buffer[rc - 4] == '\r')
						{
							std::cout << " ---- request received  ----"  << std::endl;
							std::cout << "POLLOUT" << std::endl;
						

// -------------------------------------------------------------------------------------------------------------
							std::cout << "\nin_buffer :\n" << in_request[poll_fd[i].fd] << "\n" << std::endl;
							// getting html files
							// if (strncmp(buffer, "GET /index.html", 15))
							if (in_request[poll_fd[i].fd].find("GET /index2.html") != std::string::npos)
							{
								std::cout << "GET index2.html" << std::endl;
								std::ifstream file("index2.html");
								std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
								response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " + int_to_string(str.size()) + "\n\n" + str;
								
								rc = send(poll_fd[i].fd, response.c_str(), response.size(), 0);
							}
							
							if (in_request[poll_fd[i].fd].find("GET /index.html") != std::string::npos)
							{
								std::cout << "GET index.html" << std::endl;
								std::ifstream file("index.html");
								std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
								response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " + int_to_string(str.size()) + "\n\n" + str;
								
								rc = send(poll_fd[i].fd, response.c_str(), response.size(), 0);
							}
							

							// getting simple responses 
							// else if (strncmp(buffer, "GET /", 5))
							else if (in_request[poll_fd[i].fd].find("GET /") != std::string::npos)
							{
								std::cout << "GET" << std::endl;
								rc = send(poll_fd[i].fd, response.c_str(), response.size(), 0);
							}
// -------------------------------------------------------------------------------------------------------------
							
							std::cout << rc << " bytes sent" << std::endl;
							break ;
						}
						// rc = send(p_iter.fd, buffer, len, 0);
					}
					else if (poll_fd[i].events | POLLHUP)
					{
						std::cout << "POLLHUP" << std::endl;
						std::cout << "Connection closed by the client" << std::endl;
						close_conn = TRUE;
						break;
					}
					else if (poll_fd[i].events | POLLERR)
					{
						std::cout << "POLLERR" << std::endl;
						close_conn = TRUE;
						break;
					}
					else
					{
						std::cout << "Error: Unknown event" << std::endl;
						close_conn = TRUE;
						break;
					}
					if (rc < 0)
					{
						if (errno == EWOULDBLOCK) {
							// Socket buffer is full, continue with other tasks or wait
							break;
						} else {
							// Handle other errors
							std::cout << "Error: send() failed" << std::endl;
							close_conn = TRUE;
							break;
						}
					}
					
					// if (rc < 0) {
					// 	std::cout << "Error: send() failed" << std::endl;
					// 	close_conn = TRUE;
					// 	break;
					// }
				
					// if (rc < 0)
					// {
					// 	std::cout << "Error: send() failed" << std::endl;
					// 	close_conn = TRUE;
					// 	break;
					// }
					ft_memset(buffer, 0, sizeof(buffer));
				} while (TRUE);
				in_request[poll_fd[i].fd] = "";	
				
				//If the close_conn flag was turned on, we need to clean up this active connection. This clean up process includes removing the descriptor
				if (close_conn)
				{
					close(poll_fd[i].fd);
					std::cout << "close_conn for: " << poll_fd[i].fd << std::endl;
					poll_fd.erase(poll_fd.begin() + i);
					
					
					
				}
			}
			p_iter++;
		}
		// if (compress_array)
		// {
		// 	compress_array = FALSE;
		// 	for ( i = 0; i < nfds; i++)
		// 	{
		// 		if (p_iter.fd == -1)
		// 		{
		// 		for( j = i; j < nfds; j++)
		// 		{
		// 			fds[j].fd = fds[j+1].fd;
		// 		}
		// 		i--;
		// 		nfds--;
		// 		}
		// 	}
		// }
		
		
	} while (end_server == FALSE);
	
	

	
	// Close the connections
	// for (int i = 0; i < nfds; i++)
	p_iter = poll_fd.begin();
	while (p_iter != poll_fd.end())	
	{
		if (p_iter->fd >= 0)
			close(p_iter->fd);
	}

	return ;
}

