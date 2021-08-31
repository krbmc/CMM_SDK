#!/bin/sh

ip=10.0.6.107
id=root
password=ketilinux
path=/root
# file=reboot
file=keti-redfish
# file=keti-redfish1024

ssh-keygen -f "/root/.ssh/known_hosts" -R "$ip"
ssh-keyscan -t rsa $ip >>~/.ssh/known_hosts

cmake CMakeLists.txt
make
# mv src/reboot .
mv src/keti-redfish .
# mv src/keti-redfish1024 .

sshpass -p $password scp $file $id@$ip:$path

