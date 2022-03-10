/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jobject <jobject@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/03 17:51:55 by jobject           #+#    #+#             */
/*   Updated: 2022/03/10 21:01:08 by jobject          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../webserv.hpp"

CGIHandler::CGIHandler() {}
CGIHandler::CGIHandler(const CGIHandler &other) { *this = other; }
CGIHandler::CGIHandler(Request & request) : body(request.getBody()), request(request) { prepareCgiEnv(request); }
CGIHandler & CGIHandler::operator=(const CGIHandler & other) {
	if (this != &other) {
		envp = other.envp;
		body = other.body;
	}
	return *this;
}
CGIHandler::~CGIHandler() {}
const char * CGIHandler::CGIHandlerException::what() const throw() { return "CGIHandler exception occured!"; }

char ** CGIHandler::getEnv() const {	
	char **env = new char* [envp.size() + 1];
	std::map<std::string, std::string>::const_iterator it = envp.begin();
	for (std::size_t j = 0; it != envp.end(); ++it, ++j) {
		env[j] = new char[it->first.size() + it->second.size() + 2];
		std::string tmp = it->first + '=' + it->second;
		std::strcpy(env[j], tmp.c_str());
	}
	int j = 0;
	env[envp.size()] = nullptr;
	return env;
}

void CGIHandler::prepareCgiEnv(Request & request) {
	envp.insert(std::make_pair("GATEWAY_INTERFACE", "CGI/1.1"));
	envp.insert(std::make_pair("AUTH_TYPE", "Basic")); 
	envp.insert(std::make_pair("CONTENT_LENGTH", ""));
	envp.insert(std::make_pair("REDIRECT_STATUS", "200"));
	envp.insert(std::make_pair("SCRIPT_NAME", request.getCgiPath()));
	envp.insert(std::make_pair("SCRIPT_FILENAME", request.PATH + "/" + request.getCgiPath()));
	envp.insert(std::make_pair("REQUEST_METHOD", request.getMethod()));
	envp.insert(std::make_pair("PATH_INFO", request.PATH));
	envp.insert(std::make_pair("QUERY_STRING", ""));
	envp.insert(std::make_pair("REMOTE_ADDR", std::to_string(request.getHost())));
	envp.insert(std::make_pair("REQUEST_URI", request.PATH));
	envp.insert(std::make_pair("SERVER_NAME", request.getServerName() == "" ? envp["REMOTE_ADDR"] : request.getServerName()));
	envp.insert(std::make_pair("SERVER_PORT", std::to_string(request.getPort())));
	envp.insert(std::make_pair("SERVER_PROTOCOL", "HTTP/1.1"));
	envp.insert(std::make_pair("SERVER_SOFTWARE", "Weebserv/1.0"));
	
	if (request.CGIArgs != "") {
		int j = 0;
		int size = 1;
		for (int i = 0; i < request.CGIArgs.size(); i++)
			if (request.CGIArgs[i] == '&')
				size++;
		while (size--) {
			std::string value;
			std::size_t i = request.CGIArgs.find('=', j);
			std::string key = request.CGIArgs.substr(j, i - j);
			j = request.CGIArgs.find("&", i) + 1;
			if (j == std::string::npos) {
				value = request.CGIArgs.substr(i + 1);
				envp.insert(std::make_pair(key, value));
				break;
			}
			value = request.CGIArgs.substr(i + 1, j - i - 2);
			envp.insert(std::make_pair(key, value));
		}
	}
	if (request.getMethod() == "POST") {
		int j = 0;
		int size = 1;
		for (int i = 0; i < body.size(); i++)
			if (body[i] == '&')
				size++;
		while (size--) {
			std::string value;
			std::size_t i = body.find('=', j);
			std::string key = body.substr(j, i - j);
			j = body.find("&", i) + 1;
			if (j == std::string::npos) {
				value = body.substr(i + 1);
				envp.insert(std::make_pair(key, value));
				break;
			}
			value = body.substr(i + 1, j - i - 2);
			envp.insert(std::make_pair(key, value));
		}
	}
}

void CGIHandler::closeFunction(int in, int out, FILE * fin, FILE * fout, int fds[], char ** env) {
	dup2(in, STDIN_FILENO);
	dup2(out, STDOUT_FILENO);
	fclose(fin);
	fclose(fout);
	close(fds[0]);
	close(fds[1]);
	close(in);
	close(out);
	for (int i = 0; i < envp.size(); ++i)
		delete [] env[i];
	delete [] env;
}

std::string CGIHandler::exec(const char * filename) {
	char ** env = getEnv();
	int in = dup(STDIN_FILENO);
	int out = dup(STDOUT_FILENO);
	FILE * fin = tmpfile();
	FILE * fout = tmpfile();
	int fds[2] = {fileno(fin), fileno(fout)};
	std::string res = "";
	
	std::string path = std::string(filename) == "" ? request.PATH : request.PATH + "/" + std::string(filename);
	write(fds[0], body.c_str(), body.size());
	lseek(fds[0], 0, SEEK_SET);
	pid_t pid = fork();
	if (pid == -1) {
		std::cerr << "Fork failure" << std::endl;
		return SERVER_ERROR;
	} 
	if (!pid) {
		dup2(fds[0], STDIN_FILENO);
		dup2(fds[1], STDOUT_FILENO);
		execve(path.c_str(), nullptr, env);
		std::cerr << "Execve failure" << std::endl;
		write(fds[1], SERVER_ERROR, std::strlen(SERVER_ERROR));
		exit(EXIT_FAILURE);
	} else {
		int ret;
		
		waitpid(-1, nullptr, 0);
		lseek(fds[1], 0, SEEK_SET);
		char buff[DEFUALT_SIZE + 1] = {0};
		while ((ret = read(fds[1], buff, DEFUALT_SIZE)) > 0)
			res += buff;
		closeFunction(in, out, fin, fout, fds, env);
	}
	return res + END;
}