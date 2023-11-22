/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helpers.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhassoun <fhassoun@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/14 11:21:05 by fhassoun          #+#    #+#             */
/*   Updated: 2023/11/22 14:37:05 by fhassoun         ###   ########.fr       */
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

char *string_to_chararray(std::string path)
{
	char *tmp;
	path = "." + path;
	tmp = new char[path.length() + 1];
	strcpy(tmp, path.c_str());
	return (tmp);
}

int		ft_strcmp(const char *s1, const char *s2)
{
	size_t	i;
	char	*buf1;
	char	*buf2;

	i = 0;
	buf1 = (char *)s1;
	buf2 = (char *)s2;
	while (buf1[i] && buf2[i] && buf1[i] == buf2[i])
		i++;
	return ((unsigned char)buf1[i] - (unsigned char)buf2[i]);
}

bool endsWithCRLF(const char* buffer, int length)
{
    if (length < 4) {
        return false;
    }
    return buffer[length - 1] == '\n' && buffer[length - 2] == '\r' && buffer[length - 3] == '\n' && buffer[length - 4] == '\r';
}