/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jobject <jobject@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/18 13:24:48 by jobject           #+#    #+#             */
/*   Updated: 2022/03/07 17:24:31 by jobject          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "../webserv.hpp"

class Request {
	public:
		Request();
		Request(const Request & other);
		Request & operator=(const Request & other);
		~Request();
		void setHost(unsigned int _host);
		void setPort(int _port);
		void setClientBodySize(int _size);
		void setServerName(std::string const & _name);
		void setRoot(std::string const & path);
		void setIndex(std::string const & _file);
		void setMethods(std::vector<std::string> & methods);
		void setCgiPath(std::string const & _path);
		void setLocation(std::vector<location> const & locale);
		void setMethod(std::string const & _method);
		void setBody(std::string const & _body);
		void setAutoindex(bool flag);
		
		unsigned int getHost() const;
		std::string const & getMethod() const ; 
		int getPort() const;
		int getClinetBodySize() const;
		std::string const & getServerName() const;
		std::string const & getRoot() const;
		std::string const & getCgiPath() const;
		std::string const & getBody() const;
		std::string const & getIndex() const;
		std::vector<std::string> const & getMethods() const;
		std::vector<location> const & getLocation() const;
		bool getAutoindex() const;

		std::string PATH;
		std::string HEAD;
	private:
		unsigned int host;
		int port;
		int client_body_size;
		std::string serverName;
		std::string root;
		std::string index;
		std::vector<std::string> methods;
		std::string method;
		std::string body;
		std::string cgi_path;
		std::vector<location> loc;
		bool autoindex;
};

#endif