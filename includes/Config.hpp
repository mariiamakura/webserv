/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sung-hle <sung-hle@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/13 11:37:48 by fhassoun          #+#    #+#             */
/*   Updated: 2024/01/08 18:59:09 by sung-hle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once 
#include "Webserv.hpp"
#include "Location.hpp"

class Location;

class Config
{
	private:
		std::string													listen;//can contain ip-address and port - 127.0.0.1:8080;  # Listen on localhost, port 8080 or listen 0.0.0.0:8000;  # Listen on all available network interfaces, port 8000
		std::vector<std::string>            serverNames;//e.g. www.example.com example.com or ip-address
		std::vector<std::string>            host;// does not exist in nginx but we need it for the host header or server_name or server_name_in_redirect
		std::string                         root;//e.g. /var/www/example.com/htdocs
		std::map<int, std::string>          errorPage;
		std::map<std::string, Location*>     location;
		std::set<std::string>               allowedMethods;
		std::vector<std::string>            index;//When a client requests a directory without specifying a particular file, Nginx will look for files listed in the index directive and serve the first one that exists in the directory.
		// std::string 							           index;
		bool                                autoindex;
		int							                    port;
		unsigned long                       client_body_buffer_size;//--------------- redirs, uploads
		// std::map<std::string, std::string>  cgi_param;
		// std::string                         cgi_pass;
		// std::vector<std::string>            locationPath;
		// std::string                         alias;
		// bool                                aliasSet;

// und für locations
		// std::string						              _path;
		// std::set<std::string>               _allowed_methods;
		// std::string                         _alias;
		// bool                                _aliasSet;
		// std::string                         _root;
		// std::map<std::string, std::string>  _cgi_param;
		// std::string                         _cgi_pass;
		// unsigned long                       _client_body_buffer_size;
		// bool                                _autoindex;
		

		void formatString(std::string& str);

	public:
		Config();
		~Config();
		Config(Config const &src);
		Config &operator=(Config const &src);
		
		int parse(std::ifstream& configFile);
		void setListen(std::string str);
		const std::string& getListen() const;
		void setServerNames(std::string str);
		const std::vector<std::string>& getServerNames() const;
		void setHost(std::string str);
		const std::vector<std::string>& getHost() const;
		void setPort(std::string str);
		int getPorts() const;
		void setRoot(std::string str);
		const std::string& getRoot() const;
		void setErrorPage(std::string str, std::string str2);
		const std::map<int, std::string>& getErrorPage() const;
		void setClientBodyBufferSize(std::string str);
		unsigned long getClientBodyBufferSize() const;
		// void setCgiParam(std::string str);
		// const std::map<std::string, std::string>& getCgiParam() const;
		// void setCgiPass(std::string str);
		// const std::string& getCgiPass() const;
		// void setLocationPath(std::string str, std::ifstream& configFile);
		// const std::vector<std::string>& getLocationPath() const;
		void setLocation(std::string str, std::ifstream& configFile);
		const std::map<std::string, Location*>& getLocation() const;
		void setAllowedMethods(std::string str);
		const std::set<std::string>& getAllowedMethods() const;
		void setIndex(std::string str);
		const std::vector<std::string>& getIndex() const;
		void setAutoindex(bool b);
		bool getAutoindex() const;
		// void setAlias(std::string str);
		// const std::string& getAlias() const;
		// void setAliasSet(bool b);
		// bool getAliasSet() const;
		static void printConfigs(std::vector<Config *>& serverConfigs);
};

void displaySet(const std::set<std::string>& strSet);
void displayVector(const std::vector<std::string>& strVector);

/* show me variations how a port directive could look like

ChatGPT
The port directive is not a standard directive in widely used web server configurations like Nginx or Apache. Instead, the common directive for specifying the port is listen. Here's an example of how listen can be used to define ports in different ways:

Listen on a specific IP address and port:

nginx
Copy code
listen 127.0.0.1:8080;
Listen on all available network interfaces and a specific port:

nginx
Copy code
listen *:80;
Listen on multiple ports for the same server block:

nginx
Copy code
listen 80;
listen 443 ssl;
Listen on IPv6 address and port:

nginx
Copy code
listen [::]:8080;
Combination of IPv6 address and port:

nginx
Copy code
listen [2001:0db8::1]:8080;
Remember, the exact syntax might vary based on the web server you are using. The examples above are for Nginx, and if you're using a different web server, the syntax could be different. Always refer to the documentation of the specific web server you're working with.



list necessary directives a config file for a server has to have in nginx

ChatGPT
A basic configuration file for an Nginx server typically includes several key directives to define the server's behavior. Here are some essential directives you would commonly find in an Nginx server configuration:

listen: Specifies the IP address and port number on which the server will listen for incoming requests.

nginx
Copy code
listen 80;
server_name: Defines the domain name or IP address that should match the server block.

nginx
Copy code
server_name example.com www.example.com;
root: Sets the root directory for requests.

nginx
Copy code
root /path/to/your/web/root;
index: Specifies the default files to look for in a directory.

nginx
Copy code
index index.html index.htm;
location: Defines how to process requests based on the URI.

nginx
Copy code
location / {
    # Configuration for the root URI
}

location /static/ {
    # Configuration for requests starting with /static/
}
error_page: Specifies the error pages for different HTTP status codes.

nginx
Copy code
error_page 404 /404.html;
error_page 500 502 503 504 /50x.html;
access_log and error_log: Defines the log files for access and error logs.

nginx
Copy code
access_log /path/to/access.log;
error_log /path/to/error.log;
location ~ \.php$: Configuration for processing PHP files (if applicable).

nginx
Copy code
location ~ \.php$ {
    # Configuration for processing PHP files
}
try_files: Attempts to serve a specified file or falls back to another if it doesn't exist.

nginx
Copy code
location / {
    try_files $uri $uri/ /index.html;
}
These directives provide a basic setup for serving static content and handling common scenarios. Depending on your specific needs, you may include additional directives for proxying, SSL/TLS configuration, security measures, and more. Always refer to the official Nginx documentation for comprehensive information and guidance.
 */