/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sung-hle <sung-hle@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/07 17:41:51 by sung-hle          #+#    #+#             */
/*   Updated: 2023/12/19 15:35:41 by sung-hle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Webserv.hpp"

class Location	{
	private:
		std::string						    					path;
		std::set<std::string>               allowMethods;
		// std::string                         alias;
		// bool                                aliasSet;
		std::string                         root;
		// std::map<std::string, std::string>  cgi_path;
		std::vector<std::string>          cgi_path;
		std::vector<std::string>            cgi_ext;
		// unsigned long                       client_body_buffer_size;
		bool                                autoindex;//
		std::string													index;//
	
	public:
		Location();
		~Location();
		Location(Location const &src);
		Location &operator=(Location const &src);
    
	// Getter and Setter for 'path'
    const std::string& getPath() const;
    void setPath(const std::string& newPath);

    // Getter and Setter for 'allowed_methods'
    const std::set<std::string>& getAllowedMethods() const;
    void setAllowedMethods(const std::set<std::string>& newMethods);

    // Getter and Setter for 'alias'
    // const std::string& getAlias() const;
    // void setAlias(const std::string& newAlias);

    // Getter and Setter for 'aliasSet'
    // bool getAliasSet() const;
    // void setAliasSet(bool newAliasSet);

    // Getter and Setter for 'root'
    const std::string& getRoot() const;
    void setRoot(const std::string& newRoot);

    // Getter and Setter for 'cgi_path'
    const std::vector<std::string>& getCGIPath() const;
    void setCGIPath(const std::vector<std::string>& newCGIPath);
    // void setCGIPath(const std::map<std::string, std::string>& newCGIPath);

    // Getter and Setter for 'cgi_ext'
    const std::vector<std::string>& getCGIExt() const;
    void setCGIExt(const std::vector<std::string>& newCGIExt);

    // Getter and Setter for 'client_body_buffer_size'
    // unsigned long getClientBodyBufferSize() const;
    // void setClientBodyBufferSize(const std::string& newSize);

    // Getter and Setter for 'autoindex'
    bool getAutoindex() const;
    void setAutoindex(bool newAutoindex);

		void setIndex(const std::string& str);
		const std::string& getIndex() const;

    const std::set<std::string> &getAllowMethods() const;
} ;