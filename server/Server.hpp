/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jobject <jobject@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/17 17:51:45 by jobject           #+#    #+#             */
/*   Updated: 2022/02/23 19:47:32 by jobject          ###   ########.fr       */
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
		void closeServer(int socket_fd) const;
		int makeNonBlocking() const;
		class ServerException : public std::exception {
			virtual const char * what() const throw();
		};
	private:
		Server();
		int server_fd;
		struct sockaddr_in address;
		unsigned int host;
		int port;

		void setAddress();
};

#endif