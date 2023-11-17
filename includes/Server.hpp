/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhassoun <fhassoun@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/13 11:22:29 by fhassoun          #+#    #+#             */
/*   Updated: 2023/11/17 15:15:38 by fhassoun         ###   ########.fr       */
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
#include "helpers.hpp"

#define DEF_CONF		"configs/default.conf"
#define DEF_PORT		9999
#define DEF_BACKLOG		100
#define TRUE			1
#define FALSE			0

class Config;

class Server
{
	private:
		std::map <int, std::string> in_request;
		std::map <int, std::string> out_response;
		
		


	public:
		std::vector<Config> _config;
		Server();
		~Server();
		Server(Server const &src);
		Server &operator=(Server const &src);
		

		void parseConfig(char *path);
		void init_server(int port, int backlog);
		void run();
		

		//setters
		// void setConfig(std::vector<Config> config);
		void setInRequest(std::map <int, std::string> in_request);
		void setOutResponse(std::map <int, std::string> out_response);

		//getters
		// std::vector<Config> getConfig();
		std::map <int, std::string> getInRequest() ;
		std::map <int, std::string> getOutResponse();
		
		

		
		
	
};
