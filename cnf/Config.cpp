/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jobject <jobject@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/17 12:37:06 by jobject           #+#    #+#             */
/*   Updated: 2022/02/17 17:45:48 by jobject          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../webserv.hpp"

Config::Config() {}
Config::Config(const char *  _path) : path(_path) {}
Config::Config(const Config & other) { *this = other; }
Config::~Config() {}

Config & Config::operator=(const Config & other) {
	if (this != &other) {
		path = other.path;
	}
	return *this;
}

const char * Config::ConfigException::what() const throw() { return "Config Exception occured"; }

std::string Config::readFile() const {
	int fd = open(path.c_str(), O_RDONLY, 0644);
	if (fd == -1)
		throw new Config::ConfigException();
	std::string res = "";
	char buf[43];
	int ret;
	while ((ret = read(fd, buf, 42)) > 0) {
		buf[ret] = '\0';
		res += buf;
	}
	close(fd);
	return res;
}