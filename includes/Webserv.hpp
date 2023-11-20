/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhassoun <fhassoun@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/20 08:10:32 by fhassoun          #+#    #+#             */
/*   Updated: 2023/11/20 14:00:17 by fhassoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <cstdlib>		// For exit() and EXIT_FAILURE
#include <iostream>		// For cout
#include <unistd.h>		// For read
#include <cerrno>		// For errno
#include <vector>
#include <map>
#include <iomanip>
#include <cctype>
#include <cstring>
#include <string>
#include <fstream>
#include <sstream>
#include <typeinfo>
#include <cmath>
#include <string>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <fcntl.h>

#include "Config.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "helpers.hpp"

#define DEF_CONF		"configs/default.conf"
#define DEF_PORT		9999
#define DEF_BACKLOG		100
#define TRUE			1
#define FALSE			0


class Config;
class Client;
class Server;

class Webserv
{
	private:
		int sockfd;
		std::vector<Server> server;
		std::vector<Server>::iterator s_iter;
		
		std::vector<Client> client;
		std::vector<Server>::iterator cl_iter;

		std::vector<Config> config;
		std::vector<Server>::iterator co_iter;

		std::vector<pollfd> poll_fd;
		std::vector<pollfd>::iterator p_iter;
		
		std::map <int, std::string> in_request;
		std::map <int, std::string> out_response;

	
	public:
		Webserv();
		~Webserv();
		Webserv(Webserv const &src);
		Webserv &operator=(Webserv const &src);
		
		//setters
		void setSockfd(int sockfd);
		void setServer(std::vector<Server> server);
		void setClient(std::vector<Client> client);
		void setConfig(std::vector<Config> config);
		void setInRequest(std::map <int, std::string> in_request);
		void setOutResponse(std::map <int, std::string> out_response);


		//getters
		int getSockfd();
		std::vector<Server> getServer();
		std::vector<Client> getClient();
		std::vector<Config> getConfig();
		std::map <int, std::string> getInRequest() ;
		std::map <int, std::string> getOutResponse();
	
		
		//methods
		
		void parseConfig(char *path);
		void init_servers();
		void run();
	
};