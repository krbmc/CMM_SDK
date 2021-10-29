#/bin/sh
#----------------Global Variables
# 사용법 -t 타겟 이미지(현재수행중인 이미지 파일위치 ) -b 백업할 이미지(update할 이미지) -n 이미지파일 수행 이름(KETI-Edge, KETI-CMM 등 )
help(){
    echo " update img 35m-t update image patch \033[0m "
}
echo "update image options .."

while getopts t:b:n: opt
do
    case "$opt" in
        t)
            arg_t=$OPTARG
        ;;
        b)
            arg_b=$OPTARG
        ;;
        n)
            arg_n=$OPTARG
        ;;

    esac
done
if [ -z "$arg_t" ] || [ -z "$arg_b" ] || [ -z "$arg_n"]
then
   echo "Some or all of the parameters are empty";
fi

echo "rm $arg_t"
rm $arg_t

cp $arg_b $arg_t
#ps -ef | grep keti-redfish | grep -v grep | awk '{print $1}' | xargs kill -9

#echo "oo $0"
pid=`ps -ef | grep "$arg_n" | grep -v 'grep' | grep -v "$0" | awk '{print $1}'`
kill -9 $pid

#reboot
#fuser -k 443/tcp
#sleep 3
#$arg_t &
#sleep 90
#$arg_t &
