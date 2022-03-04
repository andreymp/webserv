/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jobject <jobject@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/18 13:24:51 by jobject           #+#    #+#             */
/*   Updated: 2022/03/04 19:05:57 by jobject          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../webserv.hpp"

Request::Request() : host(0), port(0), client_body_size(100), serverName(""), root(""), index(""), methods(3), loc(10), autoindex(false), method(""), body("") {}
Request::~Request() {}
Request::Request(const Request & other) : host(other.host), port(other.port), client_body_size(other.client_body_size), 
	serverName(other.serverName), root(other.root), index(other.index), methods(other.methods), loc(other.loc), autoindex(other.autoindex),
	body(other.body) { *this = other; }
Request & Request::operator=(const Request & other) {
	if (this != &other) {
		host = other.host;
		port = other.port;
		client_body_size = other.client_body_size;
		serverName = other.serverName;
		root = other.root;
		index = other.index;
		methods = other.methods;
		loc = other.loc;
		autoindex = other.autoindex;
		method = other.method;
		body = other.body;
	}
	return *this;
}

void Request::setHost(unsigned int _host) { host = _host; }
void Request::setPort(int _port) { port = _port; }
void Request::setIndex(std::string const & _file) { index = _file;}
void Request::setRoot(std::string const & _path) {root = _path; }
void Request::setServerName(std::string const & _name) { serverName = _name; }
void Request::setMethods(std::vector<std::string> & _methods) { methods = _methods; }
void Request::setLocation(std::vector<location> const & locale) { loc = locale;}
void Request::setClientBodySize(int _size) { client_body_size = _size; }
void Request::setAutoindex(bool flag) { autoindex = flag; }
void Request::setMethod(std::string const & _method) { method = _method; }
void Request::setBody(std::string const & _body) {
	body = _body;
	for (int i = 0; i < 4; ++i) {
		if (!body.size() || body.at(body.size() - 1) != i % 2 ? '\r' : '\n')
			break ;
		body.resize(body.size() - 1);
	}
}

unsigned int Request::getHost() const { return host; }
int Request::getPort() const { return port; }
std::string const & Request::getServerName() const { return serverName; }
std::string const & Request::getRoot() const { return root; }
std::string const & Request::getIndex() const { return index; }
std::string const & Request::getMethod() const { return method; }
std::string const & Request::getBody() const { return body; }
std::vector<std::string> const & Request::getMethods() const { return methods; }
std::vector<location> const & Request::getLocation() const { return loc; }
int Request::getClinetBodySize() const { return client_body_size; }
bool Request::getAutoindex() const { return autoindex; }