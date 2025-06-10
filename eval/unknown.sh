#!/bin/bash

(cd ../ && gnome-terminal -- bash -c "./webserv ; bash")
sleep 1
curl -v -i -X UNKNOWN --data "but wait?" http://localhost:8080/test_unknown
