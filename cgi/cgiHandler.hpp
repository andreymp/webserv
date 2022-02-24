#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include "../webserv.hpp"
#include <fstream>
#include <cstddef>
#include <sys/types.h>
#include <sys/wait.h>
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
};

#endif