/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerHandler.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: celys <celys@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/23 15:45:17 by jobject           #+#    #+#             */
/*   Updated: 2022/03/02 18:02:25 by celys            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERHANDLER_HPP
# define SERVERHANDLER_HPP

# include "../webserv.hpp"

class ServerHandler {
	public:
		explicit ServerHandler(std::vector<Request> & req);
		ServerHandler(const ServerHandler & other);
		ServerHandler & operator=(const ServerHandler & other);
		virtual ~ServerHandler();
		void setup();
		void launch();
		void closeConnection();
		class ServerHandlerException : public std::exception {
			virtual const char * what() const throw();	
		};
	private:
		ServerHandler();
		
		std::vector<Request> reqArray;
		std::map<int, Server> servers;
		long maxFD;
		fd_set fds;
};

#endif