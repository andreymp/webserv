#ifndef CGi_HPP
# define CGi_HPP

# include "webserv.hpp"

# include "Config.hpp"
# include "Request.hpp"


class CgiHandler {
	public:
		CgiHandler(Request &request); // sets up env according to the request
		CgiHandler(CgiHandler const &src);
		virtual ~CgiHandler(void);

		CgiHandler   	&operator=(CgiHandler const &src);
		std::string		executeCgi(const std::string &scriptName);	// executes cgi and returns body
	private:
		CgiHandler(void);
		void								_initEnv(Request &request);
		char								**_getEnvAsCstrArray() const;
		int									_getSocket(unsigned int port);
		int									_connectSocket(unsigned int port);
		std::map<std::string, std::string>	_env;
		std::string							_body;
};

// ENV TO IMPLEMENT
// AUTH_TYPE
// CONTENT_LENGTH
// CONTENT_TYPE
// GATEWAY_INTERFACE
// PATH_INFO
// PATH_TRANSLATED
// QUERY_STRING˜

#endif
