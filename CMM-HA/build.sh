#!/bin/sh

ip=10.0.6.106
id=root
password=ketilinux
path=/root
file=CMM-HA

cmake CMakeLists.txt
make
mv src/CMM-HA .

sshpass -p $password scp $file $id@$ip:$path
echo "ssh pass good warking"

