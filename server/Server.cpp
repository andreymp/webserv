/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jobject <jobject@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/17 18:08:57 by jobject           #+#    #+#             */
/*   Updated: 2022/02/21 19:36:49 by jobject          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../webserv.hpp"

Server::Server() {}
Server::~Server() {}
Server::Server(unsigned int _host, int _port) : host(_host), port(_port) {}
Server::Server(const Server & other) { *this = other; }
Server & Server::operator=(const Server & other) {
	if (this != &other) {
		host = other.host;
		port = other.port;
		server_fd = other.server_fd;
		address = other.address;
	}
	return *this;
}

long Server::getServerFd() const { return server_fd; }

void Server::setup() {
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		std::cerr << "Cannot create socket" << std::endl;
		exit(EXIT_FAILURE);
	}
	std::memset((char *) &address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(host);
	address.sin_port = htons(port);
	if ((bind(server_fd, (struct sockaddr *) &address, sizeof(address))) < 0) {
		std::cerr << "Bind failure" << std::endl;
		exit(EXIT_FAILURE);
	}
	if (listen(server_fd, 100) < 0) {
		std::cerr << "Cannot listen" << std::endl;
		exit(EXIT_FAILURE);
	}
}
