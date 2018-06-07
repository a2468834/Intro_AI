#!/bin/bash
# Purpose:
# 	Compile the PR3.h and PR3.cpp and run the training model.
# History: 2018/06/07

# clean object file
rm PR3

g++ PR3.cpp -std=c++11 -o PR3 -O2 -Wall -Wextra -Wpedantic
read -p "Please enter the file name of your dataset: " filename
./PR3 ${filename}