#!/bin/bash

exe="./../webserv"

incorrect_folder="configs/incorrect"
correct_folder="configs/correct"
incorrects=($incorrect_folder/*)
corrects=($correct_folder/*)
test_exit_code=0

#prepare executable
echo -e "preparing executable for testing\n"
(cd ../ && make)

echo -e "\nincorrect config file tests\n"
counter=0
for i in "${incorrects[@]}"
do
    echo -n "$counter: $(basename $i)"

    timeout 1s $exe $i &> /dev/null
    exit_code=$(echo $?)
    
    if [ $exit_code == 124 ]; then
        echo " ❌"
        test_exit_code=1
    else
        echo " ✅" 
    fi
    counter=$((counter+1))
done

echo -e "\ncorrect config file tests\n"
counter=0
for i in "${corrects[@]}"
do
    echo -n "$counter: $(basename $i)"

    timeout 1s $exe $i &> /dev/null
    exit_code=$(echo $?)

    if [ $exit_code != 124 ]; then
        echo " ❌"
        test_exit_code=1
    else
        echo " ✅" 
    fi
    counter=$((counter+1))
done



echo
if [ $test_exit_code == 1 ]; then
    echo "Not all tests passed!"
else
    echo "All tests passed!" 
fi

rm -rf logs.log

exit $test_exit_code
