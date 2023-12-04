/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhassoun <fhassoun@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/13 11:37:48 by fhassoun          #+#    #+#             */
/*   Updated: 2023/12/04 14:39:58 by fhassoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once 
#include "Webserv.hpp"

class Config
{
	private:
		u_int16_t _listen;
		
		std::map<std::string, std::string> locations;

		


	public:
		Config();
		~Config();
		Config(Config const &src);
		Config &operator=(Config const &src);
		

		void setListen(std::string str);
		u_int16_t getListen();
};
