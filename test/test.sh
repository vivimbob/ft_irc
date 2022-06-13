#! /bin/bash

function start_server()
{
	../ircserv 6669 1234 > /dev/null 2>&1 &
	./servers/miniircd/miniircd --ports 6668 > /dev/null 2>&1 &
	cd servers/ergo
	./ergo run > /dev/null 2>&1 &
	cd ../..
}

function start_client()
{
	./srcs/a.out 6669 6668 6667
}

trap "pkill -P $$" SIGINT
trap "pkill -P $$" SIGTERM
trap "pkill -P $$" SIGQUIT

start_server
sleep 1
start_client

