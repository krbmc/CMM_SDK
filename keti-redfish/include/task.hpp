#ifndef TASK_H
#define TASK_H

#include "stdafx.hpp"
#include "resource.hpp"


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
    string result_data;
    enum WORK_STATUS result_status;
    string result_datetime;
    // 추가
    http_response result_response;
} m_Response;

typedef struct _m_Request
{
    unsigned int task_number;
    json::value request_json;
    enum WORK_STATUS cur_status;
    string path;
    m_Response result;
    string request_datetime;
    bool standby_saved;
    // 임시추가
    string method;
    string host;
    // 여기 host 리소스task json에는 들어온 요청의 페이로드중 헤더를 하는거니까 호스트 그대로 들어가서 쓰는거고
    // 여기는 걍 destination으로 해서 요청이 가야하는 곳 주소를 넣는게 맞는듯 하다.
    // 근데 요청을 쏜 곳의 주소도 알 수 있나?
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

        // void do_task(void);

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

m_Request treat_uri_cmm_post(http_request _request, m_Request _msg, json::value _jv);
m_Request do_actions(http_request _request, m_Request _msg, json::value _jv);
m_Request act_bios(http_request _request, m_Request _msg, json::value _jv, string _resource, string _what);
m_Request act_certificate(http_request _request, m_Request _msg, json::value _jv, string _resource, string _what);
m_Request act_certificate_service(http_request _request, m_Request _msg, json::value _jv, string _resource, string _what);
m_Request act_system(http_request _request, m_Request _msg, json::value _jv, string _resource, string _what);
m_Request act_eventservice(http_request _request, m_Request _msg, json::value _jv, string _resource, string _what);
m_Request act_logservice(http_request _request, m_Request _msg, json::value _jv, string _resource, string _what);
m_Request make_account(http_request _request, m_Request _msg, json::value _jv);
m_Request make_session(http_request _request, m_Request _msg, json::value _jv);
m_Request make_role(http_request _request, m_Request _msg, json::value _jv);
m_Request make_logentry(http_request _request, m_Request _msg, json::value _jv); // 제거대상

m_Request treat_uri_cmm_patch(http_request _request, m_Request _msg, json::value _jv);
m_Request treat_uri_bmc_patch(http_request _request, m_Request _msg, json::value _jv);
m_Request modify_account(http_request _request, m_Request _msg, json::value _jv, string _uri);
m_Request modify_role(http_request _request, m_Request _msg, json::value _jv, string _uri);
bool patch_account_service(json::value _jv, string _record_uri);
bool patch_session_service(json::value _jv);
void patch_manager(json::value _jv, string _record_uri);
bool patch_network_protocol(json::value _jv, string _record_uri);
void patch_fan_mode(string _mode, string _record_uri);
bool patch_ethernet_interface(json::value _jv, string _record_uri, int _flag);
void patch_system(json::value _jv, string _record_uri);
void patch_chassis(json::value _jv, string _record_uri);
void patch_power_control(json::value _jv, string _record_uri);

m_Request treat_uri_cmm_delete(http_request _request, m_Request _msg, json::value _jv);
m_Request remove_account(http_request _request, m_Request _msg, json::value _jv, string _service_uri);
m_Request remove_session(http_request _request, m_Request _msg);//, json::value _jv, string _service_uri);
m_Request remove_role(http_request _request, m_Request _msg, json::value _jv, string _service_uri);

m_Request reply_error(http_request _request, m_Request _msg, json::value _jv, web::http::status_code _status);
json::value get_error_json(string _message);
m_Request reply_success(http_request _request, m_Request _msg, json::value _jv, web::http::status_code _status);


#endif