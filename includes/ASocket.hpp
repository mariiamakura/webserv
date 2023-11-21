/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ASocket.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhassoun <fhassoun@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/21 13:32:04 by fhassoun          #+#    #+#             */
/*   Updated: 2023/11/21 13:39:26 by fhassoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Webserv.hpp"

class ASocket
{
	protected:
		int type;
		int fd;
		int port;
		int backlog;
		struct sockaddr_in address;
	
	public:
		ASocket();
		virtual ~ASocket();
		ASocket(ASocket const &src);
		ASocket &operator=(ASocket const &src);
		
		//setters
		virtual void setType(int type) = 0;
		virtual void setFd(int fd) = 0;
		virtual void setPort(int port) = 0;
		virtual void setBacklog(int backlog) = 0;
		virtual void setAddress(struct sockaddr_in address) = 0;
		
		//getters
		virtual int getType() = 0;
		virtual int getFd() = 0;
		virtual int getPort() = 0;
		virtual int getBacklog() = 0;
		virtual struct sockaddr_in getAddress() = 0;
};