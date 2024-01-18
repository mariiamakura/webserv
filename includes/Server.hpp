/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sung-hle <sung-hle@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/13 11:22:29 by fhassoun          #+#    #+#             */
/*   Updated: 2024/01/17 17:33:38 by sung-hle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Webserv.hpp"


class Config;
class Client;
class Webserv;

class Server
{
	private:
		std::map <int, std::string> in_request;
		std::map <int, std::string> out_response;
		int sockfd;
		int port;
		int backlog;
		


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
		int getSockfd();
		
		
		//cookie
		void handleRequest(Request& request, Response& response);
		
		
	
};
