#!/bin/bash

(cd ../ && gnome-terminal -- bash -c "./webserv ; bash")
sleep 1
curl -i --resolve example.com:8080:127.0.0.1 http://example.com:8080/

