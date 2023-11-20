/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhassoun <fhassoun@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/20 08:09:23 by fhassoun          #+#    #+#             */
/*   Updated: 2023/11/20 09:36:08 by fhassoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client()
{
	this->server_socket = 0;
	this->server_address.sin_family = AF_INET;
	this->server_address.sin_port = htons(8080);
	this->server_address.sin_addr.s_addr = htonl(INADDR_ANY);
}

Client::~Client()
{
}

Client::Client(Client const &src)
{
	*this = src;
}

Client &Client::operator=(Client const &src)
{
	if (this == &src)
		return (*this);
	this->server_socket = src.server_socket;
	this->server_address = src.server_address;
	return (*this);
}

//setters
void Client::setServerSocket(int server_socket)
{
	this->server_socket = server_socket;
}

void Client::setserverAddress(struct sockaddr_in server_address)
{
	this->server_address = server_address;
}

//getters
int Client::getServerSocket()
{
	return (this->server_socket);
}

struct sockaddr_in Client::getServerAddress()
{
	return (this->server_address);
}
