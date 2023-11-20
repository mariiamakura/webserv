/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhassoun <fhassoun@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/20 08:09:29 by fhassoun          #+#    #+#             */
/*   Updated: 2023/11/20 10:31:26 by fhassoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Webserv.hpp"

class Client
{
	private:
        int                 server_socket;
        struct sockaddr_in  server_address;
		// pollfd				poll_fd;




	public:
		Client();
		~Client();
		Client(Client const &src);
		Client &operator=(Client const &src);

		
		// iterators
		std::vector<Client>::iterator c_iter;
		
		//setters
		void setServerSocket(int server_socket);
		void setserverAddress(struct sockaddr_in server_address);

		//getters
		int getServerSocket();
		struct sockaddr_in getServerAddress();
};