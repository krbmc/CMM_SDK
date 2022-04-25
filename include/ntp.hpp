#ifndef __NTP_HPP__
#define __NTP_HPP__

#include "stdafx.hpp";


typedef struct _NTP
{
    bool protocol_enabled;
    int port;
    string primary_ntp_server;
    string secondary_ntp_server;
    vector<string> ntp_servers;
    string date_str;
    string time_str;
    string timezone;
} NTP;

// date로 출력이랑 수동변경하고
// rdate로 서버변경
// 데몬이랑 conf관계 파악하고

#endif

// date로 시간 볼수있고 최적화된 형식은 찾아보고
// date -s 로 수동시간조정
// rdate -s 로 타임서버의 시간으로 조정
// 서버 리스트 변경하면 conf 파일에 서버 부분 수정해줘야하고 ㅇㅇ
// conf 파일에 restrict 부분만 파악하면될듯