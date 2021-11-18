#ifndef HANDLER_H
#define HANDLER_H
#include "stdafx.hpp"

#define SERVER_CERTIFICATE_CHAIN_PATH "/conf/ssl/rootca.crt"
#define SERVER_PRIVATE_KEY_PATH "/conf/ssl/rootca.key"
#define SERVER_TMP_DH_PATH "/conf/ssl/dh2048.pem"

#define SERVER_REQUEST_TIMEOUT 10
// #define SERVER_ENTRY_POINT "https://0.0.0.0:443"
#define SERVER_ENTRY_POINT "http://0.0.0.0:443"

class Handler
{
public:
    // Constructor
    Handler(){};
    Handler(utility::string_t _url, http_listener_config _config);

    // Destructor
    ~Handler(){};

    // Server operation
    pplx::task<void> open() { return m_listener.open(); }
    pplx::task<void> close() { return m_listener.close(); }

private:
    http_listener m_listener;
    vector<string> list;

    // Request handler
    void handle_get(http_request _request);
    void handle_put(http_request _request);
    void handle_post(http_request _request);
    void handle_delete(http_request _request);
    void handle_patch(http_request _request);
    void handle_options(http_request _request);
};

// json::value/*pplx::task<http_response>*/ redfish_request_get(string _uri, string _host, int _port);
json::value redfish_request_get(string _path, string _url);
void redfish_request_post(string _path, string _url);//, json::value _jv);
void redfish_request_patch(string _path, string _url);
void redfish_request_delete(string _path, string _url);
// @@@@@@

// ssdp
void *ssdp_handler(void);

// Log
void log_operation(http_request _request);
// reading 관련처리함수랑 event 관련처리함수 2가지로 분기
void reading_operation(http_request _request, http_response &_response);
void event_operation(http_request _request, http_response &_response);

bool check_reading_type(string _types);
bool check_reading_detail(string _type, string _detail);
bool check_reading_time_option(string _option);

#endif