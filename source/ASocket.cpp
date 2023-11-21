/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ASocket.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhassoun <fhassoun@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/21 13:34:59 by fhassoun          #+#    #+#             */
/*   Updated: 2023/11/21 14:28:44 by fhassoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Webserv.hpp"

ASocket::ASocket()
{
	type = 0;
	fd = 0;
	port = 0;
	backlog = 0;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = 0;

}

ASocket::~ASocket()
{
}

ASocket::ASocket(ASocket const &src)
{
	*this = src;
}

ASocket &ASocket::operator=(ASocket const &src)
{
	if (this == &src)
		return (*this);
	this->type = src.type;
	this->fd = src.fd;
	this->port = src.port;
	this->backlog = src.backlog;
	this->address = src.address;
	return (*this);
}






