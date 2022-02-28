#include "Response.hpp"
#include "ResponseHeader.hpp"
#include <sys/time.h>
#include <sys/stat.h>
#include <fstream>      // std::ifstream
#include <sstream>      // std::stringstream
#include <sstream>
#include <string.h>
#include <stdio.h>
#include <dirent.h>

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
	_errorMap = request.getErrorPage();
	_isAutoIndex = request.get_autoindex();
	_host = request.get_host();
	_port = request.get_port();
	_code = request.getRet();
	_path = request.getRoot();

	if (request.getMethods().find(request.getMethod()) == request.getMethods().end())
		_code = 405;
	else if (request.getClientBodyBufferSize() < request.getBody().size())
		_code = 413;

/*
405 Method Not Allowed — указанный клиентом метод нельзя применить к текущему ресурсу. 
В ответе сервер должен указать доступные методы в заголовке Allow, разделив их запятой.
 Эту ошибку сервер должен возвращать, если метод ему известен, но он не применим именно
  к указанному в запросе ресурсу, если же указанный метод не применим на всём сервере, 
  то клиенту нужно вернуть код 501 (Not Implemented). Появился в HTTP/1.1.
  */

/* 
 413 Payload Too Large — возвращается в случае, если сервер отказывается обработать
  запрос по причине слишком большого размера тела запроса. Сервер может закрыть соединение, 
  чтобы прекратить дальнейшую передачу запроса. Если проблема временная, то рекомендуется
   в ответ сервера включить заголовок Retry-After с указанием времени, по истечении которого
    можно повторить аналогичный запрос. Появился в HTTP/1.1. Ранее назывался 
	«Request Entity Too Large».
*/

	if (_code == 405 || _code == 413)
	{
		ResponseHeader	head;

		_response = head.notAllowed(request.getMethods(), request.getContentLocation(), _code, request.getLang()) + "\r\n";
		return ;
	}

	(this->*Response::_method[request.getMethod()])(request);
}

// Methods
void			Response::getMethod(Request & request)
{
	ResponseHeader	head;

	// if (requestConf.getCgiPass() != "")
	// {
	// 	CgiHandler	cgi(request, requestConf);
	// 	size_t		i = 0;
	// 	size_t		j = _response.size() - 2;

	// 	_response = cgi.executeCgi(requestConf.getCgiPass());

	// 	while (_response.find("\r\n\r\n", i) != std::string::npos || _response.find("\r\n", i) == i)
	// 	{
	// 		std::string	str = _response.substr(i, _response.find("\r\n", i) - i);
	// 		if (str.find("Status: ") == 0)
	// 			_code = std::atoi(str.substr(8, 3).c_str());
	// 		else if (str.find("Content-type: ") == 0)
	// 			_type = str.substr(14, str.size());
	// 		i += str.size() + 2;
	// 	}
	// 	while (_response.find("\r\n", j) == j)
	// 		j -= 2;

	// 	_response = _response.substr(i, j - i);
	// }
	if  (_code == 200)
		_code = readContent();
	else
		_response = this->readHtml(_errorMap[_code]);
	if (_code == 500)
		_response = this->readHtml(_errorMap[_code]);

	_response = head.getHeader(_response.size(), _path, _code, _type, request.getContentLocation(), request.getLang()) + "\r\n" + _response;
}

void			Response::headMethod(Request & request)
{
	ResponseHeader	head;
	(void)request;

	_code = readContent();
	_response = head.getHeader(_response.size(), _path, _code, _type, request.getContentLocation(), request.getLang()) + "\r\n";
}

// void			Response::postMethod(Request & request, Request & requestConf)
// {
	// ResponseHeader	head;

	// if (requestConf.getCgiPass() != "")
	// {
	// 	CgiHandler	cgi(request, requestConf);
	// 	size_t		i = 0;
	// 	size_t		j = _response.size() - 2;

	// 	_response = cgi.executeCgi(requestConf.getCgiPass());

	// 	while (_response.find("\r\n\r\n", i) != std::string::npos || _response.find("\r\n", i) == i)
	// 	{
	// 		std::string	str = _response.substr(i, _response.find("\r\n", i) - i);
	// 		if (str.find("Status: ") == 0)
	// 			_code = std::atoi(str.substr(8, 3).c_str());
	// 		else if (str.find("Content-Type: ") == 0)
	// 			_type = str.substr(14, str.size());
	// 		i += str.size() + 2;
	// 	}
	// 	while (_response.find("\r\n", j) == j)
	// 		j -= 2;

	// 	_response = _response.substr(i, j - i);
	// }
	// else
	// {
	// 	_code = 204;
	// 	_response = "";
	// }
	// if (_code == 500)
	// 	_response = this->readHtml(_errorMap[_code]);
	// _response = head.getHeader(_response.size(), _path, _code, _type, requestConf.getContentLocation(), requestConf.getLang()) + "\r\n" + _response;
// }

// void			Response::deleteMethod(Request & request, Request & requestConf)
// {
	// ResponseHeader	head;
	// (void)request;

	// _response = "";
	// if (pathIsFile(_path))
	// {
	// 	if (remove(_path.c_str()) == 0)
	// 		_code = 204;
	// 	else
	// 		_code = 403;
	// }
	// else
	// 	_code = 404;
	// if (_code == 403 || _code == 404)
	// 	_response = this->readHtml(_errorMap[_code]);
	// _response = head.getHeader(_response.size(), _path, _code, _type, requestConf.getContentLocation(), requestConf.getLang()) + "\r\n" + _response;
// }


// }


// Utils


std::string         Response::getPage_autoindex(const char *path, std::string const &host, int port) {
    std::string dirName(path);
    DIR *dir = opendir(path);
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
    if (dirName[0] != '/')
        dirName = "/" + dirName;
    for (struct dirent *dirEntry = readdir(dir); dirEntry; dirEntry = readdir(dir)) 
    {
        std::string a = "\t\t<p><a href=\"http://" + host + ": " + " " \
        + std::to_string(port) + " " + dirName + "/" + std::string(dirEntry->d_name) + "\">" + std::string(dirEntry->d_name) + "</a></p>\n";
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

	_response = "";

	if (pathIsFile(_path))
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
	else if (_isAutoIndex) {
		buffer << this -> getPage_autoindex(_path.c_str(),\
			std::to_string(_host), _port);
		_response = buffer.str();
		_type = "text/html";
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

int				Response::writeContent(std::string content)
{
	std::ofstream	file;

/* "Нет содержимого". Нет содержимого для ответа на запрос, 
но заголовки ответа, которые могут быть полезны, присылаются. Клиент может использовать 
их для обновления кешированных заголовков полученных ранее для этого ресурса.	
*/
	if (pathIsFile(_path))
	{
		file.open(_path.c_str());
		file << content;
		file.close();
		return (204);
	}
	else
	{
		/*
		"Запрещено". У клиента 
		нет прав доступа к содержимому, поэтому сервер отказывается дать надлежащий ответ. 	
		*/
		file.open(_path.c_str(), std::ofstream::out | std::ofstream::trunc);
		if (file.is_open() == false)
			return (403);

/* "Создано". Запрос успешно выполнен и в результате
 был создан ресурс. Этот код обычно присылается в ответ на запрос PUT "ПОМЕСТИТЬ".
 */
		file << content;
		file.close();
		return (201);
	}
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

// Getter functions

std::string		Response::getResponse(void)
{
	return (_response);
}

// Overloaders

Response & Response::operator=(const Response & src)
{
	_response = src._response;
	_path = src._path;
	_code = src._code;
	return (*this);
}

// Constructors and destructors

Response::Response(void)
{
}

Response::Response(const Response & src)
{
	*this = src;
}

Response::~Response(void)
{
}
