#include <iostream>
#include <vector>
#include <poll.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <map>
#include <string.h>
#include <ctime>

#define PORT 8080
#define MAX_CLIENTS 10
#define TIMEOUT 15000 // 5 seconds timeout in milliseconds

/**
 * @file test_server.cpp
 * @brief A simple server implementation using poll() for handling multiple clients.
 * 
 * This server listens for incoming connections on a specified port, accepts new clients,
 * and handles client communication using the poll() system call. It supports basic HTTP
 * responses and includes timeout handling for inactive clients.
 * 
 * The server performs the following steps:
 * 1. Creates a server socket.
 * 2. Binds the socket to a specified port.
 * 3. Listens for incoming connections.
 * 4. Uses poll() to monitor multiple sockets for events.
 * 5. Accepts new client connections and adds them to the poll list.
 * 6. Reads data from client sockets and sends HTTP responses.
 * 7. Handles client disconnections and errors.
 * 8. Checks for inactive clients and closes their connections if they time out.
 * 
 * @return int Exit status of the program.
 */
int main()
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    
    // Create server socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Bind socket to port 8080 on localhost
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons(PORT);
    bind(server_fd, (struct sockaddr *)&address, sizeof(address));

    // Listen for connections
    listen(server_fd, MAX_CLIENTS);
    std::cout << "Server listening on port " << PORT << std::endl;

    // Polling setup
    std::vector<pollfd> poll_fds;
    std::map<int, time_t> client_activity; // Stores last activity time for each client

    // Add listening socket to poll list 
    pollfd serverPFD;
    serverPFD.fd = server_fd;
    serverPFD.events = POLLIN;
    serverPFD.revents = 0;
    poll_fds.push_back(serverPFD);


    while (true)
    {
        // Call poll() with a timeout of TIMEOUT milliseconds
        int activity = poll(poll_fds.data(), poll_fds.size(), TIMEOUT);
        if (activity < 0)
        {
            perror("poll error");
            continue;
        }

        // Get current time for timeout tracking
        time_t now = time(NULL);

        // Loop through poll_fds to handle each socket
        for (size_t i = 0; i < poll_fds.size(); i++)
        {
            // Check if there's an event on this socket
            if (poll_fds[i].revents & POLLIN)
            {
                // There is a data to read from the poll_fds[i].fd
                // Check if it's a server FD (new connection)=> accept a new connection.
                if (poll_fds[i].fd == server_fd)
                {
                    new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
                    if (new_socket < 0)
                    {
                        perror("accept");
                        continue;
                    }
                    std::cout << "New client connected: " << inet_ntoa(address.sin_addr) << std::endl;

                    pollfd clientFD;
                    clientFD.fd = new_socket;
                    clientFD.events = POLLIN | POLLOUT;
                    clientFD.revents = 0;
                    poll_fds.push_back(clientFD);
                    client_activity[new_socket] = now; // Update activity time
                }
                // If POLLIN event is not on server_fd, then it's a client socket (read data)
                else
                {
                    char buffer[1024] = {0};
                    int valread = read(poll_fds[i].fd, buffer, 1024);

                    if (valread == 0)
                    { // Client disconnected
                        std::cout << "Client disconnected\n";
                        close(poll_fds[i].fd);
                        client_activity.erase(poll_fds[i].fd);
                        poll_fds.erase(poll_fds.begin() + i);
                        i--; // Adjust index
                    }
                    else
                    {
                        std::cout << "Received: " << buffer << std::endl;
                        client_activity[poll_fds[i].fd] = now; // Update activity timestamp
                    }
                }
            }

            // Check if socket is ready to write 
            if (poll_fds[i].revents & POLLOUT)
            {
                const char *body = "Hello, World!";
                std::string response = "HTTP/1.1 200 OK\r\nContent-Length: " + std::to_string(strlen(body)) + "\r\n\r\n" + body;
                send(poll_fds[i].fd, response.c_str(), response.length(), 0);
            }
            if (poll_fds[i].revents & POLLERR) {
                std::cerr << "Error on socket " << poll_fds[i].fd << std::endl;
            }
            if (poll_fds[i].revents & POLLHUP) {
                std::cerr << "Connection closed on socket " << poll_fds[i].fd << std::endl;
            }
        
        }

        // **Check for Inactive Clients (Timeout Handling)**
        for (size_t i = 1; i < poll_fds.size(); i++)
        { // Start from 1 to skip server_fd
            int client_fd = poll_fds[i].fd;
            if (client_activity.find(client_fd) != client_activity.end() &&
                now - client_activity[client_fd] > TIMEOUT / 1000)
            { // Convert ms to seconds
                std::cout << "Client timed out: " << client_fd << std::endl;
                close(client_fd);
                client_activity.erase(client_fd);
                poll_fds.erase(poll_fds.begin() + i);
                i--; // Adjust index
            }
        }
    }

    return 0;
}
