/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jobject <jobject@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/17 18:08:57 by jobject           #+#    #+#             */
/*   Updated: 2022/02/24 17:14:27 by jobject          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../webserv.hpp"

Server::Server() {}
Server::~Server() {}
Server::Server(unsigned int _host, int _port) : host(_host), port(_port), messages() {}
Server::Server(const Server & other) { *this = other; }
Server & Server::operator=(const Server & other) {
	if (this != &other) {
		host = other.host;
		port = other.port;
		server_fd = other.server_fd;
		address = other.address;
		messages = other.messages;
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


int Server::makeNonBlocking() {
	int socket_fd, addrlen = sizeof(address);
	if ((socket_fd = accept(server_fd, (struct sockaddr *) &address, (socklen_t*) &addrlen)) < 0) {
		std::cerr << "Accept failure" << std::endl;
		throw Server::ServerException();
	}
	else if (fcntl(socket_fd, F_SETFL, O_NONBLOCK) < 0)
		throw Server::ServerException();
	messages.insert(std::make_pair(socket_fd, std::string("")));
	return socket_fd;
}

#define DEFUALT_SIZE 65536

int Server::recieve(int socket_fd) {
	char buffer[DEFUALT_SIZE];
	int ret = recv(socket_fd, buffer, DEFUALT_SIZE - 1, 0);
	if (ret == -1) {
		closeServer(socket_fd);
		std::cout << "Error while reading" << std::endl;
		return ret;
	}
	if (!ret) {
		closeServer(socket_fd);
		std::cout << "Connection closed by client" << std::endl;
		return ret;
	}
	messages[socket_fd].append(buffer);
	if (messages[socket_fd].find("\r\n\r\n") != std::string::npos) {
		if (messages[socket_fd].find("Content-Length: ") == std::string::npos) {
			if (messages[socket_fd].find("Transfer-Encoding: chunked") == std::string::npos)
				return 0;
			else {
				std::size_t j = messages[socket_fd].find("0\r\n\r\n");
				return j != std::string::npos && j + 5 == messages[socket_fd].size() ? 0 : 1;
			}
			size_t	cl = std::atoi(messages[socket_fd].substr(messages[socket_fd].find("Content-Length: ") + 16, 10).c_str());
			return messages[socket_fd].size() < cl + messages[socket_fd].find("\r\n\r\n") + 4 ? 1 : 0;
		}
	}
	return 1;
}

int Server::send(int socket_fd) {
	return 1;
}


void Server::closeServer(int socket_fd) const { close(socket_fd); }

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
