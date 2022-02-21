/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jobject <jobject@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/17 12:37:06 by jobject           #+#    #+#             */
/*   Updated: 2022/02/21 21:04:39 by jobject          ###   ########.fr       */
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
	char buf[1001];
	int ret;
	while ((ret = read(fd, buf, 1000)) > 0) {
		buf[ret] = '\0';
		res += buf;
	}
	close(fd);
	return res;
}

std::vector<Request> Config::parse() const {
	std::vector<Request> requests;
	std::string file = readFile();
	for (std::size_t i = 0; i < file.size(); ++i) {
		if ((i = file.find("server")) == std::string::npos)
			throw Config::ConfigException();
		i += std::strlen("server") + 1;
		if (i >= file.size() || file[i] != '{')
			throw Config::ConfigException();
		Request tmp = parseServer(++i, file);
		requests.push_back(tmp);
		std::cout << "[ " << file[i - 1] << " ]" << std::endl;
		if (file[i] != '}')
			throw Config::ConfigException();
	}
	return requests;
}

std::vector<std::list<location> > Config::parseLocation(std::size_t & i, std::string const & file) const {
	static_cast<void>(i);
	static_cast<void>(file);
	std::vector<std::list<location> > loc;
	return loc;	
}

Request Config::parseServer(std::size_t & index, std::string const & file) const {
	Request req;
	for (std::size_t i = index; i < file.size() && file[i] != '}'; ++i) {
		if (!file.compare(i, std::strlen("server_name "), "server_name ")) {
			i += std::strlen("srever_name ");
			std::size_t tmp = file.find('\n', i);
			if (tmp == std::string::npos)
				throw Config::ConfigException();
			req.setServerName(file.substr(i, tmp));
			i = tmp + 1;
		} else if (!file.compare(i, std::strlen("root "), "root ")) {
			i += std::strlen("root ");
			std::size_t tmp = file.find('\n', i);
			if (tmp == std::string::npos)
				throw Config::ConfigException();
			req.setRoot(file.substr(i, tmp));
			i = tmp + 1;
		} else if (!file.compare(i, std::strlen("index "), "index ")) {
			i += std::strlen("index ");
			std::size_t tmp = file.find('\n', i);
			if (tmp == std::string::npos)
				throw Config::ConfigException();
			req.setIndex(file.substr(i, tmp));
			i = tmp + 1;
		} else if (!file.compare(i, std::strlen("allow_methods "), "allow_methods ")) {
			++i;
			std::vector<std::string> allowMeth;
			if (!file.compare(i, std::strlen("GET"), "GET")) {
				allowMeth.push_back("GET");
				i += std::strlen("GET ");
			}
			else if (!file.compare(i, std::strlen("POST"), "POST")) {
				allowMeth.push_back("POST");
				i += std::strlen("POST ");
			}
			else if (!file.compare(i, std::strlen("DELETE"), "DELETE")) {
				allowMeth.push_back("DELETE");
				i += std::strlen("DELETE ");
			}
			req.setMethods(allowMeth);
		} else if (!file.compare(i, std::strlen("location "), "location "))
			req.setLocation(parseLocation(i, file));
		else if (!file.compare(i, std::strlen("listen "), "listen ")) {
			i += std::strlen("listen ");
			std::size_t tmp = file.find(':', i);
			if (tmp == std::string::npos)
				throw Config::ConfigException();
			std::size_t eol = file.find('\n', i);
			if (eol == std::string::npos)
				throw Config::ConfigException();
			req.setPort(std::atoi(file.substr(tmp, eol - tmp).c_str()));
			req.setHost(hostToUInt(file.substr(i, tmp - i)));
		}
	}
	return req;
}
