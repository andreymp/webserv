/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jobject <jobject@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/18 13:24:51 by jobject           #+#    #+#             */
/*   Updated: 2022/02/21 20:07:48 by jobject          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../webserv.hpp"

Request::Request() : host(0), port(0), serverName(""), root(""), index(""), methods(3) {}
Request::~Request() {}
Request::Request(const Request & other) { *this = other; }
Request & Request::operator=(const Request & other) { static_cast<void>(other); return *this;}

void Request::setHost(unsigned int _host) { host = _host; }
void Request::setPort(int _port) { port = _port; }
void Request::setIndex(std::string const & _file) { index = _file;}
void Request::setRoot(std::string const & _path) {root = _path; }
void Request::setServerName(std::string const & _name) { serverName = _name; }
void Request::setMethods(std::vector<std::string> & _methods) { methods = _methods; }
void Request::setLocation(std::vector<std::list<location> > const & locale) { loc = locale;}

unsigned int Request::getHost() const { return host; }
int Request::getPort() const { return port; }
std::string const & Request::getServerName() const { return serverName; }
std::string const & Request::getRoot() const { return root; }
std::string const & Request::getIndex() const { return index; }
std::vector<std::string> const & Request::getMethods() const { return methods; }
std::vector<std::list<location> > const & Request::getLocation() const { return loc; }