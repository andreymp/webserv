/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jobject <jobject@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/17 17:51:45 by jobject           #+#    #+#             */
/*   Updated: 2022/02/18 12:06:31 by jobject          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

class Server {
	public:
		Server(unsigned int _host, int _port);
		Server(const Server & other);
		Server & operator=(const Server & other);
		~Server();
		long getServerFd() const;
		void setup();
		void launch();
	private:
		Server();
		long server_fd;
		struct sockaddr_in address;
		unsigned int host;
		int port;
};

#endif