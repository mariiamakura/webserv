/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhassoun <fhassoun@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/13 11:37:48 by fhassoun          #+#    #+#             */
/*   Updated: 2023/11/13 14:50:20 by fhassoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once 
#include "Server.hpp"

class Config
{
	private:
		u_int16_t _listen;


	public:
		Config();
		~Config();
		Config(Config const &src);
		Config &operator=(Config const &src);
		

		void setListen(std::string str);
		u_int16_t getListen();
};
