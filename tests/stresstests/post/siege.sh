#!/bin/bash

urls=./urls.txt
log=./siege.log
test_log=./test.log
test_exit_code=0

rm -rf $log
rm -rf $test_log

conditions=(1 2 4)

for conc in ${conditions[@]}; do
    for rep in ${conditions[@]}; do
        echo -ne "concurrent $conc - reps $rep: "
        siege -m "concurrent $conc - reps $rep" --delay=0.5 --file=$urls --log=$log --internet --verbose --reps=$rep --concurrent=$conc --no-parser &>> $test_log
        exit_code=$(echo $?)
        if [[ $exit_code != 0 ]]; then
            echo " ❌"
            test_exit_code=1
            echo $exit_code
        else
            echo " ✅" 
        fi
    done
done

if [[ $test_exit_code != 0 ]]; then
    echo "Not all tests passed! ❌"
else
    echo "All tests passed! ✅"
fi
