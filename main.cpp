#include "webserv.hpp"

bool checkArgs(int argc, char **argv) 
{
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

std::vector<int> fd;
void endServer(int signal)
{
	(void)signal;
	for (std::vector<int>::iterator it = fd.begin(); it != fd.end(); ++it)
	{
		close(*it);
		std::cout << "IT:" <<  *it << std::endl;
	}
	exit(0);
}

int main(int argc, char **argv) 
{
	if (!checkArgs(argc, argv)) 
	{
		std::cerr << "Wrong arguments" << std::endl;
		return 1;
	}
	Config conf(argc == 1 ? "./configs/default.config" : argv[1]);
	try {
		std::vector<Request> requests(conf.parse());
		ServerHandler server(requests);
		signal(SIGINT, endServer);
		server.setup();
		fd = server.get_fd();
		server.launch();
		server.closeConnection();
	} 
	catch (const std::exception & e) 
	{
		std::cout << e.what() << std::endl;
	}
	return 0;
}