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
    json::value response_json;


    // connect to Task_manager
    t_manager->list_request.push_back(msg);



    // Check X-Auth-Token field of request
    if(!_request.headers().has("X-Auth-Token"))
    {
        http_response tmp_res(status_codes::NetworkAuthenticationRequired);
        response.set_status_code(tmp_res.status_code());
        msg.result.result_datetime = currentDateTime();
        msg.result.result_status = WORK_FAIL;
        msg.result.result_response = response;
        _request.reply(response);
        //_request.reply(status_codes::NetworkAuthenticationRequired);
        // return ;
    }

    // Check Session is valid
    else if(!is_session_valid(_request.headers()["X-Auth-Token"]))
    {
        http_response tmp_res(status_codes::Unauthorized);
        response.set_status_code(tmp_res.status_code());
        msg.result.result_datetime = currentDateTime();
        msg.result.result_status = WORK_FAIL;
        msg.result.result_response = response;
        _request.reply(response);
        // _request.reply(status_codes::Unauthorized);
        // return ;
    }

    // Check Resource is exist
    else if(!record_is_exist(uri))
    {
        http_response tmp_res(status_codes::NotFound);
        response.set_status_code(tmp_res.status_code());
        msg.result.result_datetime = currentDateTime();
        msg.result.result_status = WORK_FAIL;
        msg.result.result_response = response;
        _request.reply(response);
        // _request.reply(status_codes::NotFound);
        // return ;
    }
    // 응답주고 바로 리턴하지말고 m_Response에 상태기록해서 매니지task에 반영하고 매니지task컴플리트로 이동시키자
    else
    {
        http_response tmp_res(status_codes::OK);
        json::value jj;
        jj = record_get_json(uri);
        if(jj.as_object().find("Password") != jj.as_object().end())
            jj[U("Password")] = json::value::null();
        response.set_status_code(tmp_res.status_code());
        response.set_body(jj);
        msg.result.result_datetime = currentDateTime();
        msg.result.result_status = WORK_SUCCESS;
        msg.result.result_response = response;
        _request.reply(response);
    }
    // 응답처리 완료했고 이제 매니징task랑 json 수정작업해주면됨

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
    string new_uri; // [BMC_id] 빼고 다시 만든 uri
    for(int i=0; i<uri_tokens.size(); i++)
    {
        if(i == 3)
            continue;
        new_uri += "/";
        new_uri += uri_tokens[i];
    }

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
    http_response response;
    json::value response_json;

    // Send Request
    try
    {
        /* code */
        
        pplx::task<http_response> responseTask = client.request(req);
        // pplx::task<http_response> responseTask = client.request(msg.method, msg.uri);
        response = responseTask.get();
        response_json = response.extract_json().get();
        cout << "rserser : " << response.status_code() << endl;
        // cout << response.body() << endl;
        // 요청 트라이캐치?

        
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        cout << "BMC 서버 닫혀있을거임~~" << endl;
        /* 지금 짜기에 BMC서버 닫혀있는걸로 치고 BMC에서 정보를 받았다치고 이걸로 레코드화/json화 해서 저장하는것을 할거임
        그래서 우선 레코드를 만들어야할거고 만들때 맞는 리소스 만들어야할거야 그때 안에 들어가는 내용들 하나씩 넣어줘야
        하지 그리고 그걸 savejson하면 됨*/

        
    }
    

    // http_headers::iterator it_header;
    // for(it_header = req.headers().begin(); it_header != req.headers().end(); it_header++)
    // {
    //     cout << "REQEUST Headers : " << it_header->first << " / " << it_header->second << endl;
    // }
    

    

    cout << "********************* In pplx Task **********************" << endl;
    cout << response_json << endl;
    cout << "********************* Out pplx Task **********************" << endl;

    // Make m_Response
    m_Response msg_res;
    msg_res.result_datetime = currentDateTime();
    msg_res.result_response = response;
    msg_res.res_number = msg.task_number;
    // msg_res.result_status = WORK_SUCCESS; // 응답 response의 상태에 따라 다르게 해야할텐데
    msg.result = msg_res;


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
    // 아직 status같은건 처리안해줌
    // + m_Request에 m_Response는  t_manager에는 연결안되어있음 
    // c_manager로 넘어가야 비로소 연결되어있음

    
    /* After Move 테스트용 출력 */
    // cout << "After Move ----------------------------------------------" << endl;
    // cout << "taskmap size : " << task_map.size() << endl;
    // cout << "t_list size : " << t_manager->list_request.size() << endl;
    // cout << "c_list size : " << c_manager->list_request.size() << endl;


    // // std::list<m_Request>::iterator iter; 이미 위에 생성햇네
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


    // ((Task *)g_record[task_odata])->end_time = msg.result.result_datetime;
    // ((Task *)g_record[task_odata])->task_state = TASK_STATE_COMPLETED;
    // record_save_json();


    // _request.reply(response); // 예전엔 이렇게 해도 제이슨 나왓는데 왜 안나오지
    _request.reply(U(response.status_code()), response_json);




    // -----------------------------------------------------------------------------------------    
    // std::unordered_map<uint8_t, Task_Manager *>::iterator iter;
    // for(iter = task_map.begin(); iter != task_map.end(); iter++)
    // {
    //     if(!(iter->second->list_request.empty())) // list 내용 있으면
    //     {
    //         std::list<m_Request> req_list = iter->second->list_request;

    //         std::list<m_Request>::iterator req_iter;
    //         //create_task!!
    //         for(req_iter = req_list.begin(); req_iter != req_list.end(); req_iter++)
    //         {
    //             // std::list<m_Request> *tmp = *req_iter;
    //             m_Request tmp = *req_iter;
    //             // std::list<m_Request> *
    //             pplx::create_task([tmp]{
    //                 cout << "CHECK @@@@@@@@@@@@@@@@@@@ " << endl;
    //                 cout << "host : " << tmp.host << endl;//req_iter->host << endl;
    //                 cout << "method : " << tmp.method << endl; //req_iter->method << endl;
    //                 cout << "uri : " << tmp.uri << endl;    //req_iter->uri << endl;
    //                 cout << "Send Json : " << tmp.request_json << endl;
    //                 // string tmp = req_iter->host;
    //                 http_client client(tmp.host);
    //                 pplx::task<http_response> responseTask = client.request(tmp.method, tmp.uri);//req_iter->method, req_iter->uri);
    //                 http_response response = responseTask.get();
    //                 json::value response_json = response.extract_json().get();

    //                 cout << "********************* In pplx Task **********************" << endl;
    //                 cout << response_json << endl;
    //                 cout << "********************* Out pplx Task **********************" << endl;

    //                 tmp.sender.reply(status_codes::OK, response_json);


    //                 cout << "****************** FINISH *****************" << endl;
                    
    //                 // 요청보내는 와중에 리소스 태스크하나 만들어야하고 
    //                 // 응답오면 그거 response로 해서 관리태스크에 넣고 컴플리트로 이동!
    //                 // 응답 reply해주고
    //                 // 근데 단계별로합시다 응답받는거부터 ㄱㄱ
                    

    //             }).then([]{});

    //             // cout << "Size : " << req_list.size() << endl;
    //             // cout << "Size iter : " << iter->second->list_request.size() << endl;
    //             // req_list.erase(req_iter);
    //             // cout << "Size2 : " << req_list.size() << endl;
    //             // cout << "Size2 iter : " << iter->second->list_request.size() << endl;
    //             // 안지워짐


    //         }
    //     }
    // }
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
    
    // msg.request_json = jv;
    // // msg.host = "https://10.0.6.107:443";
    // msg.host = CMM_ADDRESS;
    // msg.method = "POST";
    // msg.uri = uri;
    // msg.request_datetime = currentDateTime();
    // msg.task_number = allocate_task_num();

    // Make m_Response
    // m_Response msg_res;
    // msg_res.res_number = msg.task_number;
    // msg.result = msg_res;
    

    http_response response;
    json::value response_json;

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

    // Uri에 따른 처리동작
    if(uri == ODATA_ACCOUNT_ID)
    {

        msg = make_account(_request, msg, jv);

        // string user_name;
        // string password;
        // string odata_id;
        // string role_id = "ReadOnly";
        // Account *account;
        // bool enabled = true;

        // if(jv.as_object().find("UserName") == jv.as_object().end())
        // {
        //     _request.reply(status_codes::BadRequest);
        //     return;
        // }
        // if(jv.as_object().find("Password") == jv.as_object().end())
        // {
        //     _request.reply(status_codes::BadRequest);
        //     return;
        // }
        // Json body에 UserName, Password 없으면 BadRequest
        

        // user_name = jv.at("UserName").as_string();
        // password = jv.at("Password").as_string();

        // // Check password length enought
        // if (password.size() < ((AccountService *)g_record[ODATA_ACCOUNT_SERVICE_ID])->min_password_length)
        // {
        //     // cout << "dddd" << endl; // @@@@@@@@
        //     _request.reply(status_codes::BadRequest);
        //     return;
        // }
        // odata_id = ODATA_ACCOUNT_ID;
        // odata_id = odata_id + '/' + user_name;

        // // Check account already exist
        // if (record_is_exist(odata_id))
        // {
        //     _request.reply(status_codes::Conflict);
        //     return;
        // }

        // // Additinal account information check
        // if (jv.as_object().find("RoleId") != jv.as_object().end())
        // {
        //     odata_id = ODATA_ROLE_ID;
        //     role_id = jv.at("RoleId").as_string();
        //     odata_id = odata_id + '/' + role_id;
        //     // Check role exist
        //     if (!record_is_exist(odata_id))
        //     {
        //         _request.reply(status_codes::BadRequest);
        //         return;
        //     }
        // }
        // if (jv.as_object().find("Enabled") != jv.as_object().end())
        //     enabled = jv.at("Enabled").as_bool();

        // // TODO id를 계정 이름 말고 숫자로 변경 필요
        // odata_id = uri + '/' + user_name;
        // account = new Account(odata_id, role_id);
        // account->name = "User Account";
        // account->user_name = user_name;
        // account->id = user_name;
        // account->password = password;
        // account->enabled = enabled;
        // account->locked = false;

        // record_save_json(); // @@@@@@@@@@@@@@@ json생성

        // // Collection *account_collection = (Collection *)g_record[ODATA_ACCOUNT_ID];
        // //account_collection->add_member(account);
        // // 이 부분이 위에 new Account할 때 Account에서 이미 만들어줘서
        // // 여기서도 하니깐 2개가 만들어짐 계정이

        // http_response tmp_res(status_codes::Created);
        // response.set_status_code(tmp_res.status_code());
        // msg.result.result_datetime = currentDateTime();
        // msg.result.result_status = WORK_SUCCESS;
        // msg.result.result_response = response;
        // response.set_body(record_get_json(ODATA_ACCOUNT_ID)); // 확인용임 실제에선 빼셈 -- 넣는거같음

        // _request.reply(response);

        //@@@@@@@@@
        // cout << account->user_name << " / " << account->password << endl;
        // Account *acc = (Account *)g_record[ODATA_ACCOUNT_ID];
        //_request.reply(status_codes::Created, acc->get_json());
        //_request.reply(status_codes::OK, record_get_json(filtered_uri));
        //cout << (Collection *)g_record[ODATA_ACCOUNT_ID].members[1]->id << endl;
        // _request.reply(status_codes::Created, record_get_json(odata_id));

        // 기존 리플라이
        // _request.reply(status_codes::Created, record_get_json(ODATA_ACCOUNT_ID));
        // 원래 created까지였음
    }
    else if(uri == ODATA_SESSION_ID)
    {
        msg = make_session(_request, msg, jv);
        // string user_name;
        // string password;
        // string odata_id;
        // Account *account;

        // if(jv.as_object().find("UserName") == jv.as_object().end())
        // {
        //     _request.reply(status_codes::BadRequest);
        //     return;
        // }
        // if(jv.as_object().find("Password") == jv.as_object().end())
        // {
        //     _request.reply(status_codes::BadRequest);
        //     return;
        // }
        // // Json body에 UserName, Password 없으면 BadRequest
    
        // user_name = jv.at("UserName").as_string();
        // password = jv.at("Password").as_string();


        // Collection *col = (Collection *)g_record[ODATA_ACCOUNT_ID];
        // std::vector<Resource *>::iterator iter;
        // bool exist=false;
        // for(iter=col->members.begin(); iter!=col->members.end(); iter++)
        // {
        //     if(((Account *)(*iter))->user_name == user_name)
        //     {
        //         // 계정 존재
        //         exist = true;
        //         account = ((Account *)(*iter));
        //         break;
        //     }
        // }
        
        // if(!exist)
        // {
        //     _request.reply(status_codes::BadRequest);
        //     return;
        // }

        // // odata_id = ODATA_ACCOUNT_ID;
        // // odata_id = odata_id + '/' + user_name;

        // // // Check account exist
        // // if (!record_is_exist(odata_id))
        // // {
        // //     _request.reply(status_codes::BadRequest);
        // //     return;
        // // }

        // // account = (Account *)g_record[odata_id];

        // // Check password correct
        // if (account->password != password)
        // {
        //     _request.reply(status_codes::BadRequest);
        //     return;
        // }

        // // TODO 세션 id 생성 필요
        // // string odata_id = ODATA_SESSION_ID;
        // odata_id = ODATA_SESSION_ID;
        // string token = generate_token(16);
        // odata_id = odata_id + '/' + token;
        // Session *session = new Session(odata_id, token, account);
        // session->start();
        // record_save_json(); // @@@@@@@@json


        // http_response tmp_res(status_codes::Created);
        // response.set_status_code(tmp_res.status_code());
        // response.headers().add("X-Auth-Token", token);
        // response.headers().add("Location", session->odata.id);
        // response.set_body(json::value::object());
        // msg.result.result_datetime = currentDateTime();
        // msg.result.result_status = WORK_SUCCESS;
        // msg.result.result_response = response;

        // _request.reply(response);
        // // return;
    }
    else
    {
        json::value j;
        j[U("Error")] = json::value::string(U("No Action by POST"));
        _request.reply(status_codes::BadRequest, j);
        // return ;
        // 리멤버@@@@ 여기 리턴하면 안되겠는데? 여기걸리면 taskmap 리소스task 다 수정안되고 끝나서
    }


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

    // if(task_map.find(TASK_TYPE_COMPLETED) == task_map.end())
    // {
    //     c_manager = new Task_Manager();
    //     c_manager->task_type = TASK_TYPE_COMPLETED;
    //     task_map.insert(make_pair(TASK_TYPE_COMPLETED, c_manager));
    // }
    // else
    //     c_manager = task_map.find(TASK_TYPE_COMPLETED)->second;

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
    string new_uri; // [BMC_id] 빼고 다시 만든 uri
    for(int i=0; i<uri_tokens.size(); i++)
    {
        if(i == 3)
            continue;
        new_uri += "/";
        new_uri += uri_tokens[i];
    }

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
    json::value response_json;


    // Send Request
    try
    {
        /* code */
        pplx::task<http_response> responseTask = client.request(req);
        response = responseTask.get();
        response_json = response.extract_json().get();
        cout << "rserser : " << response.status_code() << endl;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    cout << "********************* In pplx Task **********************" << endl;
    cout << response_json << endl;
    cout << "********************* Out pplx Task **********************" << endl;

    // Make m_Response
    m_Response msg_res;
    msg_res.result_datetime = currentDateTime();
    msg_res.result_response = response;
    msg_res.res_number = msg.task_number;
    // msg_res.result_status = WORK_SUCCESS; // 응답 response의 상태에 따라 다르게 해야할텐데
    msg.result = msg_res;



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
    _request.reply(U(response.status_code()), response_json);
    
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

    // http_response response;
    // json::value response_json;

    // 요청처리
    msg = treat_uri_cmm_patch(_request, msg, jv);


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

    cout << "*****************************     Out CMM PATCH     *****************************" << endl;
}

void do_task_bmc_patch(http_request _request)
{
    //bmc~~
    string uri = _request.request_uri().to_string();
    json::value jv = _request.extract_json().get();
    vector<string> uri_tokens = string_split(uri, '/');
    string new_uri; // [BMC_id] 빼고 다시 만든 uri
    for(int i=0; i<uri_tokens.size(); i++)
    {
        if(i == 3)
            continue;
        new_uri += "/";
        new_uri += uri_tokens[i];
    }

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
        response_json = response.extract_json().get();
        cout << "rserser : " << response.status_code() << endl;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    cout << "********************* In pplx Task **********************" << endl;
    cout << response_json << endl;
    cout << "********************* Out pplx Task **********************" << endl;

    // Make m_Response
    m_Response msg_res;
    msg_res.result_datetime = currentDateTime();
    msg_res.result_response = response;
    msg_res.res_number = msg.task_number;
    // msg_res.result_status = WORK_SUCCESS; // 응답 response의 상태에 따라 다르게 해야할텐데
    msg.result = msg_res;

    // work_after_request_process(t_manager, c_manager, msg);
    c_manager = work_after_request_process(t_manager, msg);

    _request.reply(U(response.status_code()), response_json);
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

    json::value response_json;
    http_response response;

    if(uri_part == ODATA_ACCOUNT_SERVICE_ID)
    {
        // /redfish/v1/AccountService 처리
        if(uri == ODATA_ACCOUNT_SERVICE_ID)
        {
            cout << "바뀌기전~~ " << endl;
            cout << record_get_json(ODATA_ACCOUNT_SERVICE_ID) << endl;

            cout << " $$$$$$$ " << endl;

            if(_jv.as_object().find("ServiceEnabled") != _jv.as_object().end())
                ((AccountService *)g_record[ODATA_ACCOUNT_SERVICE_ID])->service_enabled = _jv.at("ServiceEnabled").as_bool();

            if(_jv.as_object().find("AuthFailureLoggingThreshold") != _jv.as_object().end())
                ((AccountService *)g_record[ODATA_ACCOUNT_SERVICE_ID])->auth_failure_logging_threshold = _jv.at("AuthFailureLoggingThreshold").as_integer();

            if(_jv.as_object().find("MinPasswordLength") != _jv.as_object().end())
                ((AccountService *)g_record[ODATA_ACCOUNT_SERVICE_ID])->min_password_length = _jv.at("MinPasswordLength").as_integer();

            if(_jv.as_object().find("AccountLockoutThreshold") != _jv.as_object().end())
                ((AccountService *)g_record[ODATA_ACCOUNT_SERVICE_ID])->account_lockout_threshold = _jv.at("AccountLockoutThreshold").as_integer();

            if(_jv.as_object().find("AccountLockoutDuration") != _jv.as_object().end())
                ((AccountService *)g_record[ODATA_ACCOUNT_SERVICE_ID])->account_lockout_duration = _jv.at("AccountLockoutDuration").as_integer();

            if(_jv.as_object().find("AccountLockoutCounterResetAfter") != _jv.as_object().end())
                ((AccountService *)g_record[ODATA_ACCOUNT_SERVICE_ID])->account_lockout_counter_reset_after = _jv.at("AccountLockoutCounterResetAfter").as_integer();

            if(_jv.as_object().find("AccountLockoutCounterResetEnabled") != _jv.as_object().end())
                ((AccountService *)g_record[ODATA_ACCOUNT_SERVICE_ID])->account_lockout_counter_reset_enabled = _jv.at("AccountLockoutCounterResetEnabled").as_bool();

            cout << "바꾼후~~ " << endl;
            cout << record_get_json(ODATA_ACCOUNT_SERVICE_ID) << endl;

            response_json = record_get_json(ODATA_ACCOUNT_SERVICE_ID);
            response.set_status_code(status_codes::OK);
            response.set_body(response_json);
            _msg.result.result_datetime = currentDateTime();
            _msg.result.result_response = response;
            _msg.result.result_status = WORK_SUCCESS;

            _request.reply(response);
            return _msg;
        }

        // /redfish/v1/AccountService/~~~ 관련처리부
        int len = uri_tokens.size();
        string minus_one; // 하나뺀거
        
        for(int i=0; i<len-1; i++)
        {
            minus_one += "/";
            minus_one += uri_tokens[i];
        }
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
            cout << "ROLE POSITION" << endl;
            // 보류중
        }
        else
        {
            // 에러에러
            // /redfish/v1/AccountService/다른uri 혹은 /redfish/v1/AccountService/Accounts or Roles 여도 길이 초과하는경우
            cout << "ERROR POSITION" << endl;
            _msg = reply_error(_request, _msg, "URI Input Error in AccountService part", status_codes::BadRequest);
            return _msg;
        }
        // 요기 else밑압축가능
    }
    else if(uri_part == ODATA_SESSION_SERVICE_ID)
    {
        // /redfish/v1/SessionService 처리
        if(uri == ODATA_SESSION_SERVICE_ID)
        {
            cout << "바뀌기전~~ " << endl;
            cout << record_get_json(ODATA_SESSION_SERVICE_ID) << endl;

            cout << " $$$$$$$ " << endl;

            unsigned int change_timeout;
            if(_jv.as_object().find("SessionTimeout") != _jv.as_object().end())
            {
                change_timeout = _jv.at("SessionTimeout").as_integer();
                ((SessionService *)g_record[ODATA_SESSION_SERVICE_ID])->session_timeout = change_timeout;
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

            response_json = record_get_json(ODATA_SESSION_SERVICE_ID);
            response.set_status_code(status_codes::OK);
            response.set_body(response_json);
            _msg.result.result_datetime = currentDateTime();
            _msg.result.result_response = response;
            _msg.result.result_status = WORK_SUCCESS;

            _request.reply(response);
            return _msg;
        }
        else
        {
            // 세션서비스 자체 그외의 uri는 에러처리
            _msg = reply_error(_request, _msg, "URI Input Error in SessionService part", status_codes::BadRequest);
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
            _msg = reply_error(_request, _msg, "URI Input Error in Manager part", status_codes::BadRequest);
            return _msg;
        }
        // 요기 else밑압축가능

        // 통과했으면 cmm_patch로 들어온거라 /redfish/v1/Managers/cmm-id 까지는 붙어있음
        string cmm_manager = ODATA_MANAGER_ID;
        cmm_manager = cmm_manager + "/" + CMM_ID;

        // /redfish/v1/Managers/cmm_id 그 자체
        if(uri == cmm_manager)
        {
            // 매니저 자체 정보수정 위치
        }

        if(uri == cmm_manager + "/NetworkProtocol")
        {
            // 네트워크프로토콜 정보수정 위치
            if(!record_is_exist(uri))
            {
                // 해당 네트워크프로토콜 레코드 없음
                _msg = reply_error(_request, _msg, "No NetworkProtocol", status_codes::BadRequest);
                return _msg;
            }

            cout << "바뀌기전~~ " << endl;
            cout << record_get_json(uri) << endl;
            cout << " $$$$$$$ " << endl;

            if(_jv.as_object().find("Description") != _jv.as_object().end())
                ((NetworkProtocol *)g_record[uri])->description = _jv.at("Description").as_string();

            if(_jv.as_object().find("SNMP") != _jv.as_object().end())
            {
                json::value j = json::value::object();
                j = _jv.at("SNMP");
                
                if(j.as_object().find("ProtocolEnabled") != j.as_object().end())
                    ((NetworkProtocol *)g_record[uri])->snmp_enabled = j.at("ProtocolEnabled").as_bool();
                if(j.as_object().find("Port") != j.as_object().end())
                    ((NetworkProtocol *)g_record[uri])->snmp_port = j.at("Port").as_integer();
            }

            if(_jv.as_object().find("IPMI") != _jv.as_object().end())
            {
                json::value j = json::value::object();
                j = _jv.at("IPMI");
                
                if(j.as_object().find("ProtocolEnabled") != j.as_object().end())
                    ((NetworkProtocol *)g_record[uri])->ipmi_enabled = j.at("ProtocolEnabled").as_bool();
                if(j.as_object().find("Port") != j.as_object().end())
                    ((NetworkProtocol *)g_record[uri])->ipmi_port = j.at("Port").as_integer();
            }

            if(_jv.as_object().find("KVMIP") != _jv.as_object().end())
            {
                json::value j = json::value::object();
                j = _jv.at("KVMIP");
                
                if(j.as_object().find("ProtocolEnabled") != j.as_object().end())
                    ((NetworkProtocol *)g_record[uri])->kvmip_enabled = j.at("ProtocolEnabled").as_bool();
                if(j.as_object().find("Port") != j.as_object().end())
                    ((NetworkProtocol *)g_record[uri])->kvmip_port = j.at("Port").as_integer();
            }

            if(_jv.as_object().find("HTTP") != _jv.as_object().end())
            {
                json::value j = json::value::object();
                j = _jv.at("HTTP");
                
                if(j.as_object().find("ProtocolEnabled") != j.as_object().end())
                    ((NetworkProtocol *)g_record[uri])->http_enabled = j.at("ProtocolEnabled").as_bool();
                if(j.as_object().find("Port") != j.as_object().end())
                    ((NetworkProtocol *)g_record[uri])->http_port = j.at("Port").as_integer();
            }

            if(_jv.as_object().find("HTTPS") != _jv.as_object().end())
            {
                json::value j = json::value::object();
                j = _jv.at("HTTPS");
                
                if(j.as_object().find("ProtocolEnabled") != j.as_object().end())
                    ((NetworkProtocol *)g_record[uri])->https_enabled = j.at("ProtocolEnabled").as_bool();
                if(j.as_object().find("Port") != j.as_object().end())
                    ((NetworkProtocol *)g_record[uri])->https_port = j.at("Port").as_integer();
            }

            if(_jv.as_object().find("NTP") != _jv.as_object().end())
            {
                json::value j = json::value::object();
                j = _jv.at("NTP");
                
                if(j.as_object().find("ProtocolEnabled") != j.as_object().end())
                    ((NetworkProtocol *)g_record[uri])->ntp_enabled = j.at("ProtocolEnabled").as_bool();
                if(j.as_object().find("Port") != j.as_object().end())
                    ((NetworkProtocol *)g_record[uri])->ntp_port = j.at("Port").as_integer();
            }

            // NTPServers 는 여러개가 들어가있는 형식인데 어떻게 수정해야하지?
            // request에서 json에 두 공간에서 받아야할듯 a,b입력란이라고 하면
            // a에서 받은 ntpservers가 존재하면 b검사하고 b에도 입력한게 있으면 그걸로 수정
            // a에서 받은 ntpservers가 존재하지않으면 걍 추가 이런식으로??

            cout << "바꾼후~~ " << endl;
            cout << record_get_json(uri) << endl;

            response_json = record_get_json(uri);
            response.set_status_code(status_codes::OK);
            response.set_body(response_json);
            _msg.result.result_datetime = currentDateTime();
            _msg.result.result_response = response;
            _msg.result.result_status = WORK_SUCCESS;

            _request.reply(response);
            return _msg;
        }

        int len = uri_tokens.size();
        string minus_one;

        for(int i=0; i<len-1; i++)
        {
            minus_one += "/";
            minus_one += uri_tokens[i];
        }
        cout << "MINUS_ONE INFO : " << minus_one << endl;

        // /redfish/v1/Managers/EthernetInterfaces/[Ethernet_id]
        if(minus_one == cmm_manager + "/EthernetInterfaces")
        {
            if(!record_is_exist(uri))
            {
                // 해당 이더넷 레코드 없음
                _msg = reply_error(_request, _msg, "No EthernetInterface", status_codes::BadRequest);
                return _msg;
            }

            cout << "바뀌기전~~ " << endl;
            cout << record_get_json(uri) << endl;
            cout << " $$$$$$$ " << endl;

            if(_jv.as_object().find("Description") != _jv.as_object().end())
                ((EthernetInterfaces *)g_record[uri])->description = _jv.at("Description").as_string();

            if(_jv.as_object().find("FQDN") != _jv.as_object().end())
                ((EthernetInterfaces *)g_record[uri])->fqdn = _jv.at("FQDN").as_string();

            if(_jv.as_object().find("HostName") != _jv.as_object().end())
                ((EthernetInterfaces *)g_record[uri])->hostname = _jv.at("HostName").as_string();

            // if(_jv.as_object().find("LinkStatus") != _jv.as_object().end())
            //     ((EthernetInterfaces *)g_record[uri])->link_status = _jv.at("LinkStatus").as_string();
            // read-only

            if(_jv.as_object().find("MACAddress") != _jv.as_object().end())
                ((EthernetInterfaces *)g_record[uri])->mac_address = _jv.at("MACAddress").as_string();

            // if(_jv.as_object().find("IPv6DefaultGateway") != _jv.as_object().end())
            //     ((EthernetInterfaces *)g_record[uri])->ipv6_default_gateway = _jv.at("IPv6DefaultGateway").as_string();
            // read-only였음

            if(_jv.as_object().find("MTUSize") != _jv.as_object().end())
                ((EthernetInterfaces *)g_record[uri])->mtu_size = _jv.at("MTUSize").as_integer();


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

                int size_v4 = ((EthernetInterfaces *)g_record[uri])->v_ipv4.size();
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

                    ((EthernetInterfaces *)g_record[uri])->v_ipv4.push_back(tmp);
                }
                else
                {
                    // 1개이상일 때 수정은 1개만있는거로 보기로 했으니깐 v_ipv4벡터의 0번째인덱스만 수정
                    if(j.as_object().find("Address") != j.as_object().end())
                        ((EthernetInterfaces *)g_record[uri])->v_ipv4[0].address = j.at("Address").as_string();
                    
                    // if(j.as_object().find("AddressOrigin") != j.as_object().end())
                    //     ((EthernetInterfaces *)g_record[uri])->v_ipv4[0].address_origin = j.at("AddressOrigin").as_string();
                    // read-only

                    if(j.as_object().find("SubnetMask") != j.as_object().end())
                        ((EthernetInterfaces *)g_record[uri])->v_ipv4[0].subnet_mask = j.at("SubnetMask").as_string();

                    if(j.as_object().find("Gateway") != j.as_object().end())
                        ((EthernetInterfaces *)g_record[uri])->v_ipv4[0].gateway = j.at("Gateway").as_string();
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

                int size_v6 = ((EthernetInterfaces *)g_record[uri])->v_ipv6.size();
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

                    ((EthernetInterfaces *)g_record[uri])->v_ipv6.push_back(tmp);
                }
                else
                {
                    // 1개이상일 때 수정은 1개만있는거로 보기로 했으니깐 v_ipv6벡터의 0번째인덱스만 수정
                    if(j.as_object().find("Address") != j.as_object().end())
                        ((EthernetInterfaces *)g_record[uri])->v_ipv6[0].address = j.at("Address").as_string();
                    
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
            cout << record_get_json(uri) << endl;

            response_json = record_get_json(uri);
            response.set_status_code(status_codes::OK);
            response.set_body(response_json);
            _msg.result.result_datetime = currentDateTime();
            _msg.result.result_response = response;
            _msg.result.result_status = WORK_SUCCESS;

            _request.reply(response);
            return _msg;
        }


    }
    else if(uri_part == ODATA_SYSTEM_ID)
    {
        string cmm_system = ODATA_SYSTEM_ID;
        cmm_system = cmm_system + "/" + CMM_ID;

        if(uri == cmm_system)
        {
            cout << "바뀌기전~~ " << endl;
            cout << record_get_json(uri) << endl;
            cout << " $$$$$$$ " << endl;
            // log(trace) << "바뀌기전~~ ";
            // log(debug) << record_get_json(uri);
            // log(info) << " $$$$$$$ ";

            if(_jv.as_object().find("Description") != _jv.as_object().end())
                ((Systems *)g_record[uri])->description = _jv.at("Description").as_string();

            if(_jv.as_object().find("HostName") != _jv.as_object().end())
                ((Systems *)g_record[uri])->hostname = _jv.at("HostName").as_string();

            if(_jv.as_object().find("AssetTag") != _jv.as_object().end())
                ((Systems *)g_record[uri])->asset_tag = _jv.at("AssetTag").as_string();

            if(_jv.as_object().find("IndicatorLED") != _jv.as_object().end())
            {
                string led = _jv.at("IndicatorLED").as_string();
                if(led == "Off")
                    ((Systems *)g_record[uri])->indicator_led = LED_OFF;
                else if(led == "Blinking")
                    ((Systems *)g_record[uri])->indicator_led = LED_BLINKING;
                else if(led == "Lit")
                    ((Systems *)g_record[uri])->indicator_led = LED_LIT;
            }

            if(_jv.as_object().find("Boot") != _jv.as_object().end())
            {
                json::value j = json::value::object();
                j = _jv.at("Boot");

                if(j.as_object().find("BootSourceOverrideEnabled") != j.as_object().end())
                    ((Systems *)g_record[uri])->boot.boot_source_override_enabled = j.at("BootSourceOverrideEnabled").as_string();

                if(j.as_object().find("BootSourceOverrideTarget") != j.as_object().end())
                    ((Systems *)g_record[uri])->boot.boot_source_override_target = j.at("BootSourceOverrideTarget").as_string();

                if(j.as_object().find("BootSourceOverrideMode") != j.as_object().end())
                    ((Systems *)g_record[uri])->boot.boot_source_override_mode = j.at("BootSourceOverrideMode").as_string();

                if(j.as_object().find("UefiTargetBootSourceOverride") != j.as_object().end())
                    ((Systems *)g_record[uri])->boot.uefi_target_boot_source_override = j.at("UefiTargetBootSourceOverride").as_string();
            }


            cout << "바꾼후~~ " << endl;
            cout << record_get_json(uri) << endl;
            // log(warning) << "바꾼후~~ ";
            // log(error) << record_get_json(uri);
            // log(fatal) << "FATAL!";

            response_json = record_get_json(uri);
            response.set_status_code(status_codes::OK);
            response.set_body(response_json);
            _msg.result.result_datetime = currentDateTime();
            _msg.result.result_response = response;
            _msg.result.result_status = WORK_SUCCESS;

            _request.reply(response);
            return _msg;
                
        }
        else
        {
            _msg = reply_error(_request, _msg, "URI Input Error in Systems part", status_codes::BadRequest);
            return _msg;
        }// 맨아래 reply_error로 통합가능
    }
    else if(uri_part == ODATA_CHASSIS_ID)
    {
        string cmm_chassis = ODATA_CHASSIS_ID;
        cmm_chassis = cmm_chassis + "/" + CMM_ID;

        if(uri == cmm_chassis)
        {
            cout << "바뀌기전~~ " << endl;
            cout << record_get_json(uri) << endl;
            cout << " $$$$$$$ " << endl;

            if(_jv.as_object().find("AssetTag") != _jv.as_object().end())
                ((Chassis *)g_record[uri])->asset_tag = _jv.at("AssetTag").as_string();

            if(_jv.as_object().find("IndicatorLED") != _jv.as_object().end())
            {
                string led = _jv.at("IndicatorLED").as_string();
                if(led == "Off")
                    ((Chassis *)g_record[uri])->indicator_led = LED_OFF;
                else if(led == "Blinking")
                    ((Chassis *)g_record[uri])->indicator_led = LED_BLINKING;
                else if(led == "Lit")
                    ((Chassis *)g_record[uri])->indicator_led = LED_LIT;
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
                        ((Chassis *)g_record[uri])->location.postal_address.country = k.at("Country").as_string();
                    if(k.as_object().find("Territory") != k.as_object().end())
                        ((Chassis *)g_record[uri])->location.postal_address.territory = k.at("Territory").as_string();
                    if(k.as_object().find("City") != k.as_object().end())
                        ((Chassis *)g_record[uri])->location.postal_address.city = k.at("City").as_string();
                    if(k.as_object().find("Street") != k.as_object().end())
                        ((Chassis *)g_record[uri])->location.postal_address.street = k.at("Street").as_string();
                    if(k.as_object().find("HouseNumber") != k.as_object().end())
                        ((Chassis *)g_record[uri])->location.postal_address.house_number = k.at("HouseNumber").as_string();
                    if(k.as_object().find("Name") != k.as_object().end())
                        ((Chassis *)g_record[uri])->location.postal_address.name = k.at("Name").as_string();
                    if(k.as_object().find("PostalCode") != k.as_object().end())
                        ((Chassis *)g_record[uri])->location.postal_address.postal_code = k.at("PostalCode").as_string();
                }

                if(j.as_object().find("Placement") != j.as_object().end())
                {
                    json::value k;
                    k = j.at("Placement");

                    if(k.as_object().find("Row") != k.as_object().end())
                        ((Chassis *)g_record[uri])->location.placement.row = k.at("Row").as_string();
                    if(k.as_object().find("Rack") != k.as_object().end())
                        ((Chassis *)g_record[uri])->location.placement.rack = k.at("Rack").as_string();
                    if(k.as_object().find("RackOffsetUnits") != k.as_object().end())
                        ((Chassis *)g_record[uri])->location.placement.rack_offset_units = k.at("RackOffsetUnits").as_string();
                    if(k.as_object().find("RackOffset") != k.as_object().end())
                        ((Chassis *)g_record[uri])->location.placement.rack_offset = k.at("RackOffset").as_integer();
                }
            }

            cout << "바꾼후~~ " << endl;
            cout << record_get_json(uri) << endl;

            response_json = record_get_json(uri);
            response.set_status_code(status_codes::OK);
            response.set_body(response_json);
            _msg.result.result_datetime = currentDateTime();
            _msg.result.result_response = response;
            _msg.result.result_status = WORK_SUCCESS;

            _request.reply(response);
            return _msg;
        }

        // int len = uri_tokens.size();
        string minus_one;
        minus_one = get_parent_object_uri(uri, "/");
        cout << "MINUS_ONE INFO : " << minus_one << endl;

        if(minus_one == cmm_chassis + "/Power/PowerControl")
        {
            if(!record_is_exist(uri))
            {
                // 해당 파워컨트롤 레코드 없음
                _msg = reply_error(_request, _msg, "No PowerControl", status_codes::BadRequest);
                return _msg;
            }

            cout << "바뀌기전~~ " << endl;
            cout << record_get_json(uri) << endl;
            cout << " $$$$$$$ " << endl;

            if(_jv.as_object().find("PowerLimit") != _jv.as_object().end())
            {
                json::value j;
                j = _jv.at("PowerLimit");

                if(j.as_object().find("CorrectionInMs") != j.as_object().end())
                    ((PowerControl *)g_record[uri])->power_limit.correction_in_ms = j.at("CorrectionInMs").as_integer();
                if(j.as_object().find("LimitException") != j.as_object().end())
                    ((PowerControl *)g_record[uri])->power_limit.limit_exception = j.at("LimitException").as_string();
                if(j.as_object().find("LimitInWatts") != j.as_object().end())
                    ((PowerControl *)g_record[uri])->power_limit.limit_in_watts = j.at("LimitInWatts").as_double();
            }

            cout << "바꾼후~~ " << endl;
            cout << record_get_json(uri) << endl;

            response_json = record_get_json(uri);
            response.set_status_code(status_codes::OK);
            response.set_body(response_json);
            _msg.result.result_datetime = currentDateTime();
            _msg.result.result_response = response;
            _msg.result.result_status = WORK_SUCCESS;

            _request.reply(response);
            return _msg;
        }
        // 아래에서 reply_error
    }
    else
    {
        _msg = reply_error(_request, _msg, "URI Input Error in Whole part", status_codes::BadRequest);
        return _msg;
    }

    _msg = reply_error(_request, _msg, "URI Input Error in Below part", status_codes::BadRequest);
    // 위에 조건문들에서 처리에 걸렸으면 처리하는거고 아니면 걍 여기서 한꺼번에 reply_error처리로 하지
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
        _msg = reply_error(_request, _msg, "No UserName or Password", status_codes::BadRequest);
        return _msg;
    } // json request body에 UserName, Password없으면 BadRequest

    user_name = _jv.at("UserName").as_string();
    password = _jv.at("Password").as_string();
    // 이거 입력 타입오류 try-catch? 어차피 입력하는곳 만드는 곳에서 입력란에것을 string으로 주게하면되긴함

    unsigned int min_pass_length = ((AccountService *)g_record[ODATA_ACCOUNT_SERVICE_ID])->min_password_length;
    if(password.size() < min_pass_length)
    {
        _msg = reply_error(_request, _msg, "Password length must be at least " + to_string(min_pass_length), status_codes::BadRequest);
        // json::value err;
        // err[U("Error")] = json::value::string(U("Password length must be at least " + to_string(min_pass_length)));
        // http_response res(status_codes::BadRequest);
        // res.set_body(err);
        // _request.reply(res);
        // // _request.reply(status_codes::BadRequest, err);
        // _msg.result.result_datetime = currentDateTime();
        // _msg.result.result_status = WORK_FAIL;
        // _msg.result.result_response = res;
        return _msg;
    }// password 길이가 짧으면 BadRequest

    // Collection col = *((Collection *)g_record[ODATA_ACCOUNT_ID]); // 이건 왜 터졌지?
    Collection *col = (Collection *)g_record[ODATA_ACCOUNT_ID];
    std::vector<Resource *>::iterator iter;
    for(iter=col->members.begin(); iter!=col->members.end(); iter++)
    {
        if(((Account *)(*iter))->user_name == user_name)
        {
            _msg = reply_error(_request, _msg, "UserName already exists", status_codes::Conflict);
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
            _msg = reply_error(_request, _msg, "No Role", status_codes::BadRequest);
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
        _msg = reply_error(_request, _msg, "No UserName or Password", status_codes::BadRequest);
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
        _msg = reply_error(_request, _msg, "No Account using that UserName", status_codes::BadRequest);
        return _msg;
    } // 입력한 username에 해당하는 계정없음

    if(account->password != password)
    {
        _msg = reply_error(_request, _msg, "Password does not match", status_codes::BadRequest);
        return _msg;
    } // 비밀번호 맞지않음

    odata_id = ODATA_SESSION_ID;
    string token = generate_token(16);
    odata_id = odata_id + '/' + token;
    Session *session = new Session(odata_id, token, account);
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
        _msg = reply_error(_request, _msg, "No Account", status_codes::BadRequest);
        return _msg;
    }

    cout << "ACCOUNT POSITION" << endl;
    cout << "바뀌기전~~ " << endl;
    cout << record_get_json(_uri) << endl;
    cout << " $$$$$$$ " << endl;

    if(!_request.headers().has("X-Auth-Token"))
    {
        // 로그인이 안되어있음(X-Auth-Token이 존재하지않음)
        _msg = reply_error(_request, _msg, "Login Required", status_codes::BadRequest);
        return _msg;
    }

    string session_token = _request.headers()["X-Auth-Token"];

    if(!is_session_valid(session_token))
    {
        // 세션이 유효하지않음(X-Auth-Token은 존재하나 유효하지 않음)
        _msg = reply_error(_request, _msg, "Session Unvalid", status_codes::BadRequest);
        return _msg;
    }

    string session_uri = ODATA_SESSION_ID;
    session_uri = session_uri + "/" + session_token;
    cout << "SESSION ! : " << session_uri << endl;

    // Session session = *((Session *)g_record[session_uri]);
    Session *session = (Session *)g_record[session_uri];

    string last = get_current_object_name(_uri, "/"); // account_id임 last는

    if(session->account->role->id == "Administrator")
    {
        // 관리자만 롤 변경가능하게 설계해봄
        if(_jv.as_object().find("RoleId") != _jv.as_object().end())
        {
            string role_odata = ODATA_ROLE_ID;
            string role_id = _jv.at("RoleId").as_string();
            role_odata = role_odata + "/" + role_id;

            if(!record_is_exist(role_odata))
            {
                // 해당 롤 없음
                _msg = reply_error(_request, _msg, role_id + " does not exist", status_codes::BadRequest);
                return _msg;
            }

            ((Account *)g_record[_uri])->role = ((Role *)g_record[role_odata]);
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
            string input_username = _jv.at("UserName").as_string();
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
                    _msg = reply_error(_request, _msg, "UserName is already in use", status_codes::BadRequest);
                    return _msg;
                }

            } // user_name 중복검사

            ((Account *)g_record[_uri])->user_name = input_username;
        }

        if(_jv.as_object().find("Password") != _jv.as_object().end())
            ((Account *)g_record[_uri])->password = _jv.at("Password").as_string();


    }
    // role_id로 하기엔 role을 생성할 수도 있는 거여서 나중에 role안에 privileges에 권한 확인하는걸로 바꿔야 할 수도있음
    // + 현재 로그인이 관리자로되어있어서 위에서 role관련 수행했는데 성공하고 아래 유저네임 패스워드에서 에러잡혀서
    // 수행을 하지 않으면 response에는 BadRequest 인데 role은 바뀐게 적용되고있음 기억해 둬야함 .. 리멤버
    // 로직 수정해야겠는데? ex) jv에 롤있는데 관리자권한이 아니라든가 위에 리멤버라든가 

    

    cout << "바꾼후~~ " << endl;
    cout << record_get_json(_uri) << endl;
    cout << "세션에 연결된 계정정보" << endl;
    cout << record_get_json(session->account->odata.id) << endl;

    http_response response;
    json::value response_json;

    response_json = record_get_json(_uri);
    response.set_status_code(status_codes::OK);
    response.set_body(response_json);
    _msg.result.result_datetime = currentDateTime();
    _msg.result.result_response = response;
    _msg.result.result_status = WORK_SUCCESS;

    _request.reply(response);
    return _msg;
}

m_Request reply_error(http_request _request, m_Request _msg, string _message, web::http::status_code _status)
{
    json::value err;
    err[U("Error")] = json::value::string(U(_message));
    http_response res(_status);
    res.set_body(err);
    _request.reply(res);
    // _request.reply(status_codes::BadRequest, err);
    _msg.result.result_datetime = currentDateTime();
    _msg.result.result_status = WORK_FAIL;
    _msg.result.result_response = res;
    return _msg;
}