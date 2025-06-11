#!/bin/bash

(cd ../../ && gnome-terminal -- ./webserv ./eval/bodylimit_test/eval.conf)
sleep 1
echo "longer than body limit:"
curl -v -i -X POST -H "Content-Type: plain/text" --data "lollllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllll" http://localhost:9000/test
echo -n "content of file:" 
cat ../../html/test
rm -rf ../../html/test2
echo
echo "short than body limit:"
curl -v -i -X POST -H "Content-Type: plain/text" --data "lol" http://localhost:9000/test2
echo -n "content of file:" 
cat ../../html/test2
rm -rf ../../html/test2
echo

