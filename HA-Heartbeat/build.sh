#!/bin/sh

ip=10.0.6.107
id=root
password=ketilinux
path=/root
file=HA-Heartbeat

cmake CMakeLists.txt
make
mv src/HA-Heartbeat.

sshpass -p $password scp $file $id@$ip:$path

