#!/bin/bash

#timout test to see if we are not getting in a infinite loop checking an empty body even though Content-Length is set
(cd ../ && gnome-terminal -- bash -c "./webserv ; bash")
sleep 1
curl -i -H "Content-Length: 2000" http://localhost:8080
