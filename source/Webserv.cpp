/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhassoun <fhassoun@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/20 08:53:31 by fhassoun          #+#    #+#             */
/*   Updated: 2023/11/21 15:19:08 by fhassoun         ###   ########.fr       */
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

void Webserv::logging(std::string str, int status)
{
	//get time
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
	time_t rawtime;
	struct tm * timeinfo;
	char buffer [80];
	time (&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buffer, sizeof(buffer), "[%d/%m/%Y %H:%M:%S] ", timeinfo);
	std::string time(buffer);
	std::cout << color1 << time << color2 << " " << str << RESET << std::endl;
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
		

		p_iter->fd = s_iter->getSockfd();
		p_iter->events = POLLIN;
		p_iter++;
		s_iter++;
	}
}

void Webserv::run()
{
	// int rc = poll(&poll_fd[0], poll_fd.size(), -1);
	// int rc = poll(&poll_fd[0], poll_fd.size(), -1);
	int  end_server = FALSE, close_conn = FALSE;
	// int current_size = 0;
	char   buffer[80];
	// int nfds = 1;
	int new_sd = 0;
	int len = 1;
	this->sockfd = server[0].getSockfd();
	

	// std::cout << "Listening socket is " << sockfd << std::endl;
	logging("Listening socket is " + int_to_string(sockfd), DEBUG);
	// Set the timeout to 3 minutes
	// int timeout = (3 * 60 * 1000);
	// int timeout = (30 * 1000);
	do
	{
		// std::cout << "Waiting on poll()..." << std::endl;
		logging("Waiting on poll()...", INFO);
		// rc = poll(fds, nfds, timeout);
		// int rc = poll(&poll_fd[0], poll_fd.size(), timeout);
		int rc = poll(&poll_fd[0], poll_fd.size(), -1);
		if (rc < 0)
		{
			logging("Error: poll() failed", ERROR);
			// std::cout << "Error: poll() failed" << std::endl;
			break;
		}
		if (rc == 0)
		{
			logging("Error: poll() timed out. End program.", ERROR);
			// std::cout << "Error: poll() timed out. End program." << std::endl;
			break;	
		}
	
		// loop through all the descriptors to see which ones are ready 
		std::vector<pollfd>::size_type size = poll_fd.size();
		for (std::vector<pollfd>::size_type i = 0; i < size; i++)
		{
			//Loop through to find the descriptors that returned POLLIN
			if (poll_fd[i].revents == 0)
				continue;
			//if revents is not POLLIN than it's an unexpected result
			if (poll_fd[i].revents != POLLIN)
			{
				logging("Error: revents = " + int_to_string(poll_fd[i].revents) + " from " + int_to_string(poll_fd[i].fd), ERROR );
				// std::cout << "Error: revents = " << poll_fd[i].revents << std::endl;
				end_server = TRUE;
				break;
			}

			if (poll_fd[i].fd == sockfd)
			{
				logging("Listening socket is readable", DEBUG);
				// std::cout << "Listening socket is readable" << std::endl;
				//Accept all incoming connections that are queued up on the listening socket before we loop back and call poll again
				do
				{
					//Accept each incoming connection
					new_sd = accept(sockfd, NULL, NULL);
					if (new_sd < 0)
					{
						if (errno != EWOULDBLOCK)
						{
							logging("Error: accept() failed", ERROR);
							// std::cout << "Error: accept() failed" << std::endl;
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
				// std::cout << "Descriptor " << poll_fd[i].fd << " is readable" << std::endl;
				close_conn = FALSE;

				do
				{
					// Receive all incoming data on this socket before we loop back and call poll again
					if (poll_fd[i].events | POLLIN)
					{
						// std::cout << "POLLIN" << std::endl;
						rc = recv(poll_fd[i].fd, buffer, sizeof(buffer), 0);
						logging("POLLIN fd" +  int_to_string(poll_fd[i].fd) +": added " +  int_to_string(rc) + " bytes into buffer", DEBUG);
						
						in_request[poll_fd[i].fd] += buffer;
						
						
						// std::cout << "\nin_buffer :\n" << in_request[p_iter.fd] << "\n" << std::endl;
						// std::cout << "buffer: " << buffer << std::endl;
					}
					else if (poll_fd[i].events & POLLHUP)
					{
						logging("POLLHUP", DEBUG);
						// std::cout << "POLLHUP" << std::endl;
						close_conn = TRUE;
						break;
					}
					else if (poll_fd[i].events & POLLERR)
					{
						logging("POLLERR", DEBUG);
						// std::cout << "POLLERR" << std::endl;
						close_conn = TRUE;
						break;
					}
					else
					{
						logging("Error: Unknown event", DEBUG);
						// std::cout << "Error: Unknown event" << std::endl;
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
							logging("Error: recv() failed", ERROR);
							// std::cout << "Error: recv() failed" << std::endl;
							close_conn = TRUE;
							break;
						}
					}

					
					//Check to see if the connection has been closed by the client
					if (rc == 0)
					{
						logging("Connection closed", DEBUG);
						// std::cout << "Connection closed" << std::endl;
						close_conn = TRUE;
						break;
					}
					// std::cout << "buffer: " << buffer << std::endl;
					//Data was received
					len = rc;
					// logging(int_to_string(len) + " bytes received", DEBUG);
					// std::cout << len << " bytes received" << std::endl;
					
				
					if (poll_fd[i].events | POLLOUT)
					{
						if (buffer[rc - 1] == '\n' && buffer[rc - 2] == '\r' && buffer[rc - 3] == '\n' && buffer[rc - 4] == '\r')
						{
							// logging(int_to_string(in_request[poll_fd[i].fd].size() ) + " bytes received", DEBUG);
							logging(" ---- request: " + int_to_string(in_request[poll_fd[i].fd].size()) + " bytes received  ----", DEBUG);
							// std::cout << " ---- request received  ----"  << std::endl;
							// logging(in_request[poll_fd[i].fd], DEBUG);
							// std::cout << "POLLOUT" << std::endl;
						

// -------------------------------------------------------------------------------------------------------------
							//for debugging, print out what is in buffer
							// logging("in_buffer :\n" + in_request[poll_fd[i].fd] + "\n", DEBUG);
							// std::cout << "\nin_buffer :\n" << in_request[poll_fd[i].fd] << "\n" << std::endl;
							
							// getting html files
							// if (strncmp(buffer, "GET /index.html", 15))

							if (in_request[poll_fd[i].fd].find("GET /over42/index.html") != std::string::npos)
							{
								// std::cout << "GET index2.html" << std::endl;
								logging("getting the site", DEBUG);
								std::ifstream file("/home/fhassoun/dev_area/core/my_github/webserv_git/over42/index.html");
								std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
								out_response[poll_fd[i].fd] = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " + int_to_string(str.size()) + "\n\n" + str;
								// response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " + int_to_string(str.size()) + "\n\n" + str;
								
								// std::cout << response << std::endl;
								// rc = send(poll_fd[i].fd, response.c_str(), response.size(), 0);
							}
							
							else if (in_request[poll_fd[i].fd].find("/over42/images/muppet_opas.jpg") != std::string::npos)
							{
								// std::cout << "GET index2.html" << std::endl;
								logging("getting the image", DEBUG);
								std::ifstream file("/home/fhassoun/dev_area/core/my_github/webserv_git/over42/images/muppet_opas.jpg");
								
								std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
								out_response[poll_fd[i].fd] = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " + int_to_string(str.size()) + "\n\n" + str;
								// response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " + int_to_string(str.size()) + "\n\n" + str;
								
								// std::cout << response << std::endl;
								// rc = send(poll_fd[i].fd, response.c_str(), response.size(), 0);
							}
							
							else if (in_request[poll_fd[i].fd].find("GET /index2.html") != std::string::npos)
							{
								// std::cout << "GET index2.html" << std::endl;
								std::ifstream file("index2.html");
								std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
								out_response[poll_fd[i].fd] = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " + int_to_string(str.size()) + "\n\n" + str;
								// response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " + int_to_string(str.size()) + "\n\n" + str;
								
								// rc = send(poll_fd[i].fd, response.c_str(), response.size(), 0);
							}
							
							else if (in_request[poll_fd[i].fd].find("GET /index.html") != std::string::npos)
							{
								// std::cout << "GET index.html" << std::endl;
								std::ifstream file("index.html");
								std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
								out_response[poll_fd[i].fd] = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " + int_to_string(str.size()) + "\n\n" + str;
								// response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " + int_to_string(str.size()) + "\n\n" + str;
								
								// rc = send(poll_fd[i].fd, response.c_str(), response.size(), 0);
							}
							

							// getting simple responses 
							// else if (strncmp(buffer, "GET /", 5))
							else if (in_request[poll_fd[i].fd].find("GET /") != std::string::npos)
							{
								// std::cout << "GET" << std::endl;
								out_response[poll_fd[i].fd] = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 23\n\nHello, World from fd " + int_to_string(poll_fd[i].fd) + "\n";
								// response = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 23\n\nHello, World from fd " + int_to_string(p_iter->fd) + "\n";

							}
							rc = send(poll_fd[i].fd, out_response[poll_fd[i].fd].c_str(), out_response[poll_fd[i].fd].size(), 0);
							// rc = send(poll_fd[i].fd, response.c_str(), response.size(), 0);
// -------------------------------------------------------------------------------------------------------------
							
							logging(" ---- response: " + int_to_string(rc) + " bytes sent  ----", DEBUG);
							// logging(int_to_string(rc) + " bytes sent", DEBUG);
							
							// std::cout << rc << " bytes sent" << std::endl;
							break ;
						}
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
						if (errno == EWOULDBLOCK) {
							// Socket buffer is full, continue with other tasks or wait
							break;
						} else {
							// Handle other errors
							logging("Error: send() failed", ERROR);
							// std::cout << "Error: send() failed" << std::endl;
							close_conn = TRUE;
							break;
						}
					}
					

					ft_memset(buffer, 0, sizeof(buffer));
				} while (TRUE);
				in_request[poll_fd[i].fd] = "";	
				
				//If the close_conn flag was turned on, we need to clean up this active connection. This clean up process includes removing the descriptor
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
	return ;
}

