/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sung-hle <sung-hle@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/13 11:37:48 by fhassoun          #+#    #+#             */
/*   Updated: 2023/12/24 16:06:29 by sung-hle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once 
#include "Webserv.hpp"
#include "Location.hpp"

class Location;

class Config
{
	private:
		u_int16_t 													listen;
		std::string                         serverName;
		std::vector<std::string>            host;
		std::string                         root;
		std::map<int, std::string>          errorPage;
		std::map<std::string, Location*>     location;
		std::set<std::string>               allowedMethods;
		// std::vector<std::string>            index;
		std::string 							           index;
		bool                                autoindex;
		// std::vector<int>                    ports;
		// unsigned long                       client_body_buffer_size;//--------------- redirs, uploads
		// std::map<std::string, std::string>  cgi_param;
		// std::string                         cgi_pass;
		// std::vector<std::string>            locationPath;
		// std::string                         alias;
		// bool                                aliasSet;

// und für locations
		// std::string						              _path;
		// std::set<std::string>               _allowed_methods;
		// std::string                         _alias;
		// bool                                _aliasSet;
		// std::string                         _root;
		// std::map<std::string, std::string>  _cgi_param;
		// std::string                         _cgi_pass;
		// unsigned long                       _client_body_buffer_size;
		// bool                                _autoindex;
		


	public:
		Config();
		~Config();
		Config(Config const &src);
		Config &operator=(Config const &src);
		
		void parse(std::ifstream& configFile);
		void setListen(std::string str);
		u_int16_t getListen() const;
		void setServerName(std::string str);
		const std::string& getServerName() const;
		void setHost(std::string str);
		const std::vector<std::string>& getHost() const;
		// void setPorts(std::string str);
		// const std::vector<int>& getPorts() const;
		void setRoot(std::string str);
		const std::string& getRoot() const;
		void setErrorPage(std::string str, std::string str2);
		const std::map<int, std::string>& getErrorPage() const;
		// void setClientBodyBufferSize(std::string str);
		// unsigned long getClientBodyBufferSize() const;
		// void setCgiParam(std::string str);
		// const std::map<std::string, std::string>& getCgiParam() const;
		// void setCgiPass(std::string str);
		// const std::string& getCgiPass() const;
		// void setLocationPath(std::string str, std::ifstream& configFile);
		// const std::vector<std::string>& getLocationPath() const;
		void setLocation(std::string str, std::ifstream& configFile);
		const std::map<std::string, Location*>& getLocation() const;
		void setAllowedMethods(std::string str);
		const std::set<std::string>& getAllowedMethods() const;
		void setIndex(std::string str);
		const std::string& getIndex() const;
		void setAutoindex(bool b);
		bool getAutoindex() const;
		// void setAlias(std::string str);
		// const std::string& getAlias() const;
		// void setAliasSet(bool b);
		// bool getAliasSet() const;
		static void printConfigs(std::vector<Config *>& serverConfigs);
		
};

void displaySet(const std::set<std::string>& strSet);
void displayVector(const std::vector<std::string>& strVector);
