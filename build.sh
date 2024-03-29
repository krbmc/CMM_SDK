#!/bin/sh

#ip=10.0.6.98
ip=10.0.0.13
#ip=10.0.6.106
id=root
password=ketilinux
path=/usr/sbin
# path=/root
# file=reboot
#file=keti-redfish_binary
 file=keti-redfish
# file=keti-redfish1024

ssh-keygen -f "/root/.ssh/known_hosts" -R "$ip"
ssh-keyscan -t rsa $ip >>~/.ssh/known_hosts

cmake CMakeLists.txt
make -j 60
# mv src/reboot .
mv src/keti-redfish .
#mv ./keti-redfish $file
# mv src/keti-redfish1024 .

sshpass -p $password scp $file $id@$ip:$path

