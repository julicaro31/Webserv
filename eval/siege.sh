#!/bin/bash

(cd ../ && gnome-terminal -- bash -c "./webserv ; bash")
sleep 1
siege http://localhost:8080
