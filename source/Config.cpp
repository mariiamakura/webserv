/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sung-hle <sung-hle@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/13 11:44:28 by fhassoun          #+#    #+#             */
/*   Updated: 2023/12/30 16:50:54 by sung-hle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

Config::Config()
{
}

Config::~Config()
{
	for (std::map<std::string, Location*>::iterator it = location.begin(); it != location.end(); ++it) {
        delete it->second;
    }
}

Config::Config(Config const &src)
{
	*this = src;
}

Config &Config::operator=(Config const &src)
{
	if (this == &src)
		return (*this);
	return (*this);
}


void Config::parse(std::ifstream& configFile) {
	std::string line;
	std::string tmp;
	std::string tmp2;
	while (std::getline(configFile, line)) {
		std::istringstream iss(line);
		std::string keyword;
		iss >> keyword;

		if (keyword == "server") {
			// Start parsing the server block
			size_t openingBracePos = line.find("{");
			if (openingBracePos != std::string::npos) {
				// Opening curly bracket found on the same line
				line.erase(0, openingBracePos + 1);  // Erase everything before {
			} else {
				// Look for the opening curly bracket in the following lines
				while (std::getline(configFile, line)) {
						size_t openingBracePos = line.find("{");
						if (openingBracePos != std::string::npos) {
								// Opening curly bracket found on this line
								line.erase(0, openingBracePos + 1);  // Erase everything before {
								break;
						}
				}
			}
				while (std::getline(configFile, line)) {
					if (line.find("}") != std::string::npos) {
//---------------------------                    
						return ;
					} else if (line.find("listen") != std::string::npos) {
						keyword = "listen";
						iss.clear();
						iss.str(line);
						iss >> keyword >> tmp;
						setListen(tmp);
					}
					// Parse other server-related configuration here
					else if (line.find("server_name") != std::string::npos) {
						keyword = "server_name";
						iss.clear();
						iss.str(line);
						iss >> keyword >> tmp;
						if (!tmp.empty()) {
							if (tmp[tmp.size() - 1] == ';')
								tmp.erase(tmp.size() - 1);
						}
						setServerName(tmp);
					} else if (line.find("host") != std::string::npos) {
						keyword = "host";
						iss.clear();
						iss.str(line);
						iss >> keyword >> tmp;
						// std::cout << "TMP HOST: " << tmp << std::endl;
						if (!tmp.empty()) {
							if (tmp[tmp.size() - 1] == ';')
								tmp.erase(tmp.size() - 1);
						}
						host.push_back(tmp);
					} else if (line.find("root") != std::string::npos) {
						keyword = "root";
						iss.clear();
						iss.str(line);
						iss >> keyword >> tmp;
						if (!tmp.empty()) {
							if (tmp[tmp.size() - 1] == ';')
								tmp.erase(tmp.size() - 1);
						}
						setRoot(tmp);
					} else if (line.find("error_page") != std::string::npos) {
						keyword = "error_page";
						iss.clear();
						iss.str(line);
						std::string tmp2;
						iss >> keyword >> tmp >> tmp2;
						if (!tmp2.empty()) {
							if (tmp2[tmp2.size() - 1] == ';')
								tmp2.erase(tmp2.size() - 1);
						}
						setErrorPage(tmp, tmp2);
					} else if (line.find("location") != std::string::npos) {
						std::istringstream iss(line);
						iss >> keyword;
						if (keyword == "location") {
							size_t openingBracePos = line.find("{");
							if (openingBracePos != std::string::npos) {
								tmp = line.substr(line.find("location") + 8, openingBracePos - line.find("location") - 8);
								std::istringstream issTmp(tmp);
								issTmp >> std::ws;
								std::getline(issTmp, tmp2, ' ');
								line.erase(0, openingBracePos + 1);  // Erase everything before {
							} else {
								while (std::getline(configFile, line)) {
									size_t openingBracePos = line.find("{");
									if (openingBracePos != std::string::npos) {
										tmp = line.substr(line.find("location") + 8, openingBracePos - line.find("location") - 8);
										std::istringstream issTmp(tmp);
										issTmp >> std::ws;
										std::getline(issTmp, tmp2, ' ');
										line.erase(0, openingBracePos + 1);  // Erase everything before {
										break;
									}
								}
							}
						}
						setLocation(tmp2, configFile);
					} else if (line.find("autoindex") != std::string::npos) {
						keyword = "autoindex";
						iss.clear();
						iss.str(line);
						iss >> keyword >> tmp;
						if (!tmp.empty()) {
							if (tmp[tmp.size() - 1] == ';')
								tmp.erase(tmp.size() - 1);
						}
						if (tmp.compare("on") == 0)
							this->autoindex = true;
						else
							this->autoindex = false;
					} else if (line.find("index") != std::string::npos) {
						keyword = "index";
						iss.clear();
						iss.str(line);
						iss >> keyword >> tmp;
						if (!tmp.empty()) {
							if (tmp[tmp.size() - 1] == ';')
								tmp.erase(tmp.size() - 1);
						}
						setIndex(tmp);
					}
				}
				// End of server block
		}
	}
}


void Config::setListen(std::string str)
{
	int port = 0;
			port = static_cast<int>(std::strtol(str.c_str(), NULL, 10));
			this->listen = port;
	
}

u_int16_t Config::getListen()  const
{
	return (this->listen);
}

void Config::setServerName(std::string str)
{
	this->serverName = str;
}

const std::string& Config::getServerName() const
{
	return (this->serverName);
}

void Config::setHost(std::string str)
{
	this->host.push_back(str);
}

const std::vector<std::string>& Config::getHost() const
{
	return (this->host);
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

void Config::setLocation(std::string str, std::ifstream& configFile) {
	std::string line;
	std::string tmp;
	size_t pos;
	Location *loc = new Location();

	while (std::getline(configFile, line)) {
		std::istringstream iss(line);
		std::string keyword;
		iss >> keyword;

		loc->setPath(str);

		if (line.find("}") != std::string::npos) {
			// Closing curly bracket found, end parsing server block
			// std::cout << "END OF LOCATION" << std::endl;
			location.insert(std::make_pair(str, loc));
			break;
		} else if ((pos = line.find("allow_methods")) != std::string::npos) {
			std::istringstream iss(line.substr(pos + strlen("allow_methods") + 1));
			std::set<std::string> methods;
			std::string method;
			while (iss >> method) {
				if (!method.empty()) {
					if (method[method.size() - 1] == ';')
						method.erase(method.size() - 1);
				}
						methods.insert(method);
			}
			// for (std::set<std::string>::const_iterator it = methods.begin(); it != methods.end(); ++it) {
			// 		std::cout << *it << " ";
			// }
			std::cout << std::endl;
			loc->setAllowedMethods(methods);
		} else if (line.find("root") != std::string::npos) {
			keyword = "root";
			iss.clear();
			iss.str(line);
			iss >> keyword >> tmp;
			if (!tmp.empty()) {
				if (tmp[tmp.size() - 1] == ';')
					tmp.erase(tmp.size() - 1);
			}
			loc->setRoot(tmp);
		} else if (line.find("autoindex") != std::string::npos) {
			keyword = "autoindex";
			iss.clear();
			iss.str(line);
			iss >> keyword >> tmp;
			if (!tmp.empty()) {
				if (tmp[tmp.size() - 1] == ';')
					tmp.erase(tmp.size() - 1);
			} 	
			if (tmp.compare("on") == 0)
				loc->setAutoindex(true);
			else
				loc->setAutoindex(false);
		} else if (line.find("index") != std::string::npos) {
			keyword = "index";
			iss.clear();
			iss.str(line);
			iss >> keyword >> tmp;
			if (!tmp.empty()) {
				if (tmp[tmp.size() - 1] == ';')
					tmp.erase(tmp.size() - 1);
			} 	
			loc->setIndex(tmp);
		} else if ((pos = line.find("cgi_path")) != std::string::npos) {
			std::istringstream iss(line.substr(pos + strlen("cgi_path") + 1));
			std::vector<std::string> cgiPathMap;
			while (iss >> tmp) {
				if (!tmp.empty() && tmp[tmp.size() - 1] == ';') {
					tmp.erase(tmp.size() - 1);
				}
				cgiPathMap.push_back(tmp);
			}
			loc->setCGIPath(cgiPathMap);
		} else if ((pos = line.find("cgi_ext")) != std::string::npos) {
			std::istringstream iss(line.substr(pos + strlen("cgi_ext") + 1));
			std::vector<std::string> cgiExtVector;
			while (iss >> tmp) {
				if (!tmp.empty() && tmp[tmp.size() - 1] == ';') {
					tmp.erase(tmp.size() - 1);
				}
				cgiExtVector.push_back(tmp);
			}
			loc->setCGIExt(cgiExtVector);
		}
	}
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
	return (index);
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
	for (std::vector<Config *>::iterator itz = serverConfigs.begin(); itz != serverConfigs.end(); ++itz) {
    std::cout << "Server Configuration:" << std::endl;
    std::cout << "Listen Port: ." << (*itz)->getListen() << "." << std::endl;
    std::cout << "Server Name: ." << (*itz)->getServerName() << "." << std::endl;

    const std::vector<std::string>& hosts = (*itz)->getHost();
    std::cout << "Hosts: ";
    displayVector(hosts);
    std::cout << std::endl;

    std::cout << "Root: ." << (*itz)->getRoot() << "." << std::endl;
		std::cout << "Index: ." << (*itz)->getIndex() << "." << std::endl;
		std::cout << "Autoindex: ." << ((*itz)->getAutoindex() ? "on" : "off") << std::endl;
    const std::map<int, std::string>& errorPages = (*itz)->getErrorPage();
    std::cout << "Error Pages:" << std::endl;
    for (std::map<int, std::string>::const_iterator it = errorPages.begin(); it != errorPages.end(); ++it) {
        std::cout << "\t." << it->first << ". => ." << it->second << "." << std::endl;
    }

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
			std::cout << "\tIndex: ." << it->second->getIndex() << "." << std::endl;
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
		}
	}
}