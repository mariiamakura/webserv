/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhassoun <fhassoun@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/13 11:44:28 by fhassoun          #+#    #+#             */
/*   Updated: 2023/11/13 15:25:11 by fhassoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

Config::Config()
{
}

Config::~Config()
{
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

void Config::setListen(std::string str)
{
	int port = 0;
	std::string tmp;

	std::stringstream ss(str);
	while (std::getline(ss, tmp, ' '))
	{
		if (tmp.compare("listen") == 0)
		{
			std::getline(ss, tmp, ' ');
			std::cout << "before setListen: " << tmp << std::endl;
			tmp.erase(tmp.size() - 1);
			port = static_cast<int>(std::strtol(tmp.c_str(), NULL, 10));
			//std::stringstream(tmp) >> port;
			std::cout << "after setListen: " << port << std::endl;
		}
	}
	
	this->_listen = port;
	std::cout << "after setListen this->_listen: " << this->_listen << std::endl;
	
}

u_int16_t Config::getListen() 
{
	return (this->_listen);
}