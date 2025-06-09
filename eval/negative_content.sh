#!/bin/bash

#to test if we can still accept a request even though the Content-Length is set negative
(cd ../ && gnome-terminal -- bash -c "./webserv ; bash")
sleep 1
curl -i -H "Content-Length: -1" http://localhost:8080
