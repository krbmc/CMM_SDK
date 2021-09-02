#include "task.hpp"
#include "resource.hpp"

extern unordered_map<uint8_t, Task_Manager *> task_map;


/**
 * @brief Request GET
 * @author 강
 * @details get handler에서 호출시키는 함수
 * m_Request를 만들어서 해당 타입의 task_manager에 연결시키고 keti-edge에 요청하면서 리소스task json파일을 생성,저장한다.
 * keti-edge에서 응답이 오면 m_Response를 만들어서 연결시키고 처음요청한 사용자에게 응답하며 Complete타입의 task_manager로
 * m_Request를 이동시킨다.
 */

// 락 걸어야할 부분
// task_map쓰는 category관련
// g_record쓰는 record_save_json관련
// task_numset(번호할당) 쓰는 allocate_task_num관련
// 보니깐 요청처리하는데에도 record exist나 session valid g_record 사용되네

void do_task_cmm_get(http_request _request)
{
    // cout << "Body가 궁금해서 찍어봄" << endl;
    // cout << _request.to_string() << endl;
    // cout << "Body 끝" << endl;

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

    // msg.request_json = jv;
    // // msg.host = "https://10.0.6.107:443";
    // msg.host = CMM_ADDRESS;
    // msg.method = "GET";
    // msg.uri = uri;
    // msg.request_datetime = currentDateTime();
    // msg.task_number = allocate_task_num(); // @@@@ LOCK

    // Make m_Response
    // m_Response msg_res;
    // msg_res.res_number = msg.task_number;
    // msg.result = msg_res;

    http_response response;
    response.headers().add("Access-Control-Allow-Origin", "*");
    response.headers().add("Access-Control-Allow-Credentials", "true");
    response.headers().add("Access-Control-Allow-Methods", "POST, GET, OPTIONS, DELETE");
    response.headers().add("Access-Control-Max-Age", "3600");
    response.headers().add("Access-Control-Allow-Headers", "Content-Type, Accept, X-Requested-With, remember-me");


    json::value response_json;


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
        _request.reply(response);
        // _request.reply(status_codes::NotFound);
        // return ;
    }
    else
    {
        // http_response tmp_res(status_codes::OK);
        json::value jj;
        jj = record_get_json(uri);
        if(jj.as_object().find("Password") != jj.as_object().end())
            jj[U("Password")] = json::value::null();
        // account의 password
        if(jj.as_object().find("AuthToken") != jj.as_object().end())
            jj[U("AuthToken")] = json::value::null();
        // session의 authtoken
        response.set_status_code(status_codes::OK);
        response.set_body(jj);
        msg.result.result_datetime = currentDateTime();
        msg.result.result_status = WORK_SUCCESS;
        msg.result.result_response = response;
        _request.reply(response);
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
    // _request.reply(status_codes::OK, record_get_json(uri));
    cout << record_get_json(uri) << endl;
    cout << "*************************     Out CMM GET    *******************" << endl;

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
    cout << "taskmap size : " << task_map.size() << endl;

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

    // msg.request_json = jv;
    // // msg.host = "http://10.0.6.104:443";
    // msg.host = module_id_table[uri_tokens[3]];
    // msg.method = "GET";
    // msg.uri = new_uri;
    // msg.request_datetime = currentDateTime();
    // msg.task_number = allocate_task_num();

    // connect to Task_manager
    t_manager->list_request.push_back(msg);
    // 이거 할때 요청 uri잘못들어오면 /redfish/v1/Managers인데 /Manager로 들어오면 제대론된 포인터가 연결안되어있어서 터짐
    
    
    // Make Resource Task Json
    // string task_odata = ODATA_TASK_ID;
    // task_odata = task_odata + "/" + to_string(msg.task_number);
    // cout << "task_data : " << task_odata << endl;
    // Task *task = new Task(task_odata, "Task-" + to_string(msg.task_number));
    // task->start_time = msg.request_datetime;
    // task->set_payload(_request.headers(), "GET", jv, uri);
    // record_save_json();
    
    http_client client(msg.host);
    http_request req(methods::GET);
    // req.set_request_uri(msg.uri);
    req.set_request_uri(new_uri);
    req.set_body(jv);
    
    // cout << "이거 때문이야?" << endl;
    if(_request.headers().find("X-Auth-Token") != _request.headers().end())
        req.headers().add(_request.headers().find("X-Auth-Token")->first, _request.headers().find("X-Auth-Token")->second);
    // req.headers().add(U("TEST_HEADER"), U("VALUE!!"));
    // cout << "그럴거야 아마" << endl;
    // 나중에는 이 헤더붙이는게 이런식이 아니라 현재 이 cmm을 이용할수있게 하는 세션session에 접근해서 거기에 저장되어있는
    // bmc_id에 맞는 토큰이 있으면 그걸로 여기서 헤더에 추가를 해주면 bmc토큰인증이 되는식으로 바뀌어야함
    // 만약없다면 로그인하라고 해야하고 아 여기가 아니고 응답오는 위치에서 response status가 인증을 못뚫는 status면 그때
    // 해야할듯
    // >> cmm이 보내는건 bmc로그인 필요없게끔해야되는것이 아닌가 (cmm만 로그인)
    http_response response;
    response.headers().add("Access-Control-Allow-Origin", "*");
    response.headers().add("Access-Control-Allow-Credentials", "true");
    response.headers().add("Access-Control-Allow-Methods", "POST, GET, OPTIONS, DELETE");
    response.headers().add("Access-Control-Max-Age", "3600");
    response.headers().add("Access-Control-Allow-Headers", "Content-Type, Accept, X-Requested-With, remember-me");
    
    json::value response_json;
    m_Response msg_res;

    // Send Request
    try
    {
        /* code */
        
        pplx::task<http_response> responseTask = client.request(req);
        response = responseTask.get();
        // response_json = response.extract_json().get();
        cout << "bmc get reply status : " << response.status_code() << endl;
        // cout << response.body() << endl;
        // response.set_body(response_json);

        // 여기서 응답온거 이거 get이니까 ok or 아닌거 해서 ok면 성공 아니면 실패로해서 나눠 리플라이

        if(response.status_code() == status_codes::OK)
            msg.result.result_status = WORK_SUCCESS;
        else
            msg.result.result_status = WORK_FAIL;

        msg.result.result_datetime = currentDateTime();
        msg.result.result_response = response;
        _request.reply(response);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        cout << "BMC 서버 닫혀있을거임~~" << endl;

        msg = reply_error(_request, msg, get_error_json("BMC Server Connection Error"), status_codes::InternalError);
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


    // ((Task *)g_record[task_odata])->end_time = msg.result.result_datetime;
    // ((Task *)g_record[task_odata])->task_state = TASK_STATE_COMPLETED;
    // record_save_json();
    // _request.reply(U(response.status_code()), response_json);
}

/**
 * @todo 1.현재 엣지로 요청을 보낼때 헤더를 같이 못보내고 있어서 x-auth-token 같은거 뚫는 테스트를 못함 -- 해결
 *  2.요청 보내고 응답받고나서 매니지task 가 컴플리트로 간 후에 언제 삭제될 지  그리고 그때 만들어 둔 리소스task 의 json파일
 *  삭제 처리 해줘야함
 *  3. 예외처리 try,catch 등..
 * 
 */

void do_task_cmm_post(http_request _request)
{
    string uri = _request.request_uri().to_string();
    json::value jv = _request.extract_json().get();
    vector<string> uri_tokens = string_split(uri, '/');

    Task_Manager *t_manager; // 작업 매니저
    Task_Manager *c_manager; // 컴플리트 매니저

    cout << "!@#$ CMM POST TASK ~~~~" << endl;
    cout << "Task size : " << task_map.size() << endl;
    log(error) << "Task size : " << task_map.size();

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

    cout << "After T" << endl;
    cout << "Task size : " << task_map.size() << endl;

    msg = work_before_request_process("POST", CMM_ADDRESS, uri, jv, _request.headers());
    

    http_response response;
    json::value response_json;

    // connect to Task_manager
    t_manager->list_request.push_back(msg);


    // Uri에 따른 처리동작
    msg = treat_uri_cmm_post(_request, msg, jv);
    // if(uri == ODATA_ACCOUNT_ID)
    // {
    //     msg = make_account(_request, msg, jv);
    // }
    // else if(uri == ODATA_SESSION_ID)
    // {
    //     msg = make_session(_request, msg, jv);
    // }
    // else
    // {
    //     // json::value j;
    //     // j[U("Error")] = json::value::string(U("No Action by POST"));
    //     // _request.reply(status_codes::BadRequest, j);
    //     msg = reply_error(_request, msg, "No Action by POST", status_codes::NotImplemented);
    // }
    // post동작이 추가되고 bmc post로 인해 cmm에 반영할게 많아지면 함수화 해야할듯

    // c_manager = wwww(t_manager, msg);
    // cout << "After C" << endl;
    // cout << "Task size : " << task_map.size() << endl;
    // cout << "c_manager info" << endl;

    // m_Request test_m1, test_m2;
    // test_m1 = c_manager->list_request.front();
    // test_m2 = task_map.find(TASK_TYPE_COMPLETED)->second->list_request.front();
    // cout << "c_list tasknum : " << test_m1.task_number << " / " << test_m2.task_number << endl;
    // cout << "c_list host : " << test_m1.host << " / " << test_m2.host << endl;
    // cout << "c_list uri : " << test_m1.uri << " / " << test_m2.uri << endl;
    // cout << "c_list method : " << test_m1.method << " / " << test_m2.method << endl;
    // cout << "c_list datetime : " << test_m1.request_datetime << " / " << test_m2.request_datetime << endl;
    // cout << "c_list result->resnum : " << test_m1.result.res_number << " / " << test_m2.result.res_number << endl;
    // cout << "c_list result->datetime : " << test_m1.result.result_datetime << " / " << test_m2.result.result_datetime << endl;

    c_manager = work_after_request_process(t_manager, msg);
    

    // msg.result = msg_res;

    // completed로 연결
    // c_manager->list_request.push_back(msg);

    // // json 파일 내용 갱신
    // ((Task *)g_record[task_odata])->end_time = msg.result.result_datetime;
    // ((Task *)g_record[task_odata])->task_state = TASK_STATE_COMPLETED;
    // record_save_json();

    cout << "*****************************     Out CMM POST     *****************************" << endl;
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
    cout << "ss : " << task_map.size() << endl;

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

    // msg.request_json = jv;
    // // msg.host = "http://10.0.6.104:443";
    // msg.host = module_id_table[uri_tokens[3]];
    // msg.method = "POST";
    // msg.uri = new_uri;
    // msg.request_datetime = currentDateTime();
    // msg.task_number = allocate_task_num();

    // connect to Task_manager
    t_manager->list_request.push_back(msg);

    // Make Resource Task Json
    // string task_odata = ODATA_TASK_ID;
    // task_odata = task_odata + "/" + to_string(msg.task_number);
    // cout << "task_data : " << task_odata << endl;
    // Task *task = new Task(task_odata, "Task-" + to_string(msg.task_number));
    // task->start_time = msg.request_datetime;
    // task->set_payload(_request.headers(), "POST", jv, uri);
    // record_save_json();

    // cout << "Before Request" << endl;

    http_client client(msg.host);
    http_request req(methods::POST);
    // req.headers().add(U("MY_HEADER"), U("HEADER_VALUE"));
    req.set_request_uri(new_uri);
    // req.set_request_uri(msg.uri);
    req.set_body(jv);

    http_response response;
    response.headers().add("Access-Control-Allow-Origin", "*");
    response.headers().add("Access-Control-Allow-Credentials", "true");
    response.headers().add("Access-Control-Allow-Methods", "POST, GET, OPTIONS, DELETE");
    response.headers().add("Access-Control-Max-Age", "3600");
    response.headers().add("Access-Control-Allow-Headers", "Content-Type, Accept, X-Requested-With, remember-me");

    json::value response_json;
    m_Response msg_res;

    // Send Request
    try
    {
        /* code */
        pplx::task<http_response> responseTask = client.request(req);
        response = responseTask.get();
        // response_json = response.extract_json().get();
        cout << "bmc post reply status : " << response.status_code() << endl;

        if(response.status_code() == status_codes::OK)
        {
            msg.result.result_status = WORK_SUCCESS;
            // cmm에 적용시켜야할거 있으면 여기서 해주면됨
        }
        else
            msg.result.result_status = WORK_FAIL;

        msg.result.result_datetime = currentDateTime();
        msg.result.result_response = response;
        _request.reply(response);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        cout << "BMC 서버 닫혀있을거임~~" << endl;

        msg = reply_error(_request, msg, get_error_json("BMC Server Connection Error"), status_codes::InternalError);
    }

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



    // Completed로 이동시키기 위해 기존 task_manager에서 삭제
    // std::list<m_Request>::iterator iter;
    // for(iter=t_manager->list_request.begin(); iter!=t_manager->list_request.end(); iter++)
    // {
    //     if(iter->task_number == msg.task_number)
    //     {
    //         t_manager->list_request.erase(iter);
    //         break;
    //     }
    // }

    // if(task_map.find(TASK_TYPE_COMPLETED) == task_map.end())
    // {
    //     c_manager = new Task_Manager();
    //     c_manager->task_type = TASK_TYPE_COMPLETED;
    //     task_map.insert(make_pair(TASK_TYPE_COMPLETED, c_manager));
    // }
    // else
    //     c_manager = task_map.find(TASK_TYPE_COMPLETED)->second;

    // work_after_request_process(t_manager, c_manager, msg);
    c_manager = work_after_request_process(t_manager, msg);


    // completed로 연결
    // c_manager->list_request.push_back(msg);
    

    // ((Task *)g_record[task_odata])->end_time = msg.result.result_datetime;
    // ((Task *)g_record[task_odata])->task_state = TASK_STATE_COMPLETED;
    // record_save_json();
    

    // cout << "After Request" << endl;
    // cout << req.headers().find("MY_HEADER")->second << endl;

    // _request.reply(response);
    // _request.reply(U(response.status_code()), response_json);
    
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
    msg = work_before_request_process("PATCH", CMM_ADDRESS, uri, jv, _request.headers());

    t_manager->list_request.push_back(msg);


    // 요청처리
    msg = treat_uri_cmm_patch(_request, msg, jv);

    // work_after_request_process(t_manager, c_manager, msg);
    c_manager = work_after_request_process(t_manager, msg);

    cout << "*****************************     Out CMM PATCH     *****************************" << endl;
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
    cout << "ss : " << task_map.size() << endl;

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
            msg = treat_uri_bmc_patch(_request, msg, jv);
        }
        else
        {
            // 실패, 바로응답
            msg = reply_error(_request, msg, get_error_json(""), response.status_code());
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        cout << "BMC 서버 닫혀있을거임~~" << endl;

        msg = reply_error(_request, msg, get_error_json("BMC Server Connection Error"), status_codes::InternalError);
    }

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

    // work_after_request_process(t_manager, c_manager, msg);
    c_manager = work_after_request_process(t_manager, msg);

    // _request.reply(U(response.status_code()), response_json);
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
    msg = work_before_request_process("DELETE", CMM_ADDRESS, uri, jv, _request.headers());

    t_manager->list_request.push_back(msg);

    // 처리처리~~
    msg = treat_uri_cmm_delete(_request, msg, jv);

    c_manager = work_after_request_process(t_manager, msg);
    cout << "*****************************     Out CMM DELETE     *****************************" << endl;
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
    cout << "ss : " << task_map.size() << endl;

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
            msg = reply_success(_request, msg, json::value::null(), status_codes::OK);
            // 임시로 그냥 성공답변
        }
        else
        {
            // 실패
            msg = reply_error(_request, msg, get_error_json("bmc delete fail"), response.status_code());
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        cout << "BMC 서버 닫혀있을거임~~" << endl;

        msg = reply_error(_request, msg, get_error_json("BMC Server Connection Error"), status_codes::InternalError);
    }

    c_manager = work_after_request_process(t_manager, msg);
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
    msg.task_number = allocate_task_num(); // @@@@ LOCK

    m_Response res;
    res.res_number = msg.task_number;
    msg.result = res;
    // 해당 task_manager에 들어갈 request msg와 response msg의 사전작업

    string task_odata = ODATA_TASK_ID;
    task_odata = task_odata + "/" + to_string(msg.task_number);
    cout << "task_data in wbrp func!! : " << task_odata << endl;
    Task *task = new Task(task_odata, "Task-" + to_string(msg.task_number));
    ((Collection *)g_record[ODATA_TASK_ID])->add_member(task);
    task->start_time = msg.request_datetime;
    task->set_payload(_header, _method, _jv, _uri);
    record_save_json();
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
    record_save_json();
    // request의 리소스 task 정보 추가,변경

    return complete;
}

m_Request treat_uri_cmm_post(http_request _request, m_Request _msg, json::value _jv)
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
        _msg = do_actions(_request, _msg, _jv);
        return _msg;
        // Actions전까지 딴  uri가 맞는리소스면 첫번째통과
        // 그 리소스의 action맵에 target과 uri전체가 맞아떨어지면 두번째통과
        // 그다음에 액션종류에 따라 나눠지기

        // string uri_before_action = get_parent_object_uri(minus_one, "/"); // 액션리소스 uri가 될것임
        // cout << "URI_BEFORE_ACTION : " << uri_before_action << endl;
        // if(!record_is_exist(uri_before_action))
        // {
        //     _msg = reply_error(_request, _msg, "URI Input Error in action before(resource)", status_codes::BadRequest);
        //     return _msg;
        // }

        // string uri_after_action = get_current_object_name(uri, "/");
        // cout << "URI_AFTER_ACTION : " << uri_after_action << endl;

        // string action_by, action_what;
        // action_by = get_parent_object_uri(uri_after_action, ".");
        // action_what = get_current_object_name(uri_after_action, ".");
        // cout << "By : " << action_by << endl;
        // cout << "What : " << action_what << endl;

        // if(action_by == "Bios")
        // {

        // }
        // else if(action_by == "Certificate")
        // {}
        // else if(action_by == "CertificateService")
        // {}
        // else if(action_by == "ComputerSystem")
        // {}
        // else if(action_by == "Chassis")
        // {}
        // else if(action_by == "Manager")
        // {}
        // else if(action_by == "EventService")
        // {}
        // else if(action_by == "LogService")
        // {
        //     LogService *log_service = (LogService *)g_record[uri_before_action];
        //     if(log_service->actions.find(action_what) == log_service->actions.end())
        //     {
        //         // action_what에 해당하는 액션정보가 없음 error
        //         _msg = reply_error(_request, _msg, "No Action in LogService", status_codes::BadRequest);
        //         return _msg;
        //     }
        //     // cout << "BOOM!" << endl;
        //     // 클리어로그밖에 없으니까 그냥 바로 실행
        //     if(!(log_service->ClearLog()))
        //     {
        //         _msg = reply_error(_request, _msg, "Problem occur in ClearLog()", status_codes::BadRequest);
        //         return _msg;
        //     }

        //     _msg = reply_success(_request, _msg, "", status_codes::OK);

        // }
        // else if(action_by == "UpdateService")
        // {}
        // else
        // {
        //     _msg = reply_error(_request, _msg, "URI Input Error in action_by check", status_codes::BadRequest);
        //     return _msg;
        // }

        // // action_by로 캐스팅이 안돼서 action_by로 그냥 액션있는리소스들중에 뭔지 비교일일이 해야함
        // // 그렇게 해서 리소스찾고 거기에 actions맵 에 actions[actions_what]으로 하면 해당 Actions구조체 접근가능
        // // 액션왓으로 맵에 접근까지 되면 맞는 uri긴 하겟네 굳이 target하고inputuri랑 비교안해도될거같고
        // // 찾으면 함수호출 ㅇㅋ

    }

    if(uri == ODATA_ACCOUNT_ID)
    {
        _msg = make_account(_request, _msg, _jv);
        return _msg;
    }
    else if(uri == ODATA_SESSION_ID)
    {
        _msg = make_session(_request, _msg, _jv);
        return _msg;
    }
    else if(uri_part == ODATA_SYSTEM_ID)
    {
        string cmm_system = ODATA_SYSTEM_ID;
        cmm_system = cmm_system + "/" + CMM_ID;

        if(minus_one == cmm_system + "/LogServices")
        {
            //uri가 ~~/LogServices/Logservice_id 까지 들어왓을거니깐
            _msg = make_logentry(_request, _msg, _jv);
            return _msg;
            
        } 
        // 액션검사를 uri이용하면되겟네 우선  minus_one이 Actions이면 
        // 정상적인 액션uri라면 ~~~/Actions/LogService.ClearLog 처럼 Action의 target이 uri로 들어오겟지
        // 그게 아니면 정상적인 액션요청uri가 아니니깐 아웃시키면되고
        // 그러면 액션전까지만 딴 uri로 리소스접근하면 Actions맵이 있을테니깐 거기서 target하고 비교해
        // 그러면 uri == target해서 맞으면 맞는액션uri지 

    }

    // part화 해야겟네 시스템에서 매니저에서 로그만드는거 >> 핸들러로 필요없음

    // 위에서 안걸리면 에러
    _msg = reply_error(_request, _msg, get_error_json("URI Input Error in treat POST"), status_codes::NotImplemented);
    return _msg;
}

m_Request treat_uri_cmm_patch(http_request _request, m_Request _msg, json::value _jv)
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

    // json::value response_json;
    // http_response response;

// adf
    if(uri_part == ODATA_ACCOUNT_SERVICE_ID)
    {
        // /redfish/v1/AccountService 처리
        if(uri == ODATA_ACCOUNT_SERVICE_ID)
        {
            patch_account_service(_jv, ODATA_ACCOUNT_SERVICE_ID);

            _msg = reply_success(_request, _msg, record_get_json(uri), status_codes::OK);
            return _msg;
        }

        // /redfish/v1/AccountService/~~~ 관련처리부
        string minus_one = get_parent_object_uri(uri, "/");
        cout << "MINUS_ONE INFO : " << minus_one << endl;

        // /redfish/v1/AccountService/Accounts/[Account_id]
        if(minus_one == ODATA_ACCOUNT_ID)
        {
            _msg = modify_account(_request, _msg, _jv, uri);
            return _msg;
        }
        // /redfish/v1/AccountService/Roles/[Role_id]
        else if(minus_one == ODATA_ROLE_ID)
        {
            _msg = modify_role(_request, _msg, _jv, uri);
            return _msg;
        }
        else
        {
            // 에러에러
            // /redfish/v1/AccountService/다른uri 혹은 /redfish/v1/AccountService/Accounts or Roles 여도 길이 초과하는경우
            cout << "ERROR POSITION" << endl;
            _msg = reply_error(_request, _msg, get_error_json("URI Input Error in AccountService part"), status_codes::BadRequest);
            return _msg;
        }
        // 요기 else밑압축가능
    }
    else if(uri_part == ODATA_SESSION_SERVICE_ID)
    {
        // /redfish/v1/SessionService 처리
        if(uri == ODATA_SESSION_SERVICE_ID)
        {
            patch_session_service(_jv);

            _msg = reply_success(_request, _msg, record_get_json(uri), status_codes::OK);
            return _msg;
        }
        else
        {
            // 세션서비스 자체 그외의 uri는 에러처리
            _msg = reply_error(_request, _msg, get_error_json("URI Input Error in SessionService part"), status_codes::BadRequest);
            return _msg;
        }
        // 요기 else밑압축가능

    }
    else if(uri_part == ODATA_MANAGER_ID)
    {
        // 매니저는 권한 관리자급이어야함!! 나중에 권한검사 필요

        if(uri_tokens.size() == 3)
        {
            // 이거는 딱 /redfish/v1/Managers/ 까지임 아웃
            _msg = reply_error(_request, _msg, get_error_json("URI Input Error in Manager part"), status_codes::BadRequest);
            return _msg;
        }
        // 요기 else밑압축가능

        // 통과했으면 cmm_patch로 들어온거라 /redfish/v1/Managers/cmm-id 까지는 붙어있음
        string cmm_manager = ODATA_MANAGER_ID;
        cmm_manager = cmm_manager + "/" + CMM_ID;

        // /redfish/v1/Managers/cmm_id 그 자체
        if(uri == cmm_manager)
        {
            patch_manager(_jv, uri);

            _msg = reply_success(_request, _msg, record_get_json(uri), status_codes::OK);
            return _msg;
        }

        // /redfish/v1/Managers/cmm_id/NetworkProtocol
        if(uri == cmm_manager + "/NetworkProtocol")
        {
            // 네트워크프로토콜 정보수정 위치
            if(!record_is_exist(uri))
            {
                // 해당 네트워크프로토콜 레코드 없음
                _msg = reply_error(_request, _msg, get_error_json("No NetworkProtocol"), status_codes::BadRequest);
                return _msg;
            }

            patch_network_protocol(_jv, uri);
            _msg = reply_success(_request, _msg, record_get_json(uri), status_codes::OK);
            return _msg;
        }

        // /redfish/v1/Managers/cmm_id/FanMode
        if(uri == cmm_manager + "/FanMode")
        {
            string mode;
            // cout << " jv : " << _jv.serialize() << endl;
            if(_jv == json::value::null())
            {
                _msg = reply_error(_request, _msg, get_error_json("Json Body is Null"), status_codes::BadRequest);
                return _msg;
            }

            if(_jv.as_object().find("Mode") != _jv.as_object().end())
            {
                mode = _jv.at("Mode").as_string();
                if(!(mode == "Standard" || mode == "FullSpeed" || mode == "Auto"))
                {
                    _msg = reply_error(_request, _msg, get_error_json("Incorrect Mode"), status_codes::BadRequest);
                    return _msg;
                }
            }
            else
            {
                _msg = reply_error(_request, _msg, get_error_json("No Mode"), status_codes::BadRequest);
                return _msg;
            }

            patch_fan_mode(mode, uri);

            _msg = reply_success(_request, _msg, json::value::null(), status_codes::OK);
            return _msg;
        }

        string minus_one = get_parent_object_uri(uri, "/");
        cout << "MINUS_ONE INFO : " << minus_one << endl;

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
                _msg = reply_error(_request, _msg, get_error_json("No EthernetInterface"), status_codes::BadRequest);
                return _msg;
            }

            patch_ethernet_interface(_jv, uri);
            patch_ethernet_interface(_jv, sys_uri);
            // EthernetInterfaces PATCH를 Manager에서 하면 System에도 반영되고
            // System에선 지원안하는걸로

            _msg = reply_success(_request, _msg, record_get_json(uri), status_codes::OK);
            return _msg;
        }


    }
    else if(uri_part == ODATA_SYSTEM_ID)
    {
        string cmm_system = ODATA_SYSTEM_ID;
        cmm_system = cmm_system + "/" + CMM_ID;

        if(uri == cmm_system)
        {
            patch_system(_jv, uri);

            _msg = reply_success(_request, _msg, record_get_json(uri), status_codes::OK);
            return _msg;
        }
        else
        {
            _msg = reply_error(_request, _msg, get_error_json("URI Input Error in Systems part"), status_codes::BadRequest);
            return _msg;
        }// 맨아래 reply_error로 통합가능
    }
    else if(uri_part == ODATA_CHASSIS_ID)
    {
        string cmm_chassis = ODATA_CHASSIS_ID;
        cmm_chassis = cmm_chassis + "/" + CMM_ID;

        if(uri == cmm_chassis)
        {
            patch_chassis(_jv, uri);
            _msg = reply_success(_request, _msg, record_get_json(uri), status_codes::OK);
    
            return _msg;
        }

        // int len = uri_tokens.size();
        string minus_one = get_parent_object_uri(uri, "/");
        cout << "MINUS_ONE INFO : " << minus_one << endl;

        if(minus_one == cmm_chassis + "/Power/PowerControl")
        {
            if(!record_is_exist(uri))
            {
                // 해당 파워컨트롤 레코드 없음
                _msg = reply_error(_request, _msg, get_error_json("No PowerControl"), status_codes::BadRequest);
                return _msg;
            }

            patch_power_control(_jv, uri);
            _msg = reply_success(_request, _msg, record_get_json(uri), status_codes::OK);

            return _msg;
        }
        // 아래에서 reply_error
    }
    else
    {
        _msg = reply_error(_request, _msg, get_error_json("URI Input Error in Whole part"), status_codes::BadRequest);
        return _msg;
    }

    _msg = reply_error(_request, _msg, get_error_json("URI Input Error in Below part"), status_codes::BadRequest);
    // 위에 조건문들에서 처리에 걸렸으면 처리하는거고 아니면 걍 여기서 한꺼번에 reply_error처리로 하지
    return _msg;

}

m_Request treat_uri_bmc_patch(http_request _request, m_Request _msg, json::value _jv)
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

// 다른예외처리들은 bmc로 요청을 보냈을때 걸러졌을거라 봄(OK 받은후 오는 함수라)
    if(uri_part == ODATA_MANAGER_ID)
    {
        string bmc_manager = ODATA_MANAGER_ID;
        bmc_manager = bmc_manager + "/" + uri_tokens[3];
        
        // /redfish/v1/Managers/bmc_id
        if(uri == bmc_manager)
        {
            patch_manager(_jv, uri);

            _msg = reply_success(_request, _msg, record_get_json(uri), status_codes::OK);
            return _msg;
        }

        // /redfish/v1/Managers/bmc_id/NetworkProtocol
        if(uri == bmc_manager + "/NetworkProtocol")
        {
            patch_network_protocol(_jv, uri);
            _msg = reply_success(_request, _msg, record_get_json(uri), status_codes::OK);
            return _msg;
        }

        // /redfish/v1/Managers/bmc_id/FanMode
        if(uri == bmc_manager + "/FanMode")
        {
            string mode;
            mode = _jv.at("Mode").as_string();
            patch_fan_mode(mode, uri);

            _msg = reply_success(_request, _msg, json::value::null(), status_codes::OK);
            return _msg;
        }

        // /redfish/v1/Managers/bmc_id/AccountService
        if(uri == bmc_manager + "/AccountService")
        {
            patch_account_service(_jv, uri);
            _msg = reply_success(_request, _msg, record_get_json(uri), status_codes::OK);
            return _msg;
        }

        string minus_one = get_parent_object_uri(uri, "/");
        cout << "MINUS_ONE INFO : " << minus_one << endl;

        // /redfish/v1/Managers/bmc_id/AccountService/Accounts/[Account_id]
        if(minus_one == bmc_manager + "/AccountService/Accounts")
        {
            //modify_account 수정후 ok받은 여기서는 바로 수행하게끔
        }

        // /redfish/v1/Managers/bmc_id/EthernetInterfaces/[Ethernet_id]
        if(minus_one == bmc_manager + "/EthernetInterfaces")
        {
            string sys_uri = ODATA_SYSTEM_ID;
            string eth_num = get_current_object_name(uri, "/");
            sys_uri = sys_uri + "/" + uri_tokens[3] + "/EthernetInterfaces";
            sys_uri = sys_uri + "/" + eth_num;

            patch_ethernet_interface(_jv, uri);
            patch_ethernet_interface(_jv, sys_uri);

            _msg = reply_success(_request, _msg, record_get_json(uri), status_codes::OK);
            return _msg;
        }

    }
    else if(uri_part == ODATA_SYSTEM_ID)
    {
        string bmc_system = ODATA_SYSTEM_ID;
        bmc_system = bmc_system + "/" + uri_tokens[3];

        if(uri == bmc_system)
        {
            patch_system(_jv, uri);

             _msg = reply_success(_request, _msg, record_get_json(uri), status_codes::OK);
            return _msg;
        }
    }
    else if(uri_part == ODATA_CHASSIS_ID)
    {
        string bmc_chassis = ODATA_CHASSIS_ID;
        bmc_chassis = bmc_chassis + "/" + uri_tokens[3];

        if(uri == bmc_chassis)
        {
            patch_chassis(_jv, uri);
            _msg = reply_success(_request, _msg, record_get_json(uri), status_codes::OK);
    
            return _msg;
        }

        string minus_one = get_parent_object_uri(uri, "/");
        cout << "MINUS_ONE INFO : " << minus_one << endl;

        if(minus_one == bmc_chassis + "/Power/PowerControl")
        {
            patch_power_control(_jv, uri);
            _msg = reply_success(_request, _msg, record_get_json(uri), status_codes::OK);
    
            return _msg;
        }

    }

    // 그럴일은 없겠지만
    _msg = reply_error(_request, _msg, get_error_json("Why error?"), status_codes::BadRequest);
    return _msg;
}

m_Request treat_uri_cmm_delete(http_request _request, m_Request _msg, json::value _jv)
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

    if(uri_part == ODATA_ACCOUNT_SERVICE_ID)
    {
        // /redfish/v1/AccountService/Accounts
        if(uri == ODATA_ACCOUNT_ID)
        {
            _msg = remove_account(_request, _msg, _jv, ODATA_ACCOUNT_SERVICE_ID);
            return _msg;
        }
    }
    else if(uri_part == ODATA_SESSION_SERVICE_ID)
    {
        if(uri == ODATA_SESSION_ID)
        {
            // 로그아웃!
            _msg = remove_session(_request, _msg);
            return _msg;
        }
    }

    _msg = reply_error(_request, _msg, get_error_json("URI Input Error in treat delete"), status_codes::BadRequest);
    return _msg;
}

m_Request do_actions(http_request _request, m_Request _msg, json::value _jv)
{
    string uri = _request.request_uri().to_string();
    string resource_uri = get_parent_object_uri(get_parent_object_uri(uri, "/"), "/");
    string action_uri = get_current_object_name(uri, "/");

    cout << "Full uri : " << uri << endl;
    cout << "Resource : " << resource_uri << endl;
    cout << "Action : " << action_uri << endl;

    if(!record_is_exist(resource_uri))
    {
        _msg = reply_error(_request, _msg, get_error_json("URI Input Error in Resource part"), status_codes::BadRequest);
        return _msg;
    }

    string action_by, action_what;
    action_by = get_parent_object_uri(action_uri, ".");
    action_what = get_current_object_name(action_uri, ".");
    cout << "By : " << action_by << endl;
    cout << "What : " << action_what << endl;

    if(action_by == "Bios")
    {
        Bios *bios = (Bios *)g_record[resource_uri];
        if(bios->actions.find(action_what) == bios->actions.end())
        {
            _msg = reply_error(_request, _msg, get_error_json("No Action in Bios"), status_codes::BadRequest);
            return _msg;
        }

        if(action_what == "ResetBios")
        {
            if(!(bios->ResetBios()))
            {
                _msg = reply_error(_request, _msg, get_error_json("Problem occur in ResetBios()"), status_codes::BadRequest);
                return _msg;
            }

            _msg = reply_success(_request, _msg, json::value::null(), status_codes::OK);
            return _msg;
        }
        else if(action_what == "ChangePassword")
        {
            // json으로 NewPassword, OldPassword, PasswordName 3개 필요
        }

    }
    else if(action_by == "Certificate")
    {
        Certificate *certi = (Certificate *)g_record[resource_uri];
        if(certi->actions.find(action_what) == certi->actions.end())
        {
            _msg = reply_error(_request, _msg, get_error_json("No Action in Certificate"), status_codes::BadRequest);
            return _msg;
        }

        if(action_what == "ReKey")
        {
            json::value result;
            result = certi->Rekey(_jv);
            if(result == json::value::null())
            {
                // 리퀘스트바디오류
                _msg = reply_error(_request, _msg, get_error_json("Request Body error in Certificate Rekey"), status_codes::BadRequest);
                return _msg;
            }
            else
            {
                json::value check;
                if(get_value_from_json_key(result, "Failed", check))
                {
                    _msg = reply_error(_request, _msg, result, status_codes::BadRequest);
                    return _msg;
                }
                else if(get_value_from_json_key(result, "CertificateCollection", check))
                {
                    _msg = reply_success(_request, _msg, result, status_codes::OK);
                    return _msg;
                }
            }
            // rsp에 Failed가 있으면 실패
            // rsp에 CertificatieCollection이 있으면 성공
        }
        else if(action_what == "ReNew")
        {
            json::value result;
            result = certi->Renew();
            if(result == json::value::null())
            {
                // 리퀘스트바디오류
                _msg = reply_error(_request, _msg, get_error_json("No Files Error in Certificate ReNew"), status_codes::BadRequest);
                return _msg;
            }
            else
            {
                json::value check;
                if(get_value_from_json_key(result, "Failed", check))
                {
                    _msg = reply_error(_request, _msg, result, status_codes::BadRequest);
                    return _msg;
                }
                else if(get_value_from_json_key(result, "CertificateCollection", check))
                {
                    _msg = reply_success(_request, _msg, result, status_codes::OK);
                    return _msg;
                }
            }
        }
    }
    else if(action_by == "CertificateService")
    {
        CertificateService *cert_service = (CertificateService *)g_record[resource_uri];
        if(cert_service->actions.find(action_what) == cert_service->actions.end())
        {
            _msg = reply_error(_request, _msg, get_error_json("No Action in Certificate Service"), status_codes::BadRequest);
            return _msg;
        }
        
        if(action_what == "GenerateCSR")
        {
            // json받아서 json으로 나옴
            json::value result;
            cert_service->GenerateCSR(_jv);
            json::value check;
            if(get_value_from_json_key(result, "Failed", check))
            {
                _msg = reply_error(_request, _msg, result, status_codes::BadRequest);
                return _msg;
            }
            else if(get_value_from_json_key(result, "CertificateCollection", check))
            {
                _msg = reply_success(_request, _msg, result, status_codes::OK);
                return _msg;
            }
            /// 함수수정해야겟다 reply_error(req, msg, string, status)에서
            // string을 jv로 해서 jv받는걸로하고 스트링받아서 기본 jv만들어주는함수를 추가로 만들기
            // reply_success(req, msg, uri(str), status)에서
            // 기존 uri받아서 안에서 record_get_json(uri)하는데 이걸 jv로 넣고 기본것들은 인자로 넣어줄때
            // record_get_json(uri)로 넘겨주는식으로

        }
        else if(action_what == "ReplaceCertificate")
        {
            if(!cert_service->ReplaceCertificate(_jv))
            {
                _msg = reply_error(_request, _msg, get_error_json("Problem occur in ReplaceCertificate()"), status_codes::BadRequest);
                return _msg;
            }

            _msg = reply_success(_request, _msg, json::value::null(), status_codes::OK);
            return _msg;
        }


    }
    else if(action_by == "ComputerSystem")
    {
        Systems *system = (Systems *)g_record[resource_uri];
        if(system->actions.find(action_what) == system->actions.end())
        {
            // action_what에 해당하는 액션정보가 없음 error
            _msg = reply_error(_request, _msg, get_error_json("No Action in ComputerSystem"), status_codes::BadRequest);
            return _msg;
        }

        if(!(system->Reset(_jv)))
        {
            _msg = reply_error(_request, _msg, get_error_json("Problem occur in Reset()"), status_codes::BadRequest);
            return _msg;
        }
        _msg = reply_success(_request, _msg, json::value::null(), status_codes::OK);
        return _msg;
        
    }
    else if(action_by == "Chassis")
    {}
    else if(action_by == "Manager")
    {}
    else if(action_by == "EventService")
    {
        EventService *ev_service = (EventService *)g_record[resource_uri];
        if(ev_service->actions.find(action_what) == ev_service->actions.end())
        {
            // action_what에 해당하는 액션정보가 없음 error
            _msg = reply_error(_request, _msg, get_error_json("No Action in EventService"), status_codes::BadRequest);
            return _msg;
        }

        
    }
    else if(action_by == "LogService")
    {
        LogService *log_service = (LogService *)g_record[resource_uri];
        if(log_service->actions.find(action_what) == log_service->actions.end())
        {
            // action_what에 해당하는 액션정보가 없음 error
            _msg = reply_error(_request, _msg, get_error_json("No Action in LogService"), status_codes::BadRequest);
            return _msg;
        }
        // cout << "BOOM!" << endl;
        // 클리어로그밖에 없으니까 그냥 바로 실행
        if(!(log_service->ClearLog()))
        {
            _msg = reply_error(_request, _msg, get_error_json("Problem occur in ClearLog()"), status_codes::BadRequest);
            return _msg;
        }

        _msg = reply_success(_request, _msg, json::value::null(), status_codes::OK);
        return _msg;
    }
    else if(action_by == "UpdateService")
    {}
    else
    {
        _msg = reply_error(_request, _msg, get_error_json("URI Input Error in action_by check"), status_codes::BadRequest);
        return _msg;
    }

    // action_by로 캐스팅이 안돼서 action_by로 그냥 액션있는리소스들중에 뭔지 비교일일이 해야함
    // 그렇게 해서 리소스찾고 거기에 actions맵 에 actions[actions_what]으로 하면 해당 Actions구조체 접근가능
    // 액션왓으로 맵에 접근까지 되면 맞는 uri긴 하겟네 굳이 target하고inputuri랑 비교안해도될거같고
    // 찾으면 함수호출 ㅇㅋ

    _msg = reply_error(_request, _msg, get_error_json("오면안되는곳"), status_codes::BadRequest);
    return _msg;
}

m_Request make_account(http_request _request, m_Request _msg, json::value _jv)
{
    string user_name;
    string password;
    string odata_id;
    string role_id = "ReadOnly";
    Account *account;
    bool enabled = true;
  
    if(_jv.as_object().find("UserName") == _jv.as_object().end() 
    || _jv.as_object().find("Password") == _jv.as_object().end())
    {
        _msg = reply_error(_request, _msg, get_error_json("No UserName or Password"), status_codes::BadRequest);
        return _msg;
    } // json request body에 UserName, Password없으면 BadRequest

    user_name = _jv.at("UserName").as_string();
    password = _jv.at("Password").as_string();
    // 이거 입력 타입오류 try-catch? 어차피 입력하는곳 만드는 곳에서 입력란에것을 string으로 주게하면되긴함

    unsigned int min_pass_length = ((AccountService *)g_record[ODATA_ACCOUNT_SERVICE_ID])->min_password_length;
    if(password.size() < min_pass_length)
    {
        _msg = reply_error(_request, _msg, get_error_json("Password length must be at least " + to_string(min_pass_length)), status_codes::BadRequest);
        return _msg;
    }// password 길이가 짧으면 BadRequest

    // Collection col = *((Collection *)g_record[ODATA_ACCOUNT_ID]); // 이건 왜 터졌지?
    Collection *col = (Collection *)g_record[ODATA_ACCOUNT_ID];
    std::vector<Resource *>::iterator iter;
    for(iter=col->members.begin(); iter!=col->members.end(); iter++)
    {
        if(((Account *)(*iter))->user_name == user_name)
        {
            _msg = reply_error(_request, _msg, get_error_json("UserName already exists"), status_codes::Conflict);
            return _msg;
        }
    }// username이 이미 있으면 Conflict

    // odata_id = ODATA_ACCOUNT_ID;
    // odata_id = odata_id + '/' + user_name;

    // if(record_is_exist(odata_id))
    // {
    //     json::value err;
    //     err[U("Error")] = json::value::string(U("UserName already exists"));
    //     http_response res(status_codes::Conflict);
    //     res.set_body(err);
    //     _request.reply(res);
    //     // _request.reply(status_codes::Conflict, err);
    //     _msg.result.result_datetime = currentDateTime();
    //     _msg.result.result_status = WORK_FAIL;
    //     _msg.result.result_response = res;
    //     return _msg;
    // }// username 이미 있으면 Conflict

    if(_jv.as_object().find("RoleId") != _jv.as_object().end())
    {
        odata_id = ODATA_ROLE_ID;
        role_id = _jv.at("RoleId").as_string();
        odata_id = odata_id + '/' + role_id;
        // Check role exist
        if(!record_is_exist(odata_id))
        {
            _msg = reply_error(_request, _msg, get_error_json("No Role"), status_codes::BadRequest);
            return _msg;
        }
    }

    if(_jv.as_object().find("Enabled") != _jv.as_object().end())
        enabled = _jv.at("Enabled").as_bool();

    // TODO id를 계정 이름 말고 숫자로 변경 필요
    odata_id = ODATA_ACCOUNT_ID;
    string acc_id = to_string(allocate_account_num());
    odata_id = odata_id + "/" + acc_id;
    // cout << "New account : " << odata_id << endl;
    // odata_id = odata_id + '/' + user_name;
    account = new Account(odata_id, acc_id, role_id);
    // account = new Account(odata_id, role_id);
    account->name = "User Account";
    account->user_name = user_name;
    // account->id = user_name;
    account->password = password;
    account->enabled = enabled;
    account->locked = false;

    // 컬렉션에 add
    ((Collection *)g_record[ODATA_ACCOUNT_ID])->add_member(account);
    record_save_json();

    http_response response;
    http_response tmp_res(status_codes::Created);
    response.set_status_code(tmp_res.status_code());
    response.set_body(record_get_json(odata_id));
    // response.set_body(record_get_json(ODATA_ACCOUNT_ID)); // 확인용임 실제에선 빼셈 -- 넣는거같음
    _msg.result.result_datetime = currentDateTime();
    _msg.result.result_status = WORK_SUCCESS;
    _msg.result.result_response = response;
    response.headers().add("Access-Control-Allow-Origin", "*");
    response.headers().add("Access-Control-Allow-Credentials", "true");
    response.headers().add("Access-Control-Allow-Methods", "POST, GET, OPTIONS, DELETE");
    response.headers().add("Access-Control-Max-Age", "3600");
    response.headers().add("Access-Control-Allow-Headers", "Content-Type, Accept, X-Requested-With, remember-me");

    _request.reply(response);
    return _msg;
}

m_Request make_session(http_request _request, m_Request _msg, json::value _jv)
{
    string user_name;
    string password;
    string odata_id;
    Account *account;

    if(_jv.as_object().find("UserName") == _jv.as_object().end() 
    || _jv.as_object().find("Password") == _jv.as_object().end())
    {
        _msg = reply_error(_request, _msg, get_error_json("No UserName or Password"), status_codes::BadRequest);
        return _msg;
    } // json request body에 UserName, Password없으면 BadRequest

    user_name = _jv.at("UserName").as_string();
    password = _jv.at("Password").as_string();

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

    // odata_id = ODATA_ACCOUNT_ID;
    // odata_id = odata_id + '/' + user_name;

    // // Check account exist
    // if (!record_is_exist(odata_id))
    // {
    //     _request.reply(status_codes::BadRequest);
    //     return;
    // }

    // account = (Account *)g_record[odata_id]; // 기존 계정검사방법

    if(!exist)
    {
        _msg = reply_error(_request, _msg, get_error_json("No Account using that UserName"), status_codes::BadRequest);
        return _msg;
    } // 입력한 username에 해당하는 계정없음

    if(account->password != password)
    {
        _msg = reply_error(_request, _msg, get_error_json("Password does not match"), status_codes::BadRequest);
        return _msg;
    } // 비밀번호 맞지않음


    // TODO 세션 id로 변경..
    odata_id = ODATA_SESSION_ID;
    string token = generate_token(16);
    string session_id = to_string(allocate_session_num());
    odata_id = odata_id + '/' + session_id;
    // odata_id = odata_id + '/' + token;
    Session *session = new Session(odata_id, session_id, account);
    session->x_token = token;
    // Session *session = new Session(odata_id, token, account);
    ((Collection *)g_record[ODATA_SESSION_ID])->add_member(session);
    session->start();
    record_save_json();


    http_response response;
    response.set_status_code(status_codes::Created);
    response.headers().add("X-Auth-Token", token);
    response.headers().add("Location", session->odata.id);
    _msg.result.result_datetime = currentDateTime();
    _msg.result.result_status = WORK_SUCCESS;
    _msg.result.result_response = response;

    _request.reply(response);

    return _msg;
}

m_Request make_logentry(http_request _request, m_Request _msg, json::value _jv)
{
    string uri = _request.request_uri().to_string();

    if(((LogService *)g_record[uri])->entry == nullptr){
        ((LogService *)g_record[uri])->entry = new Collection(uri + "/Entries", ODATA_LOG_ENTRY_COLLECTION_TYPE);
        ((LogService *)g_record[uri])->entry->name = "Log Entry Collection";
    }
    // entry collection없으면 만들어줌

    //log_entry하나만들고 연결
    string entry_id, entry_odata_id;
    if(!get_value_from_json_key(_jv, "EntryID", entry_id))
    {
        _msg = reply_error(_request, _msg, get_error_json("Need EntryID"), status_codes::BadRequest);
        return _msg;
    }
    // 받는걸로 EntryID필요하게끔함

    entry_odata_id = ((LogService *)g_record[uri])->entry->odata.id + "/" + entry_id;
    if(record_is_exist(entry_odata_id))
    {
        _msg = reply_error(_request, _msg, get_error_json("EntryID is Already Exist"), status_codes::BadRequest);
        return _msg;
    } // 중복검사

    init_log_entry(((LogService *)g_record[uri])->entry, entry_id);
    _msg = reply_success(_request, _msg, record_get_json(entry_odata_id), status_codes::Created);
    return _msg;
}

m_Request modify_account(http_request _request, m_Request _msg, json::value _jv, string _uri)
{
    // /redfish/v1/AccountService/Accounts/[Account_id] 처리함수인데

    // 우선 그 계정이 있나 보고
    // 계정이 있으면 이 계정 patch 를 하는사람이 할 권한이 있는지를 봐야댐
    // 그래서 로그인 된 녀석의 role이 관리자거나
    // 혹은 본인계정일때만 patch가능이지
    // 그걸 뚫으면 이제 비로소 변경가능
    // 변경하는 와중ㅇ에 유저네임은 중복검사해야하고 롤아이디는 롤존재하는지 검사해야함
    if(!(record_is_exist(_uri)))
    {
        // 해당 계정이 없음
        _msg = reply_error(_request, _msg, get_error_json("No Account"), status_codes::BadRequest);
        return _msg;
    }

    cout << "바뀌기전~~ " << endl;
    cout << record_get_json(_uri) << endl;
    cout << " $$$$$$$ " << endl;

    if(!_request.headers().has("X-Auth-Token"))
    {
        // 로그인이 안되어있음(X-Auth-Token이 존재하지않음)
        _msg = reply_error(_request, _msg, get_error_json("Login Required"), status_codes::BadRequest);
        return _msg;
    }

    string session_token = _request.headers()["X-Auth-Token"];

    if(!is_session_valid(session_token))
    {
        // 세션이 유효하지않음(X-Auth-Token은 존재하나 유효하지 않음)
        _msg = reply_error(_request, _msg, get_error_json("Session Unvalid"), status_codes::BadRequest);
        return _msg;
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
        // 관리자만 롤 변경가능하게 설계해봄
        if(_jv.as_object().find("RoleId") != _jv.as_object().end())
        {
            role_odata = ODATA_ROLE_ID;
            string role_id = _jv.at("RoleId").as_string();
            role_odata = role_odata + "/" + role_id;

            if(!record_is_exist(role_odata))
            {
                // 해당 롤 없음
                _msg = reply_error(_request, _msg, get_error_json(role_id + " does not exist"), status_codes::BadRequest);
                return _msg;
            }

            // ((Account *)g_record[_uri])->role = ((Role *)g_record[role_odata]);
            op_role = true;
        }
    }

    if(session->account->role->id == "Administrator" || session->account->id == last)
    {
        // 인증 다뚫음 수정 가능
        if(_jv.as_object().find("UserName") != _jv.as_object().end())
        {
            // username이 있어서 바꾸는데 유저네임을 바꾸면 그걸로 로그인한 세션정보도 바꿔줘야하는것이 아닌가
            // 자동적으로 바뀔거같은데 세션안에 어카운트가 포인터고 그 포인터는 g_record꺼에 들어가있으니깐 
            // 내가 수정하는건 g_record안의 account정보고
            input_username = _jv.at("UserName").as_string();
            // 아 이거 유저네임바꾸면 odata도 /redfish/v1/AccountService/Accounts/user_name이라 지금
            // 저거 account_id로 바꿔야하나 ㅇㅇ 바꿔야겟네 바꾸는중 ..... 여기부터 ㄱㄱ

            Collection *col = (Collection *)g_record[ODATA_ACCOUNT_ID];
            std::vector<Resource *>::iterator iter;
            for(iter=col->members.begin(); iter!=col->members.end(); iter++)
            {
                if(((Account *)(*iter))->id == last)
                    continue;

                if(((Account *)(*iter))->user_name == input_username)
                {
                    _msg = reply_error(_request, _msg, get_error_json("UserName is already in use"), status_codes::BadRequest);
                    return _msg;
                }

            } // user_name 중복검사

            // ((Account *)g_record[_uri])->user_name = input_username;
            op_name = true;
        }

        if(_jv.as_object().find("Password") != _jv.as_object().end())
        {
            input_password = _jv.at("Password").as_string();
            // ((Account *)g_record[_uri])->password = _jv.at("Password").as_string();
            op_pass = true;
        }


    }
    // role_id로 하기엔 role을 생성할 수도 있는 거여서 나중에 role안에 privileges에 권한 확인하는걸로 바꿔야 할 수도있음
    // + 현재 로그인이 관리자로되어있어서 위에서 role관련 수행했는데 성공하고 아래 유저네임 패스워드에서 에러잡혀서
    // 수행을 하지 않으면 response에는 BadRequest 인데 role은 바뀐게 적용되고있음 기억해 둬야함 .. 리멤버
    // 로직 수정해야겠는데? ex) jv에 롤있는데 관리자권한이 아니라든가 위에 리멤버라든가 

    if(!op_role && !op_name && !op_pass) // 다 false면
    {
        _msg = reply_error(_request, _msg, get_error_json("Cannot Modify Account"), status_codes::BadRequest);
        return _msg;
    }

    if(op_role)
        ((Account *)g_record[_uri])->role = ((Role *)g_record[role_odata]);
    
    if(op_name)
        ((Account *)g_record[_uri])->user_name = input_username;

    if(op_pass)
        ((Account *)g_record[_uri])->password = input_password;
    // 검사중 error나면 적용안되고 error나지 않을때 변경일괄처리
    

    cout << "바꾼후~~ " << endl;
    cout << record_get_json(_uri) << endl;
    cout << "세션에 연결된 계정정보" << endl;
    cout << record_get_json(session->account->odata.id) << endl;

    _msg = reply_success(_request, _msg, record_get_json(_uri), status_codes::OK);

    // http_response response;
    // json::value response_json;

    // response_json = record_get_json(_uri);
    // response.set_status_code(status_codes::OK);
    // response.set_body(response_json);
    // _msg.result.result_datetime = currentDateTime();
    // _msg.result.result_response = response;
    // _msg.result.result_status = WORK_SUCCESS;

    // _request.reply(response);
    return _msg;
}

m_Request modify_role(http_request _request, m_Request _msg, json::value _jv, string _uri)
{
    // /redfish/v1/AccountService/Roles/[Role_id]

    // 해당 role이 있나 보고
    // 있으면 권한 변경은 관리자만 할수있게끔해야지
    // 로그인된 녀석이 관리자냐 확인하고
    // 뚫으면 변경하는데 변경할게 assignedprivileges 밖에 없음

    if(!(record_is_exist(_uri)))
    {
        // 해당 계정이 없음
        _msg = reply_error(_request, _msg, get_error_json("No Role"), status_codes::BadRequest);
        return _msg;
    }

    if(!_request.headers().has("X-Auth-Token"))
    {
        // 로그인이 안되어있음(X-Auth-Token이 존재하지않음)
        _msg = reply_error(_request, _msg, get_error_json("Login Required"), status_codes::BadRequest);
        return _msg;
    }

    string session_token = _request.headers()["X-Auth-Token"];

    if(!is_session_valid(session_token))
    {
        // 세션이 유효하지않음(X-Auth-Token은 존재하나 유효하지 않음)
        _msg = reply_error(_request, _msg, get_error_json("Session Unvalid"), status_codes::BadRequest);
        return _msg;
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
            _msg = reply_error(_request, _msg, get_error_json("No Privileges"), status_codes::BadRequest);
            return _msg;
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
                    cout << input << " 이미 있어요!!" << endl;
            }
            else
            {
                //올바른 privilege 가 아님!!
                _msg = reply_error(_request, _msg, get_error_json("Incorrect Previlege"), status_codes::BadRequest);
                return _msg;
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
        _msg = reply_error(_request, _msg, get_error_json("Only Administrator Can Use"), status_codes::BadRequest);
        return _msg;
    }
    // 일단 assignedprivileges 있는걸 추가하는식으로만 구현해봄 삭제는 보류

    _msg = reply_success(_request, _msg, record_get_json(_uri), status_codes::OK);
    return _msg;
}

m_Request remove_account(http_request _request, m_Request _msg, json::value _jv, string _service_uri)
{
    string username, password, odata_id;

    if(_jv.as_object().find("UserName") == _jv.as_object().end()
    || _jv.as_object().find("Password") == _jv.as_object().end())
    {
        _msg = reply_error(_request, _msg, get_error_json("No UserName or Password"), status_codes::BadRequest);
        return _msg;
    } // json request body에 UserName, Password없으면 BadRequest

    username = _jv.at("UserName").as_string();
    password = _jv.at("Password").as_string();

    bool exist=false;
    AccountService *acc_service = (AccountService *)g_record[_service_uri];
    std::vector<Resource *>::iterator iter;
    for(iter = acc_service->account_collection->members.begin(); iter != acc_service->account_collection->members.end(); iter++)
    {
        Account *t = (Account *)*iter;

        if(t->user_name == username && t->password == password)
        {
            odata_id = t->odata.id;
            exist = true;
            break;
        }
    }

    if(!exist)
    {
        _msg = reply_error(_request, _msg, get_error_json("No Account or Does not match Password"), status_codes::BadRequest);
        return _msg;
    }

    cout << "지우기전~~ " << endl;
    cout << record_get_json(acc_service->account_collection->odata.id) << endl;
    cout << " $$$$$$$ " << endl;

    unsigned int id_num;
    id_num = stoi(((Account *)g_record[odata_id])->id);
    delete_account_num(id_num);
    // numset에서 num id 삭제
    // 이거땜에 bmc는 username으로 id쓰는거 수정하든가 새로 처리하든가 해야함

    delete(*iter);
    acc_service->account_collection->members.erase(iter);
    // account자체 객체삭제, g_record에서 삭제, account collection에서 삭제

    string delete_path = odata_id + ".json";
    if(remove(delete_path.c_str()) < 0)
    {
        cout << "account json file delete error" << endl;
    }
    // account json파일 삭제

    cout << "지운후~~ " << endl;
    cout << "지워진놈 : " << odata_id << endl;
    cout << record_get_json(acc_service->account_collection->odata.id) << endl;

    _msg = reply_success(_request, _msg, record_get_json(acc_service->account_collection->odata.id), status_codes::Gone);
    // status ok로 바꿔
    return _msg;
}

m_Request remove_session(http_request _request, m_Request _msg)
{
    // 아니근데 이거 x-auth-token가지고만 하면 a가 로그인한채로 b꺼 x토큰 보내서 로그아웃 시킬수도있는데??
    if(!(is_session_valid(_request.headers()["X-Auth-Token"])))
    {
        _msg = reply_error(_request, _msg, get_error_json("Unvalid Session"), status_codes::BadRequest);
        return _msg;
    }

    // 토큰에 해당하는 세션 종료
    string token = _request.headers()["X-Auth-Token"];
    string session_uri = get_session_odata_id_by_token(token);
    // string session_uri = ODATA_SESSION_ID;
    // session_uri = session_uri + '/' + token;
    Session *session;

    session = (Session *)g_record[session_uri];
    // cout << "YYYY : " << session->id << endl;
    session->_remain_expires_time = 1;

    _msg = reply_success(_request, _msg, json::value::null(), status_codes::Gone);
    // status ok로..
    return _msg;
}

void patch_account_service(json::value _jv, string _record_uri)
{
    cout << "바뀌기전~~ " << endl;
    cout << record_get_json(_record_uri) << endl;
    cout << " $$$$$$$ " << endl;

    if(_jv.as_object().find("ServiceEnabled") != _jv.as_object().end())
        ((AccountService *)g_record[_record_uri])->service_enabled = _jv.at("ServiceEnabled").as_bool();

    if(_jv.as_object().find("AuthFailureLoggingThreshold") != _jv.as_object().end())
        ((AccountService *)g_record[_record_uri])->auth_failure_logging_threshold = _jv.at("AuthFailureLoggingThreshold").as_integer();

    if(_jv.as_object().find("MinPasswordLength") != _jv.as_object().end())
        ((AccountService *)g_record[_record_uri])->min_password_length = _jv.at("MinPasswordLength").as_integer();

    if(_jv.as_object().find("AccountLockoutThreshold") != _jv.as_object().end())
        ((AccountService *)g_record[_record_uri])->account_lockout_threshold = _jv.at("AccountLockoutThreshold").as_integer();

    if(_jv.as_object().find("AccountLockoutDuration") != _jv.as_object().end())
        ((AccountService *)g_record[_record_uri])->account_lockout_duration = _jv.at("AccountLockoutDuration").as_integer();

    if(_jv.as_object().find("AccountLockoutCounterResetAfter") != _jv.as_object().end())
        ((AccountService *)g_record[_record_uri])->account_lockout_counter_reset_after = _jv.at("AccountLockoutCounterResetAfter").as_integer();

    if(_jv.as_object().find("AccountLockoutCounterResetEnabled") != _jv.as_object().end())
        ((AccountService *)g_record[_record_uri])->account_lockout_counter_reset_enabled = _jv.at("AccountLockoutCounterResetEnabled").as_bool();

    cout << "바꾼후~~ " << endl;
    cout << record_get_json(_record_uri) << endl;
}

void patch_session_service(json::value _jv)
{
    cout << "바뀌기전~~ " << endl;
    cout << record_get_json(ODATA_SESSION_SERVICE_ID) << endl;
    cout << " $$$$$$$ " << endl;

    unsigned int change_timeout;
    if(_jv.as_object().find("SessionTimeout") != _jv.as_object().end())
    {
        change_timeout = _jv.at("SessionTimeout").as_integer();
        ((SessionService *)g_record[ODATA_SESSION_SERVICE_ID])->session_timeout = change_timeout;
        // ((SessionService *)g_record[_record_uri])->session_timeout = change_timeout;
        // 세션은 _record_uri필요없겟는데?
    }
    // 타임아웃 시간 변경이 관리자가 할거 같긴한데 일단 권한검사는 추가안함

    Collection *col = (Collection *)g_record[ODATA_SESSION_ID];
    std::vector<Resource *>::iterator iter;
    for(iter=col->members.begin(); iter!=col->members.end(); iter++)
    {
        if(((Session *)(*iter))->_remain_expires_time > change_timeout)
            ((Session *)(*iter))->_remain_expires_time = change_timeout;
    }
    // 타임아웃 시간을 변경한 것이 현재 만들어져있는 세션들에게도 적용되면 이 코드 적용

    cout << "바꾼후~~ " << endl;
    cout << record_get_json(ODATA_SESSION_SERVICE_ID) << endl;

}

void patch_manager(json::value _jv, string _record_uri)
{
    cout << "바뀌기전~~ " << endl;
    cout << record_get_json(_record_uri) << endl;
    cout << " $$$$$$$ " << endl;

    if(_jv.as_object().find("Description") != _jv.as_object().end())
        ((Manager *)g_record[_record_uri])->description = _jv.at("Description").as_string();

    if(_jv.as_object().find("DateTime") != _jv.as_object().end())
        ((Manager *)g_record[_record_uri])->datetime = _jv.at("DateTime").as_string();

    if(_jv.as_object().find("DateTimeLocalOffset") != _jv.as_object().end())
        ((Manager *)g_record[_record_uri])->datetime_offset = _jv.at("DateTimeLocalOffset").as_string();

    cout << "바꾼후~~ " << endl;
    cout << record_get_json(_record_uri) << endl;
}

void patch_network_protocol(json::value _jv, string _record_uri)
{
    cout << "바뀌기전~~ " << endl;
    cout << record_get_json(_record_uri) << endl;
    cout << " $$$$$$$ " << endl;

    if(_jv.as_object().find("Description") != _jv.as_object().end())
        ((NetworkProtocol *)g_record[_record_uri])->description = _jv.at("Description").as_string();

    if(_jv.as_object().find("SNMP") != _jv.as_object().end())
    {
        json::value j = json::value::object();
        j = _jv.at("SNMP");
        
        if(j.as_object().find("ProtocolEnabled") != j.as_object().end())
            ((NetworkProtocol *)g_record[_record_uri])->snmp_enabled = j.at("ProtocolEnabled").as_bool();
        if(j.as_object().find("Port") != j.as_object().end())
            ((NetworkProtocol *)g_record[_record_uri])->snmp_port = j.at("Port").as_integer();
    }

    if(_jv.as_object().find("IPMI") != _jv.as_object().end())
    {
        json::value j = json::value::object();
        j = _jv.at("IPMI");
        
        if(j.as_object().find("ProtocolEnabled") != j.as_object().end())
            ((NetworkProtocol *)g_record[_record_uri])->ipmi_enabled = j.at("ProtocolEnabled").as_bool();
        if(j.as_object().find("Port") != j.as_object().end())
            ((NetworkProtocol *)g_record[_record_uri])->ipmi_port = j.at("Port").as_integer();
    }

    if(_jv.as_object().find("KVMIP") != _jv.as_object().end())
    {
        json::value j = json::value::object();
        j = _jv.at("KVMIP");
        
        if(j.as_object().find("ProtocolEnabled") != j.as_object().end())
            ((NetworkProtocol *)g_record[_record_uri])->kvmip_enabled = j.at("ProtocolEnabled").as_bool();
        if(j.as_object().find("Port") != j.as_object().end())
            ((NetworkProtocol *)g_record[_record_uri])->kvmip_port = j.at("Port").as_integer();
    }

    if(_jv.as_object().find("HTTP") != _jv.as_object().end())
    {
        json::value j = json::value::object();
        j = _jv.at("HTTP");
        
        if(j.as_object().find("ProtocolEnabled") != j.as_object().end())
            ((NetworkProtocol *)g_record[_record_uri])->http_enabled = j.at("ProtocolEnabled").as_bool();
        if(j.as_object().find("Port") != j.as_object().end())
            ((NetworkProtocol *)g_record[_record_uri])->http_port = j.at("Port").as_integer();
    }

    if(_jv.as_object().find("HTTPS") != _jv.as_object().end())
    {
        json::value j = json::value::object();
        j = _jv.at("HTTPS");
        
        if(j.as_object().find("ProtocolEnabled") != j.as_object().end())
            ((NetworkProtocol *)g_record[_record_uri])->https_enabled = j.at("ProtocolEnabled").as_bool();
        if(j.as_object().find("Port") != j.as_object().end())
            ((NetworkProtocol *)g_record[_record_uri])->https_port = j.at("Port").as_integer();
    }

    if(_jv.as_object().find("NTP") != _jv.as_object().end())
    {
        json::value j = json::value::object();
        j = _jv.at("NTP");
        
        if(j.as_object().find("ProtocolEnabled") != j.as_object().end())
            ((NetworkProtocol *)g_record[_record_uri])->ntp_enabled = j.at("ProtocolEnabled").as_bool();
        if(j.as_object().find("Port") != j.as_object().end())
            ((NetworkProtocol *)g_record[_record_uri])->ntp_port = j.at("Port").as_integer();
    }

    // NTPServers 는 여러개가 들어가있는 형식인데 어떻게 수정해야하지?
    // request에서 json에 두 공간에서 받아야할듯 a,b입력란이라고 하면
    // a에서 받은 ntpservers가 존재하면 b검사하고 b에도 입력한게 있으면 그걸로 수정
    // a에서 받은 ntpservers가 존재하지않으면 걍 추가 이런식으로??

    cout << "바꾼후~~ " << endl;
    cout << record_get_json(_record_uri) << endl;

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


        // 여기서 temperature돌아서 센서넘버 같은녀석의 온도체크후 바꿔줌
        // 근데 그것도 Auto일 경우에만/ 스탠다드, 풀스피드일경우엔 팬 자체정보로 변경가능
    }
}

void patch_ethernet_interface(json::value _jv, string _record_uri)
{
    cout << "바뀌기전~~ " << endl;
    cout << record_get_json(_record_uri) << endl;
    cout << " $$$$$$$ " << endl;

    if(_jv.as_object().find("Description") != _jv.as_object().end())
        ((EthernetInterfaces *)g_record[_record_uri])->description = _jv.at("Description").as_string();

    if(_jv.as_object().find("FQDN") != _jv.as_object().end())
        ((EthernetInterfaces *)g_record[_record_uri])->fqdn = _jv.at("FQDN").as_string();

    if(_jv.as_object().find("HostName") != _jv.as_object().end())
        ((EthernetInterfaces *)g_record[_record_uri])->hostname = _jv.at("HostName").as_string();

    // if(_jv.as_object().find("LinkStatus") != _jv.as_object().end())
    //     ((EthernetInterfaces *)g_record[uri])->link_status = _jv.at("LinkStatus").as_string();
    // read-only

    if(_jv.as_object().find("MACAddress") != _jv.as_object().end())
        ((EthernetInterfaces *)g_record[_record_uri])->mac_address = _jv.at("MACAddress").as_string();

    // if(_jv.as_object().find("IPv6DefaultGateway") != _jv.as_object().end())
    //     ((EthernetInterfaces *)g_record[uri])->ipv6_default_gateway = _jv.at("IPv6DefaultGateway").as_string();
    // read-only였음

    if(_jv.as_object().find("MTUSize") != _jv.as_object().end())
        ((EthernetInterfaces *)g_record[_record_uri])->mtu_size = _jv.at("MTUSize").as_integer();


    // IPv4, IPv6 Addresses는 원래 스키마에는 array로 되어있는데 
    // 일단 여기서의 patch는 하나만 있는거라고 생각하고 함
    if(_jv.as_object().find("IPv4Addresses") != _jv.as_object().end())
    {
        // json::value ip_array = json::value::array();
        // ip_array = _jv.at("IPv4Addresses");
        // int size_v4 = ((EthernetInterfaces *)g_record[uri])->v_ipv4.size();
        // // ((EthernetInterfaces *)g_record[uri])->v_ipv4.clear();
        // // 기존벡터 클리어
        // size_v4 
        // for(int ip_num=0; ip_num<ip_array.size(); ip_num++)
        // {
        //     IPv4_Address tmp;
        //     if(ip_array[ip_num].as_object().find("Address") != ip_array[ip_num].as_object().end())
        //         tmp.address = ip_array[ip_num].at("Address").as_string();
            
        //     // if(ip_array[ip_num].as_object().find("AddressOrigin") != ip_array[ip_num].as_object().end())
        //     //     tmp.address_origin = ip_array[ip_num].at("AddressOrigin").as_string();
        //     // read-only

        //     if(ip_array[ip_num].as_object().find("SubnetMask") != ip_array[ip_num].as_object().end())
        //         tmp.subnet_mask = ip_array[ip_num].at("SubnetMask").as_string();

        //     if(ip_array[ip_num].as_object().find("Gateway") != ip_array[ip_num].as_object().end())
        //         tmp.gateway = ip_array[ip_num].at("Gateway").as_string();


        //     ((EthernetInterfaces *)g_record[uri])->v_ipv4.push_back(tmp);
        // } //원래 여러개일수도있다하고 한부분이고 수정중이었음

        json::value j = json::value::object();
        j = _jv.at("IPv4Addresses");

        int size_v4 = ((EthernetInterfaces *)g_record[_record_uri])->v_ipv4.size();
        if(size_v4 == 0)
        {
            // 없을때 하나 만들어서 넣어줌
            IPv4_Address tmp;
            if(j.as_object().find("Address") != j.as_object().end())
                tmp.address = j.at("Address").as_string();

            // if(j.as_object().find("AddressOrigin") != j.as_object().end())
            //     tmp.address_origin = j.at("AddressOrigin").as_string();
            // read-only

            if(j.as_object().find("SubnetMask") != j.as_object().end())
                tmp.subnet_mask = j.at("SubnetMask").as_string();

            if(j.as_object().find("Gateway") != j.as_object().end())
                tmp.gateway = j.at("Gateway").as_string();

            ((EthernetInterfaces *)g_record[_record_uri])->v_ipv4.push_back(tmp);
        }
        else
        {
            // 1개이상일 때 수정은 1개만있는거로 보기로 했으니깐 v_ipv4벡터의 0번째인덱스만 수정
            if(j.as_object().find("Address") != j.as_object().end())
                ((EthernetInterfaces *)g_record[_record_uri])->v_ipv4[0].address = j.at("Address").as_string();
            
            // if(j.as_object().find("AddressOrigin") != j.as_object().end())
            //     ((EthernetInterfaces *)g_record[uri])->v_ipv4[0].address_origin = j.at("AddressOrigin").as_string();
            // read-only

            if(j.as_object().find("SubnetMask") != j.as_object().end())
                ((EthernetInterfaces *)g_record[_record_uri])->v_ipv4[0].subnet_mask = j.at("SubnetMask").as_string();

            if(j.as_object().find("Gateway") != j.as_object().end())
                ((EthernetInterfaces *)g_record[_record_uri])->v_ipv4[0].gateway = j.at("Gateway").as_string();
        }
    }

    if(_jv.as_object().find("IPv6Addresses") != _jv.as_object().end())
    {
        // json::value ip_array = json::value::array();
        // ip_array = _jv.at("IPv6Addresses");
        // ((EthernetInterfaces *)g_record[uri])->v_ipv6.clear();
        // // 기존벡터 클리어
        // for(int ip_num=0; ip_num<ip_array.size(); ip_num++)
        // {
        //     IPv6_Address tmp;
        //     if(ip_array[ip_num].as_object().find("Address") != ip_array[ip_num].as_object().end())
        //         tmp.address = ip_array[ip_num].at("Address").as_string();
            
        //     // if(ip_array[ip_num].as_object().find("AddressOrigin") != ip_array[ip_num].as_object().end())
        //     //     tmp.address_origin = ip_array[ip_num].at("AddressOrigin").as_string();

        //     // if(ip_array[ip_num].as_object().find("AddressState") != ip_array[ip_num].as_object().end())
        //     //     tmp.address_state = ip_array[ip_num].at("AddressState").as_string();

        //     // if(ip_array[ip_num].as_object().find("PrefixLength") != ip_array[ip_num].as_object().end())
        //     //     tmp.prefix_length = ip_array[ip_num].at("PrefixLength").as_integer();
        //     // else
        //     //     tmp.prefix_length = 0;
        //     // 셋다 read-only

        //     ((EthernetInterfaces *)g_record[uri])->v_ipv6.push_back(tmp);
        // }

        json::value j = json::value::object();
        j = _jv.at("IPv6Addresses");

        int size_v6 = ((EthernetInterfaces *)g_record[_record_uri])->v_ipv6.size();
        if(size_v6 == 0)
        {
            // 없을때 하나 만들어서 넣어줌
            IPv6_Address tmp;
            if(j.as_object().find("Address") != j.as_object().end())
                tmp.address = j.at("Address").as_string();

            // if(j.as_object().find("AddressOrigin") != j.as_object().end())
            //     tmp.address_origin = j.at("AddressOrigin").as_string();

            // if(j.as_object().find("AddressState") != j.as_object().end())
            //     tmp.address_state = j.at("AddressState").as_string();

            // if(j.as_object().find("PrefixLength") != j.as_object().end())
            //     tmp.prefix_length = j.at("PrefixLength").as_integer();
            // 셋다 read-only 지만 prefixlength는 쓰레기값 들어가길래 0넣어줌
            tmp.prefix_length = 0;

            ((EthernetInterfaces *)g_record[_record_uri])->v_ipv6.push_back(tmp);
        }
        else
        {
            // 1개이상일 때 수정은 1개만있는거로 보기로 했으니깐 v_ipv6벡터의 0번째인덱스만 수정
            if(j.as_object().find("Address") != j.as_object().end())
                ((EthernetInterfaces *)g_record[_record_uri])->v_ipv6[0].address = j.at("Address").as_string();
            
            // if(j.as_object().find("AddressOrigin") != j.as_object().end())
            //     ((EthernetInterfaces *)g_record[uri])->v_ipv6[0].address_origin = j.at("AddressOrigin").as_string();

            // if(j.as_object().find("AddressState") != j.as_object().end())
            //     ((EthernetInterfaces *)g_record[uri])->v_ipv6[0].address_state = j.at("AddressState").as_string();

            // if(j.as_object().find("PrefixLength") != j.as_object().end())
            //     ((EthernetInterfaces *)g_record[uri])->v_ipv6[0].prefix_length = j.at("PrefixLength").as_string();
            // read-only
        }
    }

    cout << "바꾼후~~ " << endl;
    cout << record_get_json(_record_uri) << endl;

}

void patch_system(json::value _jv, string _record_uri)
{
    cout << "바뀌기전~~ " << endl;
    cout << record_get_json(_record_uri) << endl;
    cout << " $$$$$$$ " << endl;
    // log(trace) << "바뀌기전~~ ";
    // log(debug) << record_get_json(uri);
    // log(info) << " $$$$$$$ ";

    if(_jv.as_object().find("Description") != _jv.as_object().end())
        ((Systems *)g_record[_record_uri])->description = _jv.at("Description").as_string();

    if(_jv.as_object().find("HostName") != _jv.as_object().end())
        ((Systems *)g_record[_record_uri])->hostname = _jv.at("HostName").as_string();

    if(_jv.as_object().find("AssetTag") != _jv.as_object().end())
        ((Systems *)g_record[_record_uri])->asset_tag = _jv.at("AssetTag").as_string();

    if(_jv.as_object().find("IndicatorLED") != _jv.as_object().end())
    {
        string led = _jv.at("IndicatorLED").as_string();
        if(led == "Off")
            ((Systems *)g_record[_record_uri])->indicator_led = LED_OFF;
        else if(led == "Blinking")
            ((Systems *)g_record[_record_uri])->indicator_led = LED_BLINKING;
        else if(led == "Lit")
            ((Systems *)g_record[_record_uri])->indicator_led = LED_LIT;
    }

    if(_jv.as_object().find("Boot") != _jv.as_object().end())
    {
        json::value j = json::value::object();
        j = _jv.at("Boot");

        if(j.as_object().find("BootSourceOverrideEnabled") != j.as_object().end())
            ((Systems *)g_record[_record_uri])->boot.boot_source_override_enabled = j.at("BootSourceOverrideEnabled").as_string();

        if(j.as_object().find("BootSourceOverrideTarget") != j.as_object().end())
            ((Systems *)g_record[_record_uri])->boot.boot_source_override_target = j.at("BootSourceOverrideTarget").as_string();

        if(j.as_object().find("BootSourceOverrideMode") != j.as_object().end())
            ((Systems *)g_record[_record_uri])->boot.boot_source_override_mode = j.at("BootSourceOverrideMode").as_string();

        if(j.as_object().find("UefiTargetBootSourceOverride") != j.as_object().end())
            ((Systems *)g_record[_record_uri])->boot.uefi_target_boot_source_override = j.at("UefiTargetBootSourceOverride").as_string();
    }


    cout << "바꾼후~~ " << endl;
    cout << record_get_json(_record_uri) << endl;
    // log(warning) << "바꾼후~~ ";
    // log(error) << record_get_json(uri);
    // log(fatal) << "FATAL!";
}

void patch_chassis(json::value _jv, string _record_uri)
{
    cout << "바뀌기전~~ " << endl;
    cout << record_get_json(_record_uri) << endl;
    cout << " $$$$$$$ " << endl;

    if(_jv.as_object().find("AssetTag") != _jv.as_object().end())
        ((Chassis *)g_record[_record_uri])->asset_tag = _jv.at("AssetTag").as_string();

    if(_jv.as_object().find("IndicatorLED") != _jv.as_object().end())
    {
        string led = _jv.at("IndicatorLED").as_string();
        if(led == "Off")
            ((Chassis *)g_record[_record_uri])->indicator_led = LED_OFF;
        else if(led == "Blinking")
            ((Chassis *)g_record[_record_uri])->indicator_led = LED_BLINKING;
        else if(led == "Lit")
            ((Chassis *)g_record[_record_uri])->indicator_led = LED_LIT;
    }

    if(_jv.as_object().find("Location") != _jv.as_object().end())
    {
        json::value j;
        j = _jv.at("Location");

        if(j.as_object().find("PostalAddress") != j.as_object().end())
        {
            json::value k;
            k = j.at("PostalAddress");

            if(k.as_object().find("Country") != k.as_object().end())
                ((Chassis *)g_record[_record_uri])->location.postal_address.country = k.at("Country").as_string();
            if(k.as_object().find("Territory") != k.as_object().end())
                ((Chassis *)g_record[_record_uri])->location.postal_address.territory = k.at("Territory").as_string();
            if(k.as_object().find("City") != k.as_object().end())
                ((Chassis *)g_record[_record_uri])->location.postal_address.city = k.at("City").as_string();
            if(k.as_object().find("Street") != k.as_object().end())
                ((Chassis *)g_record[_record_uri])->location.postal_address.street = k.at("Street").as_string();
            if(k.as_object().find("HouseNumber") != k.as_object().end())
                ((Chassis *)g_record[_record_uri])->location.postal_address.house_number = k.at("HouseNumber").as_string();
            if(k.as_object().find("Name") != k.as_object().end())
                ((Chassis *)g_record[_record_uri])->location.postal_address.name = k.at("Name").as_string();
            if(k.as_object().find("PostalCode") != k.as_object().end())
                ((Chassis *)g_record[_record_uri])->location.postal_address.postal_code = k.at("PostalCode").as_string();
        }

        if(j.as_object().find("Placement") != j.as_object().end())
        {
            json::value k;
            k = j.at("Placement");

            if(k.as_object().find("Row") != k.as_object().end())
                ((Chassis *)g_record[_record_uri])->location.placement.row = k.at("Row").as_string();
            if(k.as_object().find("Rack") != k.as_object().end())
                ((Chassis *)g_record[_record_uri])->location.placement.rack = k.at("Rack").as_string();
            if(k.as_object().find("RackOffsetUnits") != k.as_object().end())
                ((Chassis *)g_record[_record_uri])->location.placement.rack_offset_units = k.at("RackOffsetUnits").as_string();
            if(k.as_object().find("RackOffset") != k.as_object().end())
                ((Chassis *)g_record[_record_uri])->location.placement.rack_offset = k.at("RackOffset").as_integer();
        }
    }

    cout << "바꾼후~~ " << endl;
    cout << record_get_json(_record_uri) << endl;

}

void patch_power_control(json::value _jv, string _record_uri)
{
    cout << "바뀌기전~~ " << endl;
    cout << record_get_json(_record_uri) << endl;
    cout << " $$$$$$$ " << endl;

    if(_jv.as_object().find("PowerLimit") != _jv.as_object().end())
    {
        json::value j;
        j = _jv.at("PowerLimit");

        if(j.as_object().find("CorrectionInMs") != j.as_object().end())
            ((PowerControl *)g_record[_record_uri])->power_limit.correction_in_ms = j.at("CorrectionInMs").as_integer();
        if(j.as_object().find("LimitException") != j.as_object().end())
            ((PowerControl *)g_record[_record_uri])->power_limit.limit_exception = j.at("LimitException").as_string();
        if(j.as_object().find("LimitInWatts") != j.as_object().end())
            ((PowerControl *)g_record[_record_uri])->power_limit.limit_in_watts = j.at("LimitInWatts").as_double();
    }

    cout << "바꾼후~~ " << endl;
    cout << record_get_json(_record_uri) << endl;
}

// m_Request reply_error(http_request _request, m_Request _msg, string _message, web::http::status_code _status)
// {
//     json::value err;
//     http_response res(_status);
//     if(_message != "")
//     {
//         err[U("Error")] = json::value::string(U(_message));
//         res.set_body(err);
//     }
    
//     // _request.reply(status_codes::BadRequest, err);
//     _msg.result.result_datetime = currentDateTime();
//     _msg.result.result_status = WORK_FAIL;
//     _msg.result.result_response = res;

//     _request.reply(res);
//     return _msg;
// }

m_Request reply_error(http_request _request, m_Request _msg, json::value _jv, web::http::status_code _status)
{
    http_response res(_status);
    if(_jv != json::value::null())
    {
        res.set_body(_jv);
    }

    _msg.result.result_datetime = currentDateTime();
    _msg.result.result_status = WORK_FAIL;
    _msg.result.result_response = res;
    res.headers().add("Access-Control-Allow-Origin", "*");
    res.headers().add("Access-Control-Allow-Credentials", "true");
    res.headers().add("Access-Control-Allow-Methods", "POST, GET, OPTIONS, DELETE");
    res.headers().add("Access-Control-Max-Age", "3600");
    res.headers().add("Access-Control-Allow-Headers", "Content-Type, Accept, X-Requested-With, remember-me");

    _request.reply(res);
    return _msg;

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

// m_Request reply_success(http_request _request, m_Request _msg, string _uri, web::http::status_code _status)
// {
//     http_response response;
//     json::value response_json;

//     response.set_status_code(_status);

//     if(_uri != "")
//     {
//         response_json = record_get_json(_uri);
//         response.set_body(response_json);
//     }

//     _msg.result.result_datetime = currentDateTime();
//     _msg.result.result_response = response;
//     _msg.result.result_status = WORK_SUCCESS;

//     _request.reply(response);
//     return _msg;
// }

m_Request reply_success(http_request _request, m_Request _msg, json::value _jv, web::http::status_code _status)
{
    http_response res;
    res.set_status_code(_status);

    if(_jv != json::value::null())
    {
        res.set_body(_jv);
    }

    _msg.result.result_datetime = currentDateTime();
    _msg.result.result_response = res;
    _msg.result.result_status = WORK_SUCCESS;

    _request.reply(res);
    return _msg;
}