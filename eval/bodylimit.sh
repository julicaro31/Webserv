#!/bin/bash

(cd ../ && gnome-terminal -- ./webserv ./eval/eval.conf)
sleep 1
echo "longer than body limit:"
curl -X POST -H "Content-Type: plain/text" --data "lollllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllll" http://localhost:9000/test
echo "content of file:" 
cat ../html/test
echo
echo "short than body limit:"
curl -X POST -H "Content-Type: plain/text" --data "lol" http://localhost:9000/test2
echo "content of file:" 
cat ../html/test2
rm -rf ../html/test2
echo

