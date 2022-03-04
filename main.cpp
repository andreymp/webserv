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
		// char *hello = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 21\n\nThis is my first page";
		// while (true)
		// {
		// 	int socket_fd = server.makeNonBlocking();
		// 	char buffer[30000] = {0};
		// 	int valread = read(socket_fd, buffer, 30000);
		// 	printf("%s\n", buffer);
		// 	write(socket_fd , hello , std::strlen(hello));
		// 	printf("------------------Hello message sent-------------------\n");
		// 	close(socket_fd);
		// }
		ServerHandler server(requests);
		server.setup();
		server.launch();
		server.closeConnection();
	} 
	catch (const std::exception & e) 
	{
		std::cout << e.what() << std::endl;
	}
	return 0;
}
// #define PORT 8080

// int main()
// {
// 	    int server_fd, new_socket; long valread;
//     struct sockaddr_in address;
//     int addrlen = sizeof(address);
    
//     // Only this line has been changed. Everything is same.
//     char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
    
//     // Creating socket file descriptor
//     if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
//     {
//         perror("In socket");
//         exit(EXIT_FAILURE);
//     }
    

//     address.sin_family = AF_INET;
//     address.sin_addr.s_addr = INADDR_ANY;
//     address.sin_port = htons( PORT );
    
//     memset(address.sin_zero, '\0', sizeof address.sin_zero);
    
    
//     if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
//     {
//         perror("In bind");
//         exit(EXIT_FAILURE);
//     }
//     if (listen(server_fd, 10) < 0)
//     {
//         perror("In listen");
//         exit(EXIT_FAILURE);
//     }
//     while(1)
//     {
//         printf("\n+++++++ Waiting for new connection ++++++++\n\n");
//         if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
//         {
//             perror("In accept");
//             exit(EXIT_FAILURE);
//         }
        
//         char buffer[30000] = {0};
//         valread = read( new_socket , buffer, 30000);
//         // printf("%s\n",buffer );
//         write(new_socket , hello , strlen(hello));
//         printf("------------------Hello message sent-------------------");
//         close(new_socket);
//     }

// }