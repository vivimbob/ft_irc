#! /bin/bash

function start_server()
{
	../ircserv 6667 1234 > /dev/null 2>&1 &
	./servers/miniircd/miniircd --ports 6668 > /dev/null 2>&1 &
	cd servers/ergo
	./ergo run > /dev/null 2>&1 &
	cd ../..
}

function start_client()
{
	./mini $1 6667 6668 6669
}

trap "pkill -P $$" SIGINT
trap "pkill -P $$" SIGTERM
trap "pkill -P $$" SIGQUIT

c++ ./bridge/srcs/main.cpp -o mini

start_server
sleep 2
start_client test
pkill -P $$
