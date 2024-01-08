/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhassoun <fhassoun@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/20 08:10:32 by fhassoun          #+#    #+#             */
/*   Updated: 2023/12/11 12:00:47 by fhassoun         ###   ########.fr       */
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
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>
#include <algorithm>
#include <cstdio>
#include <set>


#include "ASocket.hpp"
#include "Config.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "helpers.hpp"
#include "Request.hpp"
#include "Response.hpp"

#define DEF_CONF		"configs/default.conf"
#define DEF_PORT		9999
#define DEF_BACKLOG		100
#define FALSE			0
#define TRUE			1


// define log levels
#define DEBUG			2
#define INFO			3
#define WARNING			4
#define ERROR			5

// define colors
#define RESET			"\033[0m"
#define BLACK			"\033[30m"				/* Black */
#define RED				"\033[31m"				/* Red */
#define GREEN			"\033[32m"				/* Green */
#define YELLOW			"\033[33m"				/* Yellow */
#define BLUE			"\033[34m"				/* Blue */
#define MAGENTA			"\033[35m"				/* Magenta */
#define CYAN			"\033[0;38;5;44m"		/* Cyan */
#define WHITE			"\033[37m"				/* White */
#define LILA            "\033[0;38;5;199m"		/* Lila */
#define PURPLE			"\033[38;5;105m"		/* Purple */
#define ORANGE			"\033[38;5;208m"		/* Orange */



class Config;
class Client;
class Server;
class Request;
class Response;

//struct HttpResponse {
//    std::string http_version;
//    int status_code;
//    std::string status_message;
//    std::map<std::string, std::string> headers;
//    std::string body;
//};


class Webserv
{
	private:
		char 	**env;
		int		sockfd;
		int		new_sd ;
		//char	buffer[256]; small buffer doesnt work with my request parsing
        //should i redo it?
		char 	buffer[4096];
		int		rc;
		int 	end_server ;
		int		close_conn ;
        bool    autoindexBool;
		Request *http_request;
		Response *http_response;
		
		std::vector<Server> server;
		std::vector<Server>::iterator s_iter;
		
		std::vector<Client> client;
		std::vector<Server>::iterator cl_iter;

		std::vector<Config> config;
		std::vector<Server>::iterator co_iter;

		std::vector<pollfd> poll_fd;
		std::vector<pollfd>::iterator p_iter;
		
		std::map <int, std::vector<uint8_t> > in_request; //this should be something like <int, char[]>
        std::map <int, Request *> http_requests;
		std::map <int, Response *> out_response;

	
	public:
		std::string response;
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
		void setEnv(char **env);


		//getters
		int getSockfd();
		std::vector<Server> getServer();
		std::vector<Client> getClient();
		std::vector<Config> getConfig();
		std::map <int, std::string> getInRequest() ;
		std::map <int, std::string> getOutResponse();
		char **getEnv();
	
		
		//methods
		std::map<std::string, std::string> parse_form_data(const std::string& formData);
		void parseConfig(char *path);
		void init_servers();
		void run();
		void logging(std::string str, int status);
		int handle_pollin(int i);
		int check_sockfds(std::vector<int> sockfds, int i);
		std::string autoindex(const std::string& path) ;


        Response *create_http_response(void);
        //REQUEST HTTP METHODS
        Request *parse_http_request(const std::vector<uint8_t> &request);
        int getMethod();
        int postMethod(size_t i);
        void newOrAppendRequest(size_t i);
        void deleteRequest(int i);
        void deleteResponse(int i);


        int getCgiFilesList();
        int getDownloadCgi();



    };