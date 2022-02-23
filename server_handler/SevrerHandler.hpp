/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SevrerHandler.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jobject <jobject@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/23 15:45:17 by jobject           #+#    #+#             */
/*   Updated: 2022/02/23 16:05:12 by jobject          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERHANDLER_HPP
# define SERVERHANDLER_HPP

# include "../webserv.hpp"

class ServerHadndler {
	public:
		ServerHadndler();
		~ServerHadndler();
	private:
		std::vector<Request> ReqArray;
		long maxFD;
};

#endif