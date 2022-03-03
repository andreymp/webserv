#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include "../webserv.hpp"
#include <fstream>
#include <cstddef>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "../Request/Request.hpp"

class CGIHandler{
	private:
		pid_t								_cgi_pid;
		int									_req_pipe;
		int									_resp_pipe;
		Request								*_req;
		size_t								_pos;
		std::map<std::string, std::string>	_env;
		std::string 						_cgi_path;
		int									_exit_status;
		bool								_head_parsed;
		CGIHandler();

	public:
		CGIHandler(const CGIHandler &other);
		CGIHandler & operator=(const CGIHandler &other);
		virtual ~CGIHandler();

		bool prepareCgiEnv(Request *req, const std::string &absolute_path, const std::string &client_ip,
							const std::string &server_port, const std::string &cgi_exec);

		pid_t getCgiPid() const;
		void setCgiPid(pid_t cgiPid);

		size_t getPos() const;
		void setPos(size_t pos);
		bool isHeaderParsed() const;
		void setHeaderParsed(bool);

		class CGIHandlerException : public std::exception{
			virtual const char *what() const throw();
		};

		void setRequestPipe(int req_pipe);
		void setResponsePipe(int resp_pipe);
		const std::string &getCGIPath() const;
		const std::map<std::string, std::string> &getEnv() const;
		char **getEnv(void);
		int getRequestPipe() const;
		int getResponsePipe() const;
		int &getExit();
};

#endif