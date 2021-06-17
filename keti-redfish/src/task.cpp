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
    // cout << "T_manager Type : " << t_manager->task_type << endl;

    // Make m_Request
    m_Request msg;
    msg.request_json = jv;
    msg.host = "https://10.0.6.107:443";
    msg.method = "GET";
    msg.uri = uri;
    msg.request_datetime = currentDateTime();
    msg.task_number = allocate_task_num(); // @@@@ LOCK

    // Make m_Response
    m_Response msg_res;
    msg_res.res_number = msg.task_number;
    msg.result = msg_res;

    http_response response;


    // connect to Task_manager
    t_manager->list_request.push_back(msg);

    // Make Resource Task Json
    string task_odata = ODATA_TASK_ID;
    task_odata = task_odata + "/" + to_string(msg.task_number);
    cout << "task_data : " << task_odata << endl;
    Task *task = new Task(task_odata, "Task-" + to_string(msg.task_number));
    task->start_time = msg.request_datetime;
    task->set_payload(_request.headers(), "GET", jv, uri);
    record_save_json();


    // Check X-Auth-Token field of request
    if(!_request.headers().has("X-Auth-Token"))
    {
        http_response tmp_res(status_codes::NetworkAuthenticationRequired);
        response.set_status_code(tmp_res.status_code());
        msg_res.result_datetime = currentDateTime();
        msg_res.result_status = WORK_FAIL;
        msg_res.result_response = response;
        _request.reply(response);
        //_request.reply(status_codes::NetworkAuthenticationRequired);
        // return ;
    }

    // Check Session is valid
    else if(!is_session_valid(_request.headers()["X-Auth-Token"]))
    {
        http_response tmp_res(status_codes::Unauthorized);
        response.set_status_code(tmp_res.status_code());
        msg_res.result_datetime = currentDateTime();
        msg_res.result_status = WORK_FAIL;
        msg_res.result_response = response;
        _request.reply(response);
        // _request.reply(status_codes::Unauthorized);
        // return ;
    }

    // Check Resource is exist
    else if(!record_is_exist(uri))
    {
        http_response tmp_res(status_codes::NotFound);
        response.set_status_code(tmp_res.status_code());
        msg_res.result_datetime = currentDateTime();
        msg_res.result_status = WORK_FAIL;
        msg_res.result_response = response;
        _request.reply(response);
        // _request.reply(status_codes::NotFound);
        // return ;
    }
    // 응답주고 바로 리턴하지말고 m_Response에 상태기록해서 매니지task에 반영하고 매니지task컴플리트로 이동시키자
    else
    {
        http_response tmp_res(status_codes::OK);
        response.set_status_code(tmp_res.status_code());
        response.set_body(record_get_json(uri));
        msg_res.result_datetime = currentDateTime();
        msg_res.result_status = WORK_SUCCESS;
        msg_res.result_response = response;
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







    // Completed로 이동시키기 위해 기존 task_manager에서 삭제
    std::list<m_Request>::iterator iter;
    for(iter=t_manager->list_request.begin(); iter!=t_manager->list_request.end(); iter++)
    {
        if(iter->task_number == msg.task_number)
        {
            t_manager->list_request.erase(iter);
            break;
        }
    }

    if(task_map.find(TASK_TYPE_COMPLETED) == task_map.end())
    {
        c_manager = new Task_Manager();
        c_manager->task_type = TASK_TYPE_COMPLETED;
        task_map.insert(make_pair(TASK_TYPE_COMPLETED, c_manager));
    }
    else
        c_manager = task_map.find(TASK_TYPE_COMPLETED)->second;

    msg.result = msg_res;

    // completed로 연결
    c_manager->list_request.push_back(msg);
    // cout << "MANAGING TASK SIZE ------------" << endl;
    // cout << "size : " << task_map.size() << endl;

    // json 파일 내용 갱신
    ((Task *)g_record[task_odata])->end_time = msg.result.result_datetime;
    ((Task *)g_record[task_odata])->task_state = TASK_STATE_COMPLETED;
    record_save_json();

    /* After Move 테스트용 출력 */
    // cout << "--------------------------------------------------------------After Move" << endl;
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

    t_manager = category(uri_tokens);

    // Make m_Request
    m_Request msg;
    msg.request_json = jv;
    msg.host = "http://10.0.6.104:443";
    msg.method = "GET";
    msg.uri = new_uri;
    msg.request_datetime = currentDateTime();
    msg.task_number = allocate_task_num();

    // connect to Task_manager
    t_manager->list_request.push_back(msg);

    // Make Resource Task Json
    string task_odata = ODATA_TASK_ID;
    task_odata = task_odata + "/" + to_string(msg.task_number);
    cout << "task_data : " << task_odata << endl;
    Task *task = new Task(task_odata, "Task-" + to_string(msg.task_number));
    task->start_time = msg.request_datetime;
    task->set_payload(_request.headers(), "GET", jv, uri);
    record_save_json();

    
    http_client client(msg.host);
    http_request req(methods::GET);
    req.set_request_uri(msg.uri);
    req.set_body(jv);
    req.headers().add(_request.headers().find("X-Auth-Token")->first, _request.headers().find("X-Auth-Token")->second);
    // req.headers().add(U("TEST_HEADER"), U("VALUE!!"));
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
    std::list<m_Request>::iterator iter;
    for(iter=t_manager->list_request.begin(); iter!=t_manager->list_request.end(); iter++)
    {
        if(iter->task_number == msg.task_number)
        {
            t_manager->list_request.erase(iter);
            break;
        }
    }

    if(task_map.find(TASK_TYPE_COMPLETED) == task_map.end())
    {
        c_manager = new Task_Manager();
        c_manager->task_type = TASK_TYPE_COMPLETED;
        task_map.insert(make_pair(TASK_TYPE_COMPLETED, c_manager));
    }
    else
        c_manager = task_map.find(TASK_TYPE_COMPLETED)->second;


    // completed로 연결
    c_manager->list_request.push_back(msg);
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


    ((Task *)g_record[task_odata])->end_time = msg.result.result_datetime;
    ((Task *)g_record[task_odata])->task_state = TASK_STATE_COMPLETED;
    record_save_json();


    _request.reply(response);
    // _request.reply(U(response.status_code()), response_json);




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
 * @todo 1.현재 엣지로 요청을 보낼때 헤더를 같이 못보내고 있어서 x-auth-token 같은거 뚫는 테스트를 못함
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

    t_manager = category(uri_tokens);

    // Make m_Request
    m_Request msg;
    msg.request_json = jv;
    msg.host = "https://10.0.6.107:443";
    msg.method = "POST";
    msg.uri = uri;
    msg.request_datetime = currentDateTime();
    msg.task_number = allocate_task_num();

    // Make m_Response
    m_Response msg_res;
    msg_res.res_number = msg.task_number;
    msg.result = msg_res;

    http_response response;

    // connect to Task_manager
    t_manager->list_request.push_back(msg);

    // Make Resource Task Json
    string task_odata = ODATA_TASK_ID;
    task_odata = task_odata + "/" + to_string(msg.task_number);
    cout << "task_data : " << task_odata << endl;
    Task *task = new Task(task_odata, "Task-" + to_string(msg.task_number));
    task->start_time = msg.request_datetime;
    task->set_payload(_request.headers(), "POST", jv, uri);
    record_save_json();

    // Uri에 따른 처리동작
    if(uri == ODATA_ACCOUNT_ID)
    {
        string user_name;
        string password;
        string odata_id;
        string role_id = "ReadOnly";
        Account *account;
        bool enabled = true;

        if(jv.as_object().find("UserName") == jv.as_object().end())
        {
            _request.reply(status_codes::BadRequest);
            return;
        }
        if(jv.as_object().find("Password") == jv.as_object().end())
        {
            _request.reply(status_codes::BadRequest);
            return;
        }
        // Json body에 UserName, Password 없으면 BadRequest
        

        user_name = jv.at("UserName").as_string();
        password = jv.at("Password").as_string();

        // Check password length enought
        if (password.size() < ((AccountService *)g_record[ODATA_ACCOUNT_SERVICE_ID])->min_password_length)
        {
            // cout << "dddd" << endl; // @@@@@@@@
            _request.reply(status_codes::BadRequest);
            return;
        }
        odata_id = ODATA_ACCOUNT_ID;
        odata_id = odata_id + '/' + user_name;

        // Check account already exist
        if (record_is_exist(odata_id))
        {
            _request.reply(status_codes::Conflict);
            return;
        }

        // Additinal account information check
        if (jv.as_object().find("RoleId") != jv.as_object().end())
        {
            odata_id = ODATA_ROLE_ID;
            role_id = jv.at("RoleId").as_string();
            odata_id = odata_id + '/' + role_id;
            // Check role exist
            if (!record_is_exist(odata_id))
            {
                _request.reply(status_codes::BadRequest);
                return;
            }
        }
        if (jv.as_object().find("Enabled") != jv.as_object().end())
            enabled = jv.at("Enabled").as_bool();

        // TODO id를 계정 이름 말고 숫자로 변경 필요
        odata_id = uri + '/' + user_name;
        account = new Account(odata_id, role_id);
        account->name = "User Account";
        account->user_name = user_name;
        account->id = user_name;
        account->password = password;
        account->enabled = enabled;
        account->locked = false;

        record_save_json(); // @@@@@@@@@@@@@@@ json생성

        // Collection *account_collection = (Collection *)g_record[ODATA_ACCOUNT_ID];
        //account_collection->add_member(account);
        // 이 부분이 위에 new Account할 때 Account에서 이미 만들어줘서
        // 여기서도 하니깐 2개가 만들어짐 계정이

        http_response tmp_res(status_codes::Created);
        response.set_status_code(tmp_res.status_code());
        msg_res.result_datetime = currentDateTime();
        msg_res.result_status = WORK_SUCCESS;
        msg_res.result_response = response;
        response.set_body(record_get_json(ODATA_ACCOUNT_ID)); // 확인용임 실제에선 빼셈

        _request.reply(response);

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
        string user_name;
        string password;
        string odata_id;
        Account *account;

        if(jv.as_object().find("UserName") == jv.as_object().end())
        {
            _request.reply(status_codes::BadRequest);
            return;
        }
        if(jv.as_object().find("Password") == jv.as_object().end())
        {
            _request.reply(status_codes::BadRequest);
            return;
        }
        // Json body에 UserName, Password 없으면 BadRequest
    
        user_name = jv.at("UserName").as_string();
        password = jv.at("Password").as_string();

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

        // TODO 세션 id 생성 필요
        // string odata_id = ODATA_SESSION_ID;
        odata_id = ODATA_SESSION_ID;
        string token = generate_token(16);
        odata_id = odata_id + '/' + token;
        Session *session = new Session(odata_id, token, account);
        session->start();
        record_save_json(); // @@@@@@@@json


        http_response tmp_res(status_codes::Created);
        response.set_status_code(tmp_res.status_code());
        response.headers().add("X-Auth-Token", token);
        response.headers().add("Location", session->odata.id);
        response.set_body(json::value::object());
        msg_res.result_datetime = currentDateTime();
        msg_res.result_status = WORK_SUCCESS;
        msg_res.result_response = response;

        // web::http::http_headers::iterator iter;
        // for(iter = response.headers().begin(); iter != response.headers().end(); iter++)
        //     cout << "HEAD! : " << iter->first << " / " << iter->second << endl;

        _request.reply(response);
        // return;
    }
    else
    {
        _request.reply(status_codes::BadRequest);
        return ;
    }


    // Completed로 이동시키기 위해 기존 task_manager에서 삭제
    std::list<m_Request>::iterator iter;
    for(iter=t_manager->list_request.begin(); iter!=t_manager->list_request.end(); iter++)
    {
        if(iter->task_number == msg.task_number)
        {
            t_manager->list_request.erase(iter);
            break;
        }
    }

    if(task_map.find(TASK_TYPE_COMPLETED) == task_map.end())
    {
        c_manager = new Task_Manager();
        c_manager->task_type = TASK_TYPE_COMPLETED;
        task_map.insert(make_pair(TASK_TYPE_COMPLETED, c_manager));
    }
    else
        c_manager = task_map.find(TASK_TYPE_COMPLETED)->second;

    msg.result = msg_res;

    // completed로 연결
    c_manager->list_request.push_back(msg);

    // json 파일 내용 갱신
    ((Task *)g_record[task_odata])->end_time = msg.result.result_datetime;
    ((Task *)g_record[task_odata])->task_state = TASK_STATE_COMPLETED;
    record_save_json();

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

    // Make m_Request
    m_Request msg;
    msg.request_json = jv;
    msg.host = "http://10.0.6.104:443";
    msg.method = "POST";
    msg.uri = new_uri;
    msg.request_datetime = currentDateTime();
    msg.task_number = allocate_task_num();

    // connect to Task_manager
    t_manager->list_request.push_back(msg);

    // Make Resource Task Json
    string task_odata = ODATA_TASK_ID;
    task_odata = task_odata + "/" + to_string(msg.task_number);
    cout << "task_data : " << task_odata << endl;
    Task *task = new Task(task_odata, "Task-" + to_string(msg.task_number));
    task->start_time = msg.request_datetime;
    task->set_payload(_request.headers(), "POST", jv, uri);
    record_save_json();

    // cout << "Before Request" << endl;

    http_client client(msg.host);
    http_request req(methods::POST);
    // req.headers().add(U("MY_HEADER"), U("HEADER_VALUE"));
    req.set_request_uri(msg.uri);
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
    std::list<m_Request>::iterator iter;
    for(iter=t_manager->list_request.begin(); iter!=t_manager->list_request.end(); iter++)
    {
        if(iter->task_number == msg.task_number)
        {
            t_manager->list_request.erase(iter);
            break;
        }
    }

    if(task_map.find(TASK_TYPE_COMPLETED) == task_map.end())
    {
        c_manager = new Task_Manager();
        c_manager->task_type = TASK_TYPE_COMPLETED;
        task_map.insert(make_pair(TASK_TYPE_COMPLETED, c_manager));
    }
    else
        c_manager = task_map.find(TASK_TYPE_COMPLETED)->second;


    // completed로 연결
    c_manager->list_request.push_back(msg);
    

    ((Task *)g_record[task_odata])->end_time = msg.result.result_datetime;
    ((Task *)g_record[task_odata])->task_state = TASK_STATE_COMPLETED;
    record_save_json();
    

    // cout << "After Request" << endl;
    // cout << req.headers().find("MY_HEADER")->second << endl;

    _request.reply(response);
    // _request.reply(U(response.status_code()));


    
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


    return point;
}