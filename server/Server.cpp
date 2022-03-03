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
	// else if (fcntl(socket_fd, F_SETFL, O_NONBLOCK) < 0)
	// 	throw Server::ServerException();
	messages.insert(std::make_pair(socket_fd, std::string("")));
	return socket_fd;
}

#define DEFUALT_SIZE 65536

int Server::recieve(int socket_fd) 
{
	char buffer[DEFUALT_SIZE + 1];
	int ret = recv(socket_fd, buffer, DEFUALT_SIZE, 0);
	if (ret == -1) 
	{
		closeServer(socket_fd);
		std::cout << "Error while reading" << std::endl;
		return ret;
	}
	if (!ret) 
	{
		closeServer(socket_fd);
		std::cout << "Connection closed by client" << std::endl;
		return ret;
	}
	messages[socket_fd].append(buffer);
	if (messages[socket_fd].find(END) != std::string::npos) 
	{
		if (messages[socket_fd].find("Content-Length: ") == std::string::npos) 
		{
			if (messages[socket_fd].find("Transfer-Encoding: chunked") == std::string::npos)
				return 0;
			else 
			{
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
		std::cout << messages[socket_fd] << std::endl;
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
	std::size_t chunk = messages[socket_fd].find(CHUNK);
	if (chunk != std::string::npos && chunk < messages[socket_fd].find(END))
		handleChunk(socket_fd);
	std::cout << messages[socket_fd] << std::endl;
	if (messages[socket_fd] != "") {
		Request requestForResponse;
		if (messages[socket_fd].find("GET") == 0)
			requestForResponse.setMethod("GET");
		else if (messages[socket_fd].find("POST") == 0)
			requestForResponse.setMethod("POST");
		else if (messages[socket_fd].find("DELETE") == 0)
			requestForResponse.setMethod("DELETE");
		else 
			requestForResponse.setMethod("I am the best");
		requestForResponse.setBody(messages[socket_fd].substr(messages[socket_fd].find(END)));
		std::cout << requestForResponse.getMethod();
		char *hello = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 21\n\nThis is my first page";
		write(socket_fd , hello , std::strlen(hello));

		// Response response;
		// response.call(requestForResponse);
		messages.erase(socket_fd);
		std::cout << messages[socket_fd] << std::endl;
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
