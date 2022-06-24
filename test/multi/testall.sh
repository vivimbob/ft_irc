#!/bin/bash

TEST=$(/bin/ls tests/case)

for test in ${TEST[@]}
do
	clear
	./test.sh $test
done
