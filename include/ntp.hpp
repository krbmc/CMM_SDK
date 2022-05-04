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

// get time info
void get_current_fulltime_info(string& _date, string& _time, string& _utc);
void get_current_date_info(string& _date);
void get_current_time_info(string& _time);
void get_current_utc_info(string& _utc);


// set time by user
void set_time_by_userDate(string _date, string _time);
void set_time_by_userTimezone(string _new_tz, string _origin_tz="+09:00");
void calculate_diff_time(string _origin_tz, string _new_tz, string& _op, string& _hours);
// 이거 받는 date, time, utc같은거 validate검사 필요함 --> util에 만듬
// 

// set time using ntp server
int set_time_by_ntp_server(string _server);



#endif

// date로 시간 볼수있고 최적화된 형식은 찾아보고 date "+%F %T %:z"
// 
// date -s 로 수동시간조정
// rdate -s 로 타임서버의 시간으로 조정
// 서버 리스트 변경하면 conf 파일에 서버 부분 수정해줘야하고 ㅇㅇ
// conf 파일에 restrict 부분만 파악하면될듯

// 이거 timezone을 기존 +09:00 에서 계산해서 구현하게됨..
// 그래서 나중에 리소스에 남아있는값을 어떻게 처리할지 생각좀해봐야됨
// 

// 그래서 지금 할게뭐냐면 date, time 수동설정
// timezone 계산때려서 수동설정하는 함수 만들고
// ntp_server로 rdate하는 함수 만들고 일단 개별테스트 그런다음에 (이거할때 ntp.conf보고)
// networkprotocol patch에 반영하는데 이때 timezone > date, time 우선순위 설정필요하고
// 시간값을 수동으로 설정했을때 get할때마다 실시간 갱신을 하는걸로??
// ntp enabled 변수 여부에 따른 처리도 해줘야할텐데 이건 만약에 껏다켜졋는데 ntp enabled가 false
// 였다 그러면 수동설정된걸로 해야한단말임? 근데 수동설정된 시간은 다 날라감
// 1초마다 여기다가 갱신해놓지 않았으면 위에 get할때마다 갱신했으면 안남아있지 timezone이었으면
// 로드할때 다시 계산해서 설정해 놓기라도 가능한데 걍 date, time은 몰?루
// 아이쒸 진짜 개복잡하네 

// ---------------------------------------
/* 일단 변경 방식은 세가지..
#1 우선 ntp enabled 를 활성화 시키고 ntp서버 사용해서 동기화 시키는 경우
일단 웹상에서의 ntp 서버 제공이 ntp server list에 있는 거(즉, ntp.conf)만 제공중임
여기서 골라서 입력하게 되면 primary껄로 먼저 rdate하고 리턴밸류 0아니면(0이 정상명령어수행) secondary수행
하는식으로 했음 근데 이제 웹말고 포스트맨이나 직접 json담아서 줄때는 ntp server list에 없는것을 인자로
줄 수 있음 그럼 이거를 리스트에 없으면 유효성검사식으로 에러를 띄울거냐 아니면 서버리스트에 추가를하고
rdate를 수행해볼거냐 이거임 --> 일단.. primary서버에 들어오는건 리스트에 아니어도 rdate수행하게끔할게
대신 이게 ntp.conf 파일에 없는 서버여도 추가안시킴

#2 ntp enabled를 비활성화 시키고 date와 time을 수동으로 설정하는 경우
얘는 바로 date -s이용해서 수정함

#3 ntp enabled 비활성화 상태에서 timezone이 들어왔을때
해당 타임존에 맞게 계산때리고 그걸로 date -s 이용해서 수정함(date 명령어로 timezone이 안바뀌어서.. 계산으로
하기 때문에 timezone을 date껄 믿으면 안되고 리소스껄 써야함)

timezone이 우선순위 더 높아서  date,time과 같이들어와도 timezone이 들어오면 date,time은 무시

@ ntp 변수의 date와 time값은 계속 갱신이 되어야할것인데 이문제는 일단 정확하게 어떤식으로 해야할지는
안정해짐 근데 일단 get해올때 date랑 time은 date명령어로 현재 시간 뽑아서 갱신시켜두는쪽으로
해둘거임

@ 이제 서버가 꺼졌다 켜졌을 경우가 문젠데 아 아니다 서버온오프는 지장없고 리부트되었을경우가 문제임
리부트되면 명령어로 설정바꿨던거 다 날라가고 처음초기상태로 돌아옴 근데 이게 ntp.conf를 보고 ntpd가
수행하는 거 때문에 그렇게 되는 줄 알았는데 ntpd를 못하게 init sh파일을 빼놔도 KST시간으로 돌아옴
처음 수행될때 keti-redfish를 백그라운드로 실행시켰을때 메인문에 찍었던 시간이 timezone링크 파일을 바꿨을때
그 바꾼 timezone 시간이 출력되는것을 확인했었음 ntpd는 더 먼저 했을건데
그걸로 보아 ntpd때문이 아닐수도있을거라 추측중 --> 그래서 일단은 #3까지 해둘거임 @는 나중에 처리

*/