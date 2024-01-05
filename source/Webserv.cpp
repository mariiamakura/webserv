/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhassoun <fhassoun@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/20 08:53:31 by fhassoun          #+#    #+#             */
/*   Updated: 2023/12/11 11:56:01 by fhassoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"

Webserv::Webserv()
{
	server.push_back(Server());
	poll_fd.push_back(pollfd());
}

Webserv::~Webserv()
{
    if (http_request)
        delete http_request;
    if (http_response)
        delete http_response;
}

Webserv::Webserv(Webserv const &src)
{
	*this = src;
}

Webserv &Webserv::operator=(Webserv const &src)
{
	if (this == &src)
		return (*this);
	return (*this);
}

void Webserv::logging(std::string str, int status)
{
	std::string color1 = WHITE;
	std::string color2 = WHITE;
	switch (status)
	{
	case DEBUG:
		color1 = CYAN;
		color2 = PURPLE;
		break;
	case INFO:
		color1 = CYAN;
		color2 = GREEN;
		break;
	case WARNING:
		color1 = YELLOW;
		color2 = ORANGE;
		break;
	case ERROR:
		color1 = RED;
		color2 = LILA;
		break;
	default:
		break;
	}
	// get time
	time_t rawtime;
	struct tm *timeinfo;
	char buffer[80];
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buffer, sizeof(buffer), "[%d/%m/%Y %H:%M:%S] ", timeinfo);
	std::string time(buffer);
	std::cout << color1 << time << color2 << " " << str << RESET << std::endl;
}

int Webserv::check_sockfds(std::vector<int> sockfds, int i)
{
	//check if current fd is one of the server fds
	std::vector<int>::iterator it;
	for (it = sockfds.begin(); it != sockfds.end(); it++)
	{
		if (poll_fd[i].fd == *it)
			return (1);
	}
	return (0);
}

// setters
void Webserv::setEnv(char **env)
{
	this->env = env;
}

// getters
char **Webserv::getEnv()
{
	return (this->env);
}


std::map<std::string, std::string> Webserv::parse_form_data(const std::string &formData)
{
    std::map<std::string, std::string> result;
    std::istringstream ss(formData);
    std::string field;

    while (std::getline(ss, field, '&')) {
        size_t equalPos = field.find('=');
        if (equalPos != std::string::npos) {
            std::string name = field.substr(0, equalPos);
            std::string value = field.substr(equalPos + 1);
            result[name] = value;
        }
    }

    return result;
}

void Webserv::init_servers()
{
	std::vector<int> ports;
	ports.push_back(DEF_PORT);
	// ports.push_back(8888);
	// ports.push_back(7777);

	std::vector<Server>::size_type size = server.size();
	//std::cout << "size: " << size << std::endl;
	for (std::vector<Server>::size_type i = 0; i < size; i++)
	{
		// instead of ports[i] we need to loop through config files and pass port and backlog
		server[i].init_server(ports[i], DEF_BACKLOG);
		poll_fd[i].fd = server[i].getSockfd();
		poll_fd[i].events = POLLIN;
		if (poll_fd.size() < ports.size())
		{
			server.push_back(Server());
			poll_fd.push_back(pollfd());
			size++;
		}
	}

}

int Webserv::handle_pollin(int i)
{
    //in_request.clear(); //do we need it?
    int clientID = poll_fd[i].fd;
    if (poll_fd[i].events & POLLIN) //reading data from client
    {
        rc = recv(clientID, buffer, sizeof(buffer) - 1, 0);
        logging("POLLIN fd" + int_to_string(clientID) + ": added " + int_to_string(rc) + " bytes into buffer", DEBUG);
        if (rc > 0)
        {
            size_t currentSize = in_request[clientID].size();
            in_request[clientID].resize(currentSize + rc);
            for (int i = 0; i < rc; ++i) {
                in_request[clientID][currentSize + i] = static_cast<uint8_t>(buffer[i]);
            }
        }

    }
    else if (poll_fd[i].events == POLLHUP)
    {
        logging("POLLHUP", DEBUG);
        close_conn = TRUE;
        return (1);
    }
    else if (poll_fd[i].events & POLLERR)
    {
        logging("POLLERR", DEBUG);
        close_conn = TRUE;
        return (1);
    }
    else
    {
        logging("Error: Unknown event", DEBUG);
        close_conn = TRUE;
        return (1);
    }
    if (rc < 0)
    {
        //  check if this needs to be handled differently !!!!
        if (poll_fd[i].events & POLLIN)
        {
            // Socket buffer is full, continue with other tasks or wait
            return (1);
        }


        // if (errno == EWOULDBLOCK)
        // {
        // 	// No data available, continue with other tasks or wait
        // 	return (1);
        // }
        // else
        {
            // Handle other errors
            logging("Error: recv() failed", ERROR);
            close_conn = TRUE;
            return (1);
        }
    }
    // Check to see if the connection has been closed by the client
    if (rc == 0)
    {
        logging("Connection closed", DEBUG);
        close_conn = TRUE;
        return (1);
    }
    return (0);
}



void Webserv::run()
{

	end_server = FALSE;
	close_conn = FALSE;
	std::vector<int> sockfds;
	std::vector<pollfd>::size_type size = poll_fd.size();
	for (std::vector<pollfd>::size_type i = 0; i < size; i++)
		sockfds.push_back(poll_fd[i].fd);

	in_request.clear();
	// logging("Listening socket is " + int_to_string(sockfd), DEBUG);
	do
	{
		logging("Waiting on poll()...", INFO);
		rc = poll(&poll_fd[0], poll_fd.size(), -1);
		if (rc < 0)
		{
			logging("Error: poll() failed", ERROR);
			break;
		}
		if (rc == 0)
		{
			logging("Error: poll() timed out. End program.", ERROR);
			break;
		}

		// loop through all the descriptors to see which ones are ready
		size = poll_fd.size();
		// std::cout << "size: " << size << std::endl;
		for (std::vector<pollfd>::size_type i = 0; i < size; i++)
		{
			// Loop through to find the descriptors that returned POLLIN
			if (poll_fd[i].revents == 0)
				continue;
			if (check_sockfds(sockfds, i) == 1)
			{
				logging("Listening socket is readable", DEBUG);
				// Accept all incoming connections that are queued up on the listening socket before we loop back and call poll again
				do
				{
					// Accept each incoming connection
					//  new_sd = accept(sockfd, NULL, NULL);
					new_sd = accept(poll_fd[i].fd, NULL, NULL);
					if (new_sd < 0)
					{
						if (errno != EWOULDBLOCK)
						{
							logging("Error: accept() failed", ERROR);
							end_server = TRUE;
						}
						break;
					}
					poll_fd.push_back(pollfd());
					poll_fd.back().fd = new_sd;
					poll_fd.back().events = POLLIN;
					size++;
				} while (new_sd != -1);
			}
			else
			{
				//logging("Descriptor " + int_to_string(poll_fd[i].fd) + " is readable", DEBUG);
				close_conn = FALSE;

				do
				{
					if (handle_pollin(i) != 0)
						break;
					if (poll_fd[i].events | POLLOUT)
					{
                        std::vector<uint8_t>& requestData = in_request[poll_fd[i].fd];
                        std::string requestString(requestData.begin(), requestData.end());

                        logging("request :\n" + requestString + "\n", DEBUG);
                            logging(" ---- request: " + int_to_string(in_request[poll_fd[i].fd].size()) + " bytes received  ----", DEBUG);

                            http_response = new Response();

                            newOrAppendRequest(i);
							 if (http_request->method == "GET")
							{
                                http_response->status_code = getMethod(); //set outresponse inside

                                //std::cout << "PATH: " << http_response->path << std::endl;
                                out_response[poll_fd[i].fd] = create_http_response();
                                deleteRequest(poll_fd[i].fd);
                                close_conn = TRUE;

							}
                             else if  (http_request->method == "POST") {
                                 http_response->status_code = postMethod(i);

                                 //create a response
                                 out_response[poll_fd[i].fd] = create_http_response();
                                 if (http_response->status_code == 201 || http_response->status_code == 400)
                                     deleteRequest(poll_fd[i].fd);
                             }
							else if (http_request->method == "DELETE") {
                                 std::cout << "DELETE request" << std::endl;
                                 logging("request :\n" + requestString + "\n", DEBUG);

                                 //std::cout << "DELETE PATH: " << http_request->path << std::endl;

                                 http_response->status_code = http_request->deleteMethod();
                                 out_response[poll_fd[i].fd] = create_http_response();
                                 deleteRequest(poll_fd[i].fd);
                                 close_conn = TRUE;
                             }
							else
							{
								logging("Unknown request", DEBUG);
								// std::cout << "Unknown request" << std::endl;
							}

                            size_t res_size = out_response[poll_fd[i].fd]->toString().size();
                            const std::string responseStr = out_response[poll_fd[i].fd]->toString();
                            const char *resStr = responseStr.c_str();

							rc = send(poll_fd[i].fd, resStr, res_size, 0);
							logging(" ---- response: " + int_to_string(rc) + " bytes sent  ----", DEBUG);
							logging("response :\n" + out_response[poll_fd[i].fd]->toString() + "\n", DEBUG);

                            deleteResponse(poll_fd[i].fd);
							break;
					}
					else if (poll_fd[i].events | POLLHUP)
					{
						// std::cout << "POLLHUP" << std::endl;
						logging("POLLHUP", DEBUG);
						logging("Connection closed by the client", DEBUG);
						// std::cout << "Connection closed by the client" << std::endl;
						close_conn = TRUE;
						break;
					}
					else if (poll_fd[i].events | POLLERR)
					{
						// std::cout << "POLLERR" << std::endl;
						logging("POLLERR", DEBUG);
						close_conn = TRUE;
						break;
					}
					else
					{
						// std::cout << "Error: Unknown event" << std::endl;
						logging("Error: Unknown event", DEBUG);
						close_conn = TRUE;
						break;
					}
				} while (!close_conn);
				//in_request[poll_fd[i].fd] = "";
                in_request[poll_fd[i].fd].clear();

				// If the close_conn flag was turned on, we need to clean up this active connection. This clean up process includes removing the descriptor
				if (close_conn)
				{
					close(poll_fd[i].fd);

					poll_fd[i].fd = -1;
					logging("close_conn for: " + int_to_string(poll_fd[i].fd), DEBUG);
					// std::cout << "close_conn for: " << poll_fd[i].fd << std::endl;
					poll_fd.erase(poll_fd.begin() + i);
					size--;
				}
			}
		}

	} while (end_server == FALSE);

	// Close the connections
	p_iter = poll_fd.begin();
	while (p_iter != poll_fd.end())
	{
		// std::cout << "close_conn for: " << p_iter->fd << std::endl;
		logging("close_conn after server shutdown for: " + int_to_string(p_iter->fd), DEBUG);
		if (p_iter->fd >= 0)
			close(p_iter->fd);
		p_iter++;
	}
	return;
}
