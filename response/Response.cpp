/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jobject <jobject@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/04 12:48:30 by jobject           #+#    #+#             */
/*   Updated: 2022/03/08 18:01:42 by jobject          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "../cgi/CGIHandler.hpp"
#include "ResponseHeader.hpp"
#include <sys/time.h>
#include <sys/stat.h>
#include <fstream>      // std::ifstream
#include <sstream>      // std::stringstream
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <map>
#include <algorithm>

int		pathIsFile(const std::string& path)
{
	/*
		int stat(const char *restrict path, struct stat *restrict buf);
		ОПИСАНИЕ
		Функция stat() должна получить информацию об указанном файле и записать ее в область, на которую указывает 
		аргумент buf. Аргумент пути указывает на путь, 
		именующий файл. Разрешение на чтение, запись или выполнение указанного файла не требуется.
		Upon successful completion, 0 shall be returned. Otherwise, -1 shall be 
		returned and errno set to indicate the error.
	*/

	struct stat s;
	if (stat(path.c_str(), &s) == 0 )
	{
		if (s.st_mode & S_IFDIR)
			return 0;
		else if (s.st_mode & S_IFREG)
			return 1;
		else
			return 0;
	}
	else
		return 0;
}

// Static Assets

std::map<std::string, void (Response::*)(Request &)>	Response::initMethods()
{
	std::map<std::string, void (Response::*)(Request &)> map;

	map["GET"] = &Response::getMethod;
	map["POST"] = &Response::postMethod;
	map["DELETE"] = &Response::deleteMethod;
	return map;
}

std::map<std::string, void (Response::*)(Request &)> Response::_method = Response::initMethods();

// Member functions


/*
405
"Метод не разрешён". Сервер знает о запрашиваемом методе, но он был деактивирован 
и не может быть использован. Два обязательных метода,  GET и HEAD,
  никогда не должны быть деактивированы и не должны возвращать этот код ошибки.	
413 
Размер запроса превышает лимит, объявленный сервером. Сервер может закрыть соединение, 
вернув заголовок Retry-After
  */
void			Response::call(Request & request)
{
    std::pair<int, std::string> arr[] =
    {
        std::make_pair(400,"pages/default_error_pages/400.html"),
        std::make_pair(401,"pages/default_error_pages/401.html"),
        std::make_pair(403,"pages/default_error_pages/403.html"),
        std::make_pair(404,"pages/default_error_pages/404.html"),
		std::make_pair(405,"pages/default_error_pages/405.html"),
        std::make_pair(500,"pages/default_error_pages/500.html"),
        std::make_pair(503,"pages/default_error_pages/503.html"),
    };
    int n = sizeof(arr) / sizeof(arr[0]);
    std::map<int, std::string> m(arr, arr + n);
	_errorMap = m; // код -> путь до файла

	_isAutoIndex = request.getAutoindex();
	_host = "localhost";
	_port = request.getPort();
	_code = 200;
	_path = request.PATH;
	_index = request.getIndex();
	_language = request.get_language();
	_root = request.getRoot();
	std::vector<std::string> vec = request.getMethods();
	if (std::find(vec.begin(), vec.end(), request.getMethod()) == vec.end())
		_code = 405;
	else if (request.getClinetBodySize() < request.getBody().size())
		_code = 413;

	if (_code == 405 || _code == 413)
	{
		ResponseHeader	head;

		std::string ContentLocation = "/";
		_response = head.notAllowed(request.getMethods(), ContentLocation, _code, _language) + "\r\n";
		return ;
	}
	(this->*Response::_method[request.getMethod()])(request);
}

// Methods
void			Response::getMethod(Request & request)
{
	ResponseHeader	head;

	if (request.getCgiPath() != "")
	{
		CGIHandler	cgi(request);
		size_t		i = 0;
		size_t		j = _response.size() - 2;

		_response = cgi.exec(request.getCgiPath().c_str());
		// while (_response.find(END, i) != std::string::npos || _response.find("\r\n", i) == i)
		// {
		// 	std::string	str = _response.substr(i, _response.find("\r\n", i) - i);
		// 	if (str.find("Status: ") == 0)
		// 		_code = std::atoi(str.substr(8, 3).c_str());
		// 	i += str.size() + 2;
		// }
		// while (_response.find("\r\n", j) == j)
		// 	j -= 2;

		// _response = _response.substr(i, j - i);
		// std::cout << "{" + _response + "}" << std::endl;
	}
	else if  (_code == 200)
		_code = readContent();
	else
		_response = this->readHtml(_errorMap[_code]);

	std::string ContentLocation = request.getRoot();
	_response = head.getHeader(_response.size(), _path, _code, _type, ContentLocation, _language) + "\r\n" + _response;
}


void			Response::postMethod(Request & request)
{
	ResponseHeader	head;

	if (request.getCgiPath() != "")
	{
		CGIHandler	cgi(request);
		size_t		i = 0;
		size_t		j = _response.size() - 2;

		_response = cgi.exec(request.getCgiPath().c_str());

		while (_response.find("\r\n\r\n", i) != std::string::npos || _response.find("\r\n", i) == i)
		{
			std::string	str = _response.substr(i, _response.find("\r\n", i) - i);
			if (str.find("Status: ") == 0)
				_code = std::atoi(str.substr(8, 3).c_str());
			else if (str.find("Content-Type: ") == 0)
				_type = str.substr(14, str.size());
			i += str.size() + 2;
		}
		while (_response.find("\r\n", j) == j)
			j -= 2;

		_response = _response.substr(i, j - i);
	}
	else
	{
		_code = 204;
		_response = "";
	}
	if (_code == 500)
		_response = this->readHtml(_errorMap[_code]);
	_response = head.getHeader(_response.size(), _path, _code, _type, request.PATH, request.get_language()) + "\r\n" + _response;
}

void			Response::deleteMethod(Request & __unused request)
{
	ResponseHeader	head;

	_response = "";
	if (pathIsFile(_path))
	{
		if (remove(_path.c_str()) == 0)
			_code = 204;
		else
			_code = 403;
	}
	else
		_code = 404;
	if (_code == 403 || _code == 404)
		_response = this->readHtml(_errorMap[_code]);
	std::string ContentLocation = request.getRoot();
	_response = head.getHeader(_response.size(), _path, _code, _type, ContentLocation, _language) + "\r\n" + _response;
}


// Utils


std::string         Response::getPage_autoindex() 
{
	std::string path(_path);

	std::string dirName = "";
	if (path != _root)
    	dirName = "/" + path.substr(path.find(_root) + _root.size(), path.size());

	// std::cout << "{PATH}" << dirName << std::endl;
	char* ppath = const_cast<char*>(path.c_str());
    DIR *dir = opendir(ppath);
    std::string page =\
    "<!DOCTYPE html>\n\
    <html>\n\
    <head>\n\
            <title>" + dirName + "</title>\n\
    </head>\n\
    <body>\n\
    <h1>INDEX</h1>\n\
    <p>\n";

    if (dir == NULL) {
        std::cerr << "Error: could not open [" << path << "]" << std::endl;
        return "";
    }
    for (struct dirent *dirEntry = readdir(dir); dirEntry; dirEntry = readdir(dir)) 
    {
        std::string a = "\t\t<p><a href=\"http://" + \

		 _host + ":" + std::to_string(_port) + dirName + "/" + \
		 	std::string(dirEntry->d_name) + "\">" + \
			std::string(dirEntry->d_name) + \
		"</a></p>\n";
		
        page += a;
    }
    page +="\
    </p>\n\
    </body>\n\
    </html>\n";
    closedir(dir);
    return page;
}


int				Response::readContent(void)
{
	std::ifstream		file;
	std::stringstream	buffer;

	_type = "text/html";
	if ( _path.find(".") != std::string::npos)
		_type =  _path.substr(_path.find("."), _path.size());
	if (_type == ".png")
		_type = "image/png";
	if (_type == ".html")
		_type = "text/html";
	if (_type == ".jpg")
		_type = "image/jpg";
	if (_type == ".gif")
		_type = "image/gif";
	if (_type == ".mp4")
		_type = "video/mp4";
	_response = "";
	// std::cout << "{PATH}" << _path << _index << std::endl;
	if (_isAutoIndex && !pathIsFile(_path))
	{
		buffer << this -> getPage_autoindex();
		_response = buffer.str();
	}
	else if (pathIsFile(_path))
	{
		file.open(_path.c_str(), std::ifstream::in);
		if (file.is_open() == false)
		{
			/* 
			"Запрещено". У клиента нет прав доступа к содержимому,
			 поэтому сервер отказывается дать надлежащий ответ.
			 */ 	
			_response = this->readHtml(_errorMap[403]);
			return (403);
		}

		buffer << file.rdbuf();
		_response = buffer.str();

		file.close();
	}
	else if (pathIsFile(_path + _index))
	{
		std::string file_with_index = _path + _index;
		// std::cout << _path + _index << std::endl;
		file.open(file_with_index.c_str(), std::ifstream::in);
		if ((file).is_open() == false)
		{
			/*
			"Запрещено". У клиента нет прав доступа к содержимому,
			 поэтому сервер отказывается дать надлежащий ответ.
			 */ 	
			_response = this->readHtml(_errorMap[403]);
			return (403);
		}
		buffer << file.rdbuf();
		_response = buffer.str();
	
		file.close();
	}
	else
	{
		/*
		"Не найден". Сервер не может найти запрашиваемый ресурс.
		 Код этого ответа, наверно, самый известный из-за частоты его появления в вебе. 
		 */
		_response = this->readHtml(_errorMap[404]);
		return (404);
	}

	return (200);
}

std::string		Response::readHtml(const std::string& path)
{
	std::ofstream		file;
	std::stringstream	buffer;

	if (pathIsFile(path))
	{
		file.open(path.c_str(), std::ifstream::in);
		if (file.is_open() == false)
			return ("<!DOCTYPE html>\n<html><title>40404</title><body>There was an error finding your error page</body></html>\n");

		buffer << file.rdbuf();
		file.close();
		_type = "text/html";

		return (buffer.str());
	}
	else
		return ("<!DOCTYPE html>\n<html><title>40404</title><body>There was an error finding your error page</body></html>\n");
}

std::string		Response::getResponse(void)
{
	return (_response);
}

Response & Response::operator=(const Response & src)
{
	_response = src._response;
	_path = src._path;
	_code = src._code;
	return (*this);
}

Response::Response(void) {}

Response::Response(const Response & src)
{
	*this = src;
}

Response::~Response(void) {}