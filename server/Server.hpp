/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: celys <celys@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/17 17:51:45 by jobject           #+#    #+#             */
/*   Updated: 2022/03/02 18:02:43 by celys            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# define MAX_CONNECTIONS 1000

class Server {
	public:
		Server(unsigned int _host, int _port);
		Server(const Server & other);
		Server & operator=(const Server & other);
		virtual ~Server();
		long getServerFd() const;
		void setup();
		int send(int socket_fd);
		int recieve(int socket_fd);
		void closeServer(int socket_fd) const;
		void handleChunk(int socket_fd);
		int makeNonBlocking();
		void recieveHandler(int socket_fd);
		class ServerException : public std::exception {
			virtual const char * what() const throw();
		};
	private:
		Server();

		int server_fd;
		struct sockaddr_in address;
		unsigned int host;
		int port;
		std::map<int, std::string> messages;

		void setAddress();
};

#endif