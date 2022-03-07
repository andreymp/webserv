/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: celys <celys@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/17 18:08:57 by jobject           #+#    #+#             */
/*   Updated: 2022/03/07 22:58:46 by celys            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../webserv.hpp"

Server::Server() {}
Server::~Server() {}
Server::Server(Request const & _req) : host(_req.getHost()), port(_req.getPort()), req(_req), messages() {}
Server::Server(const Server & other) { *this = other; }
Server & Server::operator=(const Server & other) {
	if (this != &other) {
		host = other.host;
		port = other.port;
		server_fd = other.server_fd;
		address = other.address;
		messages = other.messages;
		req = other.req;
	}
	return *this;
}

const char * Server::ServerException::what() const throw() { return "Server exception occured"; }

long Server::getServerFd() const { return server_fd; }

void Server::setAddress() {
	std::memset((char *) &address, 0, sizeof(address));
	address.sin_family = PF_INET;
	address.sin_addr.s_addr = htonl(host);
	address.sin_port = htons(port);
}


int Server::makeNonBlocking() {
	int socket_fd, addrlen = sizeof(address);
	if ((socket_fd = accept(server_fd, (struct sockaddr *) &address, (socklen_t*) &addrlen)) < 0) {
		std::cerr << "Accept failure" << std::endl;
		throw Server::ServerException();
	}
	// else if (fcntl(socket_fd, F_SETFL, O_NONBLOCK) < 0)
	// 	throw Server::ServerException();
	messages.insert(std::make_pair(socket_fd, std::string("")));
	return socket_fd;
}

int Server::recieve(int socket_fd) {
	char buffer[DEFUALT_SIZE + 1];
	int ret = recv(socket_fd, buffer, DEFUALT_SIZE, 0);
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
	if (messages[socket_fd].find(END) != std::string::npos) {
		if (messages[socket_fd].find("Content-Length: ") == std::string::npos) {
			if (messages[socket_fd].find("Transfer-Encoding: chunked") == std::string::npos)
				return 0;
			else {
				std::size_t j = messages[socket_fd].find("0\r\n\r\n");
				return j != std::string::npos && j + 5 == messages[socket_fd].size() ? 0 : 1;
			}
			size_t	cl = std::atoi(messages[socket_fd].substr(messages[socket_fd].find("Content-Length: ") + 16, 10).c_str());
			return messages[socket_fd].size() < cl + messages[socket_fd].find(END) + 4 ? 1 : 0;
		}
	}
	return 1;
}

int Server::send(int socket_fd) {
	static std::map<int, unsigned int> sentMessages;
	if (sentMessages.find(socket_fd) == sentMessages.end())
		sentMessages.insert(std::make_pair(socket_fd, 0));
	if (!sentMessages[socket_fd])
		std::cout << "\033[0;32m" << "SEND:" << std::endl << messages[socket_fd] << "\033[0m" <<  std::endl;
	std::string msgToSend = messages[socket_fd].substr(sentMessages[socket_fd], DEFUALT_SIZE);
	int ret = ::send(socket_fd, msgToSend.c_str(), msgToSend.size(), 0);
	if (ret >= 0) {
		sentMessages[socket_fd] += ret;
		if (sentMessages[socket_fd] < messages[socket_fd].size())
			return 1;
		else {
			messages.erase(socket_fd);
			sentMessages[socket_fd] = 0;
			return 0;
		}
	} else {
		closeServer(socket_fd);
		sentMessages[socket_fd] = 0;
		return ret;
	}
}

void Server::recieveHandler(int socket_fd) {
	std::cout << "socket_fd" << socket_fd << std::endl; 
	std::size_t chunk = messages[socket_fd].find(CHUNK);
	if (chunk != std::string::npos && chunk < messages[socket_fd].find(END))
		handleChunk(socket_fd);
	std::cout << "\033[0;33m" << "RECIEVE:" << std::endl << messages[socket_fd] << "\033[0m" <<  std::endl;
	if (messages[socket_fd] != "") {
		Request request(req);
		location locale;
		std::size_t tmp = messages[socket_fd].find('/');
		std::size_t last = tmp;
		for (; messages[socket_fd][last] != ' ' && last < messages[socket_fd].size(); last++);
		request.HEAD = messages[socket_fd][tmp + 1] == ' ' ? "/" : messages[socket_fd].substr(tmp, last - tmp);
		bool inLoc = false;
		for (std::size_t i = 0; i < req.getLocation().size(); ++i)
			if (request.HEAD == request.getLocation()[i].path) {
				locale = request.getLocation()[i];
				inLoc = true;
			}
		if (messages[socket_fd].find("GET") == 0)
			request.setMethod("GET");
		else if (messages[socket_fd].find("POST") == 0)
			request.setMethod("POST");
		else if (messages[socket_fd].find("DELETE") == 0)
			request.setMethod("DELETE");
		else
			request.setMethod("I am the best");
		request.setBody(messages[socket_fd].substr(messages[socket_fd].find(END)));
		request.PATH = req.getRoot().substr(0, req.getRoot().size() - 1) + request.HEAD;
		if (inLoc) {
			request.setMethods(locale.methods);
			request.setRoot(locale.root == "" ? req.getRoot() : locale.root);
			request.setIndex(locale.index == "" ? "" : "/" + locale.index);
			request.setAutoindex(locale.autoindex);
			request.setCgiPath(locale.cgi_path);
			request.setClientBodySize(locale.client_body_size);
		}
		std::cout << "REQUEST INFO: " << std::endl;
		// std::cout << request.getMethod() << std::endl;
		// std::cout << request.getAutoindex() << std::endl;
		// std::cout << request.getMethods().size() << std::endl;
		// std::cout << request.getMethods()[0] << std::endl;
		// std::cout << request.getMethods()[1] << std::endl;
		// std::cout << request.getMethods()[2] << std::endl;
		// std::cout << request.getRoot() << std::endl;
		// std::cout << request.PATH << std::endl;

		Response response;
		response.call(request);
		messages.erase(socket_fd);
		messages.insert(std::make_pair(socket_fd, response.getResponse()));
		// std::cout << messages[socket_fd] << std::endl;
	}
}

void Server::handleChunk(int socket_fd) {
	std::string head = messages[socket_fd].substr(0, messages[socket_fd].find(END)) + END;
	std::string body = "";
	std::string chunk = messages[socket_fd].substr(messages[socket_fd].find(END) + std::strlen(END), messages[socket_fd].size() - 1);
	std::string partition = chunk.substr(0, 100);
	long partitionSize = std::strtol(partition.c_str(), nullptr, 16);
	
	for (std::size_t i = 0; partitionSize;) {
		i = chunk.find("\r\n", i) + 2;
		body += chunk.substr(i, partitionSize);
		i += partitionSize + 2;
		partition = chunk.substr(i, 100);
		partitionSize = strtol(partition.c_str(), nullptr, 16);
	}
	body += END;
	messages[socket_fd] = head + body;
}


void Server::closeServer(int socket_fd) const { close(socket_fd); }

void Server::setup() {
	if ((server_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		std::cerr << "Cannot create socket" << std::endl;
		throw Server::ServerException();
	}
	int option = 1;
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

	setAddress();
	// std::cout << "SERVER FD" << server_fd << std::endl;
	if ((bind(server_fd, (struct sockaddr *) &address, sizeof(address))) < 0) {
		std::cerr << "Bind failure" << std::endl;
		throw Server::ServerException();
	}
	if (listen(server_fd, MAX_CONNECTIONS) < 0) {
		std::cerr << "Cannot listen" << std::endl;
		throw Server::ServerException();
	}
}
