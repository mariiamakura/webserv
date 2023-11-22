/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhassoun <fhassoun@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/20 08:09:23 by fhassoun          #+#    #+#             */
/*   Updated: 2023/11/22 07:22:06 by fhassoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(): ASocket()
{
}

Client::~Client()
{
}

Client::Client(Client const &src) : ASocket()
{
	*this = src;
}

Client &Client::operator=(Client const &src)
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

//setters

