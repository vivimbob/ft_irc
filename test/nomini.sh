#! /bin/bash

function nomini()
{
	./mini a$1 6667 &
}

trap "pkill -P $$" SIGINT
trap "pkill -P $$" SIGTERM
trap "pkill -P $$" SIGQUIT

for var in {1..500}
do
	nomini $var
done

pkill -P $$
