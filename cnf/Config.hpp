/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: celys <celys@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/17 12:34:07 by jobject           #+#    #+#             */
/*   Updated: 2022/03/04 19:06:58 by celys            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <string> 
# include "../webserv.hpp"

class Request;
struct location;

class Config {
	public:
		Config();
		Config(const char *  _path);
		Config(const Config & other);
		virtual ~Config();
		Config & operator=(const Config & other);
		class ConfigException : public std::exception {
			virtual const char * what() const throw();
		};
		std::vector<Request> parse() const;
	private:
		std::string path;
		
		std::string readFile() const;
		Request parseServer(std::size_t & i, std::string const & file) const;
		location parseLocation(std::size_t & i, std::string const & file) const;
};

# endif