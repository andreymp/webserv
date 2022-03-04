/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jobject <jobject@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/17 12:14:07 by jobject           #+#    #+#             */
/*   Updated: 2022/03/04 13:59:53 by jobject          ###   ########.fr       */
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
		// Server server(requests[0].getHost(), requests[0].getPort());
		// server.setup();
		// char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 21\n\nThis is my first page";
		// while (true) {
		// 	int socket_fd = server.makeNonBlocking();
		// 		char buffer[30000] = {0};
        // int valread = read(socket_fd , buffer, 30000);
        // printf("%s\n",buffer );
        // write(socket_fd , hello , std::strlen(hello));
        // printf("------------------Hello message sent-------------------\n");
        // close(socket_fd);
		// }
		ServerHandler server(requests);
		server.setup();
		server.launch();
		server.closeConnection();
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