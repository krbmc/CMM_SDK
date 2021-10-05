#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "stdafx.hpp"
#include "resource.hpp"


void add_new_bmc(string _bmc_id, string _ip, string _port, bool _is_https, string _username, string _password);
void add_system(string _uri, string _host);
void add_manager(string _uri, string _host);
void add_chassis(string _uri, string _host);
// void add_system(string _bmc_id, string _host, string _auth_token);
// void add_manager(string _bmc_id, string _host, string _auth_token);
// void add_chassis(string _bmc_id, string _host, string _auth_token);
// login필요하지 않게 구현해놓음 필요하면 auth_token 헤더에 보내주는작업 필요

// bring류 함수의 _addr인자는 하위 정보들을 가져올때 요청할 주소(bmc주소)가 된다.
void bring_system(Systems *_system, json::value _info, string _addr);
void bring_storages(Collection *_collection, json::value _info, string _addr);
void bring_storage_controllers(List *_list, json::value _info, string _addr);
void bring_drives(Collection *_collection, json::value _info, string _addr);
void bring_volumes(Collection *_collection, json::value _info, string _addr);
void bring_processors(Collection *_collection, json::value _info, string _addr);
void bring_memories(Collection *_collection, json::value _info, string _addr);
void bring_ethernet_interfaces(Collection *_collection, json::value _info, string _addr);
void bring_logservices(Collection *_collection, json::value _info, string _addr);
void bring_logentries(Collection *_collection, json::value _info, string _addr);
void bring_simple_storages(Collection *_collection, json::value _info, string _addr);

void bring_manager(Manager *_manager, json::value _info, string _addr);
void bring_virtual_media(Collection *_collection, json::value _info, string _addr);
void bring_account_service(Manager *_manager, json::value _info, string _addr);


// void add_new_account_service(string _bmc_id);
// void add_new_session_service(string _bmc_id);
// void add_new_event_service(string _bmc_id);
// void add_new_task_service(string _bmc_id);


json::value get_json_info(string _uri, string _host);
bool read_odata_id(json::value _jv, string &_uri);
// json::value get_json_info(string _uri, string _host, string _auth_token);

#endif