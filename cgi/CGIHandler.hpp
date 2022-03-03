/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jobject <jobject@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/03 17:53:35 by jobject           #+#    #+#             */
/*   Updated: 2022/03/03 18:39:20 by jobject          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include "../webserv.hpp"

class CGIHandler {
	public:
		CGIHandler(Request & req);
		CGIHandler(const CGIHandler &other);
		CGIHandler & operator=(const CGIHandler &other);
		virtual ~CGIHandler();
		std::string & exec();
		class CGIHandlerException : public std::exception {
			virtual const char * what() const throw();
		};
	private:
		std::map<std::string, std::string>	envp;
		std::string body;
		
		char **getEnv(void) const;
		void prepareCgiEnv(Request & request);
		CGIHandler();
};

#endif