/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: celys <celys@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/17 12:14:07 by jobject           #+#    #+#             */
/*   Updated: 2022/03/07 21:35:59 by celys            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"


std::vector<int> fd;

void endServer(int __unused signal)
{
	close(4);
	close(3);
	std::cout << "CLOSE DONE!" << std::endl;
	for (std::vector<int>::iterator it = fd.begin(); it != fd.end(); ++it)
	{
		std::cout << "IT:" <<  *it << std::endl;
	}
	exit(0);
}

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
		signal(SIGINT, endServer);
		signal(SIGKILL, endServer);
		std::vector<Request> requests(conf.parse());
		ServerHandler server(requests);
		server.setup();
		fd = server.vec_fd;
		server.launch();
		server.closeConnection();
	} catch (const std::exception & e) {
		std::cout << e.what() << std::endl;
	}
	return 0;
}