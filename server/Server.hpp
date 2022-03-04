/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jobject <jobject@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/17 17:51:45 by jobject           #+#    #+#             */
/*   Updated: 2022/03/04 18:09:41 by jobject          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "../webserv.hpp" 

class Request;

class Server {
	public:
		Server(Request const & _req);
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
		Request *req;
		std::map<int, std::string> messages;

		void setAddress();
};

#endif