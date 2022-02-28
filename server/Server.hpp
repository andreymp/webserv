/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: celys <celys@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/17 17:51:45 by jobject           #+#    #+#             */
/*   Updated: 2022/02/26 00:02:48 by celys            ###   ########.fr       */
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
		int makeNonBlocking();
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