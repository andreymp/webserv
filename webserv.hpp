/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jobject <jobject@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/17 13:02:28 by jobject           #+#    #+#             */
/*   Updated: 2022/02/23 12:43:35 by jobject          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <iostream>
# include <string>
# include <cstring>
# include <sys/socket.h>
# include <netinet/in.h>
# include <unistd.h>
# include <fcntl.h>
# include <cstdlib>
# include <poll.h>
# include <sys/select.h>
# include <limits>
# include "cnf/Config.hpp"
# include "server/Server.hpp"
# include "request/Request.hpp"

struct location {
	std::string path;
	std::vector<std::string> methods;
	std::string root;
	std::string index;
	bool autoindex;
	std::string cgi_path;
	std::string cgi_extension;
	int client_body_size;

	location() : path(""), methods(3), root(""), index(""), autoindex(false), cgi_path(""), client_body_size(100) {}
	location(const location & other) { *this = other; }
	location & operator=(const location & other) {
		if (this != &other) {
			path = other.path;
			methods = other.methods;
			root = other.root;
			index = other.index;
			autoindex = other.autoindex;
			cgi_path = other.cgi_path;
			client_body_size = other.client_body_size;
		}
		return *this;
	}
	void setMethods(std::vector<std::string> & _methods) { methods = _methods; }
};

unsigned int hostToUInt(std::string const & host);

#endif