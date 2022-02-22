/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jobject <jobject@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/17 12:34:07 by jobject           #+#    #+#             */
/*   Updated: 2022/02/22 14:05:18 by jobject          ###   ########.fr       */
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