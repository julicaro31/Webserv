#!/bin/bash

(cd ../ && gnome-terminal -- valgrind --leak-check=full --track-origins=yes --track-fds=yes --trace-children=yes ./webserv ./eval/eval.conf)

