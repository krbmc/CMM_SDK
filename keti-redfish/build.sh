#!/bin/sh

ip=10.0.6.107
id=root
password=ketilinux
path=/root
file=keti-redfish

cmake CMakeLists.txt
make
mv src/keti-redfish .

sshpass -p $password scp $file $id@$ip:$path

