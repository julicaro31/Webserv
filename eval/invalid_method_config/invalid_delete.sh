#!/bin/bash

file="delete.txt"

cp $file ../../html/
(cd ../../ && gnome-terminal -- bash -c "./webserv eval/invalid_method_config/invalid_delete.conf ; bash")
sleep 1
output=$(curl -v -i -X DELETE http://127.0.0.1:8080/delete.txt)
rm -rf ../../html/$file 
