/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jobject <jobject@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/04 12:48:30 by jobject           #+#    #+#             */
/*   Updated: 2022/03/07 14:56:19 by jobject          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "../webserv.hpp"

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
	void			postMethod(Request & request);
	void			deleteMethod(Request & request);

	int				readContent(void);
	int				writeContent(std::string content);
	int				fileExists(std::string path);
	std::string		readHtml(const std::string& path);
	std::string     getPage_autoindex();
	
private:
	std::string					_response;
	std::string					_path;
	std::string					_index;
	int							_code;
	std::string					_type;
	bool						_isAutoIndex;
	std::string					_host;
	int							_port;
	std::map<int, std::string>	_errorMap;

	static std::map<std::string, void (Response::*)(Request &)>	_method;
	static std::map<std::string, void (Response::*)(Request &)>	initMethods();
};

#endif