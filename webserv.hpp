/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jobject <jobject@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/17 13:02:28 by jobject           #+#    #+#             */
/*   Updated: 2022/02/21 19:56:00 by jobject          ###   ########.fr       */
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
# include <list>
# include "cnf/Config.hpp"
# include "server/Server.hpp"
# include "request/Request.hpp"

struct location {
	std::string path;
	std::vector<std::string> methods;
	std::string root;
	std::string index;
	bool allias;
	std::string cgi_pass;
};

unsigned int hostToUInt(std::string const & host);

#endif