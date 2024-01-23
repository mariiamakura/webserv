/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sung-hle <sung-hle@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/13 11:44:28 by fhassoun          #+#    #+#             */
/*   Updated: 2024/01/23 14:04:21 by sung-hle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

Config::Config()	: listen(""), root(""), autoindex(0), port(0), client_body_buffer_size(0) {
}

Config::~Config()	{
	for (std::map<std::string, Location*>::iterator it = location.begin(); it != location.end(); ++it) {
		delete it->second;
	}
}

Config::Config(Config const &src)	: listen(src.listen), root(src.root), autoindex(src.autoindex), port(src.port), client_body_buffer_size(src.client_body_buffer_size) {
	*this = src;
}

Config &Config::operator=(Config const &src)
{
	if (this == &src)	{
		listen = src.listen;
		serverNames = src.serverNames;
		host = src.host;
		root = src.root;
		errorPage = src.errorPage;
		location = src.location;
		allowedMethods = src.allowedMethods;
		index = src.index;
		autoindex = src.autoindex;
		port = src.port;
		client_body_buffer_size = src.client_body_buffer_size;

		// Delete existing Location objects
		for (std::map<std::string, Location*>::iterator it = location.begin(); it != location.end(); ++it) {
			delete it->second;
		}

		// Clear the map
		location.clear();

		// Copy Location objects
		for (std::map<std::string, Location*>::const_iterator it = src.location.begin(); it != src.location.end(); ++it) {
			Location *newLocation = new Location(*(it->second));
			location[it->first] = newLocation;
		}
	}
	return (*this);
}


int Config::parse(std::ifstream& configFile) {
	std::string line, tmp, tmp2, keyword;
	bool serverBlock = true;
	while (std::getline(configFile, line)) {
		std::istringstream iss(line);
		iss >> std::ws >> keyword;

		if (keyword == "server") {
			formatValueTmp(configFile, line, tmp2);
			serverBlock = false;
			// std::cout << "line: " << line << std::endl;
			while (std::getline(configFile, line)) {
				// std::istringstream iss(line);
				// iss >> std::ws >> line;
				// std::cout << "line: " << line << std::endl;
				if (line.substr(0, 6) == "server" && line != "server_name") {
					return 2;
				}
				else if (line.find("}") != std::string::npos) {
					serverBlock = true;
					return 0;
//---------------------------                    
				} else if (line.find("listen") != std::string::npos) {
					formatKeyTmp(line, tmp);
					setListen(tmp);
					if (getPort() < 0 || getPort() > 65535) {
						std::cout << "Invalid port number" << std::endl;
						return 2;
					}
				}
				else if (line.find("server_name") != std::string::npos) {
					iss.clear();
					iss.str(line);
					iss >> keyword;
					while (iss >> tmp) {
						formatString(tmp);
						setServerNames(tmp);
					}
				} else if (line.find("host") != std::string::npos) {
					iss.clear();
					iss.str(line);
					iss >> keyword;
					while (iss >> tmp) {
						formatString(tmp);
						setHost(tmp);
					}
				} else if (line.find("root") != std::string::npos) {
					formatKeyTmp(line, tmp);
					setRoot(tmp);
				} else if (line.find("error_page") != std::string::npos) {
					iss.clear();
					iss.str(line);
					iss >> keyword >> tmp >> tmp2;
					formatString(tmp2);
					setErrorPage(tmp, tmp2);
				} else if (line.find("client_max_body_size") != std::string::npos) {
					formatKeyTmp(line, tmp);
					setClientBodyBufferSize(tmp);
				} else if (line.find("location") != std::string::npos) {
					// std::cout << "hier\n";
					std::istringstream iss(line);
					iss >> keyword;
					tmp2 = "";
					if (keyword == "location") {
						formatValueTmp(configFile, line, tmp2);
					}
					// std::cout << "tmp2: ." << tmp2 << "." << std::endl;
					if (tmp2 == "" || setLocation(tmp2, configFile) == 2) {
						return 2;
					}
				} else if (line.find("autoindex") != std::string::npos) {
					formatKeyTmp(line, tmp);
					setAutoindex(tmp.compare("on") == 0 ? true : false);
				} else if (line.find("index") != std::string::npos) {
					iss.clear();
					iss.str(line);
					iss >> keyword;
					while (iss >> tmp) {
						formatString(tmp);
						setIndex(tmp);
					}
				} else if (line == "") {
					continue;
				// } else {
				// 	std::cout << "Error in server block" << std::endl;
				// 	return 2;
				}
			}
		} else if (keyword == "")
			continue;
	}
	if (configFile.eof() && serverBlock == false) {
		return 2;
	} else {
		return 1;
	}
}

// 1 segfault when missing bracket
// 2 empty lines gives error, where it should not

void Config::setListen(std::string str) {
	listen = str;
	std::istringstream iss(str);
	std::string hostPort;
	iss >> hostPort;

	std::size_t colonPos = hostPort.find(':');
	if (colonPos == std::string::npos) {
		port = static_cast<int>(std::strtol(hostPort.c_str(), NULL, 10));
	} else {
		host.push_back(hostPort.substr(0, colonPos));
		port = static_cast<int>(std::strtol(hostPort.substr(colonPos + 1).c_str(), NULL, 10));
	}
}

const std::string& Config::getListen()  const {
	return listen;
}

void Config::setServerNames(std::string str) {
	serverNames.push_back(str);
}

const std::vector<std::string>& Config::getServerNames() const
{
	return serverNames;
}

void Config::setHost(std::string str)
{
	this->host.push_back(str);
}

const std::vector<std::string>& Config::getHost() const
{
	return (this->host);
}

void Config::setPort(std::string str) {
	int _port = 0;
	_port = static_cast<int>(std::strtol(str.c_str(), NULL, 10));
	port = _port;
}

int Config::getPort() const {
	return (this->port);
}

void Config::setRoot(std::string str)
{
	this->root = str;
}

const std::string& Config::getRoot() const
{
	return (this->root);
}

void Config::setErrorPage(std::string str, std::string str2)
{
	int code = 0;
			code = static_cast<int>(std::strtol(str.c_str(), NULL, 10));
			this->errorPage.insert(std::pair<int, std::string>(code, str2));
}

const std::map<int, std::string>& Config::getErrorPage() const
{
	return (this->errorPage);
}

void Config::setClientBodyBufferSize(std::string str) {
	unsigned long _client_body_buffer_size = 0;
	_client_body_buffer_size = static_cast<unsigned long>(std::strtol(str.c_str(), NULL, 10));
	client_body_buffer_size = _client_body_buffer_size;
}

unsigned long Config::getClientBodyBufferSize() const {
	return (this->client_body_buffer_size);
}

// const std::vector<std::string>& getLocationPath() const;

int Config::setLocation(std::string str, std::ifstream& configFile) {
	std::string line;
	std::string tmp;
	size_t pos;
	Location *loc = new Location();
	loc->setPath(str);

	while (std::getline(configFile, line)) {
		std::istringstream iss(line);
		std::string keyword;
		iss >> keyword;
		if (keyword == "location") {
			delete loc;
			return 2;
		}
		// std::cout << str << std::endl;

		if (line.find("}") != std::string::npos) {
			location.insert(std::make_pair(str, loc));
			return 0;
		} else if ((pos = line.find("allow_methods")) != std::string::npos) {
			std::istringstream iss(line.substr(pos + strlen("allow_methods") + 1));
			std::set<std::string> methods;
			std::string method;
			while (iss >> method) {
				formatString(method);
				if (method != "GET" && method != "POST" && method != "DELETE") {
					std::cout << "Unknown method" << std::endl;
					delete loc;
					return 2;
				}
				methods.insert(method);
			}
			// std::cout << std::endl;
			loc->setAllowedMethods(methods);
		} else if (line.find("root") != std::string::npos) {
			formatKeyTmp(line, tmp);
			loc->setRoot(tmp);
		} else if (line.find("autoindex") != std::string::npos) {
			formatKeyTmp(line, tmp);
			loc->setAutoindex(tmp.compare("on") == 0 ? true : false);
		} else if (line.find("index") != std::string::npos) {
			iss.clear();
			iss.str(line);
			iss >> keyword;
			while (iss >> tmp) {
				formatString(tmp);
				loc->setIndex(tmp);
			}
		} else if ((pos = line.find("cgi_path")) != std::string::npos) {
			std::istringstream iss(line.substr(pos + strlen("cgi_path") + 1));
			std::vector<std::string> cgiPathMap;
			while (iss >> tmp) {
				formatString(tmp);
				cgiPathMap.push_back(tmp);
			}
			loc->setCGIPath(cgiPathMap);
		} else if ((pos = line.find("cgi_ext")) != std::string::npos) {
			std::istringstream iss(line.substr(pos + strlen("cgi_ext") + 1));
			std::vector<std::string> cgiExtVector;
			while (iss >> tmp) {
				formatString(tmp);
				cgiExtVector.push_back(tmp);
			}
			loc->setCGIExt(cgiExtVector);
		} else if (line.find("client_max_body_size") != std::string::npos) {
				formatKeyTmp(line, tmp);
				loc->setClientBodyBufferSize(tmp);
		// } else {
		// 	std::cout << "Error in location block" << std::endl;
		// 	delete loc;
		// 	return 1;
		} else if (line == "") {
			continue;
		}
	}
	delete loc;
	return 2;
}

const std::map<std::string, Location*>& Config::getLocation() const
{
	return (this->location);
}

void Config::setAllowedMethods(std::string str)
{
	std::string tmp;
	std::string tmp2;

	std::stringstream ss(str);
	while (std::getline(ss, tmp, ' '))
	{
		if (tmp.compare("allow_methods") == 0)
		{
			std::getline(ss, tmp, ' ');
			tmp.erase(tmp.size() - 1);
			std::getline(ss, tmp2, ' ');
			tmp2.erase(tmp2.size() - 1);
			this->allowedMethods.insert(tmp2);
		}
	}
}

const std::set<std::string>& Config::getAllowedMethods() const
{
	return (this->allowedMethods);
}

void Config::setIndex(std::string str)
{
	index = str;
}

const std::string& Config::getIndex() const
{
	return index;
}

void Config::setAutoindex(bool b)
{
	autoindex = b;
}

bool Config::getAutoindex() const
{
	return (autoindex);
}

void displaySet(const std::set<std::string>& strSet) {
    for (std::set<std::string>::const_iterator it = strSet.begin(); it != strSet.end(); ++it) {
        std::cout << "." << *it << ".";
    }
}

// Helper function to display a vector of strings
void displayVector(const std::vector<std::string>& strVector) {
    for (std::vector<std::string>::const_iterator it = strVector.begin(); it != strVector.end(); ++it) {
        std::cout << "." << *it << ".";
    }
}

void Config::printConfigs(std::vector<Config *>& serverConfigs) {
	for (std::vector<Config *>::iterator itz = serverConfigs.begin(); itz != serverConfigs.end(); itz++) {
    std::cout << "Server Configuration:" << std::endl;
    std::cout << "Listen: ." << (*itz)->getListen() << "." << std::endl;
    // std::cout << "Server Name: ." << (*itz)->getServerNames() << "." << std::endl;
    std::cout << "Server Name: .";
		const std::vector<std::string>& serverNames = (*itz)->getServerNames();
		displayVector(serverNames);
		std::cout << "." << std::endl;
		
    const std::vector<std::string>& hosts = (*itz)->getHost();
    std::cout << "Hosts: ";
    displayVector(hosts);
    std::cout << std::endl;
		std::cout << "Port: ." << (*itz)->getPort() << "." << std::endl;

    std::cout << "Root: ." << (*itz)->getRoot() << "." << std::endl;
		std::cout << "Index: ." << (*itz)->getIndex() << "." << std::endl;	
		std::cout << "." << std::endl;
		std::cout << "Autoindex: ." << ((*itz)->getAutoindex() ? "on" : "off") << std::endl;
    const std::map<int, std::string>& errorPages = (*itz)->getErrorPage();
    std::cout << "Error Pages:" << std::endl;
    for (std::map<int, std::string>::const_iterator it = errorPages.begin(); it != errorPages.end(); ++it) {
        std::cout << "\t." << it->first << ". => ." << it->second << "." << std::endl;
    }
		std::cout << "Client Body Buffer Size: ." << (*itz)->getClientBodyBufferSize() << "." << std::endl;

//Locations
		const std::map<std::string, Location*>& locations = (*itz)->getLocation();
		std::cout << "Locations:" << std::endl;

		for (std::map<std::string, Location*>::const_iterator it = locations.begin(); it != locations.end(); ++it) {
			std::cout << "Path: ." << it->first << "." << std::endl;
			std::cout << "\tRoot: ." << it->second->getRoot() << "." << std::endl;
			std::cout << "\tAutoindex: ." << (it->second->getAutoindex() ? "on" : "off") << std::endl;
			std::cout << "\tAllowed Methods: .";
			displaySet(it->second->getAllowedMethods());
			std::cout << std::endl;
			std::cout << "\tIndex: ." << it->second->getIndex() << std::endl;
			// const std::vector<std::string>& index = it->second->getIndex();
			// for (std::vector<std::string>::const_iterator indexIt = index.begin(); indexIt != index.end(); ++indexIt) {
			// 	std::cout << "\t\t." << *indexIt << "." << std::endl;
			// }
			std::cout << "\tcgi_path: ." << std::endl;
			const std::vector<std::string>& cgiPath = it->second->getCGIPath();
			for (std::vector<std::string>::const_iterator pathIt = cgiPath.begin(); pathIt != cgiPath.end(); ++pathIt) {
				std::cout << "\t\t." << *pathIt << "." << std::endl;
			}
			// Display cgi_ext
			std::cout << "\tcgi_ext: " << std::endl;
			const std::vector<std::string>& cgiExt = it->second->getCGIExt();
			for (std::vector<std::string>::const_iterator extIt = cgiExt.begin(); extIt != cgiExt.end(); ++extIt) {
				std::cout << "\t\t." << *extIt << "." << std::endl;
			}
			std::cout << "\tClient Body Buffer Size: ." << it->second->getClientBodyBufferSize() << "." << std::endl;
		}
	}
}

void Config::formatString(std::string& str) {
	if (!str.empty()) {
		if (str[str.size() - 1] == ';')
			str.erase(str.size() - 1);
	}
}

void Config::formatKeyTmp(std::string& str, std::string& str2) {
	std::istringstream iss;
	std::string keyword;
	iss.clear();
	iss.str(str);
	iss >> keyword >> str2;
	formatString(str2);
}

void Config::formatValueTmp(std::ifstream& configFile, std::string& line, std::string& tmp2) {
			std::string tmp;
			size_t openingBracePos = line.find("{");
			if (openingBracePos != std::string::npos) {
				tmp = line.substr(line.find("location") + 8, openingBracePos - line.find("location") - 8);
				std::istringstream issTmp(tmp);
				issTmp >> std::ws;
				std::getline(issTmp, tmp2, ' ');
				line.erase(0, openingBracePos + 1);
			} else {
				while (std::getline(configFile, line)) {
					size_t openingBracePos = line.find("{");
					if (openingBracePos != std::string::npos) {
						tmp = line.substr(line.find("location") + 8, openingBracePos - line.find("location") - 8);
						std::istringstream issTmp(tmp);
						issTmp >> std::ws;
						std::getline(issTmp, tmp2, ' ');
						line.erase(0, openingBracePos + 1);
						break;
					}
				}
			}
		}