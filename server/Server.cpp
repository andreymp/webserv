/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jobject <jobject@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/17 18:08:57 by jobject           #+#    #+#             */
/*   Updated: 2022/03/10 22:16:39 by jobject          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../webserv.hpp"
#include <fstream>

Server::Server() {}
Server::~Server() {}
Server::Server(Request const & _req) : host(_req.getHost()), port(_req.getPort()), req(_req), messages() {server_for_redir = -1;}
Server::Server(const Server & other) { *this = other; }
Server & Server::operator=(const Server & other) {
	if (this != &other) {
		host = other.host;
		port = other.port;
		server_fd = other.server_fd;
		address = other.address;
		messages = other.messages;
		req = other.req;
		server_for_redir = other.server_for_redir;
	}
	return *this;
}

const char * Server::ServerException::what() const throw() { return "Server exception occured"; }

long Server::getServerFd() const { return server_fd; }
int Server::getPort() const { return port; }

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
	else if (fcntl(socket_fd, F_SETFL, O_NONBLOCK) < 0)
		throw Server::ServerException();
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
		close(socket_fd);
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
		}
		size_t	cl = std::atoi(messages[socket_fd].substr(messages[socket_fd].find("Content-Length: ") + 16, 10).c_str());
		return messages[socket_fd].size() < cl + messages[socket_fd].find(END) + 4 ? 1 : 0;
	}
	return 1;
}

int Server::send(int socket_fd) {
	static std::map<int, unsigned int> sentMessages;
	if (sentMessages.find(socket_fd) == sentMessages.end())
		sentMessages.insert(std::make_pair(socket_fd, 0));
	if (!sentMessages[socket_fd] && messages[socket_fd] != "")
		std::cout << "\033[0;32m" << "SEND:" << std::endl << messages[socket_fd].substr(0, 1000) << "\033[0m" <<  std::endl;
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

void Server::loadFile(int socket_fd) {
	std::size_t beg = messages[socket_fd].find("boundary=");
	std::size_t end = messages[socket_fd].find("\r\n", beg);
	std::string flag = "--" + messages[socket_fd].substr(beg + std::strlen("boundary="), end - beg - std::strlen("boundary="));
	std::size_t pos = messages[socket_fd].find("filename=\"", end);
	
	std::string filename = messages[socket_fd].substr(pos + std::strlen("filename=\""), messages[socket_fd].find("\"\r\n", pos) - pos - std::strlen("filename=\""));
	std::fstream file;
	file.open("copy_" + filename, std::ios::binary | std::ios::out);
	std::size_t ending = messages[socket_fd].find(flag + "--") - 2;
	for (std::size_t i = messages[socket_fd].find(END, messages[socket_fd].find(flag)) + 4; i < ending; ++i)
		file.write(reinterpret_cast<const char*>(&messages[socket_fd][i]), sizeof(messages[socket_fd][i]));
	file.close();
}

void Server::recieveHandler(int socket_fd) { 
	std::size_t chunk = messages[socket_fd].find(CHUNK);
	if (chunk != std::string::npos && chunk < messages[socket_fd].find(END))
		handleChunk(socket_fd);
	if (messages[socket_fd] != "")
		std::cout << "\033[0;33m" << "RECIEVE:" << std::endl << messages[socket_fd] << "\033[0m" <<  std::endl;
	bool flag = false;
	if (messages[socket_fd].find("Content-Type: multipart/form-data") != std::string::npos) {
		if (messages[socket_fd].find("Content-Type: text") != std::string::npos) {
			loadFile(socket_fd);
		} else 
			std::cout << "not good file" << std::endl;
		flag = true;
	}
	if (messages[socket_fd] != "") {
		Request request(req);
		location locale;
		std::size_t tmp = messages[socket_fd].find('/');
		std::size_t last = tmp;
		for (; messages[socket_fd][last] != ' ' && last < messages[socket_fd].size() && messages[socket_fd].at(last) != '?'; last++);
		request.HEAD = messages[socket_fd][tmp + 1] == ' ' ? "/" : messages[socket_fd].substr(tmp, last - tmp);
		bool inLoc = false;
		for (std::size_t i = 0; i < req.getLocation().size(); ++i)
			if (request.HEAD == request.getLocation()[i].path) {
				locale = request.getLocation()[i];
				inLoc = true;
			}
		std::size_t i = messages[socket_fd].find('?');
		if (i != std::string::npos && messages[socket_fd][i - 1] != ' ')
			request.CGIArgs = messages[socket_fd].substr(i + 1, messages[socket_fd].find(' ', i) - i - 1);
		if (messages[socket_fd].find("GET") == 0)
			request.setMethod("GET");
		else if (messages[socket_fd].find("POST") == 0)
			request.setMethod("POST");
		else if (messages[socket_fd].find("DELETE") == 0)
			request.setMethod("DELETE");
		else
			request.setMethod("I am the best");
		request.setBody(messages[socket_fd].substr(messages[socket_fd].find(END)));
		request.set_language("en-us");
		std::string tmp5 = messages[socket_fd].substr(0, messages[socket_fd].find(END));
		while (tmp5.find("\n") != std::string::npos)
		{
			if (tmp5.find("Accept-Language:") == 0)
				request.set_language(tmp5.substr(tmp5.find("Accept-Language: ") + 17, tmp5.find("\n")));
			tmp5 = tmp5.substr(tmp5.find("\n") + 1, tmp5.size());
		}
		request.PATH = req.getRoot().substr(0, req.getRoot().size() - 1) + request.HEAD;
		if (flag)
			request.QUERY = "file";
		if (inLoc) {
			request.setMethods(locale.methods);
			request.setRoot(locale.root == "" ? req.getRoot() : locale.root);
			request.setIndex(locale.index == "" ? "" : "/" + locale.index);
			request.setAutoindex(locale.autoindex);
			request.setCgiPath(locale.cgi_path);
			request.setClientBodySize(locale.client_body_size);
		}
		Response response;
		if (req.getPortToredirect() != -1)
		{
			response._response += "HTTP/1.1 301 Moved Permanently\n";
			std::string redir_server = "http://localhost:" + std::to_string(req.getPortToredirect());
			response._response += "Location: " + redir_server + \
								request.HEAD +  END;
			closeServer(socket_fd);
		}
		else
			response.call(request);
		messages.erase(socket_fd);
		messages.insert(std::make_pair(socket_fd, response.getResponse()));
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


void Server::closeServer(int socket_fd) { 
	if (socket_fd > 0) 
		close(socket_fd); 
	messages.erase(socket_fd);
}

void Server::setup() {
	if ((server_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		std::cerr << "Cannot create socket" << std::endl;
		throw Server::ServerException();
	}
	int option = 1;
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

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
