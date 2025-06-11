#!/bin/bash

correct="correct.html"
incorrect="incorrect.html"

cp $correct ../../html/
cp $incorrect ../../html/
(cd ../../ && gnome-terminal -- bash -c "./webserv eval/resolve/resolve.conf ; bash")
sleep 1
output=$(curl -v -i --resolve correct.com:8080:127.0.0.1 http://correct.com:8080/ | tee /dev/tty)
echo $output | grep -w correct
exit_code=$(echo $?)
if [[ $exit_code == 1 ]]; then
    echo "Test failed"
else
    echo "Test succeeded"
fi

rm -rf ../../html/$correct 
rm -rf ../../html/$incorrect 
