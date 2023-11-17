/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhassoun <fhassoun@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/09 13:47:45 by fhassoun          #+#    #+#             */
/*   Updated: 2023/11/17 15:16:46 by fhassoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <cstdlib>		// For exit() and EXIT_FAILURE
#include <iostream>		// For cout
#include <unistd.h>		// For read
#include <cerrno>		// For errno

#include "Server.hpp"

int main(int argc, char **argv, char **env)
{
	Server server;

	(void)env;
	if (argc == 1)
	{
		server.parseConfig((char *)DEF_CONF);
		server.run();
	}
	else if (argc > 2)
	{
		std::cout << "Error : too many arguments." << std::endl;
		return (0);
	}
	else
	{
		server.parseConfig(argv[1]);
		
		// std::vector<Config>::iterator iter = server._config.begin();
		
		server.run();
	}
	
	
}