/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jobject <jobject@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/03 17:51:55 by jobject           #+#    #+#             */
/*   Updated: 2022/03/04 12:53:24 by jobject          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../webserv.hpp"

CGIHandler::CGIHandler() {}
CGIHandler::CGIHandler(const CGIHandler &other) { *this = other; }
CGIHandler::CGIHandler(Request & request) : body(request.getBody()) { prepareCgiEnv(request); }
CGIHandler & CGIHandler::operator=(const CGIHandler & other) {
	if (this != &other) {
		envp = other.envp;
		body = other.body;
	}
	return *this;
}
CGIHandler::~CGIHandler() {}

char ** CGIHandler::getEnv() const {
	char **env = new char* [envp.size() + 1];
	std::map<std::string, std::string>::const_iterator it = envp.begin();
	for (std::size_t j = 0; it != envp.end(); ++it, ++j) {
		env[j] = new char[it->first.size() + it->second.size() + 2];
		std::string tmp = it->first + '=' + it->second;
		std::strcpy(env[j], tmp.c_str());
	}
	return env;
}

// "" means to add to request
void CGIHandler::prepareCgiEnv(Request & request) {
	envp.insert(std::make_pair("AUTH_TYPE", "")); 
	envp.insert(std::make_pair("CONTENT_LENGTH", std::to_string(body.size())));
	envp.insert(std::make_pair("REDIRECT_STATUS", "200"));
	envp.insert(std::make_pair("GATEWAY_INTERFACE", "CGI/1.1"));
	envp.insert(std::make_pair("SCRIPT_NAME", request.getIndex()));
	envp.insert(std::make_pair("SCRIPT_FILENAME", request.getRoot() + "/" + request.getIndex()));
	envp.insert(std::make_pair("REQUEST_METHOD", request.getMethod()));
	envp.insert(std::make_pair("PATH_INFO", request.getRoot() + "/" + request.getIndex()));
	envp.insert(std::make_pair("PATH_TRANSLATED", request.getRoot() + "/" + request.getIndex()));
	envp.insert(std::make_pair("QUERY_STRING", ""));
	envp.insert(std::make_pair("REMOTE_ADDR", std::to_string(request.getHost())));
	envp.insert(std::make_pair("REMOTE_IDENT", ""));
	envp.insert(std::make_pair("REMOTE_USER", ""));
	envp.insert(std::make_pair("REQUEST_URI", ""));
	envp.insert(std::make_pair("SERVER_NAME", request.getServerName() == "" ? envp["REMOTE_ADDR"] : request.getServerName()));
	envp.insert(std::make_pair("SERVER_PORT", std::to_string(request.getPort())));
	envp.insert(std::make_pair("SERVER_PROTOCOL", "HTTP/1.1"));
	envp.insert(std::make_pair("SERVER_SOFTWARE", "webserv/1.0"));
}