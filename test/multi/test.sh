#!/bin/bash

TEST=$(/bin/ls tests/case)

for test in ${TEST[@]}
do
	clear
	./case.sh $test
done
