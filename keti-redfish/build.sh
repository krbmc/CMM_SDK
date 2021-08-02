#!/bin/sh

ip=10.0.6.107
id=root
password=ketilinux
path=/root
file=bin/keti-redfish
# file=bin/keti-redfish1024

ssh-keygen -f "/root/.ssh/known_hosts" -R "$ip"
ssh-keyscan -t rsa $ip >>~/.ssh/known_hosts

cmake CMakeLists.txt
make
mv src/keti-redfish bin/keti-redfish
# mv src/keti-redfish1024 bin/keti-redfish1024

sshpass -p $password scp $file $id@$ip:$path

