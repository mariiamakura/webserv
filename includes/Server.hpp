/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhassoun <fhassoun@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/13 11:22:29 by fhassoun          #+#    #+#             */
/*   Updated: 2023/11/16 11:50:46 by fhassoun         ###   ########.fr       */
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



	public:
		std::vector<Config> _config;
		Server();
		~Server();
		Server(Server const &src);
		Server &operator=(Server const &src);
		

		void parseConfig(char *path);
		void init_server(int port, int backlog);
		void run();
		
		

		
		//for each config in the vector, create a socket
		//listen to the port
		//accept the connection
		//read the request
		//parse the request
		//build the response
		//send the response
		//close the connection
		//close the socket
		//repeat for the next config
	
};
