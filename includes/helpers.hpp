/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helpers.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhassoun <fhassoun@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/14 11:20:18 by fhassoun          #+#    #+#             */
/*   Updated: 2023/11/20 09:00:42 by fhassoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Webserv.hpp"
#include <cstddef>

void	*ft_memset(void *s, int c, size_t n);
void	*ft_memcpy(void *dst, const void *src, size_t n);
std::string int_to_string(int value);