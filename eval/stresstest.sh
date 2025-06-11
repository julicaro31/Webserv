#!/bin/bash

(cd ../ && gnome-terminal -- bash -c "./webserv ; bash")
sleep 1
siege -c 100 -t 30s http://localhost:8080

