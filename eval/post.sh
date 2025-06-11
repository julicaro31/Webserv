#!/bin/bash

file="post_test"
(cd ../ && gnome-terminal -- bash -c "./webserv ; bash")
sleep 1
curl -X POST --data "testing post" http://localhost:8080/$file
echo 
echo "Data from post at html/$file:"
cat ../html/$file
rm -rf ../html/$file
