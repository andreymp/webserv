/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerHandler.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jobject <jobject@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/23 16:16:51 by jobject           #+#    #+#             */
/*   Updated: 2022/03/02 16:57:20 by jobject          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../webserv.hpp"

ServerHandler::ServerHandler() {}
ServerHandler::~ServerHandler() {}
ServerHandler::ServerHandler(std::vector<Request> & req) : reqArray(req), maxFD(-1), servers() {}
ServerHandler::ServerHandler(const ServerHandler & other) { *this = other; }
ServerHandler & ServerHandler::operator=(const ServerHandler & other) {
	if (this != &other) {
		reqArray = other.reqArray;
		servers = other.servers;
		maxFD = other.maxFD;
	}
	return *this;
}

const char * ServerHandler::ServerHandlerException::what() const throw() { return "ServerHandler exception occured"; }

void ServerHandler::setup() {
	FD_ZERO(&fds);
	for (int i = 0; i < reqArray.size(); ++i) {
		Server tmp(reqArray[i].getHost(), reqArray[i].getPort());
		tmp.setup();
		int tmpFD = tmp.getServerFd();
		FD_SET(tmpFD, &fds);
		servers.insert(std::make_pair(tmpFD, tmp));
		if (tmpFD > maxFD)
			maxFD = tmpFD;
	}
	if (maxFD <= 0)
		throw ServerHandler::ServerHandlerException();
}

void ServerHandler::closeConnection() {
	for (std::map<int, Server>::iterator it = servers.begin(); it != servers.end(); ++it)
		it->second.closeServer(it->first);
}

void ServerHandler::launch() {
	std::vector<int> fill;
	std::map<int, Server *> sockets;
	while (true) {
		fd_set 			readingSet;
		fd_set 			writingSet;
		struct timeval	timeout;
		int ret = 0;
		while (!ret) {
			timeout.tv_sec = 0;
			timeout.tv_usec = 1;
			std::memcpy(&readingSet, &fds, sizeof(fds));
			FD_ZERO(&writingSet);
			for (int i = 0; i < fill.size(); ++i)
				FD_SET(fill[i], &writingSet);
			ret = select(maxFD + 1, &readingSet, &writingSet, NULL, &timeout);
		}
		if (ret > 0) {
			// Sending
			for (int i = 0; i < fill.size() && ret; ++i) {
				std::vector<int>::iterator it = fill.begin() + i;
				if (FD_ISSET(fill.at(i), &writingSet)) {
					ret = sockets[fill.at(i)]->send(fill.at(i));
					if (ret == -1) {
						FD_CLR(fill.at(i), &fds);
						FD_CLR(fill.at(i), &readingSet);
						fill.erase(it);	
						sockets.erase(fill.at(i));
					} else if (!ret)
						fill.erase(it);
					ret = 0;
					break ;
				}
			}
			// Recieving
			for (std::map<int, Server *>::iterator it = sockets.begin(); it != sockets.end(); ++it) {
				if (FD_ISSET(it->first, &readingSet)) {
					ret = it->second->recieve(it->first);
					if (ret == -1) {
						FD_CLR(it->first, &fds);
						FD_CLR(it->first, &readingSet);
						sockets.erase(it->first);
					} else if (!ret) {
						it->second->recieveHandler(it->first);
						fill.push_back(it->first);
					}
					ret = 0;
					break;	
				}
			}
			// Accepting sockets
			for (std::map<int, Server>::iterator it = servers.begin(); it != servers.end() && ret; ++it)
				if (FD_ISSET(it->first, &readingSet)) {
					int tmpSocket;
					if ((tmpSocket = it->second.makeNonBlocking()) >= 0) {
						FD_SET(tmpSocket, &readingSet);
						sockets.insert(std::make_pair(tmpSocket, &it->second));
						if (tmpSocket > maxFD)
							maxFD = tmpSocket;
					}
					ret = 0;
					break ;	
				}
		} else {
			for (std::map<int, Server *>::iterator it = sockets.begin(); it != sockets.end(); ++it)
				it->second->closeServer(it->first);
			fill.clear();
			FD_ZERO(&fds);
			for (std::map<int, Server>::iterator it = servers.begin(); it != servers.end(); ++it) {
				FD_SET(it->first, &fds);
			}
		}
	}
}
