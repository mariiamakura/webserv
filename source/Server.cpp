/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhassoun <fhassoun@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/13 11:43:52 by fhassoun          #+#    #+#             */
/*   Updated: 2023/11/14 14:25:52 by fhassoun         ###   ########.fr       */
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

void Server::parseConfig(char *path)
{
	//open the file
	std::fstream conf;
	std::string str;
	Config tmpConf;
	

	memset(&tmpConf, 0, sizeof(tmpConf));
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

			std::cout << "after push_back start this: " << this->_config.begin()->getListen() << std::endl;
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

	int rc, on, len = 1;
	pollfd fds[200];
	int  end_server = FALSE, close_conn = FALSE, compress_array = FALSE;
	int current_size = 0;
	char   buffer[1000];
	int nfds = 1, i = 0, j = 1;
	int new_sd = 0;
	
	// Create a socket (IPv4, TCP)
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		std::cout << "Failed to create socket. errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}
	//Allow socket descriptor to be reuseable  
	rc = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
	if (rc < 0)
	{
		perror("setsockopt() failed");
		close(sockfd);
		exit(-1);
	}
	rc = ioctl(sockfd, FIONBIO, (char *)&on);
	if (rc < 0)
	{
		perror("ioctl() failed");
		close(sockfd);
		exit(-1);
	}
	
	
	// Listen to port  on any address
	sockaddr_in sockaddr;
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

	ft_memset(&fds, 0 , sizeof(fds));
	//setup poll structure
	fds[0].fd = sockfd;
	fds[0].events = POLLIN;

	//initialize timeout to 3 minutes
	int timeout = (3 * 60 * 1000);

	do
	{
		std::cout << "Waiting on poll()..." << std::endl;
		rc = poll(fds, nfds, timeout);
		
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
		for (i = 0; i < current_size; i++)
		{
			//Loop through to find the descriptors that returned POLLIN
			if (fds[i].revents == 0)
				continue;
			
			//if revents is not POLLIN than it's an unexpected result
			if (fds[i].revents != POLLIN)
			{
				std::cout << "Error: revents = " << fds[i].revents << std::endl;
				end_server = TRUE;
				break;
			}

			if (fds[i].fd == sockfd)
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
					std::cout << "New incoming connection - " << new_sd << std::endl;
					fds[nfds].fd = new_sd;
					fds[nfds].events = POLLIN;
					nfds++;
				} while (new_sd != -1);
			}
			else
			{
				std::cout << "Descriptor " << fds[i].fd << " is readable" << std::endl;
				close_conn = FALSE;

				do
				{
					//Receive all incoming data on this socket before we loop back and call poll again
					rc = recv(fds[i].fd, buffer, sizeof(buffer), 0);
					if (rc < 0)
					{
						if (errno != EWOULDBLOCK)
						{
							std::cout << "Error: recv() failed" << std::endl;
							close_conn = TRUE;
						}
						
						break;
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
					
					//Echo the data back to the client
					// std::string response = "HTTP/1.1 200 OK \nContent-Type: text/html\n\n<html><body><h1>Hello, World!</h1></body></html>\n";
					 std::string response = "HTTP/1.1 200 OK /n/r/n/r Content-Type: text/html /n/r/n/r <html><body><h1>Hello, World!</h1></body></html> /n/r/n/r";

		
					rc = send(fds[i].fd, response.c_str(), response.size(), 0);
					// rc = send(fds[i].fd, buffer, len, 0);
					if (rc < 0)
					{
						std::cout << "Error: send() failed" << std::endl;
						close_conn = TRUE;
						break;
					}	
				} while (TRUE);
				
				//If the close_conn flag was turned on, we need to clean up this active connection. This clean up process includes removing the descriptor
				if (close_conn)
				{
					close(fds[i].fd);
					fds[i].fd = -1;
					compress_array = TRUE;
				}
			}
			
		}
		if (compress_array)
		{
			compress_array = FALSE;
			for ( i = 0; i < nfds; i++)
			{
				if (fds[i].fd == -1)
				{
				for( j = i; j < nfds; j++)
				{
					fds[j].fd = fds[j+1].fd;
				}
				i--;
				nfds--;
				}
			}
		}
	} while (end_server == FALSE);
	
	

	
	// Close the connections
	for (int i = 0; i < nfds; i++)
	{
		if (fds[i].fd >= 0)
			close(fds[i].fd);
	}
	
	// if (bind(sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0)
	// {
	// 	std::cout << "Failed to bind to port " << port << ". errno: " << errno << std::endl;
	// 	exit(EXIT_FAILURE);
	// }

	// // Start listening. Hold at most 10 connections in the queue
	// if (listen(sockfd, backlog) < 0)
	// {
	// 	std::cout << "Failed to listen on socket. errno: " << errno << std::endl;
	// 	exit(EXIT_FAILURE);
	// }

	return ;
}

int accept_new_connection(int sockfd)
{
	sockaddr_in sockaddr;
	// Grab a connection from the queue
	int addrlen = sizeof(sockaddr);
	int connection = accept(sockfd, (struct sockaddr *)&sockaddr, (socklen_t *)&addrlen);
	if (connection < 0)
	{
		std::cout << "Failed to grab connection. errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}
	return (connection);
}

void Server::run()
{

	
	 init_server(DEF_PORT, DEF_BACKLOG);


	// int i = 0;
	
	// while (1)
	// {
	// 	// Grab a connection from the queue
	// 	int connection = accept_new_connection(sockfd);
	

	// 	// Read from the connection
	// 	char buffer[1000000];
	// 	int bytesRead = read(connection, buffer, 1000000);
	// 	(void)bytesRead; // Silence unused variable warning
	// 	std::cout << "The message was: \n" << buffer;

	// 	// Send a message to the connection
	// 	std::string response = "Good talking to you\n";


		
	// 	send(connection, response.c_str(), response.size(), 0);
	// 	close(connection);
	// 	std::cout << "connections: " << i << std::endl;
	// 	i++;
	// }
		// Close the connections
	//close(sockfd);
		
	
}