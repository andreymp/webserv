/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jobject <jobject@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/21 19:43:28 by jobject           #+#    #+#             */
/*   Updated: 2022/02/21 19:57:52 by jobject          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

unsigned int hostToUInt(std::string const & host) {
	int i = 0, j = host.find('.');
	unsigned int first = std::atoi(host.substr(i, j).c_str());
	i = ++j;
	for(; host[j] != '.'; j++);
	unsigned int second = std::atoi(host.substr(i, j).c_str());
	i = ++j;
	for(; host[j] != '.'; j++);
	unsigned int third = std::atoi(host.substr(i, j).c_str());
	i = ++j;
	for(; host[j] != '.'; j++);
	unsigned int fourth = std::atoi(host.substr(i, j).c_str());
	return (first << 24) | (second << 16) | (third << 8) | fourth;
}