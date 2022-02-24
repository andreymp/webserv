/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jobject <jobject@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/17 12:37:06 by jobject           #+#    #+#             */
/*   Updated: 2022/02/24 18:54:01 by jobject          ###   ########.fr       */
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
		if ((i = file.find("server", i)) == std::string::npos)
			break ;
		i += std::strlen("server") + 1;
		if (i >= file.size() || file[i] != '{')
			throw Config::ConfigException();
		i++;
		Request tmp = parseServer(i, file);
		requests.push_back(tmp);
		if (file[i] != '}')
			throw Config::ConfigException();
	}
	return requests;
}

Request Config::parseServer(std::size_t & i, std::string const & file) const {
	Request req;
	std::vector<location> locales;
	for (; i < file.size() && file[i] != '}'; ++i) {
		if (!file.compare(i, std::strlen("server_name "), "server_name ")) {
			i += std::strlen("srever_name ");
			std::size_t tmp = file.find('\n', i);
			if (tmp == std::string::npos)
				throw Config::ConfigException();
			req.setServerName(file.substr(i, tmp - i));
			i = tmp + 1;
		} else if (!file.compare(i, std::strlen("root "), "root ")) {
			i += std::strlen("root ");
			std::size_t tmp = file.find('\n', i);
			if (tmp == std::string::npos)
				throw Config::ConfigException();
			req.setRoot(file.substr(i, tmp - i));
			i = tmp + 1;
		} else if (!file.compare(i, std::strlen("index "), "index ")) {
			i += std::strlen("index ");
			std::size_t tmp = file.find('\n', i);
			if (tmp == std::string::npos)
				throw Config::ConfigException();
			req.setIndex(file.substr(i, tmp - i));
			i = tmp + 1;
		} else if (!file.compare(i, std::strlen("autoindex "), "autoindex ")) {
			i += std::strlen("autoindex ");
			if (!file.compare(i, 2, "on"))
				req.setAutoindex(true);
			i += req.getAutoindex() ? 2 : 3; 
		} else if (!file.compare(i, std::strlen("allow_methods "), "allow_methods ")) {
			i += std::strlen("allow_methods ");
			std::vector<std::string> allowMeth;
			while (i < file.size() && file[i] != '\n') {
				if (!file.compare(i, std::strlen("GET"), "GET")) {
					allowMeth.push_back("GET");
					i += std::strlen("GET ");
				}
				if (!file.compare(i, std::strlen("POST"), "POST")) {
					allowMeth.push_back("POST");
					i += std::strlen("POST ");
				}
				if (!file.compare(i, std::strlen("DELETE"), "DELETE")) {
					allowMeth.push_back("DELETE");
					i += std::strlen("DELETE ");
				}
				if (allowMeth.empty())
					throw Config::ConfigException();
			}
			req.setMethods(allowMeth);
		} else if (!file.compare(i, std::strlen("location "), "location ")) {
			i += std::strlen("location ");
			locales.push_back(parseLocation(i, file));
		}
		else if (!file.compare(i, std::strlen("listen "), "listen ")) {
			i += std::strlen("listen ");
			std::size_t tmp = file.find(':', i);
			if (tmp == std::string::npos)
				throw Config::ConfigException();
			std::size_t eol = file.find('\n', i);
			if (eol == std::string::npos)
				throw Config::ConfigException();
			tmp++;
			req.setPort(std::atoi(file.substr(tmp, eol - tmp).c_str()));
			req.setHost(hostToUInt(file.substr(i, tmp - i)));
			i = eol + 1;
		} else if (!file.compare(i, std::strlen("client_body_size "), "client_body_size ")) {
			i += std::strlen("client_body_size ");
			std::size_t tmp = file.find('\n', i);
			if (tmp == std::string::npos)
				throw Config::ConfigException();
			req.setClientBodySize(std::atoi(file.substr(i, tmp - i).c_str()));
			i = tmp + 1;
		}
		if (i >= file.size())
			throw Config::ConfigException();
	}
	req.setLocation(locales);
	return req;
}

location Config::parseLocation(std::size_t & i, std::string const & file) const {
	location loc;
	std::size_t tmp = file.find('{', i);
	std::size_t eol = file.find('\n', i);
	if (eol == std::string::npos || tmp == std::string::npos || eol != tmp + 1)
			throw Config::ConfigException();
	loc.path = file.substr(i, tmp - i - 1);
	i = tmp;
	for ( ; i < file.size() && file[i] != '}'; ++i) {
		if (!file.compare(i, std::strlen("allow_methods "), "allow_methods ")) {
			i += std::strlen("allow_methods ");
			std::vector<std::string> allowMeth;
			while (i < file.size() && file[i] != '\n') {
				if (!file.compare(i, std::strlen("GET"), "GET")) {
					allowMeth.push_back("GET");
					i += std::strlen("GET ");
				}
				if (!file.compare(i, std::strlen("POST"), "POST")) {
					allowMeth.push_back("POST");
					i += std::strlen("POST ");
				}
				if (!file.compare(i, std::strlen("DELETE"), "DELETE")) {
					allowMeth.push_back("DELETE");
					i += std::strlen("DELETE ");
				}
				if (allowMeth.empty())
					throw Config::ConfigException();
				if (file[i] == '\t')
					break ;
			}
			loc.setMethods(allowMeth);
		} else if (!file.compare(i, std::strlen("root "), "root ")) {
			i += std::strlen("root ");
			std::size_t tmp = file.find('\n', i);
			if (tmp == std::string::npos)
				throw Config::ConfigException();
			loc.root = file.substr(i, tmp - i);
			i = tmp + 1;
		} else if (!file.compare(i, std::strlen("index "), "index ")) {
			i += std::strlen("index ");
			std::size_t tmp = file.find('\n', i);
			if (tmp == std::string::npos)
				throw Config::ConfigException();
			loc.index = file.substr(i, tmp - i);
			i = tmp + 1;
		} else if (!file.compare(i, std::strlen("autoindex "), "autoindex ")) {
			i += std::strlen("autoindex ");
			if (!file.compare(i, 2, "on"))
				loc.autoindex = true;
			i += loc.autoindex ? 2 : 3; 
		} else if (!file.compare(i, std::strlen("cgi_path "), "cgi_path ")) {
			i += std::strlen("cgi_path ");
			std::size_t tmp = file.find('\n', i);
			if (tmp == std::string::npos)
				throw Config::ConfigException();
			loc.cgi_path = file.substr(i, tmp - i);
			i = tmp + 1;
		} else if (!file.compare(i, std::strlen("client_body_size "), "client_body_size ")) {
			i += std::strlen("client_body_size ");
			std::size_t tmp = file.find('\n', i);
			if (tmp == std::string::npos)
				throw Config::ConfigException();
			loc.client_body_size = std::atoi(file.substr(i, tmp - i).c_str());
			i = tmp + 1;
		} else if (!file.compare(i, std::strlen("cgi_extension "), "cgi_extension ")) {
			i += std::strlen("cgi_extension ");
			std::size_t tmp = file.find('\n', i);
			if (tmp == std::string::npos)
				throw Config::ConfigException();
			loc.cgi_extension = file.substr(i, tmp - i);
			i = tmp + 1;
		}
	}
	return loc;	
}
