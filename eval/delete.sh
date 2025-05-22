#!/bin/bash

echo "testing delete" > ../html/delete_test

curl -X DELETE http://localhost:8080/delete_test
