/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhassoun <fhassoun@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/07 17:42:41 by sung-hle          #+#    #+#             */
/*   Updated: 2024/01/22 12:53:58 by fhassoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

Location::Location() : path(""), allowMethods(), root(""), cgi_path(), cgi_ext(), client_body_buffer_size(0), autoindex(false)
{
}

Location::~Location()
{
}

Location::Location(Location const &src) : path(src.path), allowMethods(src.allowMethods), root(src.root), cgi_path(src.cgi_path), cgi_ext(src.cgi_ext), client_body_buffer_size(src.client_body_buffer_size), autoindex(src.autoindex)
{
	*this = src;
}

Location &Location::operator=(Location const &other)
{
	if (this != &other)
	{
		path = other.path;
		allowMethods = other.allowMethods;
		root = other.root;
		cgi_path = other.cgi_path;
		cgi_ext = other.cgi_ext;
		client_body_buffer_size = other.client_body_buffer_size;
		autoindex = other.autoindex;
	}
	return (*this);
}

// Getter and Setter for 'path'
const std::string &Location::getPath() const
{
	return path;
}

void Location::setPath(const std::string &newPath)
{
	path = newPath;
}

// Getter and Setter for 'allowed_methods'
const std::set<std::string> &Location::getAllowedMethods() const
{
	return allowMethods;
}

void Location::setAllowedMethods(const std::set<std::string> &newMethods)
{
	allowMethods = newMethods;
}

// Getter and Setter for 'alias'
const std::string &Location::getAlias() const
{
	return alias;
}

void Location::setAlias(const std::string &newAlias)
{
	alias = newAlias;
}

// Getter and Setter for 'root'
const std::string &Location::getRoot() const
{
	return root;
}

void Location::setRoot(const std::string &newRoot)
{
	root = newRoot;
}

// Getter and Setter for 'cgi_path'
const std::vector<std::string> &Location::getCGIPath() const
{
	return cgi_path;
}

void Location::setCGIPath(const std::vector<std::string> &newCGIPath)
{
	cgi_path = newCGIPath;
}

// Getter and Setter for 'cgi_ext'
const std::vector<std::string> &Location::getCGIExt() const
{
	return cgi_ext;
}

void Location::setCGIExt(const std::vector<std::string> &newCGIExt)
{
	cgi_ext = newCGIExt;
}

void Location::setClientBodyBufferSize(std::string str)
{
	unsigned long _client_body_buffer_size = 0;
	_client_body_buffer_size = static_cast<unsigned long>(std::strtol(str.c_str(), NULL, 10));
	client_body_buffer_size = _client_body_buffer_size;
}

unsigned long Location::getClientBodyBufferSize() const
{
	return (this->client_body_buffer_size);
}

// Getter and Setter for 'autoindex'
bool Location::getAutoindex() const
{
	return autoindex;
}

void Location::setAutoindex(bool newAutoindex)
{
	autoindex = newAutoindex;
}

void Location::setIndex(const std::string &str)
{
	index = str;
}

const std::string &Location::getIndex() const
{
	return (index);
}
