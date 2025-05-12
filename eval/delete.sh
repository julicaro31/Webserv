#!/bin/bash

cp get1.txt ../html/
cp get2.txt ../html/

curl -X DELETE http://localhost:8080/get1.txt
curl -X DELETE http://localhost:8080/get2.txt
