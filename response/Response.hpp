#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "webserv.hpp"
# include <map>
// # include "ResponseHeader.hpp"
# include "Request.hpp"
# include "CGI.hpp"


class Response {
public:
	Response(void);
	Response(const Response & src);
	~Response(void);

	Response & operator=(const Response & src);

	// Getter functions
	std::string		getResponse(void);

	// Member functions
	void			call(Request & request);

	void			getMethod(Request & request);
	void			headMethod(Request & request);
	void			postMethod(Request & request);
	void			putMethod(Request & request);
	void			deleteMethod(Request & request);
	void			optionsMethod(Request & request);
	void			traceMethod(Request & request);

	int				readContent(void);
	int				writeContent(std::string content);
	int				fileExists(std::string path);
	std::string		readHtml(const std::string& path);
	std::string		getPage_autoindex(const char *path, std::string const &host, int port);

private:
	std::string					_response;
	std::string					_path;
	int							_code;
	std::string					_type;
	bool						_isAutoIndex;
	unsigned int				_host;
	int							_port;
	std::map<int, std::string>	_errorMap;

	static std::map<std::string, void (Response::*)(Request &)>	_method;
	static std::map<std::string, void (Response::*)(Request &)>	initMethods();
};

#endif
