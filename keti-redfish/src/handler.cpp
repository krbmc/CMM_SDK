#include "handler.hpp"
#include "resource.hpp"
#include "task.hpp"
#include "lssdp.hpp"
#include "chassis_controller.hpp"
#include "logservice.hpp"


extern unordered_map<string, Resource *> g_record;
extern unordered_map<uint8_t, Task_Manager *> task_map;
// extern unordered_map<string, unordered_map<string, Task *> > task_map;
extern Value_About_HA ha_value;
extern int heart_beat_cycle;
unsigned int g_count = 0;
//ssdp timer
long long last_time,findlast_time;
long long current_time,findcurrent_time;

/**
 * @brief Handler class constructor with method connection
 * 
 */
Handler::Handler(utility::string_t _url, http_listener_config _config) : m_listener(_url, _config)
{
    // Handler connection
    this->m_listener.support(methods::GET, bind(&Handler::handle_get, this, placeholders::_1));
    this->m_listener.support(methods::PUT, bind(&Handler::handle_put, this, placeholders::_1));
    this->m_listener.support(methods::POST, bind(&Handler::handle_post, this, placeholders::_1));
    this->m_listener.support(methods::DEL, bind(&Handler::handle_delete, this, placeholders::_1));
    this->m_listener.support(methods::PATCH, bind(&Handler::handle_patch, this, placeholders::_1));
    this->m_listener.support(methods::OPTIONS, bind(&Handler::handle_options, this, placeholders::_1));
}

/**
 * @brief GET method request handler
 * 
 * @param _request Request object
 */
void Handler::handle_get(http_request _request)
{
    // 핸들러get, put, delete, put 전부에서 task 하나 만들고 task_map에 올려 정보넣어주고 그런다음에 pplx create때리고
    // 그 pplx task에서 원래하던 요청처리를 한다. 태스크가 끝나면서 태스크 정보수정하고 옮겨주고 task_map 수정

    log(info) << "Request method: GET";
    string uri = _request.request_uri().to_string();
    vector<string> uri_tokens = string_split(uri, '/');
    string filtered_uri = make_path(uri_tokens);
    log(info) << "Reqeust URL : " << filtered_uri;
    log(info) << "Request Body : " << _request.to_string();


    //여기에 pplx시작하면 될듯
    _request
    .extract_json()
    .then([&_request, uri_tokens, uri](pplx::task<json::value> json_value){
        try
        {
            // #오픈시스넷 CORS 처리용
            http_response response;

            response.headers().add("Access-Control-Allow-Origin", "*");
            response.headers().add("Access-Control-Allow-Credentials", "true");
            response.headers().add("Access-Control-Allow-Methods", "POST, GET, OPTIONS, DELETE, PATCH");
            response.headers().add("Access-Control-Max-Age", "3600");
            response.headers().add("Access-Control-Allow-Headers", "Content-Type, Accept, X-Requested-With, remember-me, X-Auth-Token");
            response.headers().add("Access-Control-Expose-Headers", "X-Auth-Token, Location");

            log(info) << "json_value : " << json_value.get().to_string();
            // Response Redfish Version
            if(uri_tokens.size() == 1 && uri_tokens[0] == "redfish")
            {
                json::value j;
                j[U(REDFISH_VERSION)] = json::value::string(U(ODATA_SERVICE_ROOT_ID));
                response.set_status_code(status_codes::OK);
                response.set_body(j);
                _request.reply(response);
                // _request.reply(status_codes::OK, j);
                return ;
            }

            if(uri_tokens[0] == "ppp")
            {
                cout << " RECORD PRINT !!! " << endl;
                record_print();
                _request.reply(status_codes::Found);
                return ;
            }

            // #오픈시스넷 로그 디비 샘플 처리함수
            // /log/reading/CMM/type/detail/time
            // if(uri_tokens[0] == "log")
            // {
            //     if(uri_tokens.size() < 6)
            //     {
            //         response.set_status_code(status_codes::BadRequest);
            //         _request.reply(response);
            //         // _request.reply(status_codes::BadRequest);
            //         return ;
            //     }

            //     json::value open_result;

            //     if(uri_tokens[1] == "reading" && uri_tokens[2] == "CMM1")
            //     {
            //         if(uri_tokens[3] == "power")
            //         {
            //             if(uri_tokens[4] == "powercontrol")
            //             {
            //                 if(uri_tokens[5] == "hour")
            //                     open_result = opensys_powercontrol_hour();
            //                 else if(uri_tokens[5] == "min")
            //                     open_result = opensys_powercontrol_min();
            //             }
            //             else if(uri_tokens[4] == "powervoltage")
            //             {
            //                 if(uri_tokens[5] == "hour")
            //                     open_result = opensys_powervoltage_hour();
            //                 else if(uri_tokens[5] == "min")
            //                     open_result = opensys_powervoltage_min();
            //             }
            //             else if(uri_tokens[4] == "powersupply")
            //             {
            //                 if(uri_tokens[5] == "hour")
            //                     open_result = opensys_powersupply_hour();
            //                 else if(uri_tokens[5] == "min")
            //                     open_result = opensys_powersupply_min();
            //             }

            //         }
            //         else if(uri_tokens[3] == "thermal")
            //         {
            //             if(uri_tokens[4] == "temperature")
            //             {
            //                 if(uri_tokens[5] == "hour")
            //                     open_result = opensys_thermal_hour();
            //                 else if(uri_tokens[5] == "min")
            //                     open_result = opensys_thermal_min();
            //             }
            //             else if(uri_tokens[4] == "fan")
            //             {
            //                 if(uri_tokens[5] == "hour")
            //                     open_result = opensys_fan_hour();
            //                 else if(uri_tokens[5] == "min")
            //                     open_result = opensys_fan_min();
            //             }

            //         }

            //         response.set_status_code(status_codes::OK);
            //         response.set_body(open_result);
            //         _request.reply(response);
            //         // _request.reply(status_codes::OK, open_result);
            //         return ;
            //     }
            //     else
            //     {
            //         response.set_status_code(status_codes::BadRequest);
            //         _request.reply(response);
            //         // _request.reply(status_codes::BadRequest);
            //         return ;
            //     }

            // }

            if(uri_tokens[0] == "log")
            {
                // /log/~ uri를 처리하는 함수
                log_operation(_request);
                return ;
            }

            // #오픈시스넷 /CMMHA 임시 처리부
            if(uri_tokens[0] == "CMMHA")
            {
                json::value j;
                j["ActiveIP"] = json::value::string("192.168.0.68");
                j["ActivePort"] = json::value::string("8000");
                j["StandbyIP"] = json::value::string("10.0.6.106");
                j["StandbyPort"] = json::value::string("8000");
                j["NetworkTimeOut"] = json::value::number(50);
                j["HeartbeatInterval"] = json::value::number(5);
                j["SwitchTimeOut"] = json::value::number(10);
                j["IPList"] = json::value::array();
                j["IPList"][0] = json::value::string("192.168.0.68");
                j["IPList"][1] = json::value::string("10.0.6.106");

                response.set_status_code(status_codes::OK);
                response.set_body(j);
                _request.reply(response);
                // _request.reply(status_codes::OK, j);
                return ;
                
            }
            // #오픈시스넷 /HAlastcommand 임시 처리부
            if(uri_tokens[0] == "HAlastcommand")
            {
                json::value j;
                j["CMS"] = json::value::array();
                j["SMS"] = json::value::array();
                json::value obj;
                obj["CMID"]= json::value::string("CM1");
                obj["Value"]=json::value::string("/redfish/v1/Chassis");
                obj["Time"]=json::value::string("2021-11-15 11:00");
                j["CMS"][0]=obj;
                json::value obj2;
                obj2["CMID"]= json::value::string("CM2");
                obj2["Value"]=json::value::string("/redfish/v1/Chassis");
                obj2["Time"]=json::value::string("2021-11-15 11:00");
                j["CMS"][1]=obj2;
                json::value smobj1;
                smobj1["CMID"]= json::value::string("SM1");
                smobj1["Value"]=json::value::string("/redfish/v1/Chassis");
                smobj1["Time"]=json::value::string("2021-11-15 11:00");
                j["SMS"][0]=smobj1;
                json::value smobj2;
                smobj1["CMID"]= json::value::string("SM2");
                smobj1["Value"]=json::value::string("/redfish/v1/Chassis");
                smobj1["Time"]=json::value::string("2021-11-15 11:00");
                j["SMS"][1]=smobj1;


                response.set_status_code(status_codes::OK);
                response.set_body(j);
                _request.reply(response);
                // _request.reply(status_codes::OK, j);
                return ;
                
            }


            if(uri_tokens[0] == "myuri")
            {
                string host, path, query, fragment;
                int port;
                host = _request.request_uri().host();
                port = _request.request_uri().port();
                path = _request.request_uri().path();
                query = _request.request_uri().query();
                fragment = _request.request_uri().fragment();
                cout << "PRINT >> " << endl;
                cout << "Host : " << host << endl;
                cout << "Port : " << port << endl;
                cout << "Path : " << path << endl;
                cout << "Query : " << query << endl;
                cout << "Fragment : " << fragment << endl;
                map<string, string> mm;
                mm = uri::split_query(query);
                map<string, string>::iterator iter;
                cout << "Split -------------" << endl;
                for(iter = mm.begin(); iter != mm.end(); iter++)
                {
                    cout << iter->first << " / " << iter->second << endl;
                }

                _request.reply(status_codes::Locked);
                return ;

            }

            if(uri == "/reading/temp")
            {
                json::value rep;
                // rep = select_all_reading("Temperature");
                _request.reply(status_codes::OK, rep);
                return ;
            }
            if(uri == "/reading/fan")
            {
                json::value rep;
                // rep = select_all_reading("Fan");
                _request.reply(status_codes::OK, rep);
                return ;
            }
            

            if(uri_tokens.size() == 1 && uri_tokens[0] == "upup")
            {
                json::value j;
                j["Message"] = json::value::string("This is Original File");
                
                _request.reply(status_codes::OK, j);
                return ;
            }

            if(uri_tokens.size() == 1 && uri_tokens[0] == "Map")
            {
                json::value j;
                j = get_json_task_map();
                _request.reply(status_codes::Found, j);
                return ;
            }

            if(uri_tokens.size() == 1 && uri_tokens[0] == "Boom")
            {
                // json::value j;
                create_task_map_from_json(get_json_task_map());
                // create_task_map_from_json(json::value::null());
                _request.reply(status_codes::Found);
                return ;
            }
            
            // if(uri_tokens.size() == 1 && uri_tokens[0] == "ViewLog")
            if(uri == "/redfish/v1/Systems/1/ViewLog")
            {
                json::value j_res = json::value::array();
                int index=0;
                string odata = "/redfish/v1/Systems/1/LogServices/Log1";
                LogService *service = (LogService *)g_record[odata];
                vector<Resource *>::iterator iter;
                for(iter = service->entry->members.begin(); iter!=service->entry->members.end(); iter++, index++)
                {
                    LogEntry *tmp;
                    tmp = (LogEntry *)*iter;
                    cout << tmp->get_json() << endl;
                    json::value j_tmp = tmp->get_json();
                    j_tmp.erase("type");
                    j_res[index] = j_tmp;
                }
                cout << index << endl;
                _request.reply(status_codes::OK, j_res);
                return ;
        
            }
            // if(uri_tokens.size() == 1 && uri_tokens[0] == "GetTemp")
            if(uri == "/redfish/v1/Managers/1/GetTemp")
            {
                json::value j_res = json::value::array();
                int index=0;
                string odata = "/redfish/v1/Managers/1/LogServices/Log1";
                LogService *service = (LogService *)g_record[odata];
                vector<Resource *>::iterator iter;
                for(iter = service->entry->members.begin(); iter!=service->entry->members.end(); iter++, index++)
                {
                    LogEntry *tmp;
                    tmp = (LogEntry *)*iter;
                    cout << tmp->get_json() << endl;
                    json::value j_tmp = tmp->get_json();
                    j_tmp.erase("type");
                    j_res[index] = j_tmp;
                }
                _request.reply(status_codes::OK, j_res);
                return ;

            }

            if(uri_tokens.size() == 2 || uri_tokens.size() == 3)
            {
                do_task_cmm_get(_request);
                return ;
            }

            if(uri_tokens.size() >= 4)
            {
                if(uri_tokens[2] == "AccountService" || uri_tokens[2] == "SessionService" || uri_tokens[2] == "TaskService"
                || uri_tokens[2] == "CertificateService" || uri_tokens[2] == "EventService" || uri_tokens[2] == "UpdateService")
                {
                    //CMM자체 동작으로다가 처리하시면됨
                    do_task_cmm_get(_request);
                }
                else
                {
                    if(uri_tokens[3] == CMM_ID)
                    {
                        //CMM 자체처리
                        do_task_cmm_get(_request);
                    }
                    else //if(uri_tokens[3] == "2") // 여기 나중에 분간하는거 처리 해줘야할듯
                    {
                        if(module_id_table.find(uri_tokens[3]) != module_id_table.end())
                        // 일단 104번에서 처리
                            do_task_bmc_get(_request);
                        else
                        {
                            json::value j;
                            j[U("Error")] = json::value::string(U("Unvalid Module ID"));
                            response.set_status_code(status_codes::BadRequest);
                            response.set_body(j);
                            _request.reply(response);
                            // _request.reply(status_codes::BadRequest, j);
                            cout << "Unvalid BMC_id" << endl;
                            return ;
                        }
                    }
                }
            }
            else
            {
                // uri_token size가 1/2,3/4이상에도 해당안되는거면 에러
                response.set_status_code(status_codes::BadRequest);
                _request.reply(response);
                // _request.reply(status_codes::BadRequest);
                return ;
            }

        }
        catch(json::json_exception& e)
        {
            http_response response;

            response.headers().add("Access-Control-Allow-Origin", "*");
            response.headers().add("Access-Control-Allow-Credentials", "true");
            response.headers().add("Access-Control-Allow-Methods", "POST, GET, OPTIONS, DELETE, PATCH");
            response.headers().add("Access-Control-Max-Age", "3600");
            response.headers().add("Access-Control-Allow-Headers", "Content-Type, Accept, X-Requested-With, remember-me, X-Auth-Token");
            response.headers().add("Access-Control-Expose-Headers", "X-Auth-Token, Location");

            response.set_status_code(status_codes::BadRequest);
            _request.reply(response);
            // _request.reply(status_codes::BadRequest);
            return ;
        }
    })
    .wait();

    // try
    // {
    //     // Response Redfish Version
    //     if(uri_tokens.size() == 1 && uri_tokens[0] == "redfish")
    //     {
    //         json::value j;
    //         j[U(REDFISH_VERSION)] = json::value::string(U(ODATA_SERVICE_ROOT_ID));
    //         _request.reply(status_codes::OK, j);
    //         return ;
    //     }

    //     if(uri_tokens.size() == 1 && uri_tokens[0] == "Map")
    //     {
    //         json::value j;
    //         j = get_json_task_map();
    //         _request.reply(status_codes::Found, j);
    //         return ;
    //     }

    //     if(uri_tokens.size() == 1 && uri_tokens[0] == "Boom")
    //     {
    //         // json::value j;
    //         create_task_map_from_json(get_json_task_map());
    //         // create_task_map_from_json(json::value::null());
    //         _request.reply(status_codes::Found);
    //         return ;
    //     }

    //     if(uri_tokens.size() == 2 || uri_tokens.size() == 3)
    //     {
    //         do_task_cmm_get(_request);
    //         return ;
    //     }

    //     if(uri_tokens.size() >= 4)
    //     {
    //         if(uri_tokens[2] == "AccountService" || uri_tokens[2] == "SessionService" || uri_tokens[2] == "TaskService"
    //         || uri_tokens[2] == "CertificateService" || uri_tokens[2] == "EventService" || uri_tokens[2] == "UpdateService")
    //         {
    //             //CMM자체 동작으로다가 처리하시면됨
    //             do_task_cmm_get(_request);
    //         }
    //         else
    //         {
    //             if(uri_tokens[3] == CMM_ID)
    //             {
    //                 //CMM 자체처리
    //                 do_task_cmm_get(_request);
    //             }
    //             else //if(uri_tokens[3] == "2") // 여기 나중에 분간하는거 처리 해줘야할듯
    //             {
    //                 if(module_id_table.find(uri_tokens[3]) != module_id_table.end())
    //                 // 일단 104번에서 처리
    //                     do_task_bmc_get(_request);
    //                 else
    //                 {
    //                     json::value j;
    //                     j[U("Error")] = json::value::string(U("Unvalid Module ID"));
    //                     _request.reply(status_codes::BadRequest, j);
    //                     cout << "Unvalid BMC_id" << endl;
    //                     return ;
    //                 }
    //             }
    //         }
    //     }
    //     else
    //     {
    //         // uri_token size가 1/2,3/4이상에도 해당안되는거면 에러
    //         _request.reply(status_codes::BadRequest);
    //         return ;
    //     }

    // }
    // catch(json::json_exception& e)
    // {
    //     _request.reply(status_codes::BadRequest);
    // }
    

    //여기서 create해서 do_task할듯
    // do_task_get(_request);
    
    // string my_uri = _request.request_uri().to_string();
    // json::value my_j = _request.extract_json().get();
    // if(my_uri == "/redfish")//my_uri == ODATA_ACCOUNT_ID || my_uri == ODATA_ACCOUNT_SERVICE_ID)
    // {
        
    //     Task_Manager *t_manager = new Task_Manager();
    //     t_manager->task_type = TASK_TYPE_ACCOUNTSERVICE;
    //     if(task_map.find(TASK_TYPE_ACCOUNTSERVICE) == task_map.end()) // 해당 타입의 task_manager없음
    //     {
    //         task_map.insert(make_pair(TASK_TYPE_ACCOUNTSERVICE, t_manager));
    //     }

    //     m_Request msg;
    //     msg.sender = _request;
    //     msg.request_json = my_j;
    //     msg.method = "GET";
    //     msg.host = "http://10.0.6.104:443";
    //     msg.uri = my_uri;
    //     task_map[TASK_TYPE_ACCOUNTSERVICE]->list_request.push_back(msg);
    //     cout << "TEST @@@@ @ @@@@@@@ @@ @@@@@ " << endl;
    //     cout << "host : " << task_map[TASK_TYPE_ACCOUNTSERVICE]->list_request.back().host << endl;
    //     cout << "method : " << task_map[TASK_TYPE_ACCOUNTSERVICE]->list_request.back().method << endl;
    //     cout << "uri : " << task_map[TASK_TYPE_ACCOUNTSERVICE]->list_request.back().uri << endl;

    // }

    // do_task(_request);
    // cout << _request.headers().

    // do_task();
    // string task_id = make_task
    // Task *task = new Task()

    // // 여기부터 원래 handle_get 부분
    // log(info) << "Request method: GET";
    // string uri = _request.request_uri().to_string();
    // vector<string> uri_tokens = string_split(uri, '/');
    // string filtered_uri = make_path(uri_tokens);

    // json::value j = _request.extract_json().get();
    // // cout << "JSON VALUE : " << j << endl;
    // // cout << "RAW URI : " << uri << endl;

    // // web::http::http_headers::iterator iter;
    // // for(iter = _request.headers().begin(); iter != _request.headers().end(); iter++)
    // //     cout << "HEAD! : " << iter->first << " / " << iter->second << endl;

    // string task_odata = ODATA_TASK_ID;
    // task_odata = task_odata + "/get1";
    // Task *task = new Task(task_odata, "Get Task");
    // task->set_payload(_request.headers(), "GET", j, uri);
    // record_save_json();
    // // get할때 task만드는 부분

    // log(info) << "Reqeust URL : " << filtered_uri;
    // log(info) << "Request Body : " << _request.to_string();
    // // cout << "here11" << endl;// @@@@@
    // record_print();

    // try
    // {
    //     // cout << "here12" << endl; // @@@@@
    //     // Response redfish version
    //     if (uri_tokens.size() == 1 && uri_tokens[0] == "redfish")
    //     {
    //         j[REDFISH_VERSION] = json::value::string(U(ODATA_SERVICE_ROOT_ID));
    //         _request.reply(status_codes::OK, j);
    //         return;
    //     }

    //     // Check X-Auth-Token feild of request
    //     else if (!_request.headers().has("X-Auth-Token"))
    //     {
    //         // cout << "here13" << endl;// @@@@@
    //         _request.reply(status_codes::NetworkAuthenticationRequired);
    //         return;
    //     }

    //     // Check session is valid
    //     else if (!is_session_valid(_request.headers()["X-Auth-Token"]))
    //     {
    //         // cout << "here14" << endl; // 
    //         _request.reply(status_codes::Unauthorized);
    //         return;
    //     }

    //     // Check resource is exist
    //     else if (!record_is_exist(filtered_uri)) 
    //     {
    //         // cout << "here15" << endl;// @@@@@
    //         _request.reply(status_codes::NotFound);
    //         return;
    //     }


    //     // Response json type of resource to client
    //     // cout << "here16" << endl;// @@@@@
    //     _request.reply(status_codes::OK, record_get_json(filtered_uri));
    // }
    // catch (json::json_exception &e)
    // {
    //     _request.reply(status_codes::BadRequest);
    // }

    // // cout << "here" << endl;// @@@@@
    // g_count++;
    // log(info) << g_count;
    // // cout << "there" << endl;// @@@@@
    
    // // 여기까지
}

/**
 * @brief DELETE request handler
 * 
 * @param _request Request object
 */
void Handler::handle_delete(http_request _request)
{
    /**
     * @brief ODATA_ACCOUNT_ID 로 계정삭제
     * @authors 강
     * @details UserName 이랑 Password 둘다 일치하는 녀석을 지움
     * 
     * // record(ODATA_ACCOUNT_ID+/username) 을 지웠고 AccountService의 collection에서 지웠음
     */
    string uri = _request.request_uri().to_string();
    vector<string> uri_tokens = string_split(uri, '/');
    string filtered_uri = make_path(uri_tokens);
    // json::value d = _request.extract_json().get();
    // string user_name;
    // string password;
    // string odata_id;
    // Account *account;
    // AccountService *acc_service;

    log(info) << "Request method: DELETE";
    log(info) << "Reqeust uri : " << filtered_uri;

    http_response response;

    response.headers().add("Access-Control-Allow-Origin", "*");
    response.headers().add("Access-Control-Allow-Credentials", "true");
    response.headers().add("Access-Control-Allow-Methods", "POST, GET, OPTIONS, DELETE, PATCH");
    response.headers().add("Access-Control-Max-Age", "3600");
    response.headers().add("Access-Control-Allow-Headers", "Content-Type, Accept, X-Requested-With, remember-me, X-Auth-Token");
    response.headers().add("Access-Control-Expose-Headers", "X-Auth-Token, Location");


    try
    {
        if(uri_tokens.size() == 2 || uri_tokens.size() == 3)
        {
            do_task_cmm_delete(_request);
            return ;
        }

        if(uri_tokens.size() >= 4)
        {
            if(uri_tokens[2] == "AccountService" || uri_tokens[2] == "SessionService" || uri_tokens[2] == "TaskService"
            || uri_tokens[2] == "CertificateService" || uri_tokens[2] == "EventService" || uri_tokens[2] == "UpdateService")
            {
                //CMM자체 동작으로다가 처리하시면됨
                do_task_cmm_delete(_request);
            }
            else
            {
                if(uri_tokens[3] == CMM_ID)
                {
                    //CMM 자체처리
                    do_task_cmm_delete(_request);
                }
                else //if(uri_tokens[3] == "2") // 여기 나중에 분간하는거 처리 해줘야할듯
                {
                    if(module_id_table.find(uri_tokens[3]) != module_id_table.end())
                    // 일단 104번에서 처리
                        do_task_bmc_delete(_request);
                    else
                    {
                        json::value j;
                        j[U("Error")] = json::value::string(U("Unvalid Module ID"));
                        response.set_status_code(status_codes::BadRequest);
                        response.set_body(j);
                        _request.reply(response);
                        // _request.reply(status_codes::BadRequest, j);
                        cout << "Unvalid BMC_id" << endl;
                        return ;
                    }
                }
            }
        }
        else
        {
            // uri_token size가 1/2,3/4이상에도 해당안되는거면 에러
            response.set_status_code(status_codes::BadRequest);
            _request.reply(response);
            // _request.reply(status_codes::BadRequest);
            return ;
        }



// No task화 -----------------------------------------------------------------------

//         if(filtered_uri == ODATA_ACCOUNT_ID)
//         {
//             user_name = d.at("UserName").as_string();
//             password = d.at("Password").as_string();
//             odata_id = ODATA_ACCOUNT_ID;
// //             odata_id = odata_id + '/' + user_name;
// // cout << "BOOM 0" << endl;
// //             // Check account exist
// //             if (!record_is_exist(odata_id))
// //             {
// //                 _request.reply(status_codes::BadRequest);
// //                 return;
// //             }
// // cout << "BOOM 1" << endl;
// //             account = (Account *)g_record[odata_id];
// //             // Check password correct
// //             if (account->password != password)
// //             {
// //                 _request.reply(status_codes::BadRequest);
// //                 return;
// //             }
// // cout << "BOOM 2" << endl;

//             // string acc_id = "";
//             // g_record.erase(odata_id);

//             bool exist = false;
//             acc_service = (AccountService *)g_record[ODATA_ACCOUNT_SERVICE_ID];
//             //cout << "ACC : " << typeid(acc_service).name() << endl;
//             vector<Resource *> tmp;
//             tmp = acc_service->account_collection->members;
//             //cout << "TMP : " << typeid(tmp).name() << endl;
//             std::vector<Resource *>::iterator iter;
//             for(iter = acc_service->account_collection->members.begin(); iter != acc_service->account_collection->members.end(); iter++)
//             // for(iter = tmp.begin(); iter != tmp.end(); iter++)
//             {
//                 Account *t = (Account *)*iter;
//                 //cout << typeid(t).name() << endl;
//                 //cout << t->user_name << endl;
//                 if(t->user_name == user_name && t->password == password)
//                 {
//                     odata_id = t->odata.id;
//                     exist = true;
//                     acc_service->account_collection->members.erase(iter);
//                     _request.reply(status_codes::Gone, record_get_json(ODATA_ACCOUNT_ID));
//                     // status 임시로 Gone갖다씀
//                     break;
//                 }

//             }

//             if(exist == false)
//             {
//                 json::value mm;
//                 mm[U("Error")] = json::value::string(U("No Account or Does not match Password"));
//                 _request.reply(status_codes::BadRequest, mm);
//                 return ;
//             }
//             g_record.erase(odata_id);
            
//             record_save_json();
//             string delete_path = odata_id + ".json"},";
//             // string delete_path = filtered_uri + '/' + user_name + ".json"},";
//             // cout << "dd2 : " << delete_path2 << endl;
//             if(remove(delete_path.c_str()) < 0)
//             {
//                 cout << "delete error" << endl;
//             }
//             // else
//             // cout << "HOO~" << endl;
//             // 해당 계정에 대한 json파일 삭제
            

//             // Collection *account_col = (AccountService *)g_record[ODATA_ACCOUNT_SERVICE_ID]->account_collection;


//         }
//         /**
//          * @brief AccountService/Accounts 로 토큰과함께 들어왔을 때 logout(세션종료)
//          * @authors 강
//          * @TODO : 세션종료시 레코드 갱신 작업하고 해당세션json파일 삭제 필요
//          * @TODO : 현재 Accounts에 GET에서 처리하고있는데 Sessions에 DELETE로 옮길것
//         */
        // else if(filtered_uri == ODATA_SESSION_ID)
        // {
        //     if(is_session_valid(_request.headers()["X-Auth-Token"]))
        //     {
        //         // 토큰에 해당하는 세션 종료
        //         string token = _request.headers()["X-Auth-Token"];
        //         string session_uri = ODATA_SESSION_ID;
        //         session_uri = session_uri + '/' + token;
        //         Session *session;

        //         session = (Session *)g_record[session_uri];
        //         // cout << "YYYY : " << session->id << endl;
        //         session->_remain_expires_time = 1;
        //         // 여기서는 남은시간 1로만 만들고 삭제작업은 session pplx then에서

        //         // SessionService * ses_service = (SessionService *)g_record[ODATA_SESSION_SERVICE_ID];
        //         // std::vector<Resource *>::iterator iter;
        //         // for(iter = ses_service->session_collection->members.begin(); iter != ses_service->session_collection->members.end(); iter++)
        //         // {
        //         //     Session *t = (Session *)*iter;
        //         //     if(t->id == token)
        //         //     {
        //         //         session = (Session *)*iter;
        //         //         // session->cts.cancel();
        //         //         cout << "YYYY : " << session->id << endl;
        //         //         session->_remain_expires_time = 1;
        //         //         ses_service->session_collection->members.erase(iter);
        //         //         // 이 작업을 해야 record_get_json(ODATA_SESSION_ID))때 안나옴
        //         //         break;
        //         //     }
        //         // }
        //         // 세션서비스->컬렉션->멤버스 돌면서 해당하는 세션의 남은시간 1로 만들어서 종료되게끔 함
                
        //         // cout << "Session : " << session_uri << endl;
        //         // cout << record_get_json(ODATA_SESSION_ID) << endl;
        //         // cout << "AAAAAAA" << endl;
        //         // 토큰 붙인거로 Session들어가고 세션에 취소토큰만들어서 취소줘야겠다 여기서

        //         //Session *session = (Session *)g_record[ODATA_SESSION_ID];
        //         // cout << record_get_json(session_uri) << endl;
        //         // delete session;
        //         // 리플라이
        //     }
        // }
// No task화 -----------------------------------------------------------------------

    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        response.set_status_code(status_codes::BadRequest);
        _request.reply(response);
        // _request.reply(status_codes::BadRequest);
    }
    // catch(json::json_exception &e)
    // {
    //     _request.reply(status_codes::BadRequest);
    // }

    // @@@@@@ 여기까지

    cout << "handle_delete request" << endl;

    // auto j = _request.extract_json().get();

    // _request.reply(status_codes::NotImplemented, U("DELETE Request Response"));
}

/**
 * @brief PUT request handler
 * 
 * @param _request Request object
 */
void Handler::handle_put(http_request _request)
{

    // log(info) << "Request method: PATCH";
    // string uri = _request.request_uri().to_string();
    // vector<string> uri_tokens = string_split(uri, '/');
    // string filtered_uri = make_path(uri_tokens);
    // log(info) << "Reqeust URL : " << filtered_uri;
    // log(info) << "Request Body : " << _request.to_string();

    // try
    // {
    //     /* code */
    //     // if(uri_tokens.size() )
    // }
    // catch(const std::exception& e)
    // {
    //     std::cerr << e.what() << '\n';
    // }
    

    cout << "handle_put request" << endl;

    auto j = _request.extract_json().get();

    _request.reply(status_codes::NotImplemented, U("PUT Request Response"));
}

// #include <fstream>
// #include <cpprest/filestream.h>
/**
 * @brief POST request handler
 * 
 * @param _request Request object
 */
//using namespace concurrency::streams;
void Handler::handle_post(http_request _request)
{

    log(info) << "Request method: POST";
    string uri = _request.request_uri().to_string();
    vector<string> uri_tokens = string_split(uri, '/');
    string filtered_uri = make_path(uri_tokens);
    log(info) << "Reqeust URL : " << filtered_uri;
    log(info) << "Request Body : " << _request.to_string();

    http_response response;

    response.headers().add("Access-Control-Allow-Origin", "*");
    response.headers().add("Access-Control-Allow-Credentials", "true");
    response.headers().add("Access-Control-Allow-Methods", "POST, GET, OPTIONS, DELETE, PATCH");
    response.headers().add("Access-Control-Max-Age", "3600");
    response.headers().add("Access-Control-Allow-Headers", "Content-Type, Accept, X-Requested-With, remember-me, X-Auth-Token");
    response.headers().add("Access-Control-Expose-Headers", "X-Auth-Token, Location");

    try
    {
        // ftft
        if(uri_tokens.size() == 1 && uri_tokens[0] == "ftft")
        {
            
            // auto fileStream = std::make_shared<Concurrency::streams::ostream >();
            // //저장할 파일 이름 생성
            //pplx::task<void> fileStream=Concurrency::streams::fstream::open_ostream("filename", std::ios_base::in | std::ios_base::out |std::ios::binary).get();
            string filepath="filesavetest.txt";
            // string header = _request.headers()["Content-Type"];
            // cout << "HEADER : " << header << endl;
            // string boundary = header.substr(header.find("boundary=")+9, header.find("\r")-(header.find("boundary=")+9));
            // cout << "BOUNDARY : " << boundary << endl;

            auto bodyStream = _request.body();
            auto fileStream = concurrency::streams::fstream::open_ostream(utility::conversions::to_string_t(filepath), std::ios::out | std::ios::binary).get();
            fileStream.flush();
            // cout << "ASDFASDF : " << endl << bodyStream.extract().get() << endl;
            // auto my_stream = concurrency::streams::streambuf()
            bodyStream.read_to_end(fileStream.streambuf()).wait();
            fileStream.close().wait();
            // //request 생성w
            // .then([=](ostream outFile)){
            //     *fileStream=outFile;

            // }
            // ofstream outfile;
            // const auto data = _request.content_ready().get().extract_vector().get();
            // utility::string_t body = {data.begin(), data.end()};
            // std::istringstream iss(body);
            // bool startCopying = false;
            // std::string line;
            // cout<<"start"<<endl;
            // while (std::getline(iss, line))
            // {
            //     if (startCopying)
            //     {
            //         outfile << line;
            //     }
            //     else if (line == "\r")
            //     {
            //         startCopying = true;
            //         outfile.open("test.img");
            //     }
            // }
            // outfile.close();
            // cout<<"end outfile"<<endl;
            // pplx::task<void> requestTask = ostream

            // }
            // *fileStrema=&_request.body().streambuf();
            




            // cout << "##################  start" << endl;
            // // cout << _request.body().streambuf() << endl;
            // auto data = _request.body().streambuf();
            
            
            // // auto data = _request.content_ready().get().extract_vector().get();
            // string pp = {data.begin(), data.end()};
            // cout << "!@#$" << endl;
            // cout << pp << endl;
            // cout << "!@#$" << endl;

            // istringstream iss(pp);
            // string header = _request.headers()["Content-Type"];
            // cout << "HEADER : " << header << endl;
            // // cout << header.find("boundary=")+9 << " / " << header.find("\r")<< endl;
            // // cout << " !!@!$!@$!@$!$! " << endl;
            // string boundary = header.substr(header.find("boundary=")+9);
            // // cout << "leng : " << boundary.length();
            // // string boundary = header.substr(header.find("boundary=")+9, header.find("\r")-(header.find("boundary=")+9));
            // cout << "BOUNDARY : " << boundary << endl;

            // string line;
            // string pre_line = ""; // 이전라인
            // int i=0;
            // bool write_on = false;
            // ofstream my_out("/root/test.png", ios::binary);
            // // ofstream my_out("/root/mymy.txt");
            // json::value jvjv;
            // // while(getline(iss, line))
            // // {
            // //     cout << "<< " << i << " >> ";
            // //     // cout << line << endl;
            // //     cout << line.find("\r") << endl;
            // //     cout << line << " / " << line.length() << endl;
            // //     string new_line;

            // //     cout << i << " line start" << endl;
            // //     for(int n=0; n<line.size(); n++)
            // //     {
            // //         if(line[n] == '\r')
            // //             cout << " ";
            // //         else
            // //             cout << line[n];
            // //     }
            // //     cout << i << " line end" << endl;

            // //     // jvjv[to_string(i)] = json::value::string(line);
            // //     if(line.length() == 1 && line == "\r")
            // //         new_line = "\n";
            // //     else
            // //     {
            // //         if(line[line.length()-1] == '\r')
            // //         {
            // //             new_line = line.substr(0, line.rfind("\r"));
            // //             new_line = new_line + "\n";
            // //         }
                        
            // //     }
            // //     cout << new_line << " / " << new_line.length()<< endl;
            // //     // new_line = new_line + "\n";

            // //     if(line.find(boundary) != string::npos)
            // //     {
            // //         cout << "Line " << i << " boundary exist" << endl;
            // //     }

            // //     if(write_on)
            // //     {
            // //         if(pre_line == ""){
            // //             pre_line = new_line;
            // //             i++;
            // //             continue;
            // //         }

            // //         if(line.find(boundary) != string::npos)
            // //         {
            // //             write_on = false;
            // //             pre_line = pre_line.substr(0, pre_line.length()-1);
            // //             my_out << pre_line;
            // //             pre_line = "";
            // //             continue;
            // //         }
                        
            // //         // my_out << new_line;
            // //         my_out << pre_line;
            // //         cout << "PRE : " << pre_line << endl;
            // //         pre_line = new_line;
                    
            // //     }

            // //     if(line == "\r")
            // //     {
            // //         cout << "Enter!" << endl;
            // //         write_on = true;
            // //     }
            // //     i++;
                
            // // }
            // my_out.close();
            // // cout << _request.request_body() << endl;
            // // cout << "JV ON!" << endl;
            // // cout << jvjv << endl;
            
            // cout << "####################### end" << endl;
            // // 헤더에서 바운더리값 추출하고 라인돌면서 name값으로 무엇인지 파악후 2줄뒤부터 내용컨텐츠읽어서 
            // // 추출하거나 처리 어디까지 읽냐 바운더리또나올때까지 그렇게 해서 사용해보자 파일저장을 해보자 ㅇㅇ

            _request.reply(status_codes::OK);
            return ;
        }

        // /Heartbeat 
        if(uri_tokens.size() == 1 && uri_tokens[0] == "Heartbeat")
        {
            json::value j;
            json::value j_cycle;
            j_cycle = _request.extract_json().get();
            if(j_cycle.as_object().find("Cycle") == j_cycle.as_object().end())
            {
                response.set_status_code(status_codes::BadRequest);
                _request.reply(response);
                // _request.reply(status_codes::BadRequest);
                return ;
            }

            try
            {
                /* code */
                heart_beat_cycle = j_cycle.at("Cycle").as_integer();
                // cout << heart_beat_cycle << endl;
            }
            catch(const std::exception& e)
            {
                json::value rep;
                rep[U("Error")] = json::value::string(U("Not a number"));
                std::cerr << e.what() << '\n';
                response.set_status_code(status_codes::BadRequest);
                response.set_body(rep);
                _request.reply(response);
                // _request.reply(status_codes::BadRequest, rep);
                return ;
            }
            
            

            j[U("Status")] = json::value::string("OK");
            response.set_status_code(status_codes::OK);
            response.set_body(j);
            _request.reply(response);
            // _request.reply(status_codes::OK, j);
            return ;
        }

        // /CMM_HA
        if(uri_tokens.size() == 1 && uri_tokens[0] == "CMM_HA")
        {
            json::value ha_info;
            ha_info = _request.extract_json().get();
            if(ha_info.as_object().find("PeerPrimaryAddress") == ha_info.as_object().end() || ha_info.as_object().find("PrimaryPort") == ha_info.as_object().end()
            || ha_info.as_object().find("PeerSecondaryAddress") == ha_info.as_object().end() || ha_info.as_object().find("SecondPort") == ha_info.as_object().end()
            || ha_info.as_object().find("Origin") == ha_info.as_object().end() || ha_info.as_object().find("Enabled") == ha_info.as_object().end())
            {
                response.set_status_code(status_codes::BadRequest);
                _request.reply(response);
                // _request.reply(status_codes::BadRequest);
                return ;
            }
            // Request Body에 구성 6가지 중 하나라도 누락되었을 경우 무시

            try
            {
                /* code */
                ha_value.peer_primary_address = ha_info.at("PeerPrimaryAddress").as_string();
                ha_value.primary_port = ha_info.at("PrimaryPort").as_integer();
                ha_value.peer_secondary_address = ha_info.at("PeerSecondaryAddress").as_string();
                ha_value.second_port = ha_info.at("SecondPort").as_integer();
                ha_value.origin = ha_info.at("Origin").as_bool();
                ha_value.enabled = ha_info.at("Enabled").as_bool();
            }
            catch(const std::exception& e)
            {
                json::value rep;
                rep[U("Error")] = json::value::string(U("Type Error"));
                std::cerr << e.what() << '\n';
                response.set_status_code(status_codes::BadRequest);
                response.set_body(rep);
                _request.reply(response);
                // _request.reply(status_codes::BadRequest, rep);
                return ;
                // std::cerr << e.what() << '\n';
            }

            // origin을 true로 받은녀석이 peerprimaryaddress에 해당 false받은녀석은 secondary에 해당
            // --> true이면 secondary address로 , false면 primary address로 쏘기
            string op1, op2, ip;
            if(ha_value.origin == true) // peerprimaryaddress
                ip = ha_value.peer_secondary_address;
            else
                ip = ha_value.peer_primary_address;
                
            op1 = "ssh-keygen -f \"/root/.ssh/known_hosts\" -R \"" + ip + "\"";
            op2 = "ssh-keyscan -t rsa " + ip + " >>~/.ssh/known_hosts";

            system(op1.c_str());
            system(op2.c_str());

            response.set_status_code(status_codes::OK);
            _request.reply(response);
            // _request.reply(status_codes::OK);
            return ;

        }


        // /HAswtich
        if(uri_tokens.size() == 1 && uri_tokens[0] == "HAswitch")
        {
            json::value switch_info;
            switch_info = _request.extract_json().get();
            if(switch_info.as_object().find("IsSwitch") == switch_info.as_object().end())
            {
                response.set_status_code(status_codes::BadRequest);
                _request.reply(response);
                // _request.reply(status_codes::BadRequest);
                return ;
            }

            string is_switch = switch_info.at("IsSwitch").as_string();
            if(is_switch == "Active"){
                if(ha_value.origin == true)
                    ha_value.enabled = true;
                else
                    ha_value.enabled = false;
                // 그러고 스위치할때 필요한 작업들 하겄지 ㅇㅇ
            }
            else if(is_switch == "Standby"){
                if(ha_value.origin == false)
                    ha_value.enabled = true;
                else
                    ha_value.enabled = false;
            }
            else{
                response.set_status_code(status_codes::BadRequest);
                _request.reply(response);
                // _request.reply(status_codes::BadRequest);
                return ;
            }

            json::value rep;
            rep[U("Result")] = json::value::boolean(U(true));
            response.set_status_code(status_codes::OK);
            response.set_body(rep);
            _request.reply(response);
            // _request.reply(status_codes::OK, rep);
            return ;
        }

        if(uri_tokens.size() == 1 && uri_tokens[0] == "Command")
        {
            // string path받아서 명령어처리
            json::value command_info, j;
            command_info = _request.extract_json().get();

            if(command_info.as_object().find("Cmd") == command_info.as_object().end()
            || command_info.as_object().find("Path") == command_info.as_object().end())
            {
                response.set_status_code(status_codes::BadRequest);
                _request.reply(response);
                // _request.reply(status_codes::BadRequest);
                return ;
            }

            string cmd, path, result;
            cmd = command_info.at("Cmd").as_string();
            path = command_info.at("Path").as_string();

            char *boo = (char *)path.c_str();
            char *res;

            res = get_popen_string(boo);
            result = res;

            // Todo
            // (popen하는 시간땜에 1초정도 쉬고)
            // 로드(스탠바이상태인 cmm녀석이 백업을 하는거)

            j[U("Status")] = json::value::string("OK");
            cout << "dd : " << result << endl;
            // j[U("Result")] = json::value::string(result);

            response.set_status_code(status_codes::OK);
            response.set_body(j);
            _request.reply(response);
            // _request.reply(status_codes::OK, j);
            return ;
            // Cmd가 FileSync 일 때 자기랑 반대되는 cmm에다가 json파일 보낸거 읽어서 백업하기

        }

        // 내부 api /command/~~로 정리할까 생각중
        if(uri == "/command/inner/sensorupdate")
        {
            // #1 extract_json 해서 리퀘스트 바디받아오면 array형식으로 센서 좌라락 들어옴
            // #2 거기서 먼저 모듈id읽고 모듈id 테이블에서 추가된 모듈인가 체크
            // #3 있으면 이제 array member 1개에서 센서id읽고 /~~/Chassis/모듈id/Sensors/센서id가 리소스로 존재하
            // 는지를 검사함 >>>>  존재한다->#4, 존재않는다->#5
            // #4 존재하면 리소스의 reading time과 json body의 reading time을 비교해서 최신시간으로 변경되었으면
            // 리소스의 reading time과 reading 값 업데이트
            // #5 존재하지 않는다면 리소스를 생성하고 넘겨받은 정보들로 값 넣어줌
            // #6 4,5 두 경우 모두 수행하고나서 DataBase Reading Table에 로그기록 insert해줘야함

            // ㄱㄱ

            json::value jv = _request.extract_json().get();
            json::value jv_error;
            
            string module_id;
            if(get_value_from_json_key(jv, "Module", module_id) == false)
            {
                jv_error = get_error_json("Need Module ID");
                _request.reply(status_codes::BadRequest, jv_error);
                return ;
            }

            if(module_id_table.find(module_id) == module_id_table.end())
            {
                jv_error = get_error_json("Not Registered Module ID");
                _request.reply(status_codes::BadRequest, jv_error);
                return ;
            }

            json::value sensors_info = json::value::array();
            if(get_value_from_json_key(jv, "Sensors", sensors_info) == false)
            {
                jv_error = get_error_json("Need Sensors INFO");
                _request.reply(status_codes::BadRequest, jv_error);
                return ;
            }

            for(int i=0; i<sensors_info.size(); i++)
            {
                string odata = ODATA_CHASSIS_ID;
                odata = odata + "/" + module_id;
                odata = odata + "/Sensors/";

                string s_id;
                get_value_from_json_key(sensors_info[i], "Id", s_id);
                odata = odata + s_id;
                // cout << " ODATA : " << odata << endl;

                if(record_is_exist(odata))
                {
                    // 있으면 타임비교하고 업데이트
                    // cout << " Exist ! " << endl;
                    Sensor *sensor;
                    sensor = (Sensor *)g_record[odata];

                    string time;
                    get_value_from_json_key(sensors_info[i], "ReadingTime", time);

                    double value;
                    get_value_from_json_key(sensors_info[i], "Reading", value);

                    string type;
                    get_value_from_json_key(sensors_info[i], "ReadingType", type);

                    if(sensor->reading_time < time)
                    {
                        sensor->reading_time = time;
                        sensor->reading = value;
                        resource_save_json(sensor);
                        log(info) << "[Sensor Update] : " << odata;

                        if(type == "Rotational")
                            type = "Fan";
                        // insert_reading_table(s_id, type, value, time, module_id);
                        log(info) << "[DB INSERT] : Reading TABLE";
                    }
                    else
                        ;// cout << " IGNORE ! " << endl;

                    
                }
                else
                {
                    // 없으면 생성
                    // cout << " Nope ! " << endl;
                    // 생성하고 그 받은 json으로 load때리고 odata만 수정해주면될듯 ㅇㅇ
                    Sensor *sensor = new Sensor(odata, s_id);
                    sensor->load_json(sensors_info[i]);
                    sensor->odata.id = odata;

                    string col_odata = get_parent_object_uri(odata, "/");
                    Collection *col = (Collection *)g_record[col_odata];
                    // ((Collection *)g_record[col_odata])->add_member(sensor);
                    col->add_member(sensor);

                    resource_save_json(sensor);
                    resource_save_json(col);
                    log(info) << "[Sensor Create] : " << odata;

                    string type = sensor->reading_type;
                    if(type == "Rotational")
                        type = "Fan";
                    // insert_reading_table(s_id, type, sensor->reading, sensor->reading_time, module_id);
                    log(info) << "[DB INSERT] : Reading TABLE";
                    // cout << " DB INSERT !" << endl;
                }
            }

            _request.reply(status_codes::OK);
            return ;
        }


        

//         if(uri_tokens.size() == 1 && uri_tokens[0] == "Mytest")
//         {
//             // system("ls -l");
//             string test;
//             test = "ssh-keygen -f \"/root/.ssh/known_hosts\" -R \"$ip\"";
//             cout << test << endl;
// //             char *ttt = "ls -al";
// //             string result;
// //             char *res;

// // cout << "MY TEST START------------------------" << endl;
// //             res = get_popen_string(ttt);
// //             result = res;

// //             // cout << "MY TEST START------------------------" << endl;
// //             // printf("%s\n", res);
// //             // cout << result << endl;
// //             cout << "MY TEST FIN------------------------" << endl;
// //             cout << result << endl;

//             _request.reply(status_codes::Accepted);
//             return ;
//         }

        if(uri_tokens.size() >= 4)
        {
            if(uri_tokens[2] == "AccountService" || uri_tokens[2] == "SessionService" || uri_tokens[2] == "TaskService"
            || uri_tokens[2] == "CertificateService" || uri_tokens[2] == "EventService" || uri_tokens[2] == "UpdateService")
            {
                //CMM자체 동작으로다가 처리하시면됨
                do_task_cmm_post(_request);
            }
            else
            {
                if(uri_tokens[3] == CMM_ID)
                {
                    //CMM 자체처리
                    do_task_cmm_post(_request);
                }
                else //if(uri_tokens[3] == "2") // 여기 나중에 분간하는거 처리 해줘야할듯
                {
                    if(module_id_table.find(uri_tokens[3]) != module_id_table.end())
                        do_task_bmc_post(_request);
                    else
                    {
                        json::value j;
                        j[U("Error")] = json::value::string(U("Unvalid Module ID"));
                        response.set_status_code(status_codes::BadRequest);
                        response.set_body(j);
                        _request.reply(response);
                        // _request.reply(status_codes::BadRequest, j);
                        cout << "Unvalid BMC_id" << endl;
                        return ;
                    }
                    
                }
            }
        }
        else
        {
            response.set_status_code(status_codes::BadRequest);
            _request.reply(response);
            // _request.reply(status_codes::BadRequest);
            return ;
        }
    }
    catch(json::json_exception& e)
    {
        response.set_status_code(status_codes::BadRequest);
        _request.reply(response);
        // _request.reply(status_codes::BadRequest);
        return ;
    }
    

    // do_task_post(_request);


    // // 기존 post부분
    // string uri = _request.request_uri().to_string();
    // vector<string> uri_tokens = string_split(uri, '/');
    // string filtered_uri = make_path(uri_tokens);
    // json::value b = _request.extract_json().get();
    // string user_name;
    // string password;
    // string odata_id;
    // Account *account;

    // log(info) << "Request method: POST";
    // log(info) << "Reqeust uri : " << filtered_uri;
    // try
    // {
    //     // Account handling
    //     if (filtered_uri == ODATA_ACCOUNT_ID)
    //     {
    //         string role_id = "ReadOnly";
    //         bool enabled = true;

    //         user_name = b.at("UserName").as_string();
    //         password = b.at("Password").as_string();

    //         // Check password length enought
    //         if (password.size() < ((AccountService *)g_record[ODATA_ACCOUNT_SERVICE_ID])->min_password_length)
    //         {
    //             // cout << "dddd" << endl; // @@@@@@@@
    //             _request.reply(status_codes::BadRequest);
    //             return;
    //         }
    //         odata_id = ODATA_ACCOUNT_ID;
    //         odata_id = odata_id + '/' + user_name;

    //         // Check account already exist
    //         if (record_is_exist(odata_id))
    //         {
    //             _request.reply(status_codes::Conflict);
    //             return;
    //         }

    //         // Additinal account information check
    //         if (b.as_object().find("RoleId") != b.as_object().end())
    //         {
    //             odata_id = ODATA_ROLE_ID;
    //             role_id = b.at("RoleId").as_string();
    //             odata_id = odata_id + '/' + role_id;
    //             // Check role exist
    //             if (!record_is_exist(odata_id))
    //             {
    //                 _request.reply(status_codes::BadRequest);
    //                 return;
    //             }
    //         }
    //         if (b.as_object().find("Enabled") != b.as_object().end())
    //             enabled = b.at("Enabled").as_bool();

    //         // TODO id를 계정 이름 말고 숫자로 변경 필요
    //         odata_id = filtered_uri + '/' + user_name;
    //         account = new Account(odata_id, role_id);
    //         account->name = "User Account";
    //         account->user_name = user_name;
    //         account->id = user_name;
    //         account->password = password;
    //         account->enabled = enabled;
    //         account->locked = false;

    //         record_save_json(); // @@@@@@@@@@@@@@@ json생성

    //         // Collection *account_collection = (Collection *)g_record[ODATA_ACCOUNT_ID];
    //         //account_collection->add_member(account);
    //         // 이 부분이 위에 new Account할 때 Account에서 이미 만들어줘서
    //         // 여기서도 하니깐 2개가 만들어짐 계정이

    //         //@@@@@@@@@
    //         // cout << account->user_name << " / " << account->password << endl;
    //         // Account *acc = (Account *)g_record[ODATA_ACCOUNT_ID];
    //         //_request.reply(status_codes::Created, acc->get_json());
    //         //_request.reply(status_codes::OK, record_get_json(filtered_uri));
    //         //cout << (Collection *)g_record[ODATA_ACCOUNT_ID].members[1]->id << endl;
    //         // _request.reply(status_codes::Created, record_get_json(odata_id));
    //         _request.reply(status_codes::Created, record_get_json(ODATA_ACCOUNT_ID));
    //         // 원래 created까지였음
    //     }

    //     else if (filtered_uri == ODATA_SESSION_ID)
    //     {

    //         user_name = b.at("UserName").as_string();
    //         password = b.at("Password").as_string();

    //         odata_id = ODATA_ACCOUNT_ID;
    //         odata_id = odata_id + '/' + user_name;

    //         // Check account exist
    //         if (!record_is_exist(odata_id))
    //         {
    //             _request.reply(status_codes::BadRequest);
    //             return;
    //         }

    //         account = (Account *)g_record[odata_id];
    //         // Check password correct
    //         if (account->password != password)
    //         {
    //             _request.reply(status_codes::BadRequest);
    //             return;
    //         }

    //         // TODO 세션 id 생성 필요
    //         string odata_id = ODATA_SESSION_ID;
    //         string token = generate_token(16);
    //         odata_id = odata_id + '/' + token;
    //         Session *session = new Session(odata_id, token, account);
    //         session->start();
    //         record_save_json(); // @@@@@@@@json


    //         http_response response(status_codes::Created);
    //         response.headers().add("X-Auth-Token", token);
    //         response.headers().add("Location", session->odata.id);
    //         response.set_body(json::value::object());

    //         // web::http::http_headers::iterator iter;
    //         // for(iter = response.headers().begin(); iter != response.headers().end(); iter++)
    //         //     cout << "HEAD! : " << iter->first << " / " << iter->second << endl;

    //         _request.reply(response);
    //         return;
    //     }

    //     /**
    //      * @brief heartbeat
    //      * @authors 강
    //      */

    //     else if(filtered_uri == ODATA_HEARTBEAT)
    //     {
    //         json::value j;
    //         j[U("Status")] = json::value::string("OK");
    //         _request.reply(status_codes::OK, j);
    //         return ;

    //     }
    // }
    // catch (json::json_exception &e)
    // {
    //     //cout << "eeee" << endl; // @@@@@@
    //     _request.reply(status_codes::BadRequest);
    // }


    // //cout << "ffff" << endl; // @@@@@@@
    // _request.reply(status_codes::BadRequest);
}

// http_response basic_response(status_codes _status)
// {
//     http_response response(_status);
//     response.headers().add("")

//     return response;
// }

/**
 * @brief PATCH request handler
 * @authors 강
 * @param _request Request object
 */
void Handler::handle_patch(http_request _request)
{
    log(info) << "Request method: PATCH";
    string uri = _request.request_uri().to_string();
    vector<string> uri_tokens = string_split(uri, '/');
    string filtered_uri = make_path(uri_tokens);
    log(info) << "Reqeust URL : " << filtered_uri;
    log(info) << "Request Body : " << _request.to_string();

    // cout << "HELL YEAH" << endl;
    // cout << _request.get_remote_address() << endl;
    // cout << _request.remote_address() << endl;

    http_response response;

    response.headers().add("Access-Control-Allow-Origin", "*");
    response.headers().add("Access-Control-Allow-Credentials", "true");
    response.headers().add("Access-Control-Allow-Methods", "POST, GET, OPTIONS, DELETE, PATCH");
    response.headers().add("Access-Control-Max-Age", "3600");
    response.headers().add("Access-Control-Allow-Headers", "Content-Type, Accept, X-Requested-With, remember-me, X-Auth-Token");
    response.headers().add("Access-Control-Expose-Headers", "X-Auth-Token, Location");
    

    try
    {

        // #오픈시스넷 /CMMHA 임시 처리부(patch)
        if(uri_tokens[0] == "CMMHA")
        {
            json::value j;
            j["ActiveIP"] = json::value::string("192.168.0.68");
            j["ActivePort"] = json::value::string("8000");
            j["StandbyIP"] = json::value::string("10.0.6.106");
            j["StandbyPort"] = json::value::string("8000");
            j["NetworkTimeOut"] = json::value::number(50);
            j["HeartbeatInterval"] = json::value::number(5);
            j["SwitchTimeOut"] = json::value::number(10);
            j["IPList"] = json::value::array();
            j["IPList"][0] = json::value::string("192.168.0.68");
            j["IPList"][1] = json::value::string("10.0.6.106");

            response.set_status_code(status_codes::OK);
            response.set_body(j);
            _request.reply(response);
            // _request.reply(status_codes::OK, j);
            return ;
        }



        /* code */
        if(uri_tokens.size() == 2 || uri_tokens.size() == 3)
        {
            do_task_cmm_patch(_request);
            return ;
        }

        if(uri_tokens.size() >= 4)
        {
            if(uri_tokens[2] == "AccountService" || uri_tokens[2] == "SessionService" || uri_tokens[2] == "TaskService"
            || uri_tokens[2] == "CertificateService" || uri_tokens[2] == "EventService" || uri_tokens[2] == "UpdateService")
            {
                //CMM자체 동작으로다가 처리하시면됨
                do_task_cmm_patch(_request);
            }
            else
            {
                if(uri_tokens[3] == CMM_ID)
                {
                    //CMM 자체처리
                    do_task_cmm_patch(_request);
                }
                else //if(uri_tokens[3] == "2") // 여기 나중에 분간하는거 처리 해줘야할듯
                {
                    if(module_id_table.find(uri_tokens[3]) != module_id_table.end())
                    // 일단 104번에서 처리
                        do_task_bmc_patch(_request);
                    else
                    {
                        json::value j;
                        j[U("Error")] = json::value::string(U("Unvalid Module ID"));
                        response.set_status_code(status_codes::BadRequest);
                        response.set_body(j);
                        _request.reply(response);
                        // _request.reply(status_codes::BadRequest, j);
                        cout << "Unvalid BMC_id" << endl;
                        return ;
                    }
                }
            }
        }
        else
        {
            // uri_token size가 1/2,3/4이상에도 해당안되는거면 에러
            response.set_status_code(status_codes::BadRequest);
            _request.reply(response);
            // _request.reply(status_codes::BadRequest);
            return ;
        }
       
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        response.set_status_code(status_codes::BadRequest);
        _request.reply(response);
        // _request.reply(status_codes::BadRequest);
        return ;
    }
    
}

/**
 * @brief OPTIONS request handler
 * 
 * @param _request Request object
 */
void Handler::handle_options(http_request _request)
{
    log(info) << "Request method: OPTIONS";
    string uri = _request.request_uri().to_string();
    vector<string> uri_tokens = string_split(uri, '/');
    string filtered_uri = make_path(uri_tokens);
    log(info) << "Reqeust URL : " << filtered_uri;
    log(info) << "Request Body : " << _request.to_string();

    cout << "handle_options request" << endl;

    http_response response(status_codes::OK);
    response.headers().add("Access-Control-Allow-Origin", "*");
    response.headers().add("Access-Control-Allow-Credentials", "true");
    response.headers().add("Access-Control-Allow-Methods", "POST, GET, OPTIONS, DELETE, PATCH");
    response.headers().add("Access-Control-Max-Age", "3600");
    response.headers().add("Access-Control-Allow-Headers", "Content-Type, Accept, X-Requested-With, remember-me, X-Auth-Token");
    response.headers().add("Access-Control-Expose-Headers", "X-Auth-Token, Location");
    _request.reply(response);
    return ;
}


/**
 * @brief : CMM모듈이 client가 되어 request요청하기
 * @authors 강
 * @todo : 호출될때 줄 파라미터에 따라서 수정필요, _uri 기능 추가될때마다 처리부 코드도 추가, try/catch
*/

// json::value/*pplx::task<http_response>*/ redfish_request_get(string _uri, string _host, int _port)// json::value j_data)
json::value redfish_request_get(string _path, string _url) // path가 /redfish/v1...부분  url이 http://10....부분
{
    cout << "<< GET >>" << endl;
    cout << "url : " << _url << endl;
    cout << "path : " << _path << endl;
    cout << "err1" << endl;

    // uri_builder builder;
    // builder.set_scheme(U("http"));
    // builder.set_host(_host);
    // builder.set_port(_port);
    // uri u(builder.to_uri()); 

    // cout << "err2" << endl;
    // http_client client(u);
    string tmp = _url;
    http_client client(_url);
    cout << "err2" << endl;
    // http_client client(U("http://10.0.6.104:443"));
    // uri_builder b(U("/redfish/"));
    
    // cout << "TOSTRING : " << b.to_string() << endl;

    // 여기에서 _uri가 뭐냐에 따라서 다르게 처리해주면 될거같음

    pplx::task<http_response> responseTask = client.request(methods::GET, _path);//b.to_string());
    cout << "err3" << endl;
    http_response response = responseTask.get();
    cout << "err4" << endl;
    json::value response_json = response.extract_json().get();
    cout << "####" << endl;
    cout << response_json << endl;
    // cout << typeid(response.extract_json()).name() << endl;
    // cout << response.extract_json().get() << endl;
    // cout << typeid(response.extract_json().get()).name() << endl;
    // cout << "$$$$" << endl;

    return response_json;
    //return responseTask;
    
}

// post를 따로 함수 만들기 AccountService/Accounts 로 똑같이 만들어지는거 같으니 파라미터로 json받아서
// 요청 넣는식으로 구현하기 반환값이 json이 따로 없는듯하여 status봐야할듯

void redfish_request_post(string _path, string _url)//json::value _jv)
{
    cout << "<< POST >>" << endl;
    cout << "url : " << _url << endl;
    cout << "path : " << _path << endl;
    json::value _jv;
    _jv[U("UserName")] = json::value::string(U("MY_NAME"));
    _jv[U("Password")] = json::value::string(U("MY_PASSWORD"));
    // 파라미터로 json::value를 그대로 받으면 그냥 쓰면 되고
    // 그 안에 내용물을 받았으면 이렇게 만들어주어야함  

    http_client client(_url);

    if(_path == ODATA_ACCOUNT_ID)
    {
        pplx::task<http_response> responseTask = client.request(methods::POST, _path, _jv.serialize(), U("application/json"));
        http_response response = responseTask.get();
        json::value response_json = response.extract_json().get();

        cout << "status code : " << response.status_code() << endl;
        cout << response_json << endl;
        cout << "!@#$ Good POST" << endl;

    }

    else if(_path == ODATA_SESSION_ID)
    {
        pplx::task<http_response> responseTask = client.request(methods::POST, _path, _jv.serialize(), U("application/json"));
        http_response response = responseTask.get();

        string token = response.headers()["X-Auth-Token"];

        cout << response.status_code() << endl;
        cout << "Token : " << token << endl;
        cout << "!@#$ Good POST" << endl;
    }

    

}

void redfish_request_patch(string _path, string _url)
{
    cout << "!@#$ PATCH!!" << endl;
}

void redfish_request_delete(string _path, string _url)
{
    cout << "!@#$ DELETE!!" << endl;
}
// KETI-EDGE 에서 구현된 기능을 알아보고 구현하여 테스트

/* 여기 request GET/POST/PATCH/DELETE 함수들의 위치를 따로 위치시킬건지
 * 그리고 CMM모듈에서 이 함수들을 어떻게 사용할건지에 따라서 변형될 수 있음
 * try-catch 처리해.. 그리고 테스트 위한 출력부 처리하고
 */

void log_callback(const char *file, const char *tag, int level, int line, const char *func, const char * message) {
    char *level_name = "DEBUG";
    if (level == LSSDP_LOG_INFO)    level_name = "INFO";
    if (level == LSSDP_LOG_WARN)    level_name = "WARN";
    if (level == LSSDP_LOG_ERROR)   level_name = "ERROR";

    printf("[%-5s][%s] %s", level_name, tag, message);
    return;
}


long long get_current_time() {
    struct timeval time = {};
    if (gettimeofday(&time, NULL) == -1) {
        printf("gettimeofday failed, errno = %s (%d)\n", strerror(errno), errno);
        return -1;
    }
    return (long long) time.tv_sec * 1000 + (long long) time.tv_usec / 1000;
}

int show_interface_list_and_rebind_socket(lssdp_ctx * lssdp) {
    // 1. show interface list
    printf("\nNetwork Interface List (%zu):\n", lssdp->interface_num);
    size_t i;
    for (i = 0; i < lssdp->interface_num; i++) {
        printf("%zu. %-6s: %s\n",
            i + 1,
            lssdp->interface[i].name,
            lssdp->interface[i].ip
        );
    }
    printf("%s\n", i == 0 ? "Empty" : "");

    // 2. re-bind SSDP socket
    if (lssdp_socket_create(lssdp) != 0) {
        puts("SSDP create socket failed");
        return -1;
    }

    return 0;
}

int show_ssdp_packet(struct lssdp_ctx * lssdp, const char * packet, size_t packet_len){
    //printf("%s", packet);
    vector<string> packet_info = string_split(std::string(packet), '\n');
    string resultaddr;
    string result_id;
    bool checkmyblade=false;
    for (auto str : packet_info){
        // log(info) << "확인하자 패킷 : " << str;
        // cout << " 확인하자 패킷 : " << str;

        // checkmyblade = true;

        if((str.find("SMM")!=string::npos||str.find("BMC")!=string::npos) && (str.find("NT:")!=string::npos)) // ST도 추가해야할수있음
        {           
            // checkmyblade=true;
            // log(info) << " 확인하자 패킷 : " << str;
            string nt = str;
            result_id = nt.substr(str.find("-")+1, str.find("\r")-str.find("-")-1);

            if(module_id_table.find(result_id) == module_id_table.end())
                checkmyblade=true;
        }

        if(str.find("LOCATION")!=string::npos){
            
            resultaddr=str;  
        }
    }

    if (checkmyblade)
    {
        // string result = resultaddr.substr(resultaddr.find(":")+1);
        string result = resultaddr.substr(resultaddr.find(":")+1, resultaddr.find("\r")-resultaddr.find(":")-1);
        // log(info) << " yyyy : " << result << " / " << result.length();

        if(result=="")
            return 0;
        
        module_id_table.insert({result_id, result});
        save_module_id();
        add_new_bmc(result_id, result, "root", "");
        // add_new_bmc(result_id, "10.0.6.104", "443", false, "root", "");
        // 여기서 http://10.0.6.104:443을 쪼개서 함수인자를 받아서 사용한다면 result를 파싱해서 사용해야하고
        // 통으로 받아서 사용한다면 그대로result 쓰면됨 

        findcurrent_time = get_current_time();
        log(info) << "Search Result ID & Address : " << result_id << " / " << result;
        log(info) <<"Find Computing Module ip address: "<<result;
        // for(int i =0; i<7; i++)
        for(int i =0; i<packet_info.size(); i++)
        log(info)<<packet_info[i];
        log(info)<<"find Storage Module or Computing Module time : "<<to_string((double)(findcurrent_time - findlast_time)/1000)<<"sec"; //결과 출력
        findlast_time = get_current_time();
        
    }

    return 0;
}

int show_neighbor_list(lssdp_ctx * lssdp) {
    int i = 0;
    lssdp_nbr * nbr;
    puts("\nSSDP LIST:");
    for (nbr = lssdp->neighbor_list; nbr != NULL; nbr = nbr->next) {
        printf("%d. id = %-9s, ip = %-20s, name = %-12s, device_type = %-8s (%lld)\n",
            ++i,
            nbr->sm_id,
            nbr->location,
            nbr->usn,
            nbr->device_type,
            nbr->update_time
        );
    }
    //printf("%s\n", i == 0 ? "Empty" : "");
    return 0;
}

void *ssdp_handler(void)
{
    lssdp_set_log_callback(log_callback);
    findlast_time = get_current_time();
    log(info)<<"ssdp discovery Computing Module and Storage Module .. time"<<currentDateTime();;
    lssdp_ctx lssdp = {
        .port = 1900,
        .neighbor_timeout = 15000, // 15seconds
        //.debug = true,
        .header = {
            "ST_P2P",
            "f835dd000001",
            "700000123",
            "DEV_TYPE",
        },
        // callback
        .network_interface_changed_callback = show_interface_list_and_rebind_socket,
        .neighbor_list_changed_callback = show_neighbor_list,
        .packet_received_callback = show_ssdp_packet,
    };

    lssdp_network_interface_update(&lssdp);

    last_time = get_current_time();
    if (last_time < 0){
        log(error) << "Got Invalid Timestamp : " << last_time;
        return 0;
    }

    while (1){
        fd_set fs;
        FD_ZERO(&fs);
        FD_SET(lssdp.sock, &fs);
        struct timeval tv;
        tv.tv_usec = 500 * 1000; // 500ms

        int ret = select(lssdp.sock + 1, &fs, NULL, NULL, &tv);
        if (ret < 0){
            log(error) << "select error, ret = " << ret;
            break;
        }

        if (ret > 0){
            lssdp_socket_read(&lssdp);
        }
        current_time = get_current_time();
        if (current_time < 0){
            log(error) << "Got Invalid Timestamp : " << last_time;
            break;
        }
        // // show neighbor list
       
        // lssdp_nbr *nbr = lssdp.neighbor_list;
        // if (nbr != NULL){
        //     while (nbr->next != NULL){
        //         log(info) << "location : " << nbr->location;
        //         log(info) << "device type : " << nbr->device_type;
        //         nbr = nbr->next;
        //     }
        // }
    //     for (int i = 0; i < lssdp->interface_num; i++) {
    //     printf("%zu. %-6s: %s\n",
    //         i + 1,
    //         lssdp->interface[i].name,
    //         lssdp->interface[i].ip
    //     );
    // }

        // doing task per 5 seconds
        if (current_time - last_time >= 3000) {
            
            lssdp_network_interface_update(&lssdp);
            lssdp_send_msearch(&lssdp);
            //lssdp_send_notify(&lssdp);
            lssdp_neighbor_check_timeout(&lssdp);

            last_time = current_time;
        }
    }

    return 0;
}



// Log Data for Web Service
void log_operation(http_request _request)
{
    // cors 해줘야함
    // reading 부분 , Event 부분 나눠야함
    string uri = _request.request_uri().to_string();
    vector<string> uri_tokens = string_split(uri, '/');

    http_response response;
    json::value response_json;

    response.headers().add("Access-Control-Allow-Origin", "*");
    response.headers().add("Access-Control-Allow-Credentials", "true");
    response.headers().add("Access-Control-Allow-Methods", "POST, GET, OPTIONS, DELETE, PATCH");
    response.headers().add("Access-Control-Max-Age", "3600");
    response.headers().add("Access-Control-Allow-Headers", "Content-Type, Accept, X-Requested-With, remember-me, X-Auth-Token");
    response.headers().add("Access-Control-Expose-Headers", "X-Auth-Token, Location");

    if(uri_tokens[1] == "reading")
    {
        reading_operation(_request, response);
    }
    else if(uri_tokens[1] == "event")
    {
        event_operation(_request, response);
    }
    else
    {
        json::value j = get_error_json("Not Supported URI");
        response.set_status_code(status_codes::BadRequest);
        response.set_body(j);
    }

    _request.reply(response);
    return ;
}

void reading_operation(http_request _request, http_response &_response)
{
    // DB Reading Table관련 기능 수행하는 핸들러처리
    // #1 uri 설계..
    // /log/reading까지 들어오고 /log/reading/[module]/[type]/[detail]/[time_option]
    // [module] : 모듈id,, 어느 모듈의 센서측정값들인지
    // [type] : 센서의 위치타입,, thermal / power 등등...
    // [detail] : 해당 센서타입에서의 상세구분정보 ex> 타입power일때 powercontrol, powersupply 같은 정보
    // [time_option] : 시간조건,, 시간조건없이 최신 x개 긁어오기 / 최근1시간이내 / 최근30분이내 등등...
    // time_option 변경점 : 센서가 1분단위씩으로 주기적으로 측정됨.. 그래서 min으로 주면 분단위로 30분(즉 30개)
    // hour가 시간단위로 30시간(30개) 
    // all을 없애야겠다 그러면 좀 간단해짐 쿼리도 그렇고 구분도 그렇고 all을 빼 어차피 부르는쪽에서 구분해서
    // 불러서 갖다쓰면됨 어차피 array형식이라 all로 해서 다 준다고 해도 정보가지고 필터링작업거쳐야함 ㅇㅇ
    web::uri uri = _request.request_uri();
    string uri_string = uri.to_string();
    vector<string> uri_tokens = uri::split_path(uri_string);

    cout << "READING OPERATION" << endl;
    cout << "Request URI : " << uri_string << endl;
    cout << "URI Tokens ---" << endl;
    for(int i=0; i<uri_tokens.size(); i++)
    {
        cout << uri_tokens[i] << endl;
    }

    if(uri_tokens.size() < 6)
    {
        _response.set_status_code(status_codes::BadRequest);
        _response.set_body(get_error_json("URI Format >> /log/reading/[module]/[type]/[detail]/[time_option]"));
        return ;
    }

    string param_module, param_type, param_detail, param_time_option;

    
    // [module] 처리
    if(module_id_table.find(uri_tokens[2]) == module_id_table.end())
    {
        _response.set_status_code(status_codes::BadRequest);
        _response.set_body(get_error_json("Module ID [" + uri_tokens[2] + "] does not Exist"));
        return ;
    }
    param_module = uri_tokens[2];
    

    // [type] 처리
    if(!check_reading_type(uri_tokens[3]))
    {
        _response.set_status_code(status_codes::BadRequest);
        _response.set_body(get_error_json("Reading Type [" + uri_tokens[3] + "] is Incorrect"));
        return ;
    }
    param_type = uri_tokens[3];
    
    // [detail] 처리
    if(!check_reading_detail(param_type, uri_tokens[4]))
    {
        _response.set_status_code(status_codes::BadRequest);
        _response.set_body(get_error_json("Reading Detail [" + uri_tokens[4] + "] is Incorrect"));
        return ;
    }
    param_detail = uri_tokens[4];

    // [time_option] 처리
    if(!check_reading_time_option(uri_tokens[5]))
    {
        _response.set_status_code(status_codes::BadRequest);
        _response.set_body(get_error_json("Time Option [" + uri_tokens[5] + "] is Incorrect"));
        return ;
    }
    param_time_option = uri_tokens[5];
    

    cout << param_module << " / " << param_type << " / " << param_detail << " / " << param_time_option << endl;

    // 만들어진 param가지고 함수 호출
    json::value result_jv;
    if(param_time_option == "min")
        result_jv = select_min_reading(param_module, param_type, param_detail, 30);
    else if(param_time_option == "hour")
        ;
    // result_jv = select_all_reading()

    // module 값의 uri토큰이 all 인지 특정 모듈인지에 따라서
    // DB에서 읽을때 모듈 칼럼의 where절 값에 들어가는 조건이 달라지는거지 그거에 해당하는 파라미터가 필요하고 
    // select_reading거기에서 ㅇㅇ 그리고 그 함수 인자값만 지정하는거야 여기서는
    // 그다음엔 type검사해서 type값 지정하고
    // time_option 검사해서 값 지정하고 다 모인 파라미터값들을 인자로해서
    // select_reading함수를 호출해 

    // 그러면 select_reading함수에서 인자들 가지고 쿼리문 만들어서 해당하는 DB데이터 긁어올것이고 그걸 json형태로
    // 만들어서 반환하면 그걸가지고 여기서 reply해주면됨 
    // 그래서 select_reading함수를 수정해야한다~~~~ 

    
    _response.set_status_code(status_codes::Found);
    _response.set_body(result_jv);
    return ;
    
}

void event_operation(http_request _request, http_response &_response)
{

}

bool check_reading_type(string _types)
{
    if(_types == "thermal" || _types == "power")
        return true;
    else
        return false;
}
// type하고 detail이 생기면서 type검사에 power, thermal과 같은걸로 넣어주어야하고
// 그 type에 따라서 detail값이 temp,fan // powercontrol powervoltage powersupply 로 나뉨
bool check_reading_detail(string _type, string _detail)
{
    if(_type == "thermal")
    {
        if(_detail == "temperature" || _detail == "fan")
            return true;
        
    }
    else if(_type == "power")
    {
        if(_detail == "powercontrol" || _detail == "powervoltage" || _detail == "powersupply")
            return true;
    }

    return false;
}

bool check_reading_time_option(string _option)
{
    // hour: 1시간 간격 30개로그 , min: 1분 간격 30개로그...
    if(_option == "hour" || _option == "min")
        return true;
    else
        return false;
}