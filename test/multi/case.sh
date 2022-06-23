#!/bin/bash

CASE=$(/bin/ls tests/case/$1)
for case in ${CASE[@]}
do
	echo $1
	echo $case
	./multi tests/case/$1/$case
	read
	clear
done
