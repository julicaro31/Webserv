# Welcome to Websurfers

Welcome to Project WebServ! This project is part of the 42 curriculum and focuses on creating a HTTP web server from scratch using C++.
The goal is to gain a deep understanding of how web servers function by implementing key features such as handling HTTP GET, HEAD, POST, PUT, and DELETE requests, from parsing configuration files, and can serve static files from a specified root directory or dynamic content using CGI.

## Getting Started

To get started with Project WebServ, follow these steps:

1. Clone the repository
2. Build the project using `make`
3. Run the web server
```bash
./webserv [Congitfig File] ## leave empty to use the default configuration.
```

## Features

- **HTTP/1.1 Support:** The web server supports the HTTP/1.1 protocol for handling client requests.
- **Methods:** Implemented support for the HTTP methods GET, POST, and DELETE.
- **Configurable:** Easily configure the web server using a specified configuration file.
- **Static File Serving:** Serve static files from the configured document root.
- **File Uploading:** Upload files using the POST method.
- **Error Handling:** Handle common HTTP errors and return appropriate status codes.
- **CGI:** Implemented CGI support using python.
- **IO Multiplexing:** Supports multiple concurrent client request.
