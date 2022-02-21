/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jobject <jobject@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/18 13:24:48 by jobject           #+#    #+#             */
/*   Updated: 2022/02/21 18:09:07 by jobject          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "../cnf/Config.hpp"
# include <vector>

class Request {
	public:
		Request();
		Request(const Request & other);
		Request & operator=(const Request & other);
		~Request();
		void setHost(unsigned int _host);
		void setPort(int _port);
		void setServerName(std::string const & _name);
		void setRoot(std::string const & path);
		void setIndex(std::string const & _file);
		void setMethods(std::vector<std::string> & methods);
		void setLocation(std::vector<std::list<location> > const & locale);
		unsigned int getHost() const;
		int getPort() const;
		std::string const & getServerName() const;
		std::string const & getRoot() const;
		std::string const & getIndex() const;
		std::vector<std::string> const & getMethods() const;
		std::vector<std::list<location> > const & getLocation() const;
	private:
		unsigned int host;
		int port;
		std::string serverName;
		std::string root;
		std::string index;
		std::vector<std::string> methods;
		std::vector<std::list<location> > loc;
};

#endif