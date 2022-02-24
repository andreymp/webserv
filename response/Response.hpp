#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "webserv.hpp"

// # include "ResponseHeader.hpp"
# include "Request.hpp"
// # include "RequestConfig.hpp"
// # include "CgiHandler.hpp"
// # include "AutoIndexGenerator.hpp"


class Response {
public:
	Response(void);
	Response(const Response & src);
	~Response(void);

	Response & operator=(const Response & src);

	// Getter functions
	std::string		getResponse(void);

	// Member functions
	void			call(Request & request, Request & requestConf);

	void			getMethod(Request & request, Request & requestConf);
	void			headMethod(Request & request, Request & requestConf);
	void			postMethod(Request & request, Request & requestConf);
	void			putMethod(Request & request, Request & requestConf);
	void			deleteMethod(Request & request, Request & requestConf);
	void			optionsMethod(Request & request, Request & requestConf);
	void			traceMethod(Request & request, Request & requestConf);

	int				readContent(void);
	int				writeContent(std::string content);
	int				fileExists(std::string path);
	std::string		readHtml(const std::string& path);

private:
	std::string					_response;
	std::string					_path;
	int							_code;
	std::string					_type;
	bool						_isAutoIndex;
	// t_listen					_hostPort;
	std::map<int, std::string>	_errorMap;

	static std::map<std::string, void (Response::*)(Request &, Request &)>	_method;
	static std::map<std::string, void (Response::*)(Request &, Request &)>	initMethods();
};

#endif
