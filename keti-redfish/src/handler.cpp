#include "handler.hpp"
#include "resource.hpp"
#include "task.hpp"

extern unordered_map<string, Resource *> g_record;
extern unordered_map<uint8_t, Task_Manager *> task_map;
// extern unordered_map<string, unordered_map<string, Task *> > task_map;
extern Value_About_HA ha_value;
extern int heart_beat_cycle;
unsigned int g_count = 0;

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

    try
    {
        // Response Redfish Version
        if(uri_tokens.size() == 1 && uri_tokens[0] == "redfish")
        {
            json::value j;
            j[U(REDFISH_VERSION)] = json::value::string(U(ODATA_SERVICE_ROOT_ID));
            _request.reply(status_codes::OK, j);
            return ;
        }

        if(uri_tokens.size() == 2 || uri_tokens.size() == 3)
        {
            do_task_cmm_get(_request);
            return ;
        }

        if(uri_tokens.size() >= 4)
        {
            if(uri_tokens[2] == "AccountService" || uri_tokens[2] == "SessionService" || uri_tokens[2] == "TaskService")
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
                        _request.reply(status_codes::BadRequest, j);
                        cout << "Unvalid BMC_id" << endl;
                        return ;
                    }
                }
            }
        }
        else
        {
            // uri_token size가 1/2,3/4이상에도 해당안되는거면 에러
            _request.reply(status_codes::BadRequest);
            return ;
        }

    }
    catch(json::json_exception& e)
    {
        _request.reply(status_codes::BadRequest);
    }
    

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
    json::value d = _request.extract_json().get();
    string user_name;
    string password;
    string odata_id;
    Account *account;
    AccountService *acc_service;

    log(info) << "Request method: DELETE";
    log(info) << "Reqeust uri : " << filtered_uri;

    try
    {
        if(filtered_uri == ODATA_ACCOUNT_ID)
        {
            user_name = d.at("UserName").as_string();
            password = d.at("Password").as_string();
            odata_id = ODATA_ACCOUNT_ID;
            odata_id = odata_id + '/' + user_name;

            // Check account exist
            if (!record_is_exist(odata_id))
            {
                _request.reply(status_codes::BadRequest);
                return;
            }

            account = (Account *)g_record[odata_id];
            // Check password correct
            if (account->password != password)
            {
                _request.reply(status_codes::BadRequest);
                return;
            }

            g_record.erase(odata_id);

            acc_service = (AccountService *)g_record[ODATA_ACCOUNT_SERVICE_ID];
            //cout << "ACC : " << typeid(acc_service).name() << endl;
            vector<Resource *> tmp;
            tmp = acc_service->account_collection->members;
            //cout << "TMP : " << typeid(tmp).name() << endl;
            std::vector<Resource *>::iterator iter;
            for(iter = acc_service->account_collection->members.begin(); iter != acc_service->account_collection->members.end(); iter++)
            // for(iter = tmp.begin(); iter != tmp.end(); iter++)
            {
                Account *t = (Account *)*iter;
                //cout << typeid(t).name() << endl;
                //cout << t->user_name << endl;
                if(t->user_name == user_name && t->password == password)
                {
                    acc_service->account_collection->members.erase(iter);
                    _request.reply(status_codes::Gone, record_get_json(ODATA_ACCOUNT_ID));
                    // status 임시로 Gone갖다씀
                    break;
                }

            }

            // @ TODO : 현재는 딜리트하지않고 서버를 종료하면
            // 라즈베리파이에 계정.json 파일들이 남아있음
            // 다시 서버를 켰을 경우 저장되어있는 계정들 읽어와서 레코드
            // 구성해주는 작업 필요
            
            record_save_json(); // @@@@@@@@json 레코드갱신기록
            string delete_path = filtered_uri + '/' + user_name + ".json";
            // cout << "dd2 : " << delete_path2 << endl;
            if(remove(delete_path.c_str()) < 0)
            {
                cout << "delete error" << endl;
            }
            // else
            // cout << "HOO~" << endl;
            // 해당 계정에 대한 json파일 삭제
            

            // Collection *account_col = (AccountService *)g_record[ODATA_ACCOUNT_SERVICE_ID]->account_collection;


        }
        /**
         * @brief AccountService/Accounts 로 토큰과함께 들어왔을 때 logout(세션종료)
         * @authors 강
         * @TODO : 세션종료시 레코드 갱신 작업하고 해당세션json파일 삭제 필요
         * @TODO : 현재 Accounts에 GET에서 처리하고있는데 Sessions에 DELETE로 옮길것
        */
        else if(filtered_uri == ODATA_SESSION_ID)
        {
            if(is_session_valid(_request.headers()["X-Auth-Token"]))
            {
                // 토큰에 해당하는 세션 종료
                string token = _request.headers()["X-Auth-Token"];
                string session_uri = ODATA_SESSION_ID;
                session_uri = session_uri + '/' + token;
                Session *session;

                SessionService * ses_service = (SessionService *)g_record[ODATA_SESSION_SERVICE_ID];
                std::vector<Resource *>::iterator iter;
                for(iter = ses_service->session_collection->members.begin(); iter != ses_service->session_collection->members.end(); iter++)
                {
                    Session *t = (Session *)*iter;
                    if(t->id == token)
                    {
                        session = (Session *)*iter;
                        // session->cts.cancel();
                        cout << "YYYY : " << session->id << endl;
                        session->_remain_expires_time = 1;
                        ses_service->session_collection->members.erase(iter);
                        // 이 작업을 해야 record_get_json(ODATA_SESSION_ID))때 안나옴
                        break;
                    }
                }
                // 세션서비스->컬렉션->멤버스 돌면서 해당하는 세션의 남은시간 1로 만들어서 종료되게끔 함
                
                cout << "Session : " << session_uri << endl;
                cout << record_get_json(ODATA_SESSION_ID) << endl;
                cout << "AAAAAAA" << endl;
                // 토큰 붙인거로 Session들어가고 세션에 취소토큰만들어서 취소줘야겠다 여기서

                //Session *session = (Session *)g_record[ODATA_SESSION_ID];
                // cout << record_get_json(session_uri) << endl;
                // delete session;
                // 리플라이
            }
        }

    }
    catch(json::json_exception &e)
    {
        _request.reply(status_codes::BadRequest);
    }

    // @@@@@@ 여기까지

    cout << "handle_delete request" << endl;

    auto j = _request.extract_json().get();

    _request.reply(status_codes::NotImplemented, U("DELETE Request Response"));
}

/**
 * @brief PUT request handler
 * 
 * @param _request Request object
 */
void Handler::handle_put(http_request _request)
{

    log(info) << "Request method: PATCH";
    string uri = _request.request_uri().to_string();
    vector<string> uri_tokens = string_split(uri, '/');
    string filtered_uri = make_path(uri_tokens);
    log(info) << "Reqeust URL : " << filtered_uri;
    log(info) << "Request Body : " << _request.to_string();

    try
    {
        /* code */
        // if(uri_tokens.size() )
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    

    cout << "handle_put request" << endl;

    auto j = _request.extract_json().get();

    _request.reply(status_codes::NotImplemented, U("PUT Request Response"));
}

/**
 * @brief POST request handler
 * 
 * @param _request Request object
 */
void Handler::handle_post(http_request _request)
{

    log(info) << "Request method: POST";
    string uri = _request.request_uri().to_string();
    vector<string> uri_tokens = string_split(uri, '/');
    string filtered_uri = make_path(uri_tokens);
    log(info) << "Reqeust URL : " << filtered_uri;
    log(info) << "Request Body : " << _request.to_string();

    try
    {
        // /Heartbeat 
        if(uri_tokens.size() == 1 && uri_tokens[0] == "Heartbeat")
        {
            json::value j;
            json::value j_cycle;
            j_cycle = _request.extract_json().get();
            if(j_cycle.as_object().find("Cycle") == j_cycle.as_object().end())
            {
                _request.reply(status_codes::BadRequest);
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
                _request.reply(status_codes::BadRequest, rep);
                return ;
            }
            
            

            j[U("Status")] = json::value::string("OK");
            _request.reply(status_codes::OK, j);
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
                _request.reply(status_codes::BadRequest);
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
                _request.reply(status_codes::BadRequest, rep);
                return ;
                // std::cerr << e.what() << '\n';
            }
            

            _request.reply(status_codes::OK);
            return ;

        }


        // /HAswtich
        if(uri_tokens.size() == 1 && uri_tokens[0] == "HAswitch")
        {
            json::value switch_info;
            switch_info = _request.extract_json().get();
            if(switch_info.as_object().find("IsSwitch") == switch_info.as_object().end())
            {
                _request.reply(status_codes::BadRequest);
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
                _request.reply(status_codes::BadRequest);
                return ;
            }

            json::value rep;
            rep[U("Result")] = json::value::boolean(U(true));
            _request.reply(status_codes::OK, rep);
            return ;
        }

        if(uri_tokens.size() >= 4)
        {
            if(uri_tokens[2] == "AccountService" || uri_tokens[2] == "SessionService" || uri_tokens[2] == "TaskService")
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
                        _request.reply(status_codes::BadRequest, j);
                        cout << "Unvalid BMC_id" << endl;
                        return ;
                    }
                    
                }
            }
        }
        else
        {
            _request.reply(status_codes::BadRequest);
            return ;
        }
    }
    catch(json::json_exception& e)
    {
        _request.reply(status_codes::BadRequest);
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