#!/bin/bash

file="delete_test"
location="../html/$file"
echo "DELETE test at $location"
echo "testing delete" > $location
(cd ../ && gnome-terminal -- bash -c "./webserv ; bash")
sleep 1
curl -v -i -X DELETE http://localhost:8080/$file
echo "data at $location:"
cat $location
