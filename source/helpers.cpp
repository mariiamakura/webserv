/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helpers.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhassoun <fhassoun@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/14 11:21:05 by fhassoun          #+#    #+#             */
/*   Updated: 2023/11/16 11:30:50 by fhassoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "helpers.hpp"

void	*ft_memset(void *s, int c, size_t n)
{
	size_t	i;
	char	*buf;

	i = 0;
	buf = (char *)s;
	while (i < n)
	{
		buf[i] = c;
		i++;
	}
	return (s);
}

void	*ft_memcpy(void *dst, const void *src, size_t n)
{
	size_t	i;
	char	*dstbuf;
	char	*srcbuf;

	i = 0;
	if (dst == src || n == 0)
		return (dst);
	if (dst == NULL && src == NULL)
		return (NULL);
	dstbuf = (char *)dst;
	srcbuf = (char *)src;
	while (i < n)
	{
		dstbuf[i] = srcbuf[i];
		i++;
	}
	return (dst);
}

std::string int_to_string(int value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}