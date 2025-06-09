#!/bin/bash

(cd ../ && gnome-terminal -- bash -c "./webserv ; bash")
sleep 1
curl -i http://localhost:8080/
