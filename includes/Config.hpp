/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sung-hle <sung-hle@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/13 11:37:48 by fhassoun          #+#    #+#             */
/*   Updated: 2024/01/18 13:52:52 by sung-hle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once 
#include "Webserv.hpp"
#include "Location.hpp"

class Location;

class Config
{
	private:
		std::string													listen;
		std::vector<std::string>            serverNames;//e.g. www.example.com example.com  <-only valid vars for us !!!! ///or ip-address
		std::vector<std::string>            host;// does not exist in nginx but we need it for the host header or server_name or server_name_in_redirect
		std::string                         root;//e.g. /var/www/example.com/htdocs
		std::map<int, std::string>          errorPage;//otherwise we use default error pages
		std::map<std::string, Location*>    location;
		std::set<std::string>               allowedMethods;
		std::vector<std::string>            index;//When a client requests a directory without specifying a particular file, Nginx will look for files listed in the index directive and serve the first one that exists in the directory.
		bool                                autoindex;
		int							                    port;
		unsigned long                       client_body_buffer_size;//--------------- redirs, uploads
		

		void formatString(std::string& str);
		void formatKeyTmp(std::string& str, std::string& str2);
		void formatValueTmp(std::ifstream& configFile, std::string& line, std::string& tmp2);// {

	public:
		Config();
		~Config();
		Config(Config const &src);
		Config &operator=(Config const &src);
		
		int parse(std::ifstream& configFile);
		void setListen(std::string str);
		const std::string& getListen() const;
		void setServerNames(std::string str);
		const std::vector<std::string>& getServerNames() const;
		void setHost(std::string str);
		const std::vector<std::string>& getHost() const;
		void setPort(std::string str);
		int getPort() const;
		void setRoot(std::string str);
		const std::string& getRoot() const;
		void setErrorPage(std::string str, std::string str2);
		const std::map<int, std::string>& getErrorPage() const;
		void setClientBodyBufferSize(std::string str);
		unsigned long getClientBodyBufferSize() const;
		// void setCgiParam(std::string str);
		// const std::map<std::string, std::string>& getCgiParam() const;
		// void setCgiPass(std::string str);
		// const std::string& getCgiPass() const;
		// void setLocationPath(std::string str, std::ifstream& configFile);
		// const std::vector<std::string>& getLocationPath() const;
		int setLocation(std::string str, std::ifstream& configFile);
		const std::map<std::string, Location*>& getLocation() const;
		void setAllowedMethods(std::string str);
		const std::set<std::string>& getAllowedMethods() const;
		void setIndex(std::string str);
		const std::vector<std::string>& getIndex() const;
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
