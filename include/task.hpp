#ifndef TASK_H
#define TASK_H

#include "stdafx.hpp"
#include "resource.hpp"
#include "eventservice.hpp"

enum TASK_TYPE
{
    WAIT = 0,
    TASK_TYPE_SERVICEROOT,
    TASK_TYPE_SYSTEMS,
    TASK_TYPE_CHASSIS,
    TASK_TYPE_MANAGERS,
    TASK_TYPE_TASKSERVICE,
    TASK_TYPE_EVENTSERVICE,
    TASK_TYPE_ACCOUNTSERVICE,
    TASK_TYPE_SESSIONSERVICE,
    TASK_TYPE_UPDATESERVICE,
    TASK_TYPE_CERTIFICATESERVICE,
    TASK_TYPE_COMPLETED
};

enum WORK_STATUS
{
    WORK_FAIL = 0,
    WORK_SUCCESS,
    WORK_WAIT
};

typedef struct _m_Response
{
    unsigned int res_number;
    int result_status;
    string result_datetime;
    // 추가
    http_response result_response;
    json::value response_json;
} m_Response;

typedef struct _m_Request
{
    unsigned int task_number;
    json::value request_json;
    // enum WORK_STATUS cur_status;
    m_Response result;
    string request_datetime;
    // bool standby_saved;
    // 임시추가
    string method;
    string host;
    // host는 요청이 가야하는 곳 주소(target)
    string uri;
    // http_request sender;

} m_Request;

class Task_Manager
{
    public:
        Task_Manager(){};

        ~Task_Manager(){};

    // private:
        enum TASK_TYPE task_type;
        string host_ip;
        string primary_ip;
        int primary_port;
        int secondary_port;
        string _ip;
        std::list<m_Request> list_request;

};

void do_task_cmm_get(http_request _request);
void do_task_cmm_post(http_request _request);
void do_task_cmm_patch(http_request _request);
void do_task_cmm_delete(http_request _request);
void do_task_bmc_get(http_request _request);
void do_task_bmc_post(http_request _request);
void do_task_bmc_patch(http_request _request);
void do_task_bmc_delete(http_request _request);


Task_Manager* category(vector<string> _token);
m_Request work_before_request_process(string _method, string _host, string _uri, json::value _jv, http_headers _header);
Task_Manager* work_after_request_process(Task_Manager* _t, m_Request _msg);

void treat_uri_cmm_post(http_request _request, m_Request& _msg, json::value _jv, http_response& _response);
void do_actions(http_request _request, m_Request& _msg, json::value _jv, http_response& _response);
// m_Request act_bios(http_request _request, m_Request _msg, json::value _jv, string _resource, string _what); // act_bios는 보드오면 그때 맞춰서 다시 구현
void act_certificate(m_Request& _msg, json::value _jv, string _resource, string _what, http_response& _response);
void act_certificate_service(m_Request& _msg, json::value _jv, string _resource, string _what, http_response& _response);
void act_system(m_Request& _msg, json::value _jv, string _resource, string _what, http_response& _response);
void act_eventservice(m_Request& _msg, json::value _jv, string _resource, string _what, http_response& _response);
void act_logservice(m_Request& _msg, json::value _jv, string _resource, string _what, http_response& _response);
void act_update_service(http_request _request, m_Request& _msg, json::value _jv, string _resource, string _what, http_response& _response);
void act_virtualmedia(m_Request& _msg, json::value _jv, string _resource, string _what, http_response& _response);
void make_account(m_Request& _msg, json::value _jv, http_response& _response);
void make_session(m_Request& _msg, json::value _jv, http_response& _response);
void make_role(http_request _request, m_Request& _msg, json::value _jv, http_response& _response);
// 로그인을 검증해야하는 경우는 _request가 필요함 나중에 로그인검사하는 함수를 만들어서 밖에서 처리하도록
// 하고 인자제거 하든지 해야함 지금은 로그인검증 대부분 안하는상태에서 몇몇개에 넣어놓은거라 인자 그대로 둠
void make_subscription(m_Request& _msg, json::value _jv, http_response& _response);
// http_response make_logentry(http_request _request, m_Request& _msg, json::value _jv, http_response _response); // 제거대상
// m_Request make_virtualmedia(http_request _request, m_Request _msg, json::value _jv); // 제거대상

void treat_uri_cmm_patch(http_request _request, m_Request& _msg, json::value _jv, http_response& _response);
m_Request treat_uri_bmc_patch(http_request _request, m_Request _msg, json::value _jv);
void modify_account(http_request _request, m_Request& _msg, json::value _jv, string _uri, http_response& _response);
void modify_role(http_request _request, m_Request& _msg, json::value _jv, string _uri, http_response& _response);

bool patch_account_service(json::value _jv, string _record_uri);
bool patch_session_service(json::value _jv);
bool patch_manager(json::value _jv, string _record_uri);
bool patch_syslog(json::value _jv, string _record_uri);
bool patch_network_protocol(json::value _jv, string _record_uri);
void patch_fan_mode(string _mode, string _record_uri);
bool patch_ethernet_interface(json::value _jv, string _record_uri, int _flag);
bool patch_system(json::value _jv, string _record_uri);
bool patch_chassis(json::value _jv, string _record_uri);
bool patch_power_control(json::value _jv, string _record_uri);
bool patch_event_service(json::value _jv, string _record_uri);
bool patch_subscription(json::value _jv, string _record_uri);
bool patch_logservice(json::value _jv, string _record_uri);

void treat_uri_cmm_delete(http_request _request, m_Request& _msg, json::value _jv, http_response& _response);
void remove_account(m_Request& _msg, json::value _jv, string _uri, string _service_uri, http_response& _response);
void remove_session(http_request _request, m_Request& _msg, http_response& _response);
void remove_role(http_request _request, m_Request& _msg, json::value _jv, string _service_uri, http_response& _response);
void remove_subscription(m_Request& _msg, string _uri, string _service_uri, http_response& _response);


void error_reply(m_Request& _msg, json::value _jv, status_code _status, http_response& _response);
json::value get_error_json(string _message);
void success_reply(m_Request& _msg, json::value _jv, status_code _status, http_response& _response);

void test_send_event(Event _event);

// task_map backup용
json::value get_json_task_map(void);
json::value get_json_task_manager(Task_Manager *_tm);
void create_task_map_from_json(json::value _jv);
void create_task_manager_from_json(Task_Manager *_tm, json::value _jv);

// daemon
int daemon_init(void);

#endif