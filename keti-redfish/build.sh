#!/bin/sh

ip=10.0.6.107
id=root
password=ketilinux
path=/root
file=keti-redfish
# file=keti-redfish1024

cmake CMakeLists.txt
make
mv src/keti-redfish .
# mv src/keti-redfish1024 .

sshpass -p $password scp $file $id@$ip:$path

