#include "task.hpp"
#include "resource.hpp"
#include "handler.hpp"

extern unordered_map<uint8_t, Task_Manager *> task_map;
extern unique_ptr<Handler> g_listener, HA_listener;
extern unordered_map<string, Event*> event_map;


/**
 * @brief Request GET
 * @author 강
 * @details get handler에서 호출시키는 함수
 * m_Request를 만들어서 해당 타입의 task_manager에 연결시키고 리소스task json파일을 생성,저장한다.
 * 요청 수행 후 Response를 받거나 만들면 m_Request에 m_Response를 만들어 연결시키고 처음요청한 사용자에게 응답하며 Complete타입의 task_manager로
 * m_Request를 이동시킨다.
 */

// 락 걸어야할 부분
// task_map쓰는 category관련
// g_record쓰는 record_save_json관련
// task_numset(번호할당) 쓰는 allocate_task_num관련
// 보니깐 요청처리하는데에도 record exist나 session valid g_record 사용되네

void do_task_cmm_get(http_request _request)
{
    string uri = _request.request_uri().to_string();
    json::value jv = _request.extract_json().get();
    vector<string> uri_tokens = string_split(uri, '/');

    Task_Manager *t_manager; // 작업 매니저
    Task_Manager *c_manager; // 컴플리트 매니저
    
    cout << "!@#$ CMM GET TASK ~~~~" << endl;
    // cout << "MANAGING TASK SIZE ------------" << endl;
    // cout << "size : " << task_map.size() << endl;

    t_manager = category(uri_tokens);
    if(!t_manager)
    {
        // t_manager에 연결안된거 없는거
        json::value rp;
        rp[U("Error")] = json::value::string(U("Wrong Task Category. Please Check the URI."));
        _request.reply(status_codes::BadRequest, rp);
        return ;
    }
    // cout << "T_manager Type : " << t_manager->task_type << endl;

    // Make m_Request
    m_Request msg;
    msg = work_before_request_process("GET", CMM_ADDRESS, uri, jv, _request.headers());

    http_response response;
    json::value response_json;

    response.headers().add("Access-Control-Allow-Origin", "*");
    response.headers().add("Access-Control-Allow-Credentials", "true");
    response.headers().add("Access-Control-Allow-Methods", "POST, GET, OPTIONS, DELETE, PATCH");
    response.headers().add("Access-Control-Max-Age", "3600");
    response.headers().add("Access-Control-Allow-Headers", "Content-Type, Accept, X-Requested-With, remember-me, X-Auth-Token");
    response.headers().add("Access-Control-Expose-Headers", "X-Auth-Token, Location");


    // connect to Task_manager
    t_manager->list_request.push_back(msg);

// Test용으로 현재 인증검사 하지 않겠음!!!!!!! @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    // Check X-Auth-Token field of request
    // if(!_request.headers().has("X-Auth-Token"))
    // {
    //     // http_response tmp_res(status_codes::NetworkAuthenticationRequired);
    //     response.set_status_code(status_codes::NetworkAuthenticationRequired);
    //     msg.result.result_datetime = currentDateTime();
    //     msg.result.result_status = WORK_FAIL;
    //     msg.result.result_response = response;
    //     _request.reply(response);
    //     //_request.reply(status_codes::NetworkAuthenticationRequired);
    //     // return ;
    // }

    // // Check Session is valid
    // else if(!is_session_valid(_request.headers()["X-Auth-Token"]))
    // {
    //     // http_response tmp_res(status_codes::Unauthorized);
    //     response.set_status_code(status_codes::Unauthorized);
    //     msg.result.result_datetime = currentDateTime();
    //     msg.result.result_status = WORK_FAIL;
    //     msg.result.result_response = response;
    //     _request.reply(response);
    //     // _request.reply(status_codes::Unauthorized);
    //     // return ;
    // }

    // Check Resource is exist
    /*else*/ if(!record_is_exist(uri))
    {
        // http_response tmp_res(status_codes::NotFound);
        response.set_status_code(status_codes::NotFound);
        msg.result.result_datetime = currentDateTime();
        msg.result.result_status = WORK_FAIL;
        msg.result.result_response = response;
    }
    else
    {
        // http_response tmp_res(status_codes::OK);
        json::value jj;
        jj = record_get_json(uri);
        if(jj.as_object().find("Password") != jj.as_object().end())
            jj[U("Password")] = json::value::null();
        // account의 password
        // if(jj.as_object().find("AuthToken") != jj.as_object().end())
        //     jj[U("AuthToken")] = json::value::null();
        // session의 authtoken
        response.set_status_code(status_codes::OK);
        response.set_body(jj);
        msg.result.result_datetime = currentDateTime();
        msg.result.result_status = WORK_SUCCESS;
        msg.result.result_response = response;
        msg.result.response_json = jj;
    }

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    /*Before Move 테스트용 출력 */
    // cout << "----------------------------------------------------------------------BEFORE MOVE" << endl;
    // cout << "taskmap size : " << task_map.size() << endl;
    // cout << "list size : " << t_manager->list_request.size() << endl;
    // // 옮기기전 acc맵 하나 그거에 리스트 하나 있던거 1,1 에서  옮기면 acc맵 complete맵 2개 acc맵에 리스트는 0 해서 2,0
    // // 하고 comple에 리스트 1 나와야지

    // cout << " ------------  Info -----------------" << endl;
    // cout << "t_list tasknum : " << t_manager->list_request.front().task_number << endl;
    // cout << "t_list host : " << t_manager->list_request.front().host << endl;
    // cout << "t_list uri : " << t_manager->list_request.front().uri << endl;
    // cout << "t_list method : " << t_manager->list_request.front().method << endl;
    // cout << "t_list datetime : " << t_manager->list_request.front().request_datetime << endl;
    // cout << "t_list result->resnum : " << t_manager->list_request.front().result.res_number << endl;
    // cout << "t_list result->datetime : " << t_manager->list_request.front().result.result_datetime << endl;

    // if(task_map.find(TASK_TYPE_COMPLETED) == task_map.end())
    // {
    //     c_manager = new Task_Manager();
    //     c_manager->task_type = TASK_TYPE_COMPLETED;
    //     task_map.insert(make_pair(TASK_TYPE_COMPLETED, c_manager));
    // }
    // else
    //     c_manager = task_map.find(TASK_TYPE_COMPLETED)->second;
    // // complete 카테고리 c_manager에 연결


    c_manager = work_after_request_process(t_manager, msg);


    /* After Move 테스트용 출력 */
    // cout << "--------------------------------------------------------------After Move" << endl;
    // cout << "taskmap size : " << task_map.size() << endl;
    // cout << "t_list size : " << t_manager->list_request.size() << endl;
    // cout << "c_list size : " << c_manager->list_request.size() << endl;


    // std::list<m_Request>::iterator iter; //이미 위에 생성햇네
    // m_Request completed_msg;
    // for(iter=c_manager->list_request.begin(); iter!=c_manager->list_request.end(); iter++)
    // {
    //     if(iter->task_number == msg.task_number)
    //     {
    //         completed_msg = *iter;
    //         break;
    //     }
    // }
    // cout << " ------------  Info -----------------" << endl;
    // cout << "c_list tasknum : " << completed_msg.task_number << endl;
    // cout << "c_list host : " << completed_msg.host << endl;
    // cout << "c_list uri : " << completed_msg.uri << endl;
    // cout << "c_list method : " << completed_msg.method << endl;
    // cout << "c_list datetime : " << completed_msg.request_datetime << endl;
    // cout << "c_list result->resnum : " << completed_msg.result.res_number << endl;
    // cout << "c_list result->datetime : " << completed_msg.result.result_datetime << endl;
    // cout << "c_list result->status : " << completed_msg.result.result_status << endl;




    // Response Json of Resource to Client
    cout << record_get_json(uri) << endl;
    cout << "*************************     Out CMM GET    *******************" << endl;
    
    _request.reply(response);
    return;
}


void do_task_bmc_get(http_request _request)
{
    
    string uri = _request.request_uri().to_string();
    json::value jv = _request.extract_json().get();
    vector<string> uri_tokens = string_split(uri, '/');
    string new_uri = get_extracted_bmc_id_uri(uri); // [BMC_id] 빼고 다시만든 uri

    cout << "!@#$ BMC GET TASK ~~~ " << endl;
    cout << "BMC_uri = " << uri << endl;
    cout << "BMC_new_uri : " << new_uri << endl;
    cout << "BMC_jv = " << jv << endl;
    // cout << "taskmap size : " << task_map.size() << endl;

    Task_Manager *t_manager; // 작업 매니저
    Task_Manager *c_manager; // 컴플리트 매니저

    try
    {
        /* code */
        t_manager = category(uri_tokens);
        if(t_manager)
        {
            // cout << "있으면 넘어가고 진행하고" << endl;
        }
        else
        {
            // cout << "없으면 바로 리턴시켜야겠다" << endl;
            json::value rp;
            rp[U("Error")] = json::value::string(U("Wrong Task Category. Please Check the URI."));
            _request.reply(status_codes::BadRequest, rp);
            return ;
        }
        
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return ;
    }
    // 이거 왜 try-catch에는 안잡히지?

    // Make m_Request
    m_Request msg;

    msg = work_before_request_process("GET", module_id_table[uri_tokens[3]], uri, jv, _request.headers());

    // connect to Task_manager
    t_manager->list_request.push_back(msg);
    
    http_client client(msg.host);
    http_request req(methods::GET);
    // req.set_request_uri(msg.uri);
    req.set_request_uri(new_uri);
    req.set_body(jv);
    
    if(_request.headers().find("X-Auth-Token") != _request.headers().end())
        req.headers().add(_request.headers().find("X-Auth-Token")->first, _request.headers().find("X-Auth-Token")->second);
    // req.headers().add(U("TEST_HEADER"), U("VALUE!!"));
    // 나중에는 이 헤더붙이는게 이런식이 아니라 현재 이 cmm을 이용할수있게 하는 세션session에 접근해서 거기에 저장되어있는
    // bmc_id에 맞는 토큰이 있으면 그걸로 여기서 헤더에 추가를 해주면 bmc토큰인증이 되는식으로 바뀌어야함
    // 만약없다면 로그인하라고 해야하고 아 여기가 아니고 응답오는 위치에서 response status가 인증을 못뚫는 status면 그때
    // 해야할듯
    // >> cmm이 보내는건 bmc로그인 필요없게끔해야되는것이 아닌가 (cmm만 로그인)
    http_response response;
    json::value response_json;
    m_Response msg_res;

    // Send Request
    try
    {
        /* code */
        
        pplx::task<http_response> responseTask = client.request(req);
        response = responseTask.get();
        cout << "bmc get reply status : " << response.status_code() << endl;

        // 여기서 응답온거 이거 get이니까 ok or 아닌거 해서 ok면 성공 아니면 실패로해서 나눠 리플라이

        if(response.status_code() == status_codes::OK)
            msg.result.result_status = WORK_SUCCESS;
        else
            msg.result.result_status = WORK_FAIL;

        response_json = response.extract_json().get();
        msg.result.response_json = response_json;
        response.set_body(response_json);
        msg.result.result_datetime = currentDateTime();
        msg.result.result_response = response;
        
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        cout << "BMC 서버 닫혀있을거임~~" << endl;

        error_reply(msg, get_error_json("BMC Server Connection Error"), status_codes::InternalError, response);
        // reply_error(_request, msg, get_error_json("BMC Server Connection Error"), status_codes::InternalError, response);
    }
    

    // http_headers::iterator it_header;
    // for(it_header = req.headers().begin(); it_header != req.headers().end(); it_header++)
    // {
    //     cout << "REQEUST Headers : " << it_header->first << " / " << it_header->second << endl;
    // }
    

    
    // 확인용
    // response_json = response.extract_json().get();
    // cout << "********************* In pplx Task **********************" << endl;
    // cout << response_json << endl;
    // cout << "********************* Out pplx Task **********************" << endl;

    // Make m_Response
    // m_Response msg_res;
    // msg_res.result_datetime = currentDateTime();
    // msg_res.result_response = response;
    // msg_res.res_number = msg.task_number;
    // // msg_res.result_status = WORK_SUCCESS; // 응답 response의 상태에 따라 다르게 해야할텐데
    // msg.result = msg_res;


    /*Before Move 테스트용 출력 */
    // cout << "taskmap size : " << task_map.size() << endl;
    // cout << "list size : " << t_manager->list_request.size() << endl;
    // // 옮기기전 acc맵 하나 그거에 리스트 하나 있던거 1,1 에서  옮기면 acc맵 complete맵 2개 acc맵에 리스트는 0 해서 2,0
    // // 하고 comple에 리스트 1 나와야지

    // cout << " ------------  Info -----------------" << endl;
    // cout << "t_list tasknum : " << t_manager->list_request.front().task_number << endl;
    // cout << "t_list host : " << t_manager->list_request.front().host << endl;
    // cout << "t_list uri : " << t_manager->list_request.front().uri << endl;
    // cout << "t_list method : " << t_manager->list_request.front().method << endl;
    // cout << "t_list datetime : " << t_manager->list_request.front().request_datetime << endl;
    // cout << "t_list result->resnum : " << t_manager->list_request.front().result.res_number << endl;
    // cout << "t_list result->datetime : " << t_manager->list_request.front().result.result_datetime << endl;
    

    // work_after_request_process(t_manager, c_manager, msg);
    c_manager = work_after_request_process(t_manager, msg);


    // completed로 연결
    // c_manager->list_request.push_back(msg);
    // 아직 status같은건 처리안해줌
    // + m_Request에 m_Response는  t_manager에는 연결안되어있음 
    // c_manager로 넘어가야 비로소 연결되어있음

    
    /* After Move 테스트용 출력 */
    // cout << "After Move ----------------------------------------------" << endl;
    // cout << "taskmap size : " << task_map.size() << endl;
    // cout << "t_list size : " << t_manager->list_request.size() << endl;
    // cout << "c_list size : " << c_manager->list_request.size() << endl;


    // std::list<m_Request>::iterator iter; //이미 위에 생성햇네
    // m_Request completed_msg;
    // for(iter=c_manager->list_request.begin(); iter!=c_manager->list_request.end(); iter++)
    // {
    //     if(iter->task_number == msg.task_number)
    //     {
    //         completed_msg = *iter;
    //         break;
    //     }
    // }
    // cout << " ------------  Info -----------------" << endl;
    // cout << "c_list tasknum : " << completed_msg.task_number << endl;
    // cout << "c_list host : " << completed_msg.host << endl;
    // cout << "c_list uri : " << completed_msg.uri << endl;
    // cout << "c_list method : " << completed_msg.method << endl;
    // cout << "c_list datetime : " << completed_msg.request_datetime << endl;
    // cout << "c_list result->resnum : " << completed_msg.result.res_number << endl;
    // cout << "c_list result->datetime : " << completed_msg.result.result_datetime << endl;
    // cout << "c_list result->status : " << completed_msg.result.result_status << endl;


    // _request.reply(U(response.status_code()), response_json);
    _request.reply(response);
    return ;
}

void do_task_cmm_post(http_request _request)
{
    string uri = _request.request_uri().to_string();
    json::value jv = _request.extract_json().get();
    vector<string> uri_tokens = string_split(uri, '/');

    Task_Manager *t_manager; // 작업 매니저
    Task_Manager *c_manager; // 컴플리트 매니저

    cout << "!@#$ CMM POST TASK ~~~~" << endl;
    // cout << "Task size : " << task_map.size() << endl;
    // log(error) << "Task size : " << task_map.size();

    t_manager = category(uri_tokens);
    if(!t_manager)
    {
        // t_manager에 연결안된거 없는거
        json::value rp;
        rp[U("Error")] = json::value::string(U("Wrong Task Category. Please Check the URI."));
        _request.reply(status_codes::BadRequest, rp);
        return ;
    }

    // Make m_Request
    m_Request msg;
    http_response response;
    json::value response_json;

    msg = work_before_request_process("POST", CMM_ADDRESS, uri, jv, _request.headers());
    
    // connect to Task_manager
    t_manager->list_request.push_back(msg);
    response.headers().add("Access-Control-Allow-Origin", "*");
    response.headers().add("Access-Control-Allow-Credentials", "true");
    response.headers().add("Access-Control-Allow-Methods", "POST, GET, OPTIONS, DELETE, PATCH");
    response.headers().add("Access-Control-Max-Age", "3600");
    response.headers().add("Access-Control-Allow-Headers", "Content-Type, Accept, X-Requested-With, remember-me, X-Auth-Token");
    response.headers().add("Access-Control-Expose-Headers", "X-Auth-Token, Location");


    // /*Before Move 테스트용 출력 */
    // cout << "Before Move ----------------------------------------------" << endl;
    // cout << "taskmap size : " << task_map.size() << endl;
    // cout << "list size : " << t_manager->list_request.size() << endl;
    // // 옮기기전 acc맵 하나 그거에 리스트 하나 있던거 1,1 에서  옮기면 acc맵 complete맵 2개 acc맵에 리스트는 0 해서 2,0
    // // 하고 comple에 리스트 1 나와야지

    // cout << " ------------  Info -----------------" << endl;
    // cout << "t_list tasknum : " << t_manager->list_request.front().task_number << endl;
    // cout << "t_list host : " << t_manager->list_request.front().host << endl;
    // cout << "t_list uri : " << t_manager->list_request.front().uri << endl;
    // cout << "t_list method : " << t_manager->list_request.front().method << endl;
    // cout << "t_list datetime : " << t_manager->list_request.front().request_datetime << endl;
    // cout << "t_list result->resnum : " << t_manager->list_request.front().result.res_number << endl;
    // cout << "t_list result->datetime : " << t_manager->list_request.front().result.result_datetime << endl;

    // Uri에 따른 처리동작
    treat_uri_cmm_post(_request, msg, jv, response);

    c_manager = work_after_request_process(t_manager, msg);

    // /* After Move 테스트용 출력 */
    // cout << "After Move ----------------------------------------------" << endl;
    // cout << "taskmap size : " << task_map.size() << endl;
    // cout << "t_list size : " << t_manager->list_request.size() << endl;
    // cout << "c_list size : " << c_manager->list_request.size() << endl;


    // std::list<m_Request>::iterator iter; //이미 위에 생성햇네
    // m_Request completed_msg;
    // for(iter=c_manager->list_request.begin(); iter!=c_manager->list_request.end(); iter++)
    // {
    //     if(iter->task_number == msg.task_number)
    //     {
    //         completed_msg = *iter;
    //         break;
    //     }
    // }
    // cout << " ------------  Info -----------------" << endl;
    // cout << "c_list tasknum : " << completed_msg.task_number << endl;
    // cout << "c_list host : " << completed_msg.host << endl;
    // cout << "c_list uri : " << completed_msg.uri << endl;
    // cout << "c_list method : " << completed_msg.method << endl;
    // cout << "c_list datetime : " << completed_msg.request_datetime << endl;
    // cout << "c_list result->resnum : " << completed_msg.result.res_number << endl;
    // cout << "c_list result->datetime : " << completed_msg.result.result_datetime << endl;
    // cout << "c_list result->status : " << completed_msg.result.result_status << endl;

    cout << "*****************************     Out CMM POST     *****************************" << endl;

    _request.reply(response);
    return ;
}

void do_task_bmc_post(http_request _request)
{
    string uri = _request.request_uri().to_string();
    json::value jv = _request.extract_json().get();
    vector<string> uri_tokens = string_split(uri, '/');
    string new_uri = get_extracted_bmc_id_uri(uri);

    cout << "!@#$ BMC POST TASK ~~~ " << endl;
    cout << "BMC_uri = " << uri << endl;
    cout << "BMC_new_uri : " << new_uri << endl;
    cout << "BMC_jv = " << jv << endl;
    // cout << "taskmap size : " << task_map.size() << endl;

    Task_Manager *t_manager; // 작업 매니저
    Task_Manager *c_manager; // 컴플리트 매니저

    t_manager = category(uri_tokens);
    if(!t_manager)
    {
        json::value rp;
        rp[U("Error")] = json::value::string(U("Wrong Task Category. Please Check the URI."));
        _request.reply(status_codes::BadRequest, rp);
        return ;
    }

    // Make m_Request
    m_Request msg;

    msg = work_before_request_process("POST", module_id_table[uri_tokens[3]], uri, jv, _request.headers());

    // connect to Task_manager
    t_manager->list_request.push_back(msg);

    http_client client(msg.host);
    http_request req(methods::POST);
    // req.headers().add(U("MY_HEADER"), U("HEADER_VALUE"));
    req.set_request_uri(new_uri);
    // req.set_request_uri(msg.uri);
    req.set_body(jv);

    http_response response;
    json::value response_json;
    m_Response msg_res;

    // Send Request
    try
    {
        /* code */
        pplx::task<http_response> responseTask = client.request(req);
        response = responseTask.get();
        cout << "bmc post reply status : " << response.status_code() << endl;

        if(response.status_code() == status_codes::OK)
        {
            msg.result.result_status = WORK_SUCCESS;
            // cmm에 적용시켜야할거 있으면 여기서 해주면됨
        }
        else
            msg.result.result_status = WORK_FAIL;

        response_json = response.extract_json().get();
        msg.result.response_json = response_json;
        response.set_body(response_json);
        msg.result.result_datetime = currentDateTime();
        msg.result.result_response = response;
        
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        cout << "BMC 서버 닫혀있을거임~~" << endl;

        error_reply(msg, get_error_json("BMC Server Connection Error"), status_codes::InternalError, response);
        // reply_error(_request, msg, get_error_json("BMC Server Connection Error"), status_codes::InternalError, response);
    }


    // work_after_request_process(t_manager, c_manager, msg);
    c_manager = work_after_request_process(t_manager, msg);

    _request.reply(response);
    return ;
}

void do_task_cmm_patch(http_request _request)
{
    string uri = _request.request_uri().to_string();
    json::value jv = _request.extract_json().get();
    vector<string> uri_tokens = string_split(uri, '/');

    Task_Manager *t_manager; // 작업 매니저
    Task_Manager *c_manager; // 컴플리트 매니저

    cout << "!@#$ CMM PATCH TASK ~~~~" << endl;

    t_manager = category(uri_tokens);
    if(!t_manager)
    {
        // t_manager에 연결안된거
        json::value rp;
        rp[U("Error")] = json::value::string(U("Wrong Task Category. Please Check the URI."));
        _request.reply(status_codes::BadRequest, rp);
        return ;
    }

    m_Request msg;
    http_response response;
    json::value response_json;
    msg = work_before_request_process("PATCH", CMM_ADDRESS, uri, jv, _request.headers());

    t_manager->list_request.push_back(msg);
    response.headers().add("Access-Control-Allow-Origin", "*");
    response.headers().add("Access-Control-Allow-Credentials", "true");
    response.headers().add("Access-Control-Allow-Methods", "POST, GET, OPTIONS, DELETE, PATCH");
    response.headers().add("Access-Control-Max-Age", "3600");
    response.headers().add("Access-Control-Allow-Headers", "Content-Type, Accept, X-Requested-With, remember-me, X-Auth-Token");
    response.headers().add("Access-Control-Expose-Headers", "X-Auth-Token, Location");


    // 요청처리
    treat_uri_cmm_patch(_request, msg, jv, response);
    // msg = treat_uri_cmm_patch(_request, msg, jv);

    // work_after_request_process(t_manager, c_manager, msg);
    c_manager = work_after_request_process(t_manager, msg);

    cout << "*****************************     Out CMM PATCH     *****************************" << endl;

    _request.reply(response);
    return ;
}

void do_task_bmc_patch(http_request _request)
{
    //bmc~~
    string uri = _request.request_uri().to_string();
    json::value jv = _request.extract_json().get();
    vector<string> uri_tokens = string_split(uri, '/');
    string new_uri = get_extracted_bmc_id_uri(uri);

    cout << "!@#$ BMC PATCH TASK ~~~ " << endl;
    cout << "BMC_uri = " << uri << endl;
    cout << "BMC_new_uri : " << new_uri << endl;
    cout << "BMC_jv = " << jv << endl;
    // cout << "taskmap size : " << task_map.size() << endl;

    Task_Manager *t_manager; // 작업 매니저
    Task_Manager *c_manager; // 컴플리트 매니저

    t_manager = category(uri_tokens);
    if(!t_manager)
    {
        json::value rp;
        rp[U("Error")] = json::value::string(U("Wrong Task Category. Please Check the URI."));
        _request.reply(status_codes::BadRequest, rp);
        return ;
    }

    // Make m_Request
    m_Request msg;

    msg = work_before_request_process("PATCH", module_id_table[uri_tokens[3]], uri, jv, _request.headers());

    // connect to Task_manager
    t_manager->list_request.push_back(msg);

    http_client client(msg.host);
    http_request req(methods::PATCH);
    req.set_request_uri(new_uri);
    req.set_body(jv);

    http_response response;
    json::value response_json;


    // Send Request
    try
    {
        /* code */
        pplx::task<http_response> responseTask = client.request(req);
        response = responseTask.get();
        // response_json = response.extract_json().get();
        cout << "bmc patch reply status : " << response.status_code() << endl;

        // 응답 상태가 ok 200 이면 uri에 맞게 cmm반영하고 reply
        // 응답상태가 그외 (400 badrequest같은) 라면 그냥 그 응답의 상태랑 json reply
        if(response.status_code() == status_codes::OK)
        {
            // msg = treat_uri_bmc_patch(_request, msg, jv);
            // #공사중
        }
        else
        {
            // 실패, 바로응답
            error_reply(msg, get_error_json(""), response.status_code(), response);
            // reply_error(_request, msg, get_error_json(""), response.status_code(), response);
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        cout << "BMC 서버 닫혀있을거임~~" << endl;

        error_reply(msg, get_error_json("BMC Server Connection Error"), status_codes::InternalError, response);
        // reply_error(_request, msg, get_error_json("BMC Server Connection Error"), status_codes::InternalError, response);
    }
    

    // work_after_request_process(t_manager, c_manager, msg);
    c_manager = work_after_request_process(t_manager, msg);

    // _request.reply(U(response.status_code()), response_json);
    _request.reply(response);
    return ;
}

void do_task_cmm_delete(http_request _request)
{
    string uri = _request.request_uri().to_string();
    json::value jv = _request.extract_json().get();
    vector<string> uri_tokens = string_split(uri, '/');

    Task_Manager *t_manager; // 작업 매니저
    Task_Manager *c_manager; // 컴플리트 매니저

    cout << "!@#$ CMM DELETE TASK ~~~~" << endl;

    t_manager = category(uri_tokens);
    if(!t_manager)
    {
        // t_manager에 연결안된거
        json::value rp;
        rp[U("Error")] = json::value::string(U("Wrong Task Category. Please Check the URI."));
        _request.reply(status_codes::BadRequest, rp);
        return ;
    }

    m_Request msg;
    http_response response;
    json::value response_json;

    msg = work_before_request_process("DELETE", CMM_ADDRESS, uri, jv, _request.headers());

    t_manager->list_request.push_back(msg);
    response.headers().add("Access-Control-Allow-Origin", "*");
    response.headers().add("Access-Control-Allow-Credentials", "true");
    response.headers().add("Access-Control-Allow-Methods", "POST, GET, OPTIONS, DELETE, PATCH");
    response.headers().add("Access-Control-Max-Age", "3600");
    response.headers().add("Access-Control-Allow-Headers", "Content-Type, Accept, X-Requested-With, remember-me, X-Auth-Token");
    response.headers().add("Access-Control-Expose-Headers", "X-Auth-Token, Location");

    // 처리처리~~
    treat_uri_cmm_delete(_request, msg, jv, response);
    // msg = treat_uri_cmm_delete(_request, msg, jv);

    c_manager = work_after_request_process(t_manager, msg);
    cout << "*****************************     Out CMM DELETE     *****************************" << endl;

    _request.reply(response);
    return ;
}

void do_task_bmc_delete(http_request _request)
{
    // 로그인/계정관리가 되면 기능반영하고 지금은 요청보내는 틀만구현
    string uri = _request.request_uri().to_string();
    json::value jv = _request.extract_json().get();
    vector<string> uri_tokens = string_split(uri, '/');
    string new_uri = get_extracted_bmc_id_uri(uri);

    cout << "!@#$ BMC DELETE TASK ~~~ " << endl;
    cout << "BMC_uri = " << uri << endl;
    cout << "BMC_new_uri : " << new_uri << endl;
    cout << "BMC_jv = " << jv << endl;
    // cout << "taskmap size : " << task_map.size() << endl;

    Task_Manager *t_manager; // 작업 매니저
    Task_Manager *c_manager; // 컴플리트 매니저

    t_manager = category(uri_tokens);
    if(!t_manager)
    {
        json::value rp;
        rp[U("Error")] = json::value::string(U("Wrong Task Category. Please Check the URI."));
        _request.reply(status_codes::BadRequest, rp);
        return ;
    }

    m_Request msg;

    msg = work_before_request_process("DELETE", module_id_table[uri_tokens[3]], uri, jv, _request.headers());

    // connect to Task_manager
    t_manager->list_request.push_back(msg);

    http_client client(msg.host);
    http_request req(methods::DEL);
    req.set_request_uri(new_uri);
    req.set_body(jv);

    http_response response;
    json::value response_json;

    // Send Request
    try
    {
        /* code */
        pplx::task<http_response> responseTask = client.request(req);
        response = responseTask.get();
        // response_json = response.extract_json().get();
        cout << "bmc delete reply status : " << response.status_code() << endl;

        // 응답 상태가 ok 200 이면 uri에 맞게 cmm반영하고 reply
        // 응답상태가 그외 (400 badrequest같은) 라면 그냥 그 응답의 상태랑 json reply
        if(response.status_code() == status_codes::OK)
        {
            success_reply(msg, json::value::null(), status_codes::OK, response);
            // response = reply_success(_request, msg, json::value::null(), status_codes::OK, response);
            // 임시로 그냥 성공답변
        }
        else
        {
            // 실패
            error_reply(msg, get_error_json("bmc delete fail"), response.status_code(), response);
            // response = reply_error(_request, msg, get_error_json("bmc delete fail"), response.status_code(), response);
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        cout << "BMC 서버 닫혀있을거임~~" << endl;

        error_reply(msg, get_error_json("BMC Server Connection Error"), status_codes::InternalError, response);
        // response = reply_error(_request, msg, get_error_json("BMC Server Connection Error"), status_codes::InternalError, response);
    }

    c_manager = work_after_request_process(t_manager, msg);

    _request.reply(response);
    return ;
}




Task_Manager *category(vector<string> _token)
{
    string val;
    Task_Manager *point;
    for(int i=0; i<_token.size(); i++)
    {
        if(i == 3)
            break;

        val += "/";
        val += _token[i];
    }

    cout << "category in the func: " << val << endl;

    if(val == ODATA_SERVICE_ROOT_ID)
    {
        if(task_map.find(TASK_TYPE_SERVICEROOT) == task_map.end())
        {
            point = new Task_Manager();
            point->task_type = TASK_TYPE_SERVICEROOT;
            task_map.insert(make_pair(TASK_TYPE_SERVICEROOT, point));
        }
        else
            point = task_map.find(TASK_TYPE_SERVICEROOT)->second;
    }
    else if(val == ODATA_SYSTEM_ID)
    {
        if(task_map.find(TASK_TYPE_SYSTEMS) == task_map.end())
        {
            point = new Task_Manager();
            point->task_type = TASK_TYPE_SYSTEMS;
            task_map.insert(make_pair(TASK_TYPE_SYSTEMS, point));
        }
        else
            point = task_map.find(TASK_TYPE_SYSTEMS)->second;
    }
    else if(val == ODATA_MANAGER_ID)
    {
        if(task_map.find(TASK_TYPE_MANAGERS) == task_map.end())
        {
            point = new Task_Manager();
            point->task_type = TASK_TYPE_MANAGERS;
            task_map.insert(make_pair(TASK_TYPE_MANAGERS, point));
        }
        else
            point = task_map.find(TASK_TYPE_MANAGERS)->second;
    }
    else if(val == ODATA_CHASSIS_ID)
    {
        if(task_map.find(TASK_TYPE_CHASSIS) == task_map.end())
        {
            point = new Task_Manager();
            point->task_type = TASK_TYPE_CHASSIS;
            task_map.insert(make_pair(TASK_TYPE_CHASSIS, point));
        }
        else
            point = task_map.find(TASK_TYPE_CHASSIS)->second;
    }
    else if(val == ODATA_ACCOUNT_SERVICE_ID)
    {
        if(task_map.find(TASK_TYPE_ACCOUNTSERVICE) == task_map.end())
        {
            point = new Task_Manager();
            point->task_type = TASK_TYPE_ACCOUNTSERVICE;
            task_map.insert(make_pair(TASK_TYPE_ACCOUNTSERVICE, point));
        }
        else
            point = task_map.find(TASK_TYPE_ACCOUNTSERVICE)->second;
    }
    else if(val == ODATA_SESSION_SERVICE_ID)
    {
        if(task_map.find(TASK_TYPE_SESSIONSERVICE) == task_map.end())
        {
            point = new Task_Manager();
            point->task_type = TASK_TYPE_SESSIONSERVICE;
            task_map.insert(make_pair(TASK_TYPE_SESSIONSERVICE, point));
        }
        else
            point = task_map.find(TASK_TYPE_SESSIONSERVICE)->second;
    }
    else if(val == ODATA_TASK_SERVICE_ID)
    {
        if(task_map.find(TASK_TYPE_TASKSERVICE) == task_map.end())
        {
            point = new Task_Manager();
            point->task_type = TASK_TYPE_TASKSERVICE;
            task_map.insert(make_pair(TASK_TYPE_TASKSERVICE, point));
        }
        else
            point = task_map.find(TASK_TYPE_TASKSERVICE)->second;
    }
    else if(val == ODATA_EVENT_SERVICE_ID)
    {
        if(task_map.find(TASK_TYPE_EVENTSERVICE) == task_map.end())
        {
            point = new Task_Manager();
            point->task_type = TASK_TYPE_EVENTSERVICE;
            task_map.insert(make_pair(TASK_TYPE_EVENTSERVICE, point));
        }
        else
            point = task_map.find(TASK_TYPE_EVENTSERVICE)->second;
    }
    else if(val == ODATA_UPDATE_SERVICE_ID)
    {
        if(task_map.find(TASK_TYPE_UPDATESERVICE) == task_map.end())
        {
            point = new Task_Manager();
            point->task_type = TASK_TYPE_UPDATESERVICE;
            task_map.insert(make_pair(TASK_TYPE_UPDATESERVICE, point));
        }
        else
            point = task_map.find(TASK_TYPE_UPDATESERVICE)->second;
    }
    else if(val == ODATA_CERTIFICATE_SERVICE_ID)
    {
        if(task_map.find(TASK_TYPE_CERTIFICATESERVICE) == task_map.end())
        {
            point = new Task_Manager();
            point->task_type = TASK_TYPE_CERTIFICATESERVICE;
            task_map.insert(make_pair(TASK_TYPE_CERTIFICATESERVICE, point));
        }
        else
            point = task_map.find(TASK_TYPE_CERTIFICATESERVICE)->second;
    }
    else
        point = 0;


    return point;
}

m_Request work_before_request_process(string _method, string _host, string _uri, json::value _jv, http_headers _header)
{
    m_Request msg;
    msg.method = _method;
    msg.host = _host;
    msg.uri = _uri; // 이게 cmm이건 bmc건 오리지널 uri가 들어가고있다는점 기억
    msg.request_json = _jv;
    msg.request_datetime = currentDateTime();
    msg.task_number = allocate_numset_num(ALLOCATE_TASK_NUM); // @@@@ LOCK

    m_Response res;
    res.res_number = msg.task_number;
    msg.result = res;
    // 해당 task_manager에 들어갈 request msg와 response msg의 사전작업

    string task_odata = ODATA_TASK_ID;
    task_odata = task_odata + "/" + to_string(msg.task_number);
    cout << "task_data in wbrp func!! : " << task_odata << endl;
    Task *task = new Task(task_odata, to_string(msg.task_number));

    Collection *col = (Collection *)g_record[ODATA_TASK_ID];
    col->add_member(task);

    task->start_time = msg.request_datetime;
    task->set_payload(_header, _method, _jv, _uri);
    resource_save_json(task);
    resource_save_json(col);
    // 해당하는 task의 리소스 생성후 json파일 생성

    return msg;
}

Task_Manager* work_after_request_process(Task_Manager* _t, m_Request _msg)
{

    Task_Manager *complete;

    if(task_map.find(TASK_TYPE_COMPLETED) == task_map.end())
    {
        complete = new Task_Manager();
        complete->task_type = TASK_TYPE_COMPLETED;
        task_map.insert(make_pair(TASK_TYPE_COMPLETED, complete));
    }
    else
        complete = task_map.find(TASK_TYPE_COMPLETED)->second;

    std::list<m_Request>::iterator iter;
    for(iter=_t->list_request.begin(); iter!=_t->list_request.end(); iter++)
    {
        if(iter->task_number == _msg.task_number)
        {
            _t->list_request.erase(iter);
            break;
        }
    }
    // 해당하는 카테고리task매니저인 _t의 request list에서 해당하는 request를 지우고

    complete->list_request.push_back(_msg);
    // 해당 request msg는 completed로 이동

    string task_odata = ODATA_TASK_ID;
    task_odata = task_odata + "/" + to_string(_msg.task_number);
    ((Task *)g_record[task_odata])->end_time = _msg.result.result_datetime;
    ((Task *)g_record[task_odata])->task_state = TASK_STATE_COMPLETED;
    resource_save_json(g_record[task_odata]);
    // request의 리소스 task 정보 추가,변경

    return complete;
}


void treat_uri_cmm_post(http_request _request, m_Request& _msg, json::value _jv, http_response& _response)
{
    string uri = _request.request_uri().to_string();
    vector<string> uri_tokens = string_split(uri, '/');
    string uri_part;
    for(int i=0; i<uri_tokens.size(); i++)
    {
        if(i == 3)
            break;

        uri_part += "/";
        uri_part += uri_tokens[i];
    }
    // uri_part에는 /redfish/v1/something 까지만

    string minus_one = get_parent_object_uri(uri, "/");
    cout << "MINUS_ONE : " << minus_one << endl;

    string minus_one_last = get_current_object_name(minus_one, "/");
    if(minus_one_last == "Actions")
    {
        do_actions(_request, _msg, _jv, _response);
        return ;
    }

    if(uri == ODATA_ACCOUNT_ID)
    {
        make_account(_msg, _jv, _response);
        return ;
    }
    else if(uri == ODATA_SESSION_ID)
    {
        make_session(_msg, _jv, _response);
        return ;
    }
    else if(uri_part == ODATA_SYSTEM_ID)
    {
        string cmm_system = ODATA_SYSTEM_ID;
        cmm_system = cmm_system + "/" + CMM_ID;

        if(minus_one == cmm_system + "/LogServices")
        {
            //uri가 ~~/LogServices/Logservice_id 까지 들어왓을거니깐
            // _response = make_logentry(_request, _msg, _jv, _response);
            return ;
            // return _response;
            // #공사중 - 얜삭제가능
        } 

    }
    else if(uri_part == ODATA_MANAGER_ID)
    {
        
    }
    else if(uri == ODATA_ROLE_ID)
    {
        make_role(_request, _msg, _jv, _response);
        return ;
    }
    else if(uri == ODATA_EVENT_DESTINATION_ID)
    {
        make_subscription(_msg, _jv, _response);
        return ;
    }

    // 위에서 안걸리면 에러
    error_reply(_msg, get_error_json("URI Input Error in treat POST"), status_codes::NotImplemented, _response);
    return ;
}


void treat_uri_cmm_patch(http_request _request, m_Request& _msg, json::value _jv, http_response& _response)
{
    string uri = _request.request_uri().to_string();
    vector<string> uri_tokens = string_split(uri, '/');
    string uri_part;
    for(int i=0; i<uri_tokens.size(); i++)
    {
        if(i == 3)
            break;

        uri_part += "/";
        uri_part += uri_tokens[i];
    }
    // uri_part에는 /redfish/v1/something 까지만

    string minus_one = get_parent_object_uri(uri, "/");
    cout << "MINUS_ONE INFO : " << minus_one << endl;

    if(uri_part == ODATA_ACCOUNT_SERVICE_ID)
    {
        // /redfish/v1/AccountService 처리
        if(uri == ODATA_ACCOUNT_SERVICE_ID)
        {
            if(patch_account_service(_jv, ODATA_ACCOUNT_SERVICE_ID))
            {
                resource_save_json(g_record[uri]);
                success_reply(_msg, record_get_json(uri), status_codes::OK, _response);
                return ;
            }
            else
            {
                error_reply(_msg, get_error_json("Error Occur in AccountService PATCH"), status_codes::BadRequest, _response);
                return ;
            }
        }

        // /redfish/v1/AccountService/Accounts/[Account_id]
        if(minus_one == ODATA_ACCOUNT_ID)
        {
            modify_account(_request, _msg, _jv, uri, _response);
            return ;
        }
        // /redfish/v1/AccountService/Roles/[Role_id]
        else if(minus_one == ODATA_ROLE_ID)
        {
            modify_role(_request, _msg, _jv, uri, _response);
            return ;
        }
    }
    else if(uri_part == ODATA_SESSION_SERVICE_ID)
    {
        // /redfish/v1/SessionService 처리
        if(uri == ODATA_SESSION_SERVICE_ID)
        {
            if(patch_session_service(_jv))
            {
                resource_save_json(g_record[uri]);
                success_reply(_msg, record_get_json(uri), status_codes::OK, _response);
                return ;
            }
            else
            {
                error_reply(_msg, get_error_json("Error Occur in SessionService PATCH"), status_codes::BadRequest, _response);
                return ;
            }
        }

    }
    else if(uri_part == ODATA_MANAGER_ID)
    {
        // 매니저는 권한 관리자급이어야함!! 나중에 권한검사 필요
        string cmm_manager = ODATA_MANAGER_ID;
        cmm_manager = cmm_manager + "/" + CMM_ID;

        // /redfish/v1/Managers/cmm_id 그 자체
        if(uri == cmm_manager)
        {
            if(patch_manager(_jv, uri))
            {
                resource_save_json(g_record[uri]);
                success_reply(_msg, record_get_json(uri), status_codes::OK, _response);
            }
            else
            {
                error_reply(_msg, get_error_json("Error Occur in Manager PATCH"), status_codes::BadRequest, _response);
            }
            return ;
        }

        // /redfish/v1/Managers/cmm_id/NetworkProtocol
        if(uri == cmm_manager + "/NetworkProtocol")
        {
            // 네트워크프로토콜 정보수정 위치
            if(!record_is_exist(uri))
            {
                // 해당 네트워크프로토콜 레코드 없음
                error_reply(_msg, get_error_json("No NetworkProtocol"), status_codes::BadRequest, _response);
                return ;
            }

            if(patch_network_protocol(_jv, uri))
            {
                // iptable수정 ㄱㄱ 그담에 save
                NetworkProtocol *net = (NetworkProtocol *)g_record[uri];
                resource_save_json(g_record[uri]);
                success_reply(_msg, record_get_json(uri), status_codes::OK, _response);
                patch_iptable(net);
                return ;
            }
            else
            {
                error_reply(_msg, get_error_json("Error Occur in NetworkProtocol PATCH"), status_codes::BadRequest, _response);
                return ;
            }
        }

        // /redfish/v1/Managers/cmm_id/FanMode
        if(uri == cmm_manager + "/FanMode")
        {
            string mode;
            // cout << " jv : " << _jv.serialize() << endl;
            if(_jv == json::value::null())
            {
                error_reply(_msg, get_error_json("Json Body is Null"), status_codes::BadRequest, _response);
                return ;
            }
            
            if(get_value_from_json_key(_jv, "Mode", mode))
            {
                if(!(mode == "Standard" || mode == "FullSpeed" || mode == "Auto"))
                {
                    error_reply(_msg, get_error_json("Incorrect Mode"), status_codes::BadRequest, _response);
                    return ;
                }
            }
            else
            {
                error_reply(_msg, get_error_json("No Mode in Json Body"), status_codes::BadRequest, _response);
                return ;
            }

            patch_fan_mode(mode, uri);

            success_reply(_msg, json::value::null(), status_codes::OK, _response);
            return ;
        }

        // /redfish/v1/Managers/cmm_id/EthernetInterfaces/[Ethernet_id]
        if(minus_one == cmm_manager + "/EthernetInterfaces")
        {
            // Manager eth 뿐만 아니라 System eth까지 검사해야됨(같이 수정됨)
            string sys_uri = ODATA_SYSTEM_ID;
            string eth_num = get_current_object_name(uri, "/");
            sys_uri = sys_uri + "/" + CMM_ID + "/EthernetInterfaces";
            sys_uri = sys_uri + "/" + eth_num;
            if(!record_is_exist(uri) || !record_is_exist(sys_uri))
            {
                // 해당 이더넷 레코드 없음
                error_reply(_msg, get_error_json("No EthernetInterface"), status_codes::BadRequest, _response);
                return ;
            }

            // EthernetInterfaces PATCH를 Manager에서 하면 System에도 반영되고
            // System에선 지원안하는걸로
            if(patch_ethernet_interface(_jv, uri, 1) && patch_ethernet_interface(_jv, sys_uri, 0))
            {
                resource_save_json(g_record[uri]);
                resource_save_json(g_record[sys_uri]);
                success_reply(_msg, record_get_json(uri), status_codes::OK, _response);
                return ;
            }
            else
            {
                error_reply(_msg, get_error_json("Error Occur in EthernetInterface PATCH"), status_codes::BadRequest, _response);
                return ;
            }
        }


    }
    else if(uri_part == ODATA_SYSTEM_ID)
    {
        string cmm_system = ODATA_SYSTEM_ID;
        cmm_system = cmm_system + "/" + CMM_ID;

        if(uri == cmm_system)
        {
            if(patch_system(_jv, uri))
            {
                resource_save_json(g_record[uri]);
                success_reply(_msg, record_get_json(uri), status_codes::OK, _response);
            }
            else
            {
                error_reply(_msg, get_error_json("Error Occur in System PATCH"), status_codes::BadRequest, _response);
            }
            return ;
        }
    }
    else if(uri_part == ODATA_CHASSIS_ID)
    {
        string cmm_chassis = ODATA_CHASSIS_ID;
        cmm_chassis = cmm_chassis + "/" + CMM_ID;

        if(uri == cmm_chassis)
        {
            if(patch_chassis(_jv, uri))
            {
                resource_save_json(g_record[uri]);
                success_reply(_msg, record_get_json(uri), status_codes::OK, _response);
            }
            else
                error_reply(_msg, get_error_json("Error Occur in Chassis PATCH"), status_codes::BadRequest, _response);
            return ;
        }

        if(minus_one == cmm_chassis + "/Power/PowerControl")
        {
            if(!record_is_exist(uri))
            {
                // 해당 파워컨트롤 레코드 없음
                error_reply(_msg, get_error_json("No PowerControl"), status_codes::BadRequest, _response);
                return ;
            }

            if(patch_power_control(_jv, uri))
            {
                resource_save_json(g_record[uri]);
                success_reply(_msg, record_get_json(uri), status_codes::OK, _response);
            }
            else
                error_reply(_msg, get_error_json("Error Occur in PowerControl PATCH"), status_codes::BadRequest, _response);
            return ;
        }
    }
    else if(uri_part == ODATA_EVENT_SERVICE_ID)
    {
        if(uri == ODATA_EVENT_SERVICE_ID)
        {
            // 서비스패치
            if(patch_event_service(_jv, uri))
                success_reply(_msg, record_get_json(uri), status_codes::OK, _response);
            else
                error_reply(_msg, get_error_json("Error Occur in EventService PATCH"), status_codes::BadRequest, _response);
            
            return ;
        }

        if(minus_one == ODATA_EVENT_DESTINATION_ID)
        {
            // sub/[id] 패치
            if(!record_is_exist(uri))
            {
                error_reply(_msg, get_error_json("No Subscription"), status_codes::BadRequest, _response);
                return ;
            }

            if(patch_subscription(_jv, uri))
                success_reply(_msg, record_get_json(uri), status_codes::OK, _response);
            else
                error_reply(_msg, get_error_json("Error Occur in Subscription PATCH"), status_codes::BadRequest, _response);
            
            return ;
        }

    }
    else
    {
        error_reply(_msg, get_error_json("URI Input Error in Whole part"), status_codes::BadRequest, _response);
        return ;
    }

    error_reply(_msg, get_error_json("URI Input Error in Below part"), status_codes::BadRequest, _response);
    return ;
}

// m_Request treat_uri_bmc_patch(http_request _request, m_Request _msg, json::value _jv)
// {
//     string uri = _request.request_uri().to_string();
//     vector<string> uri_tokens = string_split(uri, '/');
//     string uri_part;
//     for(int i=0; i<uri_tokens.size(); i++)
//     {
//         if(i == 3)
//             break;

//         uri_part += "/";
//         uri_part += uri_tokens[i];
//     }
//     // uri_part에는 /redfish/v1/something 까지만

// // 다른예외처리들은 bmc로 요청을 보냈을때 걸러졌을거라 봄(OK 받은후 오는 함수라)
//     if(uri_part == ODATA_MANAGER_ID)
//     {
//         string bmc_manager = ODATA_MANAGER_ID;
//         bmc_manager = bmc_manager + "/" + uri_tokens[3];
        
//         // /redfish/v1/Managers/bmc_id
//         if(uri == bmc_manager)
//         {
//             patch_manager(_jv, uri);

//             _msg = reply_success(_request, _msg, record_get_json(uri), status_codes::OK);
//             return _msg;
//         }

//         // /redfish/v1/Managers/bmc_id/NetworkProtocol
//         if(uri == bmc_manager + "/NetworkProtocol")
//         {
//             patch_network_protocol(_jv, uri);
//             _msg = reply_success(_request, _msg, record_get_json(uri), status_codes::OK);
//             return _msg;
//         }

//         // /redfish/v1/Managers/bmc_id/FanMode
//         if(uri == bmc_manager + "/FanMode")
//         {
//             string mode;
//             mode = _jv.at("Mode").as_string();
//             patch_fan_mode(mode, uri);

//             _msg = reply_success(_request, _msg, json::value::null(), status_codes::OK);
//             return _msg;
//         }

//         // /redfish/v1/Managers/bmc_id/AccountService
//         if(uri == bmc_manager + "/AccountService")
//         {
//             patch_account_service(_jv, uri);
//             _msg = reply_success(_request, _msg, record_get_json(uri), status_codes::OK);
//             return _msg;
//         }

//         string minus_one = get_parent_object_uri(uri, "/");
//         cout << "MINUS_ONE INFO : " << minus_one << endl;

//         // /redfish/v1/Managers/bmc_id/AccountService/Accounts/[Account_id]
//         if(minus_one == bmc_manager + "/AccountService/Accounts")
//         {
//             //modify_account 수정후 ok받은 여기서는 바로 수행하게끔
//         }

//         // /redfish/v1/Managers/bmc_id/EthernetInterfaces/[Ethernet_id]
//         if(minus_one == bmc_manager + "/EthernetInterfaces")
//         {
//             string sys_uri = ODATA_SYSTEM_ID;
//             string eth_num = get_current_object_name(uri, "/");
//             sys_uri = sys_uri + "/" + uri_tokens[3] + "/EthernetInterfaces";
//             sys_uri = sys_uri + "/" + eth_num;

//             patch_ethernet_interface(_jv, uri, 0);
//             patch_ethernet_interface(_jv, sys_uri, 0);

//             _msg = reply_success(_request, _msg, record_get_json(uri), status_codes::OK);
//             return _msg;
//         }

//     }
//     else if(uri_part == ODATA_SYSTEM_ID)
//     {
//         string bmc_system = ODATA_SYSTEM_ID;
//         bmc_system = bmc_system + "/" + uri_tokens[3];

//         if(uri == bmc_system)
//         {
//             patch_system(_jv, uri);

//              _msg = reply_success(_request, _msg, record_get_json(uri), status_codes::OK);
//             return _msg;
//         }
//     }
//     else if(uri_part == ODATA_CHASSIS_ID)
//     {
//         string bmc_chassis = ODATA_CHASSIS_ID;
//         bmc_chassis = bmc_chassis + "/" + uri_tokens[3];

//         if(uri == bmc_chassis)
//         {
//             patch_chassis(_jv, uri);
//             _msg = reply_success(_request, _msg, record_get_json(uri), status_codes::OK);
    
//             return _msg;
//         }

//         string minus_one = get_parent_object_uri(uri, "/");
//         cout << "MINUS_ONE INFO : " << minus_one << endl;

//         if(minus_one == bmc_chassis + "/Power/PowerControl")
//         {
//             patch_power_control(_jv, uri);
//             _msg = reply_success(_request, _msg, record_get_json(uri), status_codes::OK);
    
//             return _msg;
//         }

//     }

//     // 그럴일은 없겠지만
//     // _msg = reply_error(_request, _msg, get_error_json("Why error?"), status_codes::BadRequest);
//     // #공사중
//     return _msg;
// }


void treat_uri_cmm_delete(http_request _request, m_Request& _msg, json::value _jv, http_response& _response)
{
    string uri = _request.request_uri().to_string();
    vector<string> uri_tokens = string_split(uri, '/');
    string uri_part;
    for(int i=0; i<uri_tokens.size(); i++)
    {
        if(i == 3)
            break;

        uri_part += "/";
        uri_part += uri_tokens[i];
    }
    // uri_part에는 /redfish/v1/something 까지만

    string minus_one = get_parent_object_uri(uri, "/");
    cout << "MINUS_ONE INFO : " << minus_one << endl;

    if(uri_part == ODATA_ACCOUNT_SERVICE_ID)
    {
        // /redfish/v1/AccountService/Accounts
        // if(uri == ODATA_ACCOUNT_ID)
        // {
            // remove_account(_msg, _jv, uri, ODATA_ACCOUNT_SERVICE_ID, _response);
            // return ;
        // }
        if(minus_one == ODATA_ACCOUNT_ID)
        {
            remove_account(_msg, _jv, uri, ODATA_ACCOUNT_SERVICE_ID, _response);
            return ;
        }
        // /redfish/v1/AccountService/Accounts/[id]로 들어오면 삭제되게끔 변경해놓음
        
        if(minus_one == ODATA_ROLE_ID)
        {
            remove_role(_request, _msg, _jv, ODATA_ACCOUNT_SERVICE_ID, _response);
            return ;
        }
    }
    else if(uri_part == ODATA_SESSION_SERVICE_ID)
    {
        if(uri == ODATA_SESSION_ID)
        {
            // 로그아웃!
            remove_session(_request, _msg, _response);
            return ;
        }
    }
    else if(uri_part == ODATA_EVENT_SERVICE_ID)
    {
        if(minus_one == ODATA_EVENT_DESTINATION_ID)
        {
            remove_subscription(_msg, uri, ODATA_EVENT_SERVICE_ID, _response);
            return ;
        }
    }


    error_reply(_msg, get_error_json("URI Input Error in treat delete"), status_codes::BadRequest, _response);
    return ;
}



void do_actions(http_request _request, m_Request& _msg, json::value _jv, http_response& _response)
{
    string uri = _request.request_uri().to_string();
    string resource_uri = get_parent_object_uri(get_parent_object_uri(uri, "/"), "/");
    string action_uri = get_current_object_name(uri, "/");

    cout << "Full uri : " << uri << endl;
    cout << "Resource : " << resource_uri << endl;
    cout << "Action : " << action_uri << endl;

    if(!record_is_exist(resource_uri))
    {
        error_reply(_msg, get_error_json("URI Input Error in Resource part"), status_codes::BadRequest, _response);
        return ;
    }

    string action_by, action_what;
    action_by = get_parent_object_uri(action_uri, ".");
    action_what = get_current_object_name(action_uri, ".");
    cout << "By : " << action_by << endl;
    cout << "What : " << action_what << endl;

    if(action_by == "Bios")
    {
        // _msg = act_bios(_request, _msg, _jv, resource_uri, action_what);
        // return _msg;
    }
    else if(action_by == "Certificate")
    {
        act_certificate(_msg, _jv, resource_uri, action_what, _response);
        return ;
      
    }
    else if(action_by == "CertificateService")
    {
        act_certificate_service(_msg, _jv, resource_uri, action_what, _response);
        return ;
    }
    else if(action_by == "ComputerSystem")
    {
        act_system(_msg, _jv, resource_uri, action_what, _response);
        return ;
    }
    else if(action_by == "Chassis")
    {}
    else if(action_by == "Manager")
    {}
    else if(action_by == "EventService")
    {
        act_eventservice(_msg, _jv, resource_uri, action_what, _response);
        return ;
    }
    else if(action_by == "LogService")
    {
        act_logservice(_msg, _jv, resource_uri, action_what, _response);
        return ;
    }
    else if(action_by == "UpdateService")
    {}
    else if(action_by == "VirtualMedia")
    {
        act_virtualmedia(_msg, _jv, resource_uri, action_what, _response);
        return ;
    }
    else
    {
        error_reply(_msg, get_error_json("URI Input Error in action_by check"), status_codes::BadRequest, _response);
        return ;
    }

    // action_by로 캐스팅이 안돼서 action_by로 그냥 액션있는리소스들중에 뭔지 비교일일이 해야함
    // 그렇게 해서 리소스찾고 거기에 actions맵 에 actions[actions_what]으로 하면 해당 Actions구조체 접근가능
    // 액션왓으로 맵에 접근까지 되면 맞는 uri긴 하겟네 굳이 target하고inputuri랑 비교안해도될거같고
    // 찾으면 함수호출 ㅇㅋ

    error_reply(_msg, get_error_json("오면안되는곳"), status_codes::BadRequest, _response);
    return ;
}

// m_Request act_bios(http_request _request, m_Request _msg, json::value _jv, string _resource, string _what)
// {
//     Bios *bios = (Bios *)g_record[_resource];
//     if(bios->actions.find(_what) == bios->actions.end())
//     {
//         _msg = reply_error(_request, _msg, get_error_json("No Action in Bios"), status_codes::BadRequest);
//         return _msg;
//     }

//     if(_what == "ResetBios")
//     {
//         if(!(bios->ResetBios()))
//         {
//             _msg = reply_error(_request, _msg, get_error_json("Problem occur in ResetBios()"), status_codes::BadRequest);
//             return _msg;
//         }

//         _msg = reply_success(_request, _msg, json::value::null(), status_codes::OK);
//         return _msg;
//     }
//     else if(_what == "ChangePassword")
//     {
//         // json으로 NewPassword, OldPassword, PasswordName 3개 필요
//     }

//     // Bios는 보드오면 할 수 있는듯
// }



void act_certificate(m_Request& _msg, json::value _jv, string _resource, string _what, http_response& _response)
{
    Certificate *certi = (Certificate *)g_record[_resource];
    if(certi->actions.find(_what) == certi->actions.end())
    {
        error_reply(_msg, get_error_json("No Action in Certificate"), status_codes::BadRequest, _response);
        return ;
    }

    if(_what == "Rekey")
    {
        json::value result;
        result = certi->Rekey(_jv);
        if(result == json::value::null())
        {
            // 리퀘스트바디오류
            error_reply(_msg, get_error_json("Request Body error in Certificate Rekey"), status_codes::BadRequest, _response);
            return ;
        }
        else
        {
            json::value check;
            if(get_value_from_json_key(result, "Failed", check))
            {
                error_reply(_msg, result, status_codes::BadRequest, _response);
                return ;
            }
            else if(get_value_from_json_key(result, "CertificateCollection", check))
            {

                success_reply(_msg, result, status_codes::OK, _response);
                return ;
            }
        }
        // rsp에 Failed가 있으면 실패
        // rsp에 CertificatieCollection이 있으면 성공
    }
    else if(_what == "Renew")
    {
        json::value result;
        result = certi->Renew();
        if(result == json::value::null())
        {
            // 리퀘스트바디오류
            error_reply(_msg, get_error_json("No Files Error in Certificate ReNew"), status_codes::BadRequest, _response);
            return ;
        }
        else
        {
            json::value check;
            if(get_value_from_json_key(result, "Failed", check))
            {
                error_reply(_msg, result, status_codes::BadRequest, _response);
                return ;
            }
            else if(get_value_from_json_key(result, "CertificateCollection", check))
            {
                success_reply(_msg, result, status_codes::OK, _response);
                return ;
            }
        }
    }
}


void act_certificate_service(m_Request& _msg, json::value _jv, string _resource, string _what, http_response& _response)
{
    CertificateService *cert_service = (CertificateService *)g_record[_resource];
    if(cert_service->actions.find(_what) == cert_service->actions.end())
    {
        error_reply(_msg, get_error_json("No Action in Certificate Service"), status_codes::BadRequest, _response);
        return ;
    }
    
    if(_what == "GenerateCSR")
    {
        // json받아서 json으로 나옴
        json::value result;
        cert_service->GenerateCSR(_jv);
        json::value check;
        if(get_value_from_json_key(result, "Failed", check))
        {
            error_reply(_msg, result, status_codes::BadRequest, _response);
            return ;
        }
        else if(get_value_from_json_key(result, "CertificateCollection", check))
        {
            success_reply(_msg, result, status_codes::OK, _response);
            return ;
        }

    }
    else if(_what == "ReplaceCertificate")
    {
        if(!cert_service->ReplaceCertificate(_jv))
        {
            error_reply(_msg, get_error_json("Problem occur in ReplaceCertificate()"), status_codes::BadRequest, _response);
            return ;
        }

        success_reply(_msg, json::value::null(), status_codes::OK, _response);
        return ;
    }
}


void act_system(m_Request& _msg, json::value _jv, string _resource, string _what, http_response& _response)
{
    Systems *system = (Systems *)g_record[_resource];
    if(system->actions.find(_what) == system->actions.end())
    {
        // action_what에 해당하는 액션정보가 없음 error
        error_reply(_msg, get_error_json("No Action in ComputerSystem"), status_codes::BadRequest, _response);
        return ;
    }

    if(!(system->Reset(_jv)))
    {
        error_reply(_msg, get_error_json("Problem occur in Reset()"), status_codes::BadRequest, _response);
        return ;
    }

    success_reply(_msg, json::value::null(), status_codes::OK, _response);
    return ;
    
}


void act_eventservice(m_Request& _msg, json::value _jv, string _resource, string _what, http_response& _response)
{
    EventService *ev_service = (EventService *)g_record[_resource];
    if(ev_service->actions.find(_what) == ev_service->actions.end())
    {
        // action_what에 해당하는 액션정보가 없음 error
        error_reply(_msg, get_error_json("No Action in EventService"), status_codes::BadRequest, _response);
        return ;
    }

    json::value result;
    result = ev_service->SubmitTestEvent(_jv);
    if(result == json::value::null())
    {
        error_reply(_msg, get_error_json("Need Correct Request Body"), status_codes::BadRequest, _response);
        return ;
    }

    // event map 을 이용해서 안에있는 이벤트들을 sub들에게 보내는 걸 한번 해볼게 일단 그 함수를 짤거고
    // 근데 보내는게 맵돌면서 이벤트 타입에 따라서 안에있는 event들을 토대로 보내 누구한테? sub한테 근데 sub들도 redfish인거에만일단
    // 보내고나면 보낸내역들은 eventlog로 기록을할뿐 그러면 안에있는 event map에서 제거를해야하냐 retryattempt가 있어서
    // 음.. 이거 pplx로 해야하나? 이건.. 보여주기에도 웹쪽을 보여줄거같음 근데 웹에 보여준다는게 로그로 가면되잖아.. 아하
    // 그럼 pplx랑 retry같은건 고려하지않고 단순히 이벤트를 보내는거만 한번 짜볼게

    success_reply(_msg, result, status_codes::OK, _response);
    return ;
}


void act_logservice(m_Request& _msg, json::value _jv, string _resource, string _what, http_response& _response)
{
    LogService *log_service = (LogService *)g_record[_resource];
    if(log_service->actions.find(_what) == log_service->actions.end())
    {
        // action_what에 해당하는 액션정보가 없음 error
        error_reply(_msg, get_error_json("No Action in LogService"), status_codes::BadRequest, _response);
        return ;
    }

    // 클리어로그밖에 없으니까 그냥 바로 실행
    if(!(log_service->ClearLog()))
    {
        error_reply(_msg, get_error_json("Problem occur in ClearLog()"), status_codes::BadRequest, _response);
        return ;
    }

    success_reply(_msg, json::value::null(), status_codes::OK, _response);
    return ;
}


void act_virtualmedia(m_Request& _msg, json::value _jv, string _resource, string _what, http_response& _response)
{
    // 얘는 리소스가 무조건 virtualmedia가 아니라 insert면 virtual컬렉션 eject면 virtualmedia리소스임 그래서 리소스부터 따고
    // 들어갈수가없네 _what부터 구분해야겟네

    if(!(_what == "InsertMediaCD" || _what == "InsertMediaUSB" || _what == "EjectMedia"))
    {
        error_reply(_msg, get_error_json("No Action in VirtualMedia"), status_codes::BadRequest, _response);
        return ;
    }

    json::value result_value;
    VirtualMedia *vm = nullptr;
    unsigned int id_num;
    string type;

    if(_what == "EjectMedia")
    {
        VirtualMedia *v_media = (VirtualMedia *)g_record[_resource];
        result_value = v_media->EjectMedia();
    }
    else
    {
        string odata = _resource;
        
        if(_what == "InsertMediaCD")
        {
            id_num = allocate_numset_num(ALLOCATE_VM_CD_NUM);
            type = "CD";
            odata = odata + "/CD" + to_string(id_num);
            VirtualMedia *v_media = new VirtualMedia(odata);
            v_media->media_type.push_back("CD");
            vm = v_media;
            vm->id = "CD" + to_string(id_num);
        }
        else if(_what == "InsertMediaUSB")
        {
            id_num = allocate_numset_num(ALLOCATE_VM_USB_NUM);
            type = "USB";
            odata = odata + "/USB" + to_string(id_num);
            VirtualMedia *v_media = new VirtualMedia(odata);
            v_media->media_type.push_back("USBStick");
            vm = v_media;
            vm->id = "USB" + to_string(id_num);
        }

        vm->name = "VirtualMedia";
        vm->connected_via = "URI";
        vm->inserted = false;
        vm->write_protected = true;

        result_value = vm->InsertMedia(_jv);
    }


    if(result_value == json::value::null())
    {
        //error
        if(vm != nullptr)
        {
            // insert에 실패하면 만들었던 리소스도 다시 지워줘야함
            delete(vm);
            if(type == "CD")
                delete_numset_num(ALLOCATE_VM_CD_NUM, id_num);
            else if(type == "USB")
                delete_numset_num(ALLOCATE_VM_USB_NUM, id_num);
        }
        error_reply(_msg, get_error_json("Problem Occur in VirtualMedia Actions"), status_codes::BadRequest, _response);
        return ;
    }
    else
    {
        //success
        if(vm != nullptr)
        {
            Collection *col = (Collection *)g_record[_resource];
            col->add_member(vm);
            resource_save_json(col);
            resource_save_json(vm);
        }

        success_reply(_msg, result_value, status_codes::OK, _response);
        return ;
    }
}


void make_account(m_Request& _msg, json::value _jv, http_response& _response)
{
    string user_name;
    string password;
    string odata_id;
    string role_id = "ReadOnly";
    Account *account;
    bool enabled = true;

    if(((AccountService *)g_record[ODATA_ACCOUNT_SERVICE_ID])->service_enabled == false)
    {
        error_reply(_msg, get_error_json("Account Service isn't Enabled"), status_codes::BadRequest, _response);
        return ;
    }

    if(get_value_from_json_key(_jv, "UserName", user_name) == false
    || get_value_from_json_key(_jv, "Password", password) == false)
    {
        error_reply(_msg, get_error_json("No UserName or Password"), status_codes::BadRequest, _response);
        return ;
    } // json request body에 UserName, Password없으면 BadRequest
  

    unsigned int min_pass_length = ((AccountService *)g_record[ODATA_ACCOUNT_SERVICE_ID])->min_password_length;
    unsigned int max_pass_length = ((AccountService *)g_record[ODATA_ACCOUNT_SERVICE_ID])->max_password_length;
    if(password.size() < min_pass_length)
    {
        error_reply(_msg, get_error_json("Password length must be at least " + to_string(min_pass_length)), status_codes::BadRequest, _response);
        return ;
    }// password 길이가 짧으면 BadRequest

    if(password.size() > max_pass_length)
    {
        error_reply(_msg, get_error_json("Password length must not exceed " + to_string(max_pass_length)), status_codes::BadRequest, _response);
        return ;
    }// password 길이가 길면 BadRequest

    // Collection col = *((Collection *)g_record[ODATA_ACCOUNT_ID]); // 이건 왜 터졌지?
    Collection *col = (Collection *)g_record[ODATA_ACCOUNT_ID];
    std::vector<Resource *>::iterator iter;
    for(iter=col->members.begin(); iter!=col->members.end(); iter++)
    {
        if(((Account *)(*iter))->user_name == user_name)
        {
            error_reply(_msg, get_error_json("UserName already exists"), status_codes::Conflict, _response);
            return ;
        }
    }// username이 이미 있으면 Conflict

    if(_jv.as_object().find("RoleId") != _jv.as_object().end())
    {
        odata_id = ODATA_ROLE_ID;
        role_id = _jv.at("RoleId").as_string();
        odata_id = odata_id + '/' + role_id;
        // Check role exist
        if(!record_is_exist(odata_id))
        {
            error_reply(_msg, get_error_json("No Role"), status_codes::BadRequest, _response);
            return ;
        }
    }

    if(_jv.as_object().find("Enabled") != _jv.as_object().end())
        enabled = _jv.at("Enabled").as_bool();

    odata_id = ODATA_ACCOUNT_ID;
    string acc_id = to_string(allocate_numset_num(ALLOCATE_ACCOUNT_NUM));

    odata_id = odata_id + "/" + acc_id;
    // cout << "New account : " << odata_id << endl;
    account = new Account(odata_id, acc_id, role_id);
    account->name = "User Account";
    account->user_name = user_name;
    account->password = password;
    account->enabled = enabled;
    account->locked = false;

    // 컬렉션에 add
    ((Collection *)g_record[ODATA_ACCOUNT_ID])->add_member(account);
    resource_save_json(account);
    resource_save_json(g_record[ODATA_ACCOUNT_ID]);

    _response.set_status_code(status_codes::Created);
    _response.set_body(record_get_json(odata_id));

    _msg.result.response_json = record_get_json(odata_id);
    _msg.result.result_datetime = currentDateTime();
    _msg.result.result_status = WORK_SUCCESS;
    _msg.result.result_response = _response;

    return ;
}


void make_session(m_Request& _msg, json::value _jv, http_response& _response)
{
    string user_name;
    string password;
    string odata_id;
    Account *account;

    if(((SessionService *)g_record[ODATA_SESSION_SERVICE_ID])->service_enabled == false)
    {
        error_reply(_msg, get_error_json("Session Service isn't Enabled"), status_codes::BadRequest, _response);
        return ;
    }

    if(get_value_from_json_key(_jv, "UserName", user_name) == false
    || get_value_from_json_key(_jv, "Password", password) == false)
    {
        error_reply(_msg, get_error_json("No UserName or Password"), status_codes::BadRequest, _response);
        return ;
    } // json request body에 UserName, Password없으면 BadRequest


    Collection *col = (Collection *)g_record[ODATA_ACCOUNT_ID];
    std::vector<Resource *>::iterator iter;
    bool exist=false;
    for(iter=col->members.begin(); iter!=col->members.end(); iter++)
    {
        if(((Account *)(*iter))->user_name == user_name)
        {
            // 계정존재
            exist = true;
            account = ((Account *)(*iter));
            break;
        }
    }

    if(!exist)
    {
        error_reply(_msg, get_error_json("No Account using that UserName"), status_codes::BadRequest, _response);
        return ;
    } // 입력한 username에 해당하는 계정없음

    if(account->password != password)
    {
        error_reply(_msg, get_error_json("Password does not match"), status_codes::BadRequest, _response);
        return ;
    } // 비밀번호 맞지않음


    odata_id = ODATA_SESSION_ID;
    string token = generate_token(16);
    string session_id = to_string(allocate_numset_num(ALLOCATE_SESSION_NUM));
    odata_id = odata_id + '/' + session_id;
    // odata_id = odata_id + '/' + token;
    Session *session = new Session(odata_id, session_id, account);
    session->x_token = token;
    // Session *session = new Session(odata_id, token, account);
    ((Collection *)g_record[ODATA_SESSION_ID])->add_member(session);
    session->start();
    resource_save_json(session);
    resource_save_json(g_record[ODATA_SESSION_ID]);


    _response.set_status_code(status_codes::Created);
    _response.headers().add("X-Auth-Token", token);
    _response.headers().add("Location", session->odata.id);

    _msg.result.result_datetime = currentDateTime();
    _msg.result.result_status = WORK_SUCCESS;
    _msg.result.result_response = _response;
    // response_json 반환json없음

    return ;
}


void make_role(http_request _request, m_Request& _msg, json::value _jv, http_response& _response)
{
    if(((AccountService *)g_record[ODATA_ACCOUNT_SERVICE_ID])->service_enabled == false)
    {
        error_reply(_msg, get_error_json("Account Service isn't Enabled"), status_codes::BadRequest, _response);
        return ;
    }

    /* 어카운트 서비스 이용불가면 막히고 .. 관리자만 생성이가능하고.. role 이름(id)를 받아서 만들거고
    그러면 롤이름 중복검사 해줘야하고 일단은 다 넣을수있게 할게 프리빌리지 어차피 나중에 권한별로 기능접근으로 수정하면
    그때 바꾸는걸로..

    */

   if(!_request.headers().has("X-Auth-Token"))
    {
        // 로그인이 안되어있음(X-Auth-Token이 존재하지않음)
        error_reply(_msg, get_error_json("Login Required"), status_codes::BadRequest, _response);
        return ;
    }

    string session_token = _request.headers()["X-Auth-Token"];

    if(!is_session_valid(session_token))
    {
        // 세션이 유효하지않음(X-Auth-Token은 존재하나 유효하지 않음)
        error_reply(_msg, get_error_json("Session Unvalid"), status_codes::BadRequest, _response);
        return ;
    }

    string session_uri = get_session_odata_id_by_token(session_token);
    // session_uri = session_uri + "/" + session_token;
    cout << "SESSION ! : " << session_uri << endl;

    // Session session = *((Session *)g_record[session_uri]);
    Session *session = (Session *)g_record[session_uri];

    string role_id;
    json::value privileges = json::value::array();
    if(session->account->role->id == "Administrator")
    {
        // 관리자만 가능
        // RoleId 로 받을거고 중복검사후 프리빌리지검사 ㄱㄱ
        // if(!(get_value_from_json_key(_jv, "RoleId", role_id)))
        // {
        //     // role id가 입력으로 안왔으니 에러
        //     _msg = reply_error(_request, _msg, get_error_json("Need RoleId"), status_codes::BadRequest);
        //     return _msg;
        // }
        get_value_from_json_key(_jv, "RoleId", role_id);

        if(role_id == "")
        {
            // role id를 입력을 안해서 "" 가 들어왔거나 ""를 입력한거지 둘다 아웃으로 한번에 거르자
            error_reply(_msg, get_error_json("Need RoleId"), status_codes::BadRequest, _response);
            return ;
        }

        Collection *col = (Collection *)g_record[ODATA_ROLE_ID];
        std::vector<Resource *>::iterator iter;
        for(iter = col->members.begin(); iter != col->members.end(); iter++)
        {
            if(((Role *)(*iter))->id == role_id)
            {
                error_reply(_msg, get_error_json("RoleId is already in use"), status_codes::BadRequest, _response);
                return ;
            }
        } // Role id 중복검사

        vector<string> tmp_pri;
        get_value_from_json_key(_jv, "Privileges", privileges);
        for(int i=0; i<privileges.size(); i++)
        {
            if(!(check_role_privileges(privileges[i].as_string())))
            {
                error_reply(_msg, get_error_json("Incorrect Privileges"), status_codes::BadRequest, _response);
                return ;
            }
            tmp_pri.push_back(privileges[i].as_string());
        }

        if(tmp_pri.size() == 0)
        {
            // 제대로 된 권한이 하나도 없었다는 것으로 아웃
            error_reply(_msg, get_error_json("Need Correct Privileges"), status_codes::BadRequest, _response);
            return ;
        }

        // 여기까지 통과했으면 롤아이디도 있고 프리빌리지도 1개이상이니깐 role 생성
        Role *new_role = new Role(col->odata.id + "/" + role_id, role_id);
        new_role->name = "User Role";
        new_role->assigned_privileges = tmp_pri;
        col->add_member(new_role);
        resource_save_json(new_role);
        resource_save_json(col);

        success_reply(_msg, record_get_json(new_role->odata.id), status_codes::Created, _response);
        return ;
    }
    else
    {
        error_reply(_msg, get_error_json("Only Administrator Can Use"), status_codes::BadRequest, _response);
        return ;
    }
}



void make_subscription(m_Request& _msg, json::value _jv, http_response& _response)
{
    // todo : protocol, policy, subs_type은 들어갈수있는값이 정해져있어서 검사하는것 추가하기
    string uri = ODATA_EVENT_DESTINATION_ID;
    string service_uri = get_parent_object_uri(uri, "/");

    // 필수요소 destination, protocol 존재여부 검사
    string destination;
    if(!(get_value_from_json_key(_jv, "Destination", destination)))
    {
        error_reply(_msg, get_error_json("Need Destination in Request Body"), status_codes::BadRequest, _response);
        return ;
    }

    string protocol;
    if(!(get_value_from_json_key(_jv, "Protocol", protocol)))
    {
        error_reply(_msg, get_error_json("Need Protocol in Request Body"), status_codes::BadRequest, _response);
        return ;
    }
    
    // 나머지 멤버변수들 처리
    string policy, context, subs_type;

    if(!(get_value_from_json_key(_jv, "Context", context)))
        context = "";

    if(get_value_from_json_key(_jv, "DeliveryRetryPolicy", policy))
    {
        // enum 검사

    }
    else
        policy = "SuspendRetries";

    if(get_value_from_json_key(_jv, "SubscriptionType", subs_type))
    {
        // enum 검사
    }
    else
        subs_type = "RedfishEvent";
    


    // EventDestination 생성
    unsigned int sub_num = allocate_numset_num(ALLOCATE_SUBSCRIPTION_NUM);
    EventService *service = (EventService *)g_record[service_uri];
    // init_event_destination(service->subscriptions, to_string(sub_num));
    string odata_id = uri + "/" + to_string(sub_num);

    // record_is_exist검사... 는 있으면 안되는 구조긴 함 allocate를 한거라서 그래서 뺌

    EventDestination *event_dest = new EventDestination(odata_id, to_string(sub_num));

    event_dest->destination = destination;
    event_dest->protocol = protocol;
    event_dest->context = context;
    event_dest->subscription_type = subs_type;
    event_dest->delivery_retry_policy = policy;
    event_dest->status.state = STATUS_STATE_ENABLED;
    event_dest->status.health = STATUS_HEALTH_OK;
    event_dest->name = "Event Subscription";

    service->subscriptions->add_member(event_dest);

    resource_save_json(event_dest);
    resource_save_json(service->subscriptions);


    success_reply(_msg, record_get_json(event_dest->odata.id), status_codes::Created, _response);
    return ;
}

// m_Request make_logentry(http_request _request, m_Request _msg, json::value _jv)
// http_response make_logentry(http_request _request, m_Request& _msg, json::value _jv, http_response _response)
// {
//     string uri = _request.request_uri().to_string();

//     if(((LogService *)g_record[uri])->entry == nullptr){
//         ((LogService *)g_record[uri])->entry = new Collection(uri + "/Entries", ODATA_LOG_ENTRY_COLLECTION_TYPE);
//         ((LogService *)g_record[uri])->entry->name = "Log Entry Collection";
//     }
//     // entry collection없으면 만들어줌

//     //log_entry하나만들고 연결
//     string entry_id, entry_odata_id;
//     if(!get_value_from_json_key(_jv, "EntryID", entry_id))
//     {
//         _response = reply_error(_request, _msg, get_error_json("Need EntryID"), status_codes::BadRequest, _response);
//         return _response;
//         // _msg = reply_error(_request, _msg, get_error_json("Need EntryID"), status_codes::BadRequest);
//         // return _msg;
//     }
//     // 받는걸로 EntryID필요하게끔함

//     entry_odata_id = ((LogService *)g_record[uri])->entry->odata.id + "/" + entry_id;
//     if(record_is_exist(entry_odata_id))
//     {
//         _response = reply_error(_request, _msg, get_error_json("EntryID is Already Exist"), status_codes::BadRequest, _response);
//         return _response;
//         // _msg = reply_error(_request, _msg, get_error_json("EntryID is Already Exist"), status_codes::BadRequest);
//         // return _msg;
//     } // 중복검사

//     init_log_entry(((LogService *)g_record[uri])->entry, entry_id);
//     _response = reply_success(_request, _msg, record_get_json(entry_odata_id), status_codes::Created, _response);
//     return _response;
//     // _msg = reply_success(_request, _msg, record_get_json(entry_odata_id), status_codes::Created);
//     // return _msg;
// } // 이건 핸들러지원이아닌거라 save_json안넣었음 나중에 함수자체 삭제할것

// m_Request make_virtualmedia(http_request _request, m_Request _msg, json::value _jv)
// {
//     // __jv로 받을 변수들은 media_type, connected via, id 설정정도? 이건 mediatype+숫자
//     // 아 걍 id 숫자로 붙이자 ... 매니저에서 관리하고
//     // 이거 로그인해야하나?... - 나중에

//     string uri = _request.request_uri().to_string();
//     Manager* man = (Manager *)g_record[get_parent_object_uri(uri, "/")];
//     int num = man->vm_count + 1;

//     VirtualMedia* vm = new VirtualMedia(uri + "/" + to_string(num));
//     vm->id = get_current_object_name(vm->odata.id, "/");

//     string connected_via;
//     vector<string> mtype;

//     if(get_value_from_json_key(_jv, "ConnectedVia", connected_via))
//     {
//         vm->connected_via = connected_via;
//     }

//     json::value arr_type = json::value::array();
//     if(get_value_from_json_key(_jv, "MediaTypes", arr_type))
//     {
//         for(int i=0; i<arr_type.size(); i++)
//         {
//             mtype.push_back(arr_type[i].as_string());
//         }

//         vm->media_type = mtype;
//     }
//     // 중복검사는 아직안함

//     man->virtual_media->add_member(vm);

//     _msg = reply_success(_request, _msg, record_get_json(vm->odata.id), status_codes::Created);
//     return _msg;

// }


void modify_account(http_request _request, m_Request& _msg, json::value _jv, string _uri, http_response& _response)
{
    // /redfish/v1/AccountService/Accounts/[Account_id] 처리함수인데

    // #1 계정존재 확인우선 그 계정이 있나 보고
    // #2 patch 권한있는지 (로그인한 유저의 계정이거나, 관리자거나)
    // #3 변경시에는 유저네임 중복검사, 롤id는 롤 존재여부검사

    // service enabled 검사할건데 일단은 지금은 ODATA_ACCOUNT_SERVICE_ID로 해놓음
    // uri가 account id까지니깐 2번 get_parent해서 accountservice odata구해서 사용하면 bmc꺼도 적용 가능할 듯
    if(((AccountService *)g_record[ODATA_ACCOUNT_SERVICE_ID])->service_enabled == false)
    {
        error_reply(_msg, get_error_json("Account Service isn't Enabled"), status_codes::BadRequest, _response);
        return ;
    }

    if(!(record_is_exist(_uri)))
    {
        // 해당 계정이 없음
        error_reply(_msg, get_error_json("No Account"), status_codes::BadRequest, _response);
        return ;
    }

    cout << "바뀌기전~~ " << endl;
    cout << record_get_json(_uri) << endl;
    cout << " $$$$$$$ " << endl;

    if(!_request.headers().has("X-Auth-Token"))
    {
        // 로그인이 안되어있음(X-Auth-Token이 존재하지않음)
        error_reply(_msg, get_error_json("Login Required"), status_codes::BadRequest, _response);
        return ;
    }

    string session_token = _request.headers()["X-Auth-Token"];

    if(!is_session_valid(session_token))
    {
        // 세션이 유효하지않음(X-Auth-Token은 존재하나 유효하지 않음)
        error_reply(_msg, get_error_json("Session Unvalid"), status_codes::BadRequest, _response);
        return ;
    }

    string session_uri = get_session_odata_id_by_token(session_token);
    // session_uri = session_uri + "/" + session_token;
    cout << "SESSION ! : " << session_uri << endl;

    // Session session = *((Session *)g_record[session_uri]);
    Session *session = (Session *)g_record[session_uri];

    string last = get_current_object_name(_uri, "/"); // account_id임 last는

    bool op_role=false, op_name=false, op_pass=false; // 일괄처리를 위한 플래그
    string role_odata;
    string input_username;
    string input_password;

    if(session->account->role->id == "Administrator")
    {
        // 관리자만 role 변경가능하게 설계해봄
        string role_id;
        if(get_value_from_json_key(_jv, "RoleId", role_id))
        {
            role_odata = ODATA_ROLE_ID;
            role_odata = role_odata + "/" + role_id;

            if(!record_is_exist(role_odata))
            {
                // 해당 롤 없음
                error_reply(_msg, get_error_json(role_id + " does not exist"), status_codes::BadRequest, _response);
                return ;
            }
            op_role = true;
        }
    }

    if(session->account->role->id == "Administrator" || session->account->id == last)
    {
        // username, password 변경
        if(get_value_from_json_key(_jv, "UserName", input_username))
        {
            Collection *col = (Collection *)g_record[ODATA_ACCOUNT_ID];
            std::vector<Resource *>::iterator iter;
            for(iter=col->members.begin(); iter!=col->members.end(); iter++)
            {
                if(((Account *)(*iter))->id == last)
                    continue;

                if(((Account *)(*iter))->user_name == input_username)
                {
                    error_reply(_msg, get_error_json("UserName is already in use"), status_codes::BadRequest, _response);
                    return ;
                }

            } // user_name 중복검사

            op_name = true;
        }

        if(get_value_from_json_key(_jv, "Password", input_password))
        {
            unsigned int min = ((AccountService *)g_record[ODATA_ACCOUNT_SERVICE_ID])->min_password_length;
            unsigned int max = ((AccountService *)g_record[ODATA_ACCOUNT_SERVICE_ID])->max_password_length;
            if(input_password.length() < min || input_password.length() > max)
            {
                error_reply(_msg, get_error_json("Password Range is " + to_string(min) + " ~ " + to_string(max)), status_codes::BadRequest, _response);
                return ;
            } // password 길이 검사
            op_pass = true;   
        }

    }
    // role_id로 하기엔 role을 생성할 수도 있는 거여서 나중에 role안에 privileges에 권한 확인하는걸로 바꿔야 할 수도 있음

    if(!op_role && !op_name && !op_pass) // 다 false면
    {
        error_reply(_msg, get_error_json("Cannot Modify Account"), status_codes::BadRequest, _response);
        return ;
    }

    if(op_role){
        ((Account *)g_record[_uri])->role = ((Role *)g_record[role_odata]);
        ((Account *)g_record[_uri])->role_id = ((Role *)g_record[role_odata])->odata.id;
    }
    
    if(op_name)
        ((Account *)g_record[_uri])->user_name = input_username;

    if(op_pass)
        ((Account *)g_record[_uri])->password = input_password;
    // 검사중 error나면 적용안되고 error나지 않을때 변경일괄처리

    resource_save_json(g_record[_uri]);
    

    cout << "바꾼후~~ " << endl;
    cout << record_get_json(_uri) << endl;
    cout << "세션에 연결된 계정정보" << endl;
    cout << record_get_json(session->account->odata.id) << endl;

    success_reply(_msg, record_get_json(_uri), status_codes::OK, _response);
    return ;
}


void modify_role(http_request _request, m_Request& _msg, json::value _jv, string _uri, http_response& _response)
{
    // /redfish/v1/AccountService/Roles/[Role_id]

    // #1 해당 role 존재검사
    // #2 role 변경은 관리자만
    // 변경할게 assignedprivileges 밖에 없음
    string service_id = get_parent_object_uri(get_parent_object_uri(_uri, "/"), "/");
    if(((AccountService *)g_record[service_id])->service_enabled == false)
    {
        error_reply(_msg, get_error_json("Account Service isn't Enabled"), status_codes::BadRequest, _response);
        return ;
    }


    if(!(record_is_exist(_uri)))
    {
        // 해당 롤이 없음
        error_reply(_msg, get_error_json("No Role"), status_codes::BadRequest, _response);
        return ;
    }

    if(!_request.headers().has("X-Auth-Token"))
    {
        // 로그인이 안되어있음(X-Auth-Token이 존재하지않음)
        error_reply(_msg, get_error_json("Login Required"), status_codes::BadRequest, _response);
        return ;
    }

    string session_token = _request.headers()["X-Auth-Token"];

    if(!is_session_valid(session_token))
    {
        // 세션이 유효하지않음(X-Auth-Token은 존재하나 유효하지 않음)
        error_reply(_msg, get_error_json("Session Unvalid"), status_codes::BadRequest, _response);
        return ;
    }

    string session_uri = get_session_odata_id_by_token(session_token);
    // session_uri = session_uri + "/" + session_token;
    cout << "SESSION ! : " << session_uri << endl;

    Session *session = (Session *)g_record[session_uri];
    Role *login_role = session->account->role;
    Role *target_role = (Role *)g_record[_uri];

    if(login_role->id == "Administrator")
    {
        json::value arr = json::value::array();
        if(_jv.as_object().find("AssignedPrivileges") != _jv.as_object().end())
            arr = _jv.at("AssignedPrivileges");
        else
        {
            error_reply(_msg, get_error_json("No Privileges"), status_codes::BadRequest, _response);
            return ;
        }

        cout << "바뀌기전~~ " << endl;
        cout << record_get_json(_uri) << endl;
        cout << " $$$$$$$ " << endl;

        vector<string> store;
        for(int i=0; i<arr.size(); i++)
        {
            string input = arr[i].as_string();
            if(check_role_privileges(input))
            {
                bool exist = false;
                for(int j=0; j<target_role->assigned_privileges.size(); j++)
                {
                    if(input == target_role->assigned_privileges[j])
                    {
                        exist = true;
                        break;
                    }
                }

                if(!exist)
                    store.push_back(input); //모아서 한번에 붙이려고 store에 잠시 보관
                    // target_role->assigned_privileges.push_back(input);
                else
                    log(info) << "[" << input << "]" << " is already exist";
                    // cout << input << " 이미 있어요!!" << endl;
            }
            else
            {
                //올바른 privilege 가 아님
                error_reply(_msg, get_error_json("Incorrect Previlege"), status_codes::BadRequest, _response);
                return ;
            }
        }

        for(int i=0; i<store.size(); i++)
        {
            target_role->assigned_privileges.push_back(store[i]);
        }

        cout << "바꾼후~~ " << endl;
        cout << record_get_json(_uri) << endl;
    }
    else
    {
        // 관리자만 변경할수있음!!
        error_reply(_msg, get_error_json("Only Administrator Can Use"), status_codes::BadRequest, _response);
        return ;
    }
    // 일단 assignedprivileges 있는걸 추가하는식으로만 구현해봄 삭제는 보류

    resource_save_json(g_record[_uri]);
    success_reply(_msg, record_get_json(_uri), status_codes::OK, _response);
    return ;
}



void remove_account(m_Request& _msg, json::value _jv, string _uri, string _service_uri, http_response& _response)
{
    string username, password, odata_id;
    odata_id = _uri; //_request.request_uri().to_string(); // ## 방식변경후 추가
    // ### 방식변경 : 유저네임 패스워드 받고 /Accounts까지 받던 uri를
    // request body안받고 /Accounts/[id] 까지 받음 uri 

    if(((AccountService *)g_record[_service_uri])->service_enabled == false)
    {
        error_reply(_msg, get_error_json("Account Service isn't Enabled"), status_codes::BadRequest, _response);
        return ;
    }

    // if(get_value_from_json_key(_jv, "UserName", username) == false
    // || get_value_from_json_key(_jv, "Password", password) == false)
    // {
    //     _response = reply_error(_request, _msg, get_error_json("No UserName or Password"), status_codes::BadRequest, _response);
    //     return _response;
    //     // _msg = reply_error(_request, _msg, get_error_json("No UserName or Password"), status_codes::BadRequest);
    //     // return _msg;
    // } // json request body에 UserName, Password없으면 BadRequest
    // ## 방식변경후 주석

    string account_id = get_current_object_name(odata_id, "/"); // ## 방식변경후 추가
    bool exist=false;
    AccountService *acc_service = (AccountService *)g_record[_service_uri];
    std::vector<Resource *>::iterator iter;
    for(iter = acc_service->account_collection->members.begin(); iter != acc_service->account_collection->members.end(); iter++)
    {
        Account *t = (Account *)*iter;

        // if(t->user_name == username && t->password == password)
        // {
        //     odata_id = t->odata.id;
        //     exist = true;
        //     break;
        // } 
        // ## 방식변경후 주석
        
        if(t->id == account_id)
        {
            // odata_id = t->odata.id;
            // exist = true;
            break;
        }
    }

    if(!record_is_exist(odata_id))
    {
        error_reply(_msg, get_error_json("No Account"), status_codes::BadRequest, _response);
        return ;
    }

    // if(!exist)
    // {
    //     _response = reply_error(_request, _msg, get_error_json("No Account or Does not match Password"), status_codes::BadRequest, _response);
    //     return _response;
    //     // _msg = reply_error(_request, _msg, get_error_json("No Account or Does not match Password"), status_codes::BadRequest);
    //     // return _msg;
    // } // ## 방식변경후 주석

    cout << "지우기전~~ " << endl;
    cout << record_get_json(acc_service->account_collection->odata.id) << endl;
    cout << " $$$$$$$ " << endl;

    unsigned int id_num;
    id_num = stoi(((Account *)g_record[odata_id])->id);
    delete_numset_num(ALLOCATE_ACCOUNT_NUM, id_num);

    delete(*iter);
    acc_service->account_collection->members.erase(iter);
    // account자체 객체삭제, g_record에서 삭제, account collection에서 삭제

    delete_resource(odata_id);
    // account json파일 삭제
    resource_save_json(acc_service->account_collection);
    // collection 반영

    cout << "지운후~~ " << endl;
    cout << "지워진놈 : " << odata_id << endl;
    cout << record_get_json(acc_service->account_collection->odata.id) << endl;

    success_reply(_msg, record_get_json(acc_service->account_collection->odata.id), status_codes::OK, _response);
    return ;
}


void remove_session(http_request _request, m_Request& _msg, http_response& _response)
{
    if(((SessionService *)g_record[ODATA_SESSION_SERVICE_ID])->service_enabled == false)
    {
        error_reply(_msg, get_error_json("Session Service isn't Enabled"), status_codes::BadRequest, _response);
        return ;
    }

    // 아니근데 이거 x-auth-token가지고만 하면 a가 로그인한채로 b꺼 x토큰 보내서 로그아웃 시킬수도있는데??
    if(!(is_session_valid(_request.headers()["X-Auth-Token"])))
    {
        error_reply(_msg, get_error_json("Unvalid Session"), status_codes::BadRequest, _response);
        return ;
    }

    // 토큰에 해당하는 세션 종료
    string token = _request.headers()["X-Auth-Token"];
    string session_uri = get_session_odata_id_by_token(token);
    Session *session;

    session = (Session *)g_record[session_uri];
    // cout << "YYYY : " << session->id << endl;
    session->_remain_expires_time = 0;
    // session->cts.cancel();
    log(info) << "Session Cancel Call~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
    success_reply(_msg, json::value::null(), status_codes::OK, _response);
    return ;
}


void remove_role(http_request _request, m_Request& _msg, json::value _jv, string _service_uri, http_response& _response)
{
    // #1 AccountService enabled체크, 관리자만 가능
    // #2 지우려는 Role Record가 Predefined Role이 아니면 지움
    // #3 이때 이 지워진 Roleid로 설정된 계정들이 있으면 ReadOnly로 일괄변경해둠
    string uri = _request.request_uri().to_string();
    if(((AccountService *)g_record[_service_uri])->service_enabled == false)
    {
        error_reply(_msg, get_error_json("Account Service isn't Enabled"), status_codes::BadRequest, _response);
        return ;
    }

    if(!_request.headers().has("X-Auth-Token"))
    {
        // 로그인이 안되어있음(X-Auth-Token이 존재하지않음)
        error_reply(_msg, get_error_json("Login Required"), status_codes::BadRequest, _response);
        return ;
    }

    string session_token = _request.headers()["X-Auth-Token"];

    if(!is_session_valid(session_token))
    {
        // 세션이 유효하지않음(X-Auth-Token은 존재하나 유효하지 않음)
        error_reply(_msg, get_error_json("Session Unvalid"), status_codes::BadRequest, _response);
        return ;
    }

    string session_uri = get_session_odata_id_by_token(session_token);
    // session_uri = session_uri + "/" + session_token;
    cout << "SESSION ! : " << session_uri << endl;

    // Session session = *((Session *)g_record[session_uri]);
    Session *session = (Session *)g_record[session_uri];

    if(session->account->role->id == "Administrator")
    {
        if(!record_is_exist(uri))
        {
            error_reply(_msg, get_error_json("Role does not exist"), status_codes::BadRequest, _response);
            return ;
        }

        Role *role = (Role *)g_record[uri];
        if(role->is_predefined == true)
        {
            error_reply(_msg, get_error_json("Predefined Role Cannot DELETE"), status_codes::BadRequest, _response);
            return ;
        }

        string role_id = role->id;
        AccountService *service = ((AccountService *)g_record[_service_uri]);
        Collection *role_col = service->role_collection;
        Collection *acc_col = service->account_collection;

        vector<Resource *>::iterator iter;
        bool exist=false;
        for(iter = role_col->members.begin(); iter != role_col->members.end(); iter++)
        {
            if(((Role *)(*iter))->id == role_id)
            {
                exist = true;
                break;
            }
        }

        if(exist)
        {
            // 롤 지우기
            delete(*iter);
            role_col->members.erase(iter);
            delete_resource(uri);
            resource_save_json(role_col);
        }

        string odata = role_col->odata.id + "/ReadOnly";
        Role *read_only = (Role *)g_record[odata];
        for(iter = acc_col->members.begin(); iter != acc_col->members.end(); iter++)
        {
            Account *acc_tmp = (Account *)*iter;
            if(acc_tmp->role_id == role_id)
            {
                acc_tmp->role_id = read_only->id;
                acc_tmp->role = read_only;
                resource_save_json(acc_tmp);
            }
        }


        success_reply(_msg, record_get_json(role_col->odata.id), status_codes::OK, _response);
        return ;
    }
    else
    {
        error_reply(_msg, get_error_json("Only Administrator Can Use"), status_codes::BadRequest, _response);
        return ;
    }
    
}


void remove_subscription(m_Request& _msg, string _uri, string _service_uri, http_response& _response)
{
    string uri = _uri; //_request.request_uri().to_string();
    if(!record_is_exist(uri))
    {
        error_reply(_msg, get_error_json("No Subscription"), status_codes::BadRequest, _response);
        return ;
    }

    EventService *service = (EventService *)g_record[_service_uri];
    Collection *col = service->subscriptions;
    std::vector<Resource *>::iterator iter;
    for(iter = col->members.begin(); iter != col->members.end(); iter++)
    {
        EventDestination *del = (EventDestination *)*iter;
        if(del->id == get_current_object_name(uri, "/"))
        {
            break;
        }
    }

    cout << "지우기전!! " << endl;
    cout << record_get_json(col->odata.id) << endl;
    cout << " $$$$$$$ " << endl;

    unsigned int id_num;
    id_num = stoi(get_current_object_name(uri, "/"));
    delete_numset_num(ALLOCATE_SUBSCRIPTION_NUM, id_num);

    delete(*iter);
    col->members.erase(iter);
    delete_resource(uri);
    resource_save_json(col);

    cout << "지운후~~ " << endl;
    cout << "지워진놈 : " << uri << endl;
    cout << record_get_json(col->odata.id) << endl;

    success_reply(_msg, json::value::null(), status_codes::OK, _response);
    return ;
}

bool patch_account_service(json::value _jv, string _record_uri)
{
    cout << "바뀌기전~~ " << endl;
    cout << record_get_json(_record_uri) << endl;
    cout << " $$$$$$$ " << endl;

    bool result = false;

    // Min & Max Password Length
    unsigned int min_after;
    unsigned int min_before = ((AccountService *)g_record[_record_uri])->min_password_length;
    if(get_value_from_json_key(_jv, "MinPasswordLength", min_after))
    {
        // ((AccountService *)g_record[_record_uri])->min_password_length = min;
        if(min_after == 0)
        {
            // 이건 minpassword가 입력으로 왔지만 0을 입력했다는걸로 오류
            return false;
        }
        result = true;
    }

    unsigned int max_after;
    unsigned int max_before = ((AccountService *)g_record[_record_uri])->max_password_length;
    if(get_value_from_json_key(_jv, "MaxPasswordLength", max_after))
    {
        // ((AccountService *)g_record[_record_uri])->max_password_length = max;
        if(max_after == 0)
        {
            // 이건 maxpassword가 입력으로 왔지만 0을 입력했다는걸로 오류
            return false;
        }
        result = true;
    }

    // min max 둘다 0이면 변경안한거라 out min에는 0이 들어갈수도있는데아니지 1은 들어가야지 min도 ㅇㅋ
    // min만 0이다 그럼 min_before랑 < max_after 비교처리
    // max만 0이다 그럼 min_after랑 < max_before 비교처리
    // 둘다 0아니다 그럼 min_after < max_after 비교처리
    if(min_after == 0 && max_after == 0)
    {
        ;
    }
    else if(min_after == 0 && max_after != 0)
    {
        if(min_before <= max_after)
        {
            // ok max변경해
            ((AccountService *)g_record[_record_uri])->max_password_length = max_after;
        }
        else
        {
            // 안돼 불가능
            return false;
        }
    }
    else if(min_after != 0 && max_after == 0)
    {
        if(min_after <= max_before)
        {
            // ok min변경해
            ((AccountService *)g_record[_record_uri])->min_password_length = min_after;
        }
        else
        {
            // 안돼 불가능
            return false;
        }
    }
    else
    {
        if(min_after <= max_after)
        {
            // ok min,max변경해
            ((AccountService *)g_record[_record_uri])->max_password_length = max_after;
            ((AccountService *)g_record[_record_uri])->min_password_length = min_after;
        }
        else
        {
            // 안돼 불가능
            return false;
        }
    }

    // check_password_range(min_before, min_after, max_before, max_after)

    
    bool service_enabled;
    if(get_value_from_json_key(_jv, "ServiceEnabled", service_enabled))
    {
        ((AccountService *)g_record[_record_uri])->service_enabled = service_enabled;
        result = true;
    }

    unsigned int aflt;
    if(get_value_from_json_key(_jv, "AuthFailureLoggingThreshold", aflt))
    {
        ((AccountService *)g_record[_record_uri])->auth_failure_logging_threshold = aflt;
        result = true;
    }

    unsigned int alt;
    if(get_value_from_json_key(_jv, "AccountLockoutThreshold", alt))
    {
        ((AccountService *)g_record[_record_uri])->account_lockout_threshold = alt;
        result = true;
    }

    unsigned int ald;
    if(get_value_from_json_key(_jv, "AccountLockoutDuration", ald))
    {
        ((AccountService *)g_record[_record_uri])->account_lockout_duration = ald;
        result = true;
    }

    unsigned int alcra;
    if(get_value_from_json_key(_jv, "AccountLockoutCounterResetAfter", alcra))
    {
        ((AccountService *)g_record[_record_uri])->account_lockout_counter_reset_after = alcra;
        result = true;
    }

    bool alcre;
    if(get_value_from_json_key(_jv, "AccountLockoutCounterResetEnabled", alcre))
    {
        ((AccountService *)g_record[_record_uri])->account_lockout_counter_reset_enabled = alcre;
        result = true;
    }

    cout << "바꾼후~~ " << endl;
    cout << record_get_json(_record_uri) << endl;

    return result;
}

bool patch_session_service(json::value _jv)
{
    cout << "바뀌기전~~ " << endl;
    cout << record_get_json(ODATA_SESSION_SERVICE_ID) << endl;
    cout << " $$$$$$$ " << endl;

    bool result = false;

    unsigned int change_timeout;
    if(get_value_from_json_key(_jv, "SessionTimeout", change_timeout))
    {
        ((SessionService *)g_record[ODATA_SESSION_SERVICE_ID])->session_timeout = change_timeout;
        // 타임아웃 시간 변경이 관리자가 할거 같긴한데 일단 권한검사는 추가안함
        result = true;

        Collection *col = (Collection *)g_record[ODATA_SESSION_ID];
        std::vector<Resource *>::iterator iter;
        for(iter=col->members.begin(); iter!=col->members.end(); iter++)
        {
            if(((Session *)(*iter))->_remain_expires_time > change_timeout)
                ((Session *)(*iter))->_remain_expires_time = change_timeout;
        }
        // 타임아웃 시간을 변경한 것이 현재 만들어져있는 세션들에게도 적용되면 이 코드 적용
    }

    bool service_enabled;
    if(get_value_from_json_key(_jv, "ServiceEnabled", service_enabled))
    {
        ((SessionService *)g_record[ODATA_SESSION_SERVICE_ID])->service_enabled = service_enabled;
        result = true;
    }

    cout << "바꾼후~~ " << endl;
    cout << record_get_json(ODATA_SESSION_SERVICE_ID) << endl;

    return result;
}

bool patch_manager(json::value _jv, string _record_uri)
{
    cout << "바뀌기전~~ " << endl;
    cout << record_get_json(_record_uri) << endl;
    cout << " $$$$$$$ " << endl;

    bool result = false;

    string description, datetime, offset;
    if(get_value_from_json_key(_jv, "Description", description))
    {
        ((Manager *)g_record[_record_uri])->description = description;
        result = true;
    }

    if(get_value_from_json_key(_jv, "DateTime", datetime))
    {
        ((Manager *)g_record[_record_uri])->datetime = datetime;
        result = true;
    }

    if(get_value_from_json_key(_jv, "DateTimeLocalOffset", offset))
    {
        ((Manager *)g_record[_record_uri])->datetime_offset = offset;
        result = true;
    }

    // if(_jv.as_object().find("Description") != _jv.as_object().end())
    //     ((Manager *)g_record[_record_uri])->description = _jv.at("Description").as_string();

    // if(_jv.as_object().find("DateTime") != _jv.as_object().end())
    //     ((Manager *)g_record[_record_uri])->datetime = _jv.at("DateTime").as_string();

    // if(_jv.as_object().find("DateTimeLocalOffset") != _jv.as_object().end())
    //     ((Manager *)g_record[_record_uri])->datetime_offset = _jv.at("DateTimeLocalOffset").as_string();

    cout << "바꾼후~~ " << endl;
    cout << record_get_json(_record_uri) << endl;

    return result;
}

bool patch_network_protocol(json::value _jv, string _record_uri)
{
    cout << "바뀌기전~~ " << endl;
    cout << record_get_json(_record_uri) << endl;
    cout << " $$$$$$$ " << endl;

    bool result = false;
    NetworkProtocol *network = (NetworkProtocol *)g_record[_record_uri];

    string description;
    if(get_value_from_json_key(_jv, "Description", description))
    {
        network->description = description;
        result = true;
    }

    json::value snmp;
    if(get_value_from_json_key(_jv, "SNMP", snmp))
    {
        bool enabled;
        int port;
        if(get_value_from_json_key(snmp, "ProtocolEnabled", enabled))
        {
            network->snmp_enabled = enabled;
            result = true;
        }

        if(get_value_from_json_key(snmp, "Port", port))
        {
            network->snmp_port = port;
            result = true;
        }
        // 포트번호 범위검사 해줘야하나? QQQQ
    }
    
    json::value ipmi;
    if(get_value_from_json_key(_jv, "IPMI", ipmi))
    {
        bool enabled;
        int port;
        if(get_value_from_json_key(ipmi, "ProtocolEnabled", enabled))
        {
            network->ipmi_enabled = enabled;
            result = true;
        }

        if(get_value_from_json_key(ipmi, "Port", port))
        {
            network->ipmi_port = port;
            result = true;
        }
    }

    json::value kvmip;
    if(get_value_from_json_key(_jv, "KVMIP", kvmip))
    {
        bool enabled;
        int port;
        if(get_value_from_json_key(kvmip, "ProtocolEnabled", enabled))
        {
            network->kvmip_enabled = enabled;
            result = true;
        }

        if(get_value_from_json_key(kvmip, "Port", port))
        {
            network->kvmip_port = port;
            result = true;
        }
    }

    json::value http;
    if(get_value_from_json_key(_jv, "HTTP", http))
    {
        bool enabled;
        int port;
        if(get_value_from_json_key(http, "ProtocolEnabled", enabled))
        {
            network->http_enabled = enabled;
            result = true;
        }

        if(get_value_from_json_key(http, "Port", port))
        {
            network->http_port = port;
            result = true;
        }
    }

    json::value https;
    if(get_value_from_json_key(_jv, "HTTPS", https))
    {
        bool enabled;
        int port;
        if(get_value_from_json_key(https, "ProtocolEnabled", enabled))
        {
            network->https_enabled = enabled;
            result = true;
        }

        if(get_value_from_json_key(https, "Port", port))
        {
            network->https_port = port;
            result = true;
        }
    }

    json::value ssh;
    if(get_value_from_json_key(_jv, "SSH", ssh))
    {
        bool enabled;
        int port;
        if(get_value_from_json_key(ssh, "ProtocolEnabled", enabled))
        {
            network->ssh_enabled = enabled;
            result = true;
        }

        if(get_value_from_json_key(ssh, "Port", port))
        {
            network->ssh_port = port;
            result = true;
        }
    }


    json::value ntp;
    if(get_value_from_json_key(_jv, "NTP", ntp))
    {
        bool enabled;
        int port;
        json::value ntp_servers = json::value::array();
        if(get_value_from_json_key(ntp, "ProtocolEnabled", enabled))
        {
            network->ntp_enabled = enabled;
            result = true;
        }

        if(get_value_from_json_key(ntp, "Port", port))
        {
            network->ntp_port = port;
            result = true;
        }

        if(get_value_from_json_key(ntp, "NTPServers", ntp_servers))
        {
            for(int i=0; i<ntp_servers.size(); i++)
            {
                bool add = true;
                for(int j=0; j<network->v_netservers.size(); j++)
                {
                    if(ntp_servers[i].as_string() == network->v_netservers[j])
                    {
                        add = false;
                        break;
                    }
                }
                if(add)
                    network->v_netservers.push_back(ntp_servers[i].as_string());

                result = true;
            }
        }
        // ntpservers 는 현재 존재하지않으면 추가하는 식으로만 구현
    }

    // NTPServers 는 여러개가 들어가있는 형식인데 어떻게 수정해야하지?
    // request에서 json에 두 공간에서 받아야할듯 a,b입력란이라고 하면
    // a에서 받은 ntpservers가 존재하면 b검사하고 b에도 입력한게 있으면 그걸로 수정
    // a에서 받은 ntpservers가 존재하지않으면 걍 추가 이런식으로??

    cout << "바꾼후~~ " << endl;
    cout << record_get_json(_record_uri) << endl;

    return result;
}

void patch_fan_mode(string _mode, string _record_uri)
{
    // Fan속도 조절
    // Auto의 경우 Fan과 sensor_num이 같은 Temperature 이용
    string thermal_odata = ODATA_CHASSIS_ID;
    string module_id = get_parent_object_uri(_record_uri, "/");
    // cout << "parent : " << module_id << endl;
    module_id = get_current_object_name(module_id, "/");

    // cout << "module : " << module_id << endl;
    thermal_odata = thermal_odata + "/" + module_id;
    thermal_odata = thermal_odata + "/Thermal";
    // cout << "thermal : " << thermal_odata << endl;
    Thermal *th = (Thermal *)g_record[thermal_odata]; // thermal없는거 에러나려나?
    vector<Resource *> v;
    v = th->fans->members;

    std::vector<Resource *>::iterator fan_iter;
    std::vector<Resource *>::iterator tem_iter;

    for(fan_iter=v.begin(); fan_iter!=v.end(); fan_iter++)
    {
        cout << "바뀌기전~~ " << endl;
        cout << "fan info" << endl;
        cout << record_get_json((*fan_iter)->odata.id) << endl;
        cout << " $$$$$$$ " << endl;

        if(_mode == "Standard")
        {
            ((Fan *)*fan_iter)->reading = ((Fan *)*fan_iter)->max_reading_range / 2;
        }
        else if(_mode == "FullSpeed")
        {
            ((Fan *)*fan_iter)->reading = ((Fan *)*fan_iter)->max_reading_range;
        }
        else if(_mode == "Auto")
        {
            //iter
            vector<Resource *> tem_v;
            tem_v = th->temperatures->members;
            for(tem_iter=tem_v.begin(); tem_iter!=tem_v.end(); tem_iter++)
            {
                if(((Fan *)*fan_iter)->sensor_num == ((Temperature *)*tem_iter)->sensor_num)
                {
                    if(((Temperature *)*tem_iter)->reading_celsius < 40)
                    {
                        cout << "Memeber : " << ((Temperature *)*tem_iter)->member_id << endl;
                        cout << "Celcius : " << ((Temperature *)*tem_iter)->reading_celsius << endl;
                        ((Fan *)*fan_iter)->reading = ((Fan *)*fan_iter)->max_reading_range / 2;
                    }
                    else
                    {
                        cout << "Memeber : " << ((Temperature *)*tem_iter)->member_id << endl;
                        cout << "Celcius : " << ((Temperature *)*tem_iter)->reading_celsius << endl;
                        ((Fan *)*fan_iter)->reading = ((Fan *)*fan_iter)->max_reading_range;
                    }
                    break;
                }
            }
        }

        cout << "바꾼후~~ " << endl;
        cout << "fan info" << endl;
        cout << record_get_json((*fan_iter)->odata.id) << endl;
        resource_save_json(*fan_iter);


        // 여기서 temperature돌아서 센서넘버 같은녀석의 온도체크후 바꿔줌
        // 근데 그것도 Auto일 경우에만/ 스탠다드, 풀스피드일경우엔 팬 자체정보로 변경가능
    }
}

bool patch_ethernet_interface(json::value _jv, string _record_uri, int _flag)
{
    // _flag=0 -- 값만 바꿈, _flag=1 -- 변경처리까지
    cout << "바뀌기전~~ " << endl;
    cout << record_get_json(_record_uri) << endl;
    cout << " $$$$$$$ " << endl;

    EthernetInterfaces *eth = (EthernetInterfaces *)g_record[_record_uri];
    bool result = false;

    string description;
    if(get_value_from_json_key(_jv, "Description", description))
    {
        eth->description = description;
        result = true;
    }

    // string fqdn;
    // if(get_value_from_json_key(_jv, "FQDN", fqdn))
    // {
    //     eth->fqdn = fqdn;
    //     if(_flag == 1)
    //     {
    //         string buf = "hostname ";

    //     }
    // }

    string hostname;
    if(get_value_from_json_key(_jv, "HostName", hostname))
    {
        if(_flag == 1)
        {
            string buf = "hostname ";
            buf = buf + hostname;
            cout << "hostname buf : " << buf << endl;
            system(buf.c_str());
        }
        eth->hostname = hostname;
        result = true;
    }

    string mac;
    if(get_value_from_json_key(_jv, "MACAddress", mac))
    {
        if(_flag == 1)
        {
            string buf = "macchanger -m ";
            buf = buf + mac + " eth" + eth->id;
            cout << "macaddress buf : " << buf << endl;
            // system(buf.c_str());
        }
        eth->mac_address = mac;
        result = true;
    }

    int mtu;
    if(get_value_from_json_key(_jv, "MTUSize", mtu))
    {
        if(_flag == 1)
        {
            string buf = "ip link set ";
            buf = buf + "eth" + eth->id;
            buf = buf + " mtu " + to_string(mtu);
            cout << "mtusize buf : " << buf << endl;
            system(buf.c_str());
        }
        eth->mtu_size = mtu;
        result = true;
    }

    // ipv4, ipv6 주소들은 array로 되어있는데 patch request body로는 일단 object하나라고 생각하고 구현
    json::value ipv4;
    if(get_value_from_json_key(_jv, "IPv4Addresses", ipv4))
    {
        int size = eth->v_ipv4.size();
        if(size == 0)
        {
            return false;
        }
        // ipv4 1개이상있을때 첫번째꺼만 수정되는걸로

        string address;
        string netmask;
        string gateway;
        if(get_value_from_json_key(ipv4, "Address", address))
        {
            if(_flag == 1)
            {
                string buf = "ifconfig eth";
                buf = buf + eth->id + " " + address;
                cout << "ipv4 address buf : " << buf << endl;
                // system(buf.c_str());
            }
            eth->v_ipv4[0].address = address;
            result = true;
        }

        if(get_value_from_json_key(ipv4, "SubnetMask", netmask))
        {
            if(_flag == 1)
            {
                string buf = "ifconfig eth";
                buf = buf + eth->id + " netmask " + netmask;
                cout << "ipv4 subnetmask buf : " << buf << endl;
                // system(buf.c_str());
            }
            eth->v_ipv4[0].subnet_mask = netmask;
            result = true;
        }

        if(get_value_from_json_key(ipv4, "Gateway", gateway))
        {
            if(_flag == 1)
            {
                string buf = "route add default gw ";
                buf = buf + gateway;
                cout << "ipv4 gateway buf : " << buf << endl;
                // system(buf.c_str());
            }
            eth->v_ipv4[0].gateway = gateway;
            result = true;
        }

        // 현재 여기 변수들이 바뀌면 107번 서버동작이 안돼서 명령어 주석처리
    }

    json::value ipv6;
    if(get_value_from_json_key(_jv, "IPv6Addresses", ipv6))
    {
        // ipv6는 추가하는식
        int size = eth->v_ipv6.size();
        if(size == 0)
        {
            return false;
        }

        string address;
        if(get_value_from_json_key(ipv6, "Address", address))
        {
            IPv6_Address new_ipv6;
            new_ipv6.address = address;
            new_ipv6.address_origin = eth->v_ipv6[0].address_origin;
            new_ipv6.prefix_length = eth->v_ipv6[0].prefix_length;
            new_ipv6.address_state = eth->v_ipv6[0].address_state;
            if(_flag == 1)
            {
                string buf = "ifconfig eth";
                buf = buf + eth->id + " inet6 add " + address + "/" + to_string(new_ipv6.prefix_length);
                cout << "ipv6 address buf : " << buf << endl;
                system(buf.c_str());
            }
            eth->v_ipv6.push_back(new_ipv6);
            result = true;
        }

    }


    cout << "바꾼후~~ " << endl;
    cout << record_get_json(_record_uri) << endl;

    return result;

}

bool patch_system(json::value _jv, string _record_uri)
{
    cout << "바뀌기전~~ " << endl;
    cout << record_get_json(_record_uri) << endl;
    cout << " $$$$$$$ " << endl;
    // log(trace) << "바뀌기전~~ ";
    // log(debug) << record_get_json(uri);
    // log(info) << " $$$$$$$ ";

    bool result = false;

    

    // if(_jv.as_object().find("Description") != _jv.as_object().end())
    //     ((Systems *)g_record[_record_uri])->description = _jv.at("Description").as_string();

    // if(_jv.as_object().find("HostName") != _jv.as_object().end())
    //     ((Systems *)g_record[_record_uri])->hostname = _jv.at("HostName").as_string();

    // if(_jv.as_object().find("AssetTag") != _jv.as_object().end())
    //     ((Systems *)g_record[_record_uri])->asset_tag = _jv.at("AssetTag").as_string();


    string led;
    if(get_value_from_json_key(_jv, "IndicatorLED", led))
    {
        if(led == "Off")
            ((Systems *)g_record[_record_uri])->indicator_led = LED_OFF;
        else if(led == "Blinking")
            ((Systems *)g_record[_record_uri])->indicator_led = LED_BLINKING;
        else if(led == "Lit")
            ((Systems *)g_record[_record_uri])->indicator_led = LED_LIT;
        else
            return false;

        result = true;
    }

    string description, hostname, asset_tag;
    if(get_value_from_json_key(_jv, "Description", description))
    {
        ((Systems *)g_record[_record_uri])->description = description;
        result = true;
    }

    if(get_value_from_json_key(_jv, "HostName", hostname))
    {
        ((Systems *)g_record[_record_uri])->hostname = hostname;
        result = true;
    }

    if(get_value_from_json_key(_jv, "AssetTag", asset_tag))
    {
        ((Systems *)g_record[_record_uri])->asset_tag = asset_tag;
        result = true;
    }
    
    // if(_jv.as_object().find("IndicatorLED") != _jv.as_object().end())
    // {
    //     string led = _jv.at("IndicatorLED").as_string();
    //     if(led == "Off")
    //         ((Systems *)g_record[_record_uri])->indicator_led = LED_OFF;
    //     else if(led == "Blinking")
    //         ((Systems *)g_record[_record_uri])->indicator_led = LED_BLINKING;
    //     else if(led == "Lit")
    //         ((Systems *)g_record[_record_uri])->indicator_led = LED_LIT;
    // }

    json::value boot;
    if(get_value_from_json_key(_jv, "Boot", boot))
    {
        string bsoe, bsot, bsom, utbso;
        if(get_value_from_json_key(boot, "BootSourceOverrideEnabled", bsoe))
        {
            ((Systems *)g_record[_record_uri])->boot.boot_source_override_enabled = bsoe;
            result = true;
        }

        if(get_value_from_json_key(boot, "BootSourceOverrideTarget", bsot))
        {
            ((Systems *)g_record[_record_uri])->boot.boot_source_override_target = bsot;
            result = true;
        }

        if(get_value_from_json_key(boot, "BootSourceOverrideMode", bsom))
        {
            ((Systems *)g_record[_record_uri])->boot.boot_source_override_mode = bsom;
            result = true;
        }

        if(get_value_from_json_key(boot, "UefiTargetBootSourceOverride", utbso))
        {
            ((Systems *)g_record[_record_uri])->boot.uefi_target_boot_source_override = utbso;
            result = true;
        }
    }

    // if(_jv.as_object().find("Boot") != _jv.as_object().end())
    // {
    //     json::value j = json::value::object();
    //     j = _jv.at("Boot");

    //     if(j.as_object().find("BootSourceOverrideEnabled") != j.as_object().end())
    //         ((Systems *)g_record[_record_uri])->boot.boot_source_override_enabled = j.at("BootSourceOverrideEnabled").as_string();

    //     if(j.as_object().find("BootSourceOverrideTarget") != j.as_object().end())
    //         ((Systems *)g_record[_record_uri])->boot.boot_source_override_target = j.at("BootSourceOverrideTarget").as_string();

    //     if(j.as_object().find("BootSourceOverrideMode") != j.as_object().end())
    //         ((Systems *)g_record[_record_uri])->boot.boot_source_override_mode = j.at("BootSourceOverrideMode").as_string();

    //     if(j.as_object().find("UefiTargetBootSourceOverride") != j.as_object().end())
    //         ((Systems *)g_record[_record_uri])->boot.uefi_target_boot_source_override = j.at("UefiTargetBootSourceOverride").as_string();
    // }


    cout << "바꾼후~~ " << endl;
    cout << record_get_json(_record_uri) << endl;
    return result;
}

bool patch_chassis(json::value _jv, string _record_uri)
{
    cout << "바뀌기전~~ " << endl;
    cout << record_get_json(_record_uri) << endl;
    cout << " $$$$$$$ " << endl;

    bool result = false;

    string led;
    if(get_value_from_json_key(_jv, "IndicatorLED", led))
    {
        if(led == "Off")
            ((Chassis *)g_record[_record_uri])->indicator_led = LED_OFF;
        else if(led == "Blinking")
            ((Chassis *)g_record[_record_uri])->indicator_led = LED_BLINKING;
        else if(led == "Lit")
            ((Chassis *)g_record[_record_uri])->indicator_led = LED_LIT;
        else
            return false;

        result = true;
    }

    string asset_tag;
    if(get_value_from_json_key(_jv, "AssetTag", asset_tag))
    {
        ((Chassis *)g_record[_record_uri])->asset_tag = asset_tag;
        result = true;
    }

    // if(_jv.as_object().find("AssetTag") != _jv.as_object().end())
    //     ((Chassis *)g_record[_record_uri])->asset_tag = _jv.at("AssetTag").as_string();

    // if(_jv.as_object().find("IndicatorLED") != _jv.as_object().end())
    // {
    //     string led = _jv.at("IndicatorLED").as_string();
    //     if(led == "Off")
    //         // ((Chassis *)g_record[_record_uri])->led_off
    //         ((Chassis *)g_record[_record_uri])->indicator_led = LED_OFF;
    //     else if(led == "Blinking")
    //         ((Chassis *)g_record[_record_uri])->indicator_led = LED_BLINKING;
    //     else if(led == "Lit")
    //         ((Chassis *)g_record[_record_uri])->indicator_led = LED_LIT;
    // }

    json::value location;
    if(get_value_from_json_key(_jv, "Location", location))
    {
        json::value postal_address, placement;
        if(get_value_from_json_key(location, "PostalAddress", postal_address))
        {
            string country, territory, city, street, house_number, name, postal_code;
            if(get_value_from_json_key(postal_address, "Country", country))
            {
                ((Chassis *)g_record[_record_uri])->location.postal_address.country = country;
                result = true;
            }
            if(get_value_from_json_key(postal_address, "Territory", territory))
            {
                ((Chassis *)g_record[_record_uri])->location.postal_address.territory = territory;
                result = true;
            }
            if(get_value_from_json_key(postal_address, "City", city))
            {
                ((Chassis *)g_record[_record_uri])->location.postal_address.city = city;
                result = true;
            }
            if(get_value_from_json_key(postal_address, "Street", street))
            {
                ((Chassis *)g_record[_record_uri])->location.postal_address.street = street;
                result = true;
            }
            if(get_value_from_json_key(postal_address, "HouseNumber", house_number))
            {
                ((Chassis *)g_record[_record_uri])->location.postal_address.house_number = house_number;
                result = true;
            }
            if(get_value_from_json_key(postal_address, "Name", name))
            {
                ((Chassis *)g_record[_record_uri])->location.postal_address.name = name;
                result = true;
            }
            if(get_value_from_json_key(postal_address, "PostalCode", postal_code))
            {
                ((Chassis *)g_record[_record_uri])->location.postal_address.postal_code = postal_code;
                result = true;
            }
        }

        if(get_value_from_json_key(location, "Placement", placement))
        {
            string row, rack, rack_offset_units;
            unsigned int rack_offset;
            if(get_value_from_json_key(placement, "Row", row))
            {
                ((Chassis *)g_record[_record_uri])->location.placement.row = row;
                result = true;
            }
            if(get_value_from_json_key(placement, "Rack", rack))
            {
                ((Chassis *)g_record[_record_uri])->location.placement.rack = rack;
                result = true;
            }
            if(get_value_from_json_key(placement, "RackOffsetUnits", rack_offset_units))
            {
                ((Chassis *)g_record[_record_uri])->location.placement.rack_offset_units = rack_offset_units;
                result = true;
            }
            if(get_value_from_json_key(placement, "RackOffset", rack_offset))
            {
                ((Chassis *)g_record[_record_uri])->location.placement.rack_offset = rack_offset;
                result = true;
            }

        }
    }

    // if(_jv.as_object().find("Location") != _jv.as_object().end())
    // {
    //     json::value j;
    //     j = _jv.at("Location");

    //     if(j.as_object().find("PostalAddress") != j.as_object().end())
    //     {
    //         json::value k;
    //         k = j.at("PostalAddress");

    //         if(k.as_object().find("Country") != k.as_object().end())
    //             ((Chassis *)g_record[_record_uri])->location.postal_address.country = k.at("Country").as_string();
    //         if(k.as_object().find("Territory") != k.as_object().end())
    //             ((Chassis *)g_record[_record_uri])->location.postal_address.territory = k.at("Territory").as_string();
    //         if(k.as_object().find("City") != k.as_object().end())
    //             ((Chassis *)g_record[_record_uri])->location.postal_address.city = k.at("City").as_string();
    //         if(k.as_object().find("Street") != k.as_object().end())
    //             ((Chassis *)g_record[_record_uri])->location.postal_address.street = k.at("Street").as_string();
    //         if(k.as_object().find("HouseNumber") != k.as_object().end())
    //             ((Chassis *)g_record[_record_uri])->location.postal_address.house_number = k.at("HouseNumber").as_string();
    //         if(k.as_object().find("Name") != k.as_object().end())
    //             ((Chassis *)g_record[_record_uri])->location.postal_address.name = k.at("Name").as_string();
    //         if(k.as_object().find("PostalCode") != k.as_object().end())
    //             ((Chassis *)g_record[_record_uri])->location.postal_address.postal_code = k.at("PostalCode").as_string();
    //     }

    //     if(j.as_object().find("Placement") != j.as_object().end())
    //     {
    //         json::value k;
    //         k = j.at("Placement");

    //         if(k.as_object().find("Row") != k.as_object().end())
    //             ((Chassis *)g_record[_record_uri])->location.placement.row = k.at("Row").as_string();
    //         if(k.as_object().find("Rack") != k.as_object().end())
    //             ((Chassis *)g_record[_record_uri])->location.placement.rack = k.at("Rack").as_string();
    //         if(k.as_object().find("RackOffsetUnits") != k.as_object().end())
    //             ((Chassis *)g_record[_record_uri])->location.placement.rack_offset_units = k.at("RackOffsetUnits").as_string();
    //         if(k.as_object().find("RackOffset") != k.as_object().end())
    //             ((Chassis *)g_record[_record_uri])->location.placement.rack_offset = k.at("RackOffset").as_integer();
    //     }
    // }

    cout << "바꾼후~~ " << endl;
    cout << record_get_json(_record_uri) << endl;

    return result;
}

bool patch_power_control(json::value _jv, string _record_uri)
{
    cout << "바뀌기전~~ " << endl;
    cout << record_get_json(_record_uri) << endl;
    cout << " $$$$$$$ " << endl;

    bool result = false;

    json::value power_limit;
    if(get_value_from_json_key(_jv, "PowerLimit", power_limit))
    {
        int correction_in_ms;
        string limit_exception;
        double limit_in_watts;
        if(get_value_from_json_key(power_limit, "CorrectionInMs", correction_in_ms))
        {
            ((PowerControl *)g_record[_record_uri])->power_limit.correction_in_ms = correction_in_ms;
            result = true;
        }
        if(get_value_from_json_key(power_limit, "LimitException", limit_exception))
        {
            ((PowerControl *)g_record[_record_uri])->power_limit.limit_exception = limit_exception;
            result = true;
        }
        if(get_value_from_json_key(power_limit, "LimitInWatts", limit_in_watts))
        {
            ((PowerControl *)g_record[_record_uri])->power_limit.limit_in_watts = limit_in_watts;
            result = true;
        }
    }

    // if(_jv.as_object().find("PowerLimit") != _jv.as_object().end())
    // {
    //     json::value j;
    //     j = _jv.at("PowerLimit");

    //     if(j.as_object().find("CorrectionInMs") != j.as_object().end())
    //         ((PowerControl *)g_record[_record_uri])->power_limit.correction_in_ms = j.at("CorrectionInMs").as_integer();
    //     if(j.as_object().find("LimitException") != j.as_object().end())
    //         ((PowerControl *)g_record[_record_uri])->power_limit.limit_exception = j.at("LimitException").as_string();
    //     if(j.as_object().find("LimitInWatts") != j.as_object().end())
    //         ((PowerControl *)g_record[_record_uri])->power_limit.limit_in_watts = j.at("LimitInWatts").as_double();
    // }

    cout << "바꾼후~~ " << endl;
    cout << record_get_json(_record_uri) << endl;

    return result;
}

bool patch_event_service(json::value _jv, string _record_uri)
{
    cout << "바뀌기전~~ " << endl;
    cout << record_get_json(_record_uri) << endl;
    cout << " $$$$$$$ " << endl;

    bool result = false;
    bool op_attempt=false, op_interval=false, op_enabled=false;

    int attempt, interval;
    bool service_enabled;
    EventService *service = (EventService *)g_record[_record_uri];

    if(get_value_from_json_key(_jv, "DeliveryRetryAttempts", attempt))
    {
        if(attempt >= 0){
            op_attempt = true;
            result = true;
        }
        else
            return false;
    }

    if(get_value_from_json_key(_jv, "DeliveryRetryIntervalSeconds", interval))
    {
        if(interval >= 1){
            op_interval = true;
            result = true;
        }
        else
            return false;
    }

    if(get_value_from_json_key(_jv, "ServiceEnabled", service_enabled))
    {
        op_enabled = true;
        result = true;
    }
        

    if(result)
    {
        if(op_attempt)
            service->delivery_retry_attempts = attempt;
        if(op_interval)
            service->delivery_retry_interval_seconds = interval;
        if(op_enabled)
            service->service_enabled = service_enabled;

        resource_save_json(service);
    }

    cout << "바꾼후~~ " << endl;
    cout << record_get_json(_record_uri) << endl;

    return result;
}

bool patch_subscription(json::value _jv, string _record_uri)
{
    cout << "바뀌기전~~ " << endl;
    cout << record_get_json(_record_uri) << endl;
    cout << " $$$$$$$ " << endl;

    bool result = false;
    bool op_context=false, op_policy=false;

    string context, policy;
    EventDestination *dest = (EventDestination *)g_record[_record_uri];

    if(get_value_from_json_key(_jv, "Context", context))
    {
        result = true;
        op_context = true;
    }

    if(get_value_from_json_key(_jv, "DeliveryRetryPolicy", policy))
    {
        if(policy == "RetryForever" || policy == "SuspendRetries" || policy == "TerminateAfterRetries")
        {
            result = true;
            op_policy = true;
        }
        else
            return false;
    }

    if(result)
    {
        if(op_context)
            dest->context = context;
        if(op_policy)
            dest->delivery_retry_policy = policy;

        resource_save_json(dest);
    }

    cout << "바꾼후~~ " << endl;
    cout << record_get_json(_record_uri) << endl;

    return result;
}

void error_reply(m_Request& _msg, json::value _jv, status_code _status, http_response& _response)
{
    _response.set_status_code(_status);

    if(_jv != json::value::null())
    {
        _response.set_body(_jv);
        _msg.result.response_json = _jv;
    }

    _msg.result.result_datetime = currentDateTime();
    _msg.result.result_status = WORK_FAIL;
    _msg.result.result_response = _response;

    return ;
}

json::value get_error_json(string _message)
{
    json::value err;
    if(_message != "")
        err[U("Error")] = json::value::string(U(_message));
    else
        err = json::value::null();

    return err;
}


void success_reply(m_Request& _msg, json::value _jv, status_code _status, http_response& _response)
{
    _response.set_status_code(_status);

    if(_jv != json::value::null())
    {
        _response.set_body(_jv);
        _msg.result.response_json = _jv;
    }

    _msg.result.result_datetime = currentDateTime();
    _msg.result.result_response = _response;
    _msg.result.result_status = WORK_SUCCESS;

    return ;
}
  
void test_send_event(Event _event)
{
    // string, Event* ... event_map
    // 인자받아야겟네
    // 이벤트에서 subs에 event type이 맞는걸 골라서 각자의 sub type으로 보내는거같음
    Collection* col = (Collection *)g_record[ODATA_EVENT_DESTINATION_ID];
    vector<Resource *>::iterator iter;
    EventDestination* dest;
    for(iter = col->members.begin(); iter != col->members.end(); iter++)
    {
        dest = (EventDestination *)*iter;

        if(dest->subscription_type == "RedfishEvent")
        {
            // break;
            http_client client(dest->destination); // 이게되네 개굿
            http_request req(methods::POST);
            req.set_body(_event.get_json());
            http_response res;

            try
            {
                pplx::task<http_response> responseTask = client.request(req);
                res = responseTask.get();
                log(info) << "[SUBMIT][TESTEVENT][STATUS][DEST : " << dest->context << "] : " << res.status_code();
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }

        }
        // 일단은 subs type이 RedfishEvent인거에만 보냄
        // 여기서 이제 테스트말고 실제 이벤트 관련해서는 Event타입도 고려하고
        // 전송방식도 고려하면 될거같음 그다음에 log도 같이 구현해주고
        // 받은곳에서도 받은처리해주면될듯
    }

    // http_client client(dest->destination); // 이게되네 개굿
    // http_request req(methods::POST);
    // req.set_body(_event.get_json());
    // http_response res;

    // try
    // {
    //     pplx::task<http_response> responseTask = client.request(req);
    //     res = responseTask.get();
    //     log(info) << "[SUBMIT][TESTEVENT][STATUS] : " << res.status_code();
    // }
    // catch(const std::exception& e)
    // {
    //     std::cerr << e.what() << '\n';
    // }

    
    
    // 현재 event_info 에서 event_group_id를 가지고 stoi해서 string으로 만든뒤 그걸 event map에서의 first로
    // 하고있단말임? 그리고 그게 Event클래스 객체의 id도 되고

    // 이거 기반으로 이벤트 보내면될듯 redfish타입은
    // 만들어진 event에서 eventtype이 있을거고 1가지로 지정되어있을거임
    // 그걸 토대로 sub에는 array로 eventtype이 들어가있을건데 거기에 포함되면 그 sub에다가 event를 쏴주는데
    // 쏴줄때 방식이 subs type, protocol같은거 보고 redfishevent면 http post로 SSE, SMTP뭐 이런거면 그거에 맞게
    // 현재는 redfishEvent만

}

/**
 * 이벤트는 발생하면 바로 subsciption에 이벤트 전송되게끔 event_map일단 사용않기로..
 * Event 클래스 안의 Event_Info안 멤버변수
 * groupid는 0일경우 다른이벤트들이 이 이벤트와 관련없다는 뜻. 이벤트 발생 근본 원인과 상관 관계를 나타내는 식별자
 * eventid는 이벤트식별자
 * messageid는 MessageRegistry에 지정된 메세지패턴id를 지칭
 * event type에 해당 이벤트의 타입들어감
 * 
 * 그래서 저런 정보들 담아서 이벤트가 생성되면 바로 subscription컬렉션 돌면서
 * 해당 이벤트타입을 구독하는 subscription이 있으면 그 subs 에게 보내주는데 이때 subs의 subs type하고 protocol
 * 에 맞게 전송하면됨
 * 현재는 redfish event로 http로 destination uri에 post로 전송함
 * 전송한 곳은 전송한대로 이벤트log 발생(구현필요)
 * 받는 곳은 받아서 log발생, 이벤트에 대한 처리(구현필요)해야함
 * event_map이 필요한지(백업, 모아둿다 한번에 보내기등)
 * */



json::value get_json_task_map(void)
{
    json::value j_map;
    // 카테고리가  서비스루트/시스템/매니저/샤시/계정서비스/세션서비스/태스크서비스/이벤트서비스/업데이트서비스/인증서비스

    // task_type에 따라서 나눠지고 각각의 이름으로 json::value가 들어가고 안에서 리스트가잇는셈
    // 근데 리스트는 내용 똑같음 만드는거 함수화해서 쓰면댐 일단 serviceroot꺼로 하나 만들어보고 그다음에 함수화시켜서
    // 각각 할당하면될듯
    // j_map[U("ServiceRoot")]
    Task_Manager *tm;
    json::value j_task_manager;
    if(task_map.find(TASK_TYPE_SERVICEROOT) != task_map.end())
    {
        tm = task_map.find(TASK_TYPE_SERVICEROOT)->second;
        j_task_manager = get_json_task_manager(tm);
        j_map["ServiceRoot"] = j_task_manager;
    }

    if(task_map.find(TASK_TYPE_SYSTEMS) != task_map.end())
    {
        tm = task_map.find(TASK_TYPE_SYSTEMS)->second;
        j_task_manager = get_json_task_manager(tm);
        j_map["Systems"] = j_task_manager;
    }

    if(task_map.find(TASK_TYPE_CHASSIS) != task_map.end())
    {
        tm = task_map.find(TASK_TYPE_CHASSIS)->second;
        j_task_manager = get_json_task_manager(tm);
        j_map["Chassis"] = j_task_manager;
    }

    if(task_map.find(TASK_TYPE_MANAGERS) != task_map.end())
    {
        tm = task_map.find(TASK_TYPE_MANAGERS)->second;
        j_task_manager = get_json_task_manager(tm);
        j_map["Managers"] = j_task_manager;
    }

    if(task_map.find(TASK_TYPE_TASKSERVICE) != task_map.end())
    {
        tm = task_map.find(TASK_TYPE_TASKSERVICE)->second;
        j_task_manager = get_json_task_manager(tm);
        j_map["TaskService"] = j_task_manager;
    }

    if(task_map.find(TASK_TYPE_EVENTSERVICE) != task_map.end())
    {
        tm = task_map.find(TASK_TYPE_EVENTSERVICE)->second;
        j_task_manager = get_json_task_manager(tm);
        j_map["EventService"] = j_task_manager;
    }

    if(task_map.find(TASK_TYPE_ACCOUNTSERVICE) != task_map.end())
    {
        tm = task_map.find(TASK_TYPE_ACCOUNTSERVICE)->second;
        j_task_manager = get_json_task_manager(tm);
        j_map["AccountService"] = j_task_manager;
    }

    if(task_map.find(TASK_TYPE_SESSIONSERVICE) != task_map.end())
    {
        tm = task_map.find(TASK_TYPE_SESSIONSERVICE)->second;
        j_task_manager = get_json_task_manager(tm);
        j_map["SessionService"] = j_task_manager;
    }

    if(task_map.find(TASK_TYPE_UPDATESERVICE) != task_map.end())
    {
        tm = task_map.find(TASK_TYPE_UPDATESERVICE)->second;
        j_task_manager = get_json_task_manager(tm);
        j_map["UpdateService"] = j_task_manager;
    }

    if(task_map.find(TASK_TYPE_CERTIFICATESERVICE) != task_map.end())
    {
        tm = task_map.find(TASK_TYPE_CERTIFICATESERVICE)->second;
        j_task_manager = get_json_task_manager(tm);
        j_map["CertificateService"] = j_task_manager;
    }

    if(task_map.find(TASK_TYPE_COMPLETED) != task_map.end())
    {
        tm = task_map.find(TASK_TYPE_COMPLETED)->second;
        j_task_manager = get_json_task_manager(tm);
        j_map["Completed"] = j_task_manager;
    }
    
    return j_map;
}

json::value get_json_task_manager(Task_Manager *_tm)
{
    json::value j_task;

    j_task["TaskType"] = json::value::number(_tm->task_type);
    j_task["HostIP"] = json::value::string(_tm->host_ip);
    j_task["PrimaryIP"] = json::value::string(_tm->primary_ip);
    j_task["PrimaryPort"] = json::value::number(_tm->primary_port);
    j_task["SecondaryPort"] = json::value::number(_tm->secondary_port);
    j_task["_IP"] = json::value::string(_tm->_ip);
    j_task["List"] = json::value::array();
    // Task_Manager 부분

    list<m_Request>::iterator iter;
    int i = 0;
    for(iter=_tm->list_request.begin(); iter!=_tm->list_request.end(); iter++, i++)
    {
        json::value j_request;
        j_request["TaskNumber"] = json::value::number(iter->task_number);
        j_request["Method"] = json::value::string(iter->method);
        j_request["Host"] = json::value::string(iter->host);
        j_request["Uri"] = json::value::string(iter->uri);
        j_request["Datetime"] = json::value::string(iter->request_datetime);
        j_request["RequestJson"] = iter->request_json;
        // m_Request 부분

        json::value j_response;
        j_response["ResponseNumber"] = json::value::number(iter->result.res_number);
        j_response["ResultStatus"] = json::value::number(iter->result.result_status);
        j_response["ResultDatetime"] = json::value::string(iter->result.result_datetime);
        // m_Response 부분

        json::value j_res;
        http_response response = iter->result.result_response;
        // log(info) << "[BEFORE EXTRACT RESPONSE] : " << response.to_string();
        j_res["StatusCode"] = json::value::number(response.status_code());
        j_res["ResponseJson"] = iter->result.response_json;
        // j_res["ResponseJson"] = response.extract_json().get();
        // log(info) << "[AFTER EXTRACT RESPONSE] : " << response.to_string();
    
        json::value j_header = json::value::array();
        http_headers::iterator iter_header;
        http_headers head = response.headers();
        int j=0;
        for(iter_header=head.begin(); iter_header!=head.end(); iter_header++, j++)
        {
            // j_header[iter_header->first] = json::value::string(iter_header->second);
            string val = iter_header->first + "::::" + iter_header->second;
            j_header[j] = json::value::string(val);
        }
        j_res["Headers"] = j_header;
        // header는 헤더에서 하나씩 만들어서 key:value로 저장해서 하는걸로
        // key-value로 하니깐 읽어들일때 key값을 몰라서 접근이 안되더라
        // 그래서 j_header를 리스트로하고 안에 string으로 "key:value" 로 넣은다음 :로 파싱할게
        // result_response 부분

        j_response["ResultResponse"] = j_res;

        j_request["Result"] = j_response;
        
        // tm->list_request[i].task 이거안되네 이터레이터
        j_task["List"][i] = j_request;
    }
    // list<m_Request> list_request 부분

    return j_task;
}

void create_task_map_from_json(json::value _jv)
{
    // 토할거같다!! 
    // 음 이건 테스트를 할때 cmm꺼 가지고 할거니깐 일단 지금의 task_map초기화한후 그걸로 json뽑아보고
    // 깨끗할거아니여 그런담에 이걸로 읽어서 task_map 구성하고 다시 json뽑았을때 똑같으면 됨
    // 도중에 요청들어와서 taskmap 충돌나는건  이건 ha에서 할거니깐 아 잠깐만 ha에도 요청들어오는데?
    // 하지만 ha는 요청처리에 task_map을 만들지않지 굿
    // 오히려 task_map json으로 만들때가 cmm에서 만드는데 그때 도중에 요청와서 task_map변동이 있을수있겠넹ㅇ

    json::value clear_json;
    task_map.clear();
    clear_json = get_json_task_map();

    // cout << " !@#$ CLEAR !!!! ------------------- " << endl;
    // cout << clear_json << endl;

    // 이제 로드
    json::value inner_json;
    Task_Manager *tm;
    if(get_value_from_json_key(_jv, "ServiceRoot", inner_json))
    {
        // 있으면 task_map만들고
        if(task_map.find(TASK_TYPE_SERVICEROOT) != task_map.end())
            tm = task_map.find(TASK_TYPE_SERVICEROOT)->second;
        else
        {
            tm = new Task_Manager();
            tm->task_type = TASK_TYPE_SERVICEROOT;
            task_map.insert(make_pair(TASK_TYPE_SERVICEROOT, tm));
        }

        create_task_manager_from_json(tm, inner_json);
    }

    if(get_value_from_json_key(_jv, "Systems", inner_json))
    {
        // 있으면 task_map만들고
        if(task_map.find(TASK_TYPE_SYSTEMS) != task_map.end())
            tm = task_map.find(TASK_TYPE_SYSTEMS)->second;
        else
        {
            tm = new Task_Manager();
            tm->task_type = TASK_TYPE_SYSTEMS;
            task_map.insert(make_pair(TASK_TYPE_SYSTEMS, tm));
        }

        create_task_manager_from_json(tm, inner_json);
    }

    if(get_value_from_json_key(_jv, "Chassis", inner_json))
    {
        // 있으면 task_map만들고
        if(task_map.find(TASK_TYPE_CHASSIS) != task_map.end())
            tm = task_map.find(TASK_TYPE_CHASSIS)->second;
        else
        {
            tm = new Task_Manager();
            tm->task_type = TASK_TYPE_CHASSIS;
            task_map.insert(make_pair(TASK_TYPE_CHASSIS, tm));
        }

        create_task_manager_from_json(tm, inner_json);
    }

    if(get_value_from_json_key(_jv, "Managers", inner_json))
    {
        // 있으면 task_map만들고
        if(task_map.find(TASK_TYPE_MANAGERS) != task_map.end())
            tm = task_map.find(TASK_TYPE_MANAGERS)->second;
        else
        {
            tm = new Task_Manager();
            tm->task_type = TASK_TYPE_MANAGERS;
            task_map.insert(make_pair(TASK_TYPE_MANAGERS, tm));
        }

        create_task_manager_from_json(tm, inner_json);
    }

    if(get_value_from_json_key(_jv, "TaskService", inner_json))
    {
        // 있으면 task_map만들고
        if(task_map.find(TASK_TYPE_TASKSERVICE) != task_map.end())
            tm = task_map.find(TASK_TYPE_TASKSERVICE)->second;
        else
        {
            tm = new Task_Manager();
            tm->task_type = TASK_TYPE_TASKSERVICE;
            task_map.insert(make_pair(TASK_TYPE_TASKSERVICE, tm));
        }

        create_task_manager_from_json(tm, inner_json);
    }

    if(get_value_from_json_key(_jv, "EventService", inner_json))
    {
        // 있으면 task_map만들고
        if(task_map.find(TASK_TYPE_EVENTSERVICE) != task_map.end())
            tm = task_map.find(TASK_TYPE_EVENTSERVICE)->second;
        else
        {
            tm = new Task_Manager();
            tm->task_type = TASK_TYPE_EVENTSERVICE;
            task_map.insert(make_pair(TASK_TYPE_EVENTSERVICE, tm));
        }

        create_task_manager_from_json(tm, inner_json);
    }

    if(get_value_from_json_key(_jv, "AccountService", inner_json))
    {
        // 있으면 task_map만들고
        if(task_map.find(TASK_TYPE_ACCOUNTSERVICE) != task_map.end())
            tm = task_map.find(TASK_TYPE_ACCOUNTSERVICE)->second;
        else
        {
            tm = new Task_Manager();
            tm->task_type = TASK_TYPE_ACCOUNTSERVICE;
            task_map.insert(make_pair(TASK_TYPE_ACCOUNTSERVICE, tm));
        }

        create_task_manager_from_json(tm, inner_json);
    }

    if(get_value_from_json_key(_jv, "SessionService", inner_json))
    {
        // 있으면 task_map만들고
        if(task_map.find(TASK_TYPE_SESSIONSERVICE) != task_map.end())
            tm = task_map.find(TASK_TYPE_SESSIONSERVICE)->second;
        else
        {
            tm = new Task_Manager();
            tm->task_type = TASK_TYPE_SESSIONSERVICE;
            task_map.insert(make_pair(TASK_TYPE_SESSIONSERVICE, tm));
        }

        create_task_manager_from_json(tm, inner_json);
    }

    if(get_value_from_json_key(_jv, "UpdateService", inner_json))
    {
        // 있으면 task_map만들고
        if(task_map.find(TASK_TYPE_UPDATESERVICE) != task_map.end())
            tm = task_map.find(TASK_TYPE_UPDATESERVICE)->second;
        else
        {
            tm = new Task_Manager();
            tm->task_type = TASK_TYPE_UPDATESERVICE;
            task_map.insert(make_pair(TASK_TYPE_UPDATESERVICE, tm));
        }

        create_task_manager_from_json(tm, inner_json);
    }

    if(get_value_from_json_key(_jv, "CertificateService", inner_json))
    {
        // 있으면 task_map만들고
        if(task_map.find(TASK_TYPE_CERTIFICATESERVICE) != task_map.end())
            tm = task_map.find(TASK_TYPE_CERTIFICATESERVICE)->second;
        else
        {
            tm = new Task_Manager();
            tm->task_type = TASK_TYPE_CERTIFICATESERVICE;
            task_map.insert(make_pair(TASK_TYPE_CERTIFICATESERVICE, tm));
        }

        create_task_manager_from_json(tm, inner_json);
    }

    if(get_value_from_json_key(_jv, "Completed", inner_json))
    {
        // 있으면 task_map만들고
        if(task_map.find(TASK_TYPE_COMPLETED) != task_map.end())
            tm = task_map.find(TASK_TYPE_COMPLETED)->second;
        else
        {
            tm = new Task_Manager();
            tm->task_type = TASK_TYPE_COMPLETED;
            task_map.insert(make_pair(TASK_TYPE_COMPLETED, tm));
        }

        create_task_manager_from_json(tm, inner_json);
    }


    // cout << " !@#$ REBUILD !!!! ------------------- " << endl;
    // cout << get_json_task_map() << endl;

    

}

void create_task_manager_from_json(Task_Manager *_tm, json::value _jv)
{
    get_value_from_json_key(_jv, "HostIP", _tm->host_ip);
    get_value_from_json_key(_jv, "PrimaryIP", _tm->primary_ip);
    get_value_from_json_key(_jv, "PrimaryPort", _tm->primary_port);
    get_value_from_json_key(_jv, "SecondaryPort", _tm->secondary_port);
    get_value_from_json_key(_jv, "_IP", _tm->_ip);

    json::value j_list = json::value::array();

    get_value_from_json_key(_jv, "List", j_list);

    for(int i=0; i<j_list.size(); i++)
    {
        json::value j_request = j_list[i];
        
        m_Request m_req;
        get_value_from_json_key(j_request, "TaskNumber", m_req.task_number);
        get_value_from_json_key(j_request, "Method", m_req.method);
        get_value_from_json_key(j_request, "Host", m_req.host);
        get_value_from_json_key(j_request, "Uri", m_req.uri);
        get_value_from_json_key(j_request, "Datetime", m_req.request_datetime);
        get_value_from_json_key(j_request, "RequestJson", m_req.request_json);

        json::value j_response;
        get_value_from_json_key(j_request, "Result", j_response);

        m_Response m_res;
        get_value_from_json_key(j_response, "ResponseNumber", m_res.res_number);
        get_value_from_json_key(j_response, "ResultStatus", m_res.result_status);
        get_value_from_json_key(j_response, "ResultDatetime", m_res.result_datetime);

        json::value j_http_response;
        get_value_from_json_key(j_response, "ResultResponse", j_http_response);

        http_response response;
        int status_code;
        if(get_value_from_json_key(j_http_response, "StatusCode", status_code))
        {
            response.set_status_code(status_code);
            get_value_from_json_key(j_http_response, "ResponseJson", m_res.response_json);
            json::value j_header = json::value::array();
            if(get_value_from_json_key(j_http_response, "Headers", j_header))
            {
                for(int j=0; j<j_header.size(); j++)
                {
                    string h_key, h_value;
                    h_key = get_parent_object_uri(j_header[j].as_string(), "::::");
                    // cout << "key : " << h_key << endl;
                    h_value = get_current_object_name(j_header[j].as_string(), "::::");
                    // cout << "value : " << h_value << endl;
                    response.headers().add(h_key, h_value);
                }
            }

            m_res.result_response = response;
        }

        m_req.result = m_res;
        _tm->list_request.push_back(m_req);
    }

}
// task map backup은 액티브cmm에서 스탠바이cmm으로 json 주기적으로 보내면
// 스탠바이쪽에서 task_map 구축해놓고 있는걸로