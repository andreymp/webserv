/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: celys <celys@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/03 17:51:55 by jobject           #+#    #+#             */
/*   Updated: 2022/03/07 23:11:19 by celys            ###   ########.fr       */
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
	env[envp.size()] = nullptr;
	return env;
}

// "" means to add to request
void CGIHandler::prepareCgiEnv(Request & request) {
	//envp.insert(std::make_pair("AUTH_TYPE", "")); 
	envp.insert(std::make_pair("CONTENT_LENGTH", std::to_string(body.size())));
	envp.insert(std::make_pair("REDIRECT_STATUS", "200"));
	envp.insert(std::make_pair("GATEWAY_INTERFACE", "CGI/1.1"));
	envp.insert(std::make_pair("SCRIPT_NAME", request.getCgiPath()));
	envp.insert(std::make_pair("SCRIPT_FILENAME", request.PATH + "/" + request.getCgiPath()));
	envp.insert(std::make_pair("REQUEST_METHOD", request.getMethod()));
	envp.insert(std::make_pair("PATH_INFO", request.PATH));
	// envp.insert(std::make_pair("PATH_TRANSLATED", request.getRoot() + "/" + request.getIndex()));
	envp.insert(std::make_pair("QUERY_STRING", ""));
	envp.insert(std::make_pair("REMOTE_ADDR", std::to_string(request.getHost())));
	envp.insert(std::make_pair("REQUEST_URI", request.PATH));
	envp.insert(std::make_pair("SERVER_NAME", request.getServerName() == "" ? envp["REMOTE_ADDR"] : request.getServerName()));
	envp.insert(std::make_pair("SERVER_PORT", std::to_string(request.getPort())));
	envp.insert(std::make_pair("SERVER_PROTOCOL", "HTTP/1.1"));
	envp.insert(std::make_pair("SERVER_SOFTWARE", "Weebserv/1.0"));
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
	
	const char * path = (request.PATH + "/" + std::string(filename)).c_str();
	std::cerr << path << std::endl;
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
		char * const * argv = nullptr;
		execve("/usr/bin/php", argv, env); // тут так
		std::cerr << "Execve failure\n" << strerror(errno) << std::endl;
		write(fds[1], SERVER_ERROR, std::strlen(SERVER_ERROR));
		delete [] argv[0];
		delete [] argv[1];
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
	return res;
}