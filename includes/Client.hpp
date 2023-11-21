/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhassoun <fhassoun@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/20 08:09:29 by fhassoun          #+#    #+#             */
/*   Updated: 2023/11/21 14:29:28 by fhassoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "ASocket.hpp"



class Client : public ASocket
{
	private:

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