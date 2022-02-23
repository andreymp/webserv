/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jobject <jobject@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/17 18:08:57 by jobject           #+#    #+#             */
/*   Updated: 2022/02/23 15:28:23 by jobject          ###   ########.fr       */
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

const char * Server::ServerException::what() const throw() { return "Server exception occured"; }

long Server::getServerFd() const { return server_fd; }

void Server::setAddress() {
	std::memset((char *) &address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(host);
	address.sin_port = htons(port);
}


long Server::makeNonBlocking() const {
	int socket_fd, addrlen = sizeof(address);
	if ((socket_fd = accept(server_fd, (struct sockaddr *) &address, (socklen_t*) &addrlen)) < 0) {
		std::cerr << "Accept failure" << std::endl;
		throw Server::ServerException();
	}
	if (fcntl(socket_fd, F_SETFL, O_NONBLOCK) < 0)
		throw Server::ServerException();
	return socket_fd;
}

void Server::closeServer(int socket_fd) const {
	close(socket_fd);
}

void Server::setup() {
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		std::cerr << "Cannot create socket" << std::endl;
		throw Server::ServerException();
	}
	setAddress();
	if ((bind(server_fd, (struct sockaddr *) &address, sizeof(address))) < 0) {
		std::cerr << "Bind failure" << std::endl;
		throw Server::ServerException();
	}
	if (listen(server_fd, MAX_CONNECTIONS) < 0) {
		std::cerr << "Cannot listen" << std::endl;
		throw Server::ServerException();
	}
}

void Server::launch() {
		
}