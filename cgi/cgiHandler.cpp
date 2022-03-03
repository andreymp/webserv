#include "cgiHandler.hpp"

CGIHandler::CGIHandler(){}

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

bool CGIHandler::prepareCgiEnv(Request *req, const std::string &absolute_path, const std::string &client_ip,
							const std::string &server_port, const std::string &cgi_exec){
	std::map<std::string, std::string>::iterator it;
	
	_env["AUTH_TYPE"] = "";
	_env["CONTENT_LENGTH"] = req->getClinetBodySize();
	_env["REDIRECT_STATUS"] = "200";
	_env["GATEWAY_INTERFACE"] = std::string("CGI/1.1");
	if (!cgi_exec.empty())
		_env["SCRIPT_NAME"] = cgi_exec;
	else
		_env["SCRIPT_NAME"] = "";
	_env["SCRIPT_FILENAME"] = absolute_path;
	_env["REQUEST_METHOD"] = req->getMethod();
	// _env["HTTP_ACCEPT"];
	// _env["HTTP_COOKIE"];
	_env["REMOTE_ADDR"] = client_ip;
	_env["REMOTE_HOST"] = client_ip;
	_env["REMOTE_IDENT"] = "";
	_env["REMOTE_USER"] = "";
	_env["SERVER_PORT"] = server_port;
	_env["SERVER_PROTOCOL"] = "HTTP/1.1";

	std::map<std::string, std::string> tmp_map;
	std::map<std::string, std::string>::iterator tmp_iter;
}

CGIHandler::~CGIHandler(){
	if (waitpid(_cgi_pid, &_exit_status, WNOHANG) == 0)
		kill(_cgi_pid, SIGKILL);
	if (_resp_pipe != -1)
		close(_resp_pipe);
	if (_req_pipe != -1)
		close(_req_pipe);
}

pid_t CGIHandler::getCgiPid() const{
	return _cgi_pid;
}

void CGIHandler::setCgiPid(pid_t cgiPid){
	this->_cgi_pid = cgiPid;
}

size_t CGIHandler::getPos() const{
	return _pos;
}

void CGIHandler::setPos(size_t pos){
	this->_pos = pos;
}

bool CGIHandler::isHeaderParsed() const{
	return _head_parsed;
}

void CGIHandler::setHeaderParsed(bool status){
	this->_head_parsed = status;
}

void CGIHandler::setRequestPipe(int req_pipe){
	this->_req_pipe = req_pipe;
}

void CGIHandler::setResponsePipe(int resp_pipe){
	this->_resp_pipe = resp_pipe;
}

int CGIHandler::getRequestPipe() const{
	return _req_pipe;
}

int CGIHandler::getResponsePipe() const{
	return _resp_pipe;
}
