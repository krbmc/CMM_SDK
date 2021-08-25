#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "stdafx.hpp"
#include "resource.hpp"


void add_new_bmc(string _bmc_id, string _ip, string _port, bool _is_https, string _username, string _password);
void add_system(string _bmc_id, string _host, string _auth_token);
void add_manager(string _bmc_id, string _host, string _auth_token);
void add_chassis(string _bmc_id, string _host, string _auth_token);
// void add_new_update_service(string _bmc_id);

// void add_new_account_service(string _bmc_id);
// void add_new_session_service(string _bmc_id);
// void add_new_event_service(string _bmc_id);
// void add_new_task_service(string _bmc_id);


json::value get_json_info(string _uri, string _host, string _auth_token);



















#endif