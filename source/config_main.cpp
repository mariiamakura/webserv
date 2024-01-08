/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sung-hle <sung-hle@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/09 13:47:45 by fhassoun          #+#    #+#             */
/*   Updated: 2023/12/30 16:46:43 by sung-hle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <cstdlib>		// For exit() and EXIT_FAILURE
#include <iostream>		// For cout
#include <unistd.h>		// For read
#include <cerrno>		// For errno

#include "Webserv.hpp"



int main()	{ //minimum necessary port, ip, root location

	std::ifstream configFile("configs/default.conf");

    if (!configFile.is_open()) {
        std::cerr << "Error: Unable to open the server.conf file." << std::endl;
        return 1;
    }

    std::vector<Config *> serverConfigs;

    while (!configFile.eof()) {
        Config *serverConfig = new Config();
        serverConfig->parse(configFile);
        serverConfigs.push_back(serverConfig);
    }

    configFile.close();

    // for (size_t i = 0; i < serverConfigs.size(); ++i) {
    //     std::cout << "Server Configuration #" << i + 1 << ":" << std::endl;

    //     // Output server configuration details as needed
    //     std::cout << "Listen Port: " << serverConfigs[i].getListen() << std::endl;
    //     std::cout << "Server Name: " << serverConfigs[i].getServerName() << std::endl;

    //     // Output other details for the server configuration
    // }

    // Iterate through the vector of server configurations

    std::cout << "Number of server configurations: " << serverConfigs.size() << std::endl;
    Config::printConfigs(serverConfigs);

    

    if (serverConfigs[0]->getListen() == 0 ||
        serverConfigs[0]->getHost().empty() ||
        serverConfigs[0]->getLocation().find("/") == serverConfigs[0]->getLocation().end()) {
        std::cout << "Invalid configuration." << std::endl;
        std::cout << "Min requirements are Port, Host and Root-Location." << std::endl;
        for (std::vector<Config *>::iterator itz = serverConfigs.begin(); itz != serverConfigs.end(); ++itz) {
            delete *itz;
        }
        return 1;
    } 
    

    for (std::vector<Config *>::iterator itz = serverConfigs.begin(); itz != serverConfigs.end(); ++itz) {
        delete *itz;
    }
	
    
    return 0;
}

// int main(int argc, char **argv, char **env)
// {
// 	// Server server;
// 	Webserv webserv;

// 	webserv.setEnv(env);
// 	(void)argv;
// 	if (argc == 1)
// 	{
// 		webserv.logging("No config file specified. Using default config!", INFO);
// 		// std::cout << "No config file specified. Using default config!" << std::endl;
// 		//server.parseConfig((char *)DEF_CONF);
// 		webserv.init_servers();
// 		webserv.run();
// 	}
// 	else if (argc > 2)
// 	{
// 		std::cout << "Error : too many arguments." << std::endl;
// 		return (0);
// 	}
// 	else
// 	{
// 		// webserv.parseConfig(argv[1]);
		
// 		// std::vector<Config>::iterator iter = server._config.begin();
		
// 		webserv.init_servers();
// 	}
	
	
// }