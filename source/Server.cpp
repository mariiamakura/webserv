/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhassoun <fhassoun@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/13 11:43:52 by fhassoun          #+#    #+#             */
/*   Updated: 2023/11/13 15:36:00 by fhassoun         ###   ########.fr       */
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
			if (str.find("listen") != std::string::npos)
			{
				// std::cout << "listen" << std::endl;
				tmpConf.setListen(str);
				std::cout << "parsed port: " << tmpConf.getListen() << std::endl;
			}
			
			

			
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


// std::vector<int>::iterator iter = tmpVec.begin();

void Server::run()
{
	std::cout << "elements in vector: " << this->_config.size() << std::endl;
	std::vector<Config>::iterator iter = this->_config.begin();
	while ( iter != this->_config.end())
	{
		std::cout << "start iter: " << iter->getListen() << std::endl;
		std::cout << "start this: " << this->_config.begin()->getListen() << std::endl;
		// Create a socket (IPv4, TCP)
		int sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd == -1)
		{
			std::cout << "Failed to create socket. errno: " << errno << std::endl;
			exit(EXIT_FAILURE);
		}

		// Listen to port 9999 on any address
		sockaddr_in sockaddr;
		sockaddr.sin_family = AF_INET;
		sockaddr.sin_addr.s_addr = INADDR_ANY;
		sockaddr.sin_port = htons(iter->getListen());
		// sockaddr.sin_port = htons(9999); // htons is necessary to convert a number to
										// network byte order
		if (bind(sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0)
		{
			std::cout << "Failed to bind to port 9999. errno: " << errno << std::endl;
			exit(EXIT_FAILURE);
		}

		// Start listening. Hold at most 10 connections in the queue
		if (listen(sockfd, 10) < 0)
		{
			std::cout << "Failed to listen on socket. errno: " << errno << std::endl;
			exit(EXIT_FAILURE);
		}

		// Grab a connection from the queue
		int addrlen = sizeof(sockaddr);
		int connection = accept(sockfd, (struct sockaddr *)&sockaddr, (socklen_t *)&addrlen);
		if (connection < 0)
		{
			std::cout << "Failed to grab connection. errno: " << errno << std::endl;
			exit(EXIT_FAILURE);
		}

		// Read from the connection
		char buffer[100000];
		int bytesRead = read(connection, buffer, 100000);
		(void)bytesRead; // Silence unused variable warning
		std::cout << "The message was: \n" << buffer;

		// Send a message to the connection
		std::string response = "Good talking to you\n";
		// std::string response = "<!doctype html><html><head><title>Our Funky HTML Page</title><meta name=\"description\" content=\"Our first page\"><meta name=\"keywords\" content=\"html tutorial template\"></head><body>Content goes here.</body></html>" ;
		send(connection, response.c_str(), response.size(), 0);

		// Close the connections
		close(connection);
		close(sockfd);
		iter++;
		std::cout << "start2: " << iter->getListen() << std::endl;
	}
}