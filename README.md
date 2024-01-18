# webserv

The project aims to create an HTTP web server using C++98 that adheres to the HTTP 1.1 standard. The server's compatibility and functionalities can be evaluated by comparing its headers and behaviors with the Nginx. 

- HTTP/1.1 Support
- 100.00% availability using poll()
- Support GET, POST, DELETE
- Mimic official HTTP responses
- Listen multiple ports
- Fully configurable
- Support Cookies and CGI(PHP, Python, JavaScript)


## how to install and compile

```
git clone <repository SSH/URL>
cd <repository>
make
```
## execution
Valid configurations can be found in /config folder:

```
./webserv config/<filename>
//or run without config file for default configuration
./webserv
```

webserv runs on http://localhost:9999/ by default



