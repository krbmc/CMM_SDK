#!/bin/sh

<<<<<<< HEAD
ip=10.0.6.106
=======
ip=10.0.6.107
>>>>>>> e363a34bb856e6f86ae0963c762ae6a32375c3f1
id=root
password=ketilinux
path=/root
file=HA-Heartbeat

cmake CMakeLists.txt
make
mv src/HA-Heartbeat.

sshpass -p $password scp $file $id@$ip:$path

