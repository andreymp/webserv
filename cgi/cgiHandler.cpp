#include "cgiHandler.hpp"

CGIHandler::CGIHandler(const CGIHandler &other) : _cgi_pid(other._cgi_pid), _req(other._req), _pos(other._pos), _head_parsed(other._head_parsed) {*this = other;}

CGIHandler & CGIHandler::operator=(const CGIHandler &other){
	if (this != &other){
		_cgi_pid = other._cgi_pid;
		_req_pipe = other._req_pipe;
		_resp_pipe = other._resp_pipe;
		_req = other._req;
		_pos = other._pos;
		_env = other._env;
		_cgi_path = other._cgi_path;
		_exit_status = other._exit_status;
		_head_parsed = other._head_parsed;
	}
	return *this;
}

~CGIHandler::CGIHandler(){
	if (waitpid(_cgi_pid, &_exit_status, ))
}