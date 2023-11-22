/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helpers.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhassoun <fhassoun@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/14 11:20:18 by fhassoun          #+#    #+#             */
/*   Updated: 2023/11/22 14:36:59 by fhassoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Webserv.hpp"
#include <cstddef>

void	*ft_memset(void *s, int c, size_t n);
void	*ft_memcpy(void *dst, const void *src, size_t n);
int		ft_strcmp(const char *s1, const char *s2);
std::string int_to_string(int value);
char *string_to_chararray(std::string path);
bool endsWithCRLF(const char* buffer, int length);