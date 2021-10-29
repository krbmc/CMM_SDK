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

#endif