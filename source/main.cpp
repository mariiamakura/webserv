/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhassoun <fhassoun@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/09 13:47:45 by fhassoun          #+#    #+#             */
/*   Updated: 2023/11/20 14:12:17 by fhassoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <cstdlib>		// For exit() and EXIT_FAILURE
#include <iostream>		// For cout
#include <unistd.h>		// For read
#include <cerrno>		// For errno

#include "Webserv.hpp"

int main(int argc, char **argv, char **env)
{
	// Server server;
	Webserv webserv;

	(void)env;
	(void)argv;
	if (argc == 1)
	{
		std::cout << "No config file specified. Using default config!" << std::endl;
		//server.parseConfig((char *)DEF_CONF);
		webserv.init_servers();
		webserv.run();
	}
	else if (argc > 2)
	{
		std::cout << "Error : too many arguments." << std::endl;
		return (0);
	}
	else
	{
		// webserv.parseConfig(argv[1]);
		
		// std::vector<Config>::iterator iter = server._config.begin();
		
		webserv.init_servers();
	}
	
	
}