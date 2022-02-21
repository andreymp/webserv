/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jobject <jobject@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/17 12:34:07 by jobject           #+#    #+#             */
/*   Updated: 2022/02/21 20:06:17 by jobject          ###   ########.fr       */
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
		std::vector<std::list<location> > parseLocation(std::size_t & i, std::string const & file) const;
};

# endif