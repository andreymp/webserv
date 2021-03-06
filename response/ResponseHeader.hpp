/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponceHeader.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jobject <jobject@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/07 12:17:18 by jobject           #+#    #+#             */
/*   Updated: 2022/03/07 12:18:25 by jobject          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSEHEADER_HPP
# define RESPONSEHEADER_HPP

# include "../webserv.hpp"
# include <set>

class ResponseHeader {
public:
	ResponseHeader(void);
	ResponseHeader(const ResponseHeader & src);
	~ResponseHeader(void);

	ResponseHeader & operator=(const ResponseHeader & src);

	// Setter functions
	void			setAllow(std::vector<std::string> methods);
	void			setAllow(const std::string& allow = "");
	void			setContentLanguage(const std::string& lang = "");
	void			setContentLength(size_t size);
	void			setContentLocation(const std::string& path, int code);
	void			setContentType(std::string type, std::string path);
	void			setDate(void);
	void			setLastModified(const std::string& path);
	void			setLocation(int code, const std::string& redirect);
	void			setRetryAfter(int code, int sec);
	void			setServer(void);
	void			setTransferEncoding(void);
	void			setWwwAuthenticate(int code);

	// Member functions
	std::string		getHeader(size_t size, const std::string& path, int code, std::string type, const std::string& contentLocation, const std::string& lang);
	std::string		notAllowed(std::vector<std::string> methods, const std::string& path, int code, const std::string& lang);
	std::string		writeHeader(void);
	void			setValues(size_t size, const std::string& path, int code, std::string type, const std::string& contentLocation, const std::string& lang);
	void			resetValues(void);
	std::string		getStatusMessage(int code);

private:
	std::map<int, std::string>	_errors;
	std::string					_allow;
	std::string					_contentLanguage;
	std::string					_contentLength;
	std::string					_contentLocation;
	std::string					_contentType;
	std::string					_date;
	std::string					_lastModified;
	std::string					_location;
	std::string					_retryAfter;
	std::string					_server;
	std::string					_transferEncoding;
	std::string					_wwwAuthenticate;

	void						initErrorMap();
};

#endif