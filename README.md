# webserv (42 project)
This project is about writing your HTTP server. \
You will be able to test it with an actual browser. \
HTTP is one of the most used protocols on the internet. \
Knowing its arcane will be useful, even if you won’t be working on a website.
 
## Description
• Program takes a configuration file as argument. \
• Program can’t execve another web server. \
• Server never block and the client can be bounced properly if necessary. \
• Server is non-blocking and uses only 1 select() for all the I/O operations between the client and the server (listen included). \
• select() checks read and write at the same time. \
• It is never do a read or a write operation without going through select(). \
• A request should is never hang forever. \
• Server is compatible with Safari and Google Chrome. \
• HTTP response status codes is accurate. \
• It has have default error pages if none are provided. \
• It is able to serve a fully static website. \
• Clients are able to upload files. \
• It was implemented GET, POST, and DELETE methods. \
• Server can listen to multiple ports. \
• It has CGI support. \
• The configuration file is similar to nginx config.

## Compiling
Run `make` to compile. \
Run `./webserv [configuration file]`

## Team
[Me](https://github.com/andreymp) \
[Heveline](https://github.com/daria-k15) \
[Mlagranzh](https://github.com/mlagranzh)
