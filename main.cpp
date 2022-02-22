/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jobject <jobject@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/17 12:14:07 by jobject           #+#    #+#             */
/*   Updated: 2022/02/22 19:59:55 by jobject          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

bool checkArgs(int argc, char **argv) {
	if (argc > 2)
		return false;
	if (argc == 1)
		return true;
	std::string path(argv[1]);
	if (path.empty() || path.find(".config") == std::string::npos || path == ".config")
		return false;
	if (path.find(".config") + std::strlen(".config") != path.size())
		return false;
	return true;
}

int main(int argc, char **argv) {
	if (!checkArgs(argc, argv)) {
		std::cerr << "Wrong arguments" << std::endl;
		return 1;
	}
	Config conf(argc == 1 ? "./configs/default.config" : argv[1]);
	try {
		std::vector<Request> requests(conf.parse());
		std::cout << "-----------" << requests.size() << "-------------" << std::endl;
		for (std::size_t i = 0; i < requests.size(); ++i) {
			std::cout << requests[i].getServerName() << std::endl
				<< requests[i].getHost() << ":" << requests[i].getPort() << std::endl
				<< requests[i].getRoot() << std::endl << requests[i].getIndex() << std::endl;
				for (std::size_t j = 0; j < requests[i].getMethods().size(); j++)
					std::cout << requests[i].getMethods()[j] + " ";
				std::cout << std::endl;
				std::cout << "-----------" << requests[i].getLocation().size() << "-------------" << std::endl;
				for (std::size_t j = 0; j < requests[i].getLocation().size(); j++) {
					std::cout << "location " + requests[i].getLocation()[j].path << std::endl
				<< requests[i].getLocation()[j].root << std::endl << requests[i].getLocation()[j].index << std::endl
				<< requests[i].getLocation()[j].client_body_size << std::endl << requests[i].getLocation()[j].cgi_path << std::endl
				<< requests[i].getLocation()[j].cgi_extension << std::endl << requests[i].getLocation()[j].autoindex << std::endl;
				std::cout << "--------" << requests[i].getLocation()[j].methods.size() << "--------" << std::endl;
				for (std::size_t k = 0; k < requests[i].getLocation()[j].methods.size(); k++)
					std::cout << requests[i].getLocation()[j].methods[k] + " ";
				}
				std::cout << std::endl;
		}
	} catch (const std::exception & e) {
		std::cout << e.what() << std::endl;
	}
	// int addrlen = sizeof(address);
	// int newSocket;
	// char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 21\n\nThis is my first page";
	// while (true) {
	// 	if ((newSocket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
	// 		std::cerr << "Cannot accept" << std::endl;
	// 		return 1;
	// 	}
	// 	char buffer[30000] = {0};
    //     int valread = read(newSocket , buffer, 30000);
    //     printf("%s\n",buffer );
    //     write(newSocket , hello , strlen(hello));
    //     printf("------------------Hello message sent-------------------\n");
    //     close(newSocket);

	// }
	return 0;
}