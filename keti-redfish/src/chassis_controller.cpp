#include "chassis_controller.hpp"

extern unordered_map<string, Resource *> g_record;


/**
 * @brief BMC를 등록하는 함수
 * @authors 강
 * 
 * @param _bmc_id BMC_id
 * @param _ip  0.0.0.0 형식의 ip주소
 * @param _port 포트번호 :xxx로 붙을거임
 * @param _is_https http인지 https인지 (true가 https)
 * @param _username BMC에서 정보를 가져와야하는데 이때 로그인이 필요해서 유저네임과
 * @param _password 패스워드를 받는다.
 */
void add_new_bmc(string _bmc_id, string _ip, string _port, bool _is_https, string _username, string _password)
{
    string odata_chassis, odata_system, odata_manager, odata_update;

    // check exist
    odata_system = ODATA_SYSTEM_ID;
    odata_system = odata_system + "/" + _bmc_id;
    odata_chassis = ODATA_CHASSIS_ID;
    odata_chassis = odata_chassis + "/" + _bmc_id;
    odata_manager = ODATA_MANAGER_ID;
    odata_manager = odata_manager + "/" + _bmc_id;
    odata_update = ODATA_UPDATE_SERVICE_ID;
    odata_update = odata_update + "/" + _bmc_id;

    cout << " 잘 나오나? " << endl;
    cout << "sys : " << odata_system << endl;
    cout << "cha : " << odata_chassis << endl;
    cout << "man : " << odata_manager << endl;
    cout << "upd : " << odata_update << endl;

    if(record_is_exist(odata_system) || record_is_exist(odata_chassis) ||
    record_is_exist(odata_manager) || record_is_exist(odata_update))
    {
        cout << "이미 존재하는 BMC_id입니다." << endl;
        cout << "입력한 BMC_id : " << _bmc_id << endl;
        return ;
    }

    // Login
    string host;
    if(_is_https == true)
        host = "https";
    else
        host = "http";
    host = host + "://" + _ip + ":" + _port;
    cout << "Host : " << host << endl;

    string session_uri = ODATA_SESSION_ID;//"/redfish/v1/SessionService/Sessions";
    json::value jv;
    jv[U("UserName")] = json::value::string(U(_username));
    jv[U("Password")] = json::value::string(U(_password));
    http_client client(host);
    http_request req_login(methods::POST);
    req_login.set_request_uri(session_uri);
    req_login.set_body(jv);
    http_response login_response;
    string auth_token;
    
    try
    {
        /* code */
        pplx::task<http_response> responseTask = client.request(req_login);
        login_response = responseTask.get();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        cout << "BMC 로그인 요청 실패" << endl;
        return ;
    }
    cout << login_response.status_code() << endl;

    auth_token = login_response.headers()["X-Auth-Token"];


    //make system
    add_system(odata_system, host, auth_token);
    // Systems *system = new Systems(odata, "BMC System");

    // //make chassis
    add_chassis(odata_chassis, host, auth_token);
    // odata = ODATA_CHASSIS_ID;
    // odata = odata + "/" + _bmc_id;
    // Chassis *chassis = new Chassis(odata, "BMC Chassis");

    // //make manager
    add_manager(odata_manager, host, auth_token);
    // odata = ODATA_MANAGER_ID;
    // odata = odata + "/" + _bmc_id;
    // Manager *manager = new Manager(odata, "BMC Manager");

    // //make update_service
    // odata = ODATA_UPDATE_SERVICE_ID;
    // odata = odata + "/" + _bmc_id;
    // UpdateService *update = new UpdateService(odata, "BMC Update");

}

void add_system(string _bmc_id, string _host, string _auth_token)
{
    // Systems *system = new Systems(_bmc_id, "BMC System");
    string uri = _bmc_id;
    vector<string> uri_tokens = string_split(uri, '/');
    string new_uri;
    for(int i=0; i<uri_tokens.size(); i++)
    {
        if(i == 3)
            continue;
        new_uri += "/";
        new_uri += uri_tokens[i];
    }

    cout << " ADD System " << endl;
    cout << "New Uri : " << new_uri << endl;

    // string host;
    // if(_is_https == true)
    //     host = "https";
    // else
    //     host = "http";
    // host = host + "://" + _ip + ":" + _port;
    // cout << "Host : " << host << endl;

    // string session_uri = "/redfish/v1/SessionService/Sessions";
    // json::value jv;
    // jv[U("UserName")] = json::value::string(U(_username));
    // jv[U("Password")] = json::value::string(U(_password));
    // http_client client(host);
    // http_request req_login(methods::POST);
    // req_login.set_request_uri(session_uri);
    // req_login.set_body(jv);
    // http_response login_response;
    // string auth_token;
    
    // try
    // {
    //     /* code */
    //     pplx::task<http_response> responseTask = client.request(req_login);
    //     login_response = responseTask.get();
    // }
    // catch(const std::exception& e)
    // {
    //     std::cerr << e.what() << '\n';
    //     cout << "BMC 로그인 요청 실패" << endl;
    //     return ;
    // }

    // cout << login_response.status_code() << endl;

    // auth_token = login_response.headers()["X-Auth-Token"];

    http_client client(_host);

    http_request req(methods::GET);
    req.set_request_uri(new_uri);
    req.headers().add("X-Auth-Token", _auth_token);
    http_response response;

    if(req.headers().has("X-Auth-Token"))
        cout << "Token : " << req.headers()["X-Auth-Token"] << endl;

    try
    {
        /* code */
        pplx::task<http_response> responseTask = client.request(req);
        response = responseTask.get();

    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        cout << "BMC System 정보요청 실패" << endl;
        return ;
    }

    json::value system_info;
    system_info = response.extract_json().get();

    cout << "일단 SYSTEM INFO " << endl;
    cout <<system_info << endl;


    // Resource 드디어 생성
    Systems *system = new Systems(_bmc_id, "BMC System");
    

    // System 멤버변수 넣어주기
    if(system_info.as_object().find("Id") != system_info.as_object().end()) // 존재하면
    {
        // cout << "FIRE FIRE 1" << endl;
        system->id = system_info.at("Id").as_string();
    }
    else
    cout << "System Id is X" << endl;

    if(system_info.as_object().find("SystemType") != system_info.as_object().end())
    {
        // cout << "FIRE FIRE 2" << endl;
        system->system_type = system_info.at("SystemType").as_string();
    }
    else
    cout << "System SystemType is X" << endl;

    if(system_info.as_object().find("AssetTag") != system_info.as_object().end())
    {
        // cout << "FIRE FIRE 3" << endl;
        system->asset_tag = system_info.at("AssetTag").as_string();
    }
    else
    cout << "System AssetTag is X" << endl;

    if(system_info.as_object().find("Manufacturer") != system_info.as_object().end())
    {
        // cout << "FIRE FIRE 4" << endl;
        system->manufacturer = system_info.at("Manufacturer").as_string();
    }
    else
    cout << "System Manufacturer is X" << endl;

    if(system_info.as_object().find("Model") != system_info.as_object().end())
    {
        // cout << "FIRE FIRE 5" << endl;
        system->model = system_info.at("Model").as_string();
    }
    else
    cout << "System Model is X" << endl;

    if(system_info.as_object().find("SerialNumber") != system_info.as_object().end())
    {
        // cout << "FIRE FIRE 6" << endl;
        system->serial_number = system_info.at("SerialNumber").as_string();
    }
    else
    cout << "System SerialNumber is X" << endl;

    if(system_info.as_object().find("PartNumber") != system_info.as_object().end())
    {
        // cout << "FIRE FIRE 7" << endl;
        system->part_number = system_info.at("PartNumber").as_string();
    }
    else
    cout << "System PartNumber is X" << endl;

    if(system_info.as_object().find("Description") != system_info.as_object().end())
    {
        // cout << "FIRE FIRE 8" << endl;
        system->description = system_info.at("Description").as_string();
    }
    else
    cout << "System Description is X" << endl;

    if(system_info.as_object().find("UUID") != system_info.as_object().end())
    {
        // cout << "FIRE FIRE 9" << endl;
        system->uuid = system_info.at("UUID").as_string();
    }
    else
    cout << "System UUID is X" << endl;

    if(system_info.as_object().find("HostName") != system_info.as_object().end())
    {
        // cout << "FIRE FIRE 10" << endl;
        system->hostname = system_info.at("HostName").as_string();
    }
    else
    cout << "System HostName is X" << endl;

    if(system_info.as_object().find("PowerState") != system_info.as_object().end())
    {
        // cout << "FIRE FIRE 11" << endl;
        system->power_state = system_info.at("PowerState").as_string();
    }
    else
    cout << "System PowerState is X" << endl;

    if(system_info.as_object().find("BiosVersion") != system_info.as_object().end())
    {
        // cout << "FIRE FIRE 12" << endl;
        system->bios_version = system_info.at("BiosVersion").as_string();
    }
    else
    cout << "System BiosVersion is X" << endl;



    if(system_info.as_object().find("Status") != system_info.as_object().end())
    {
        // cout << "FIRE FIRE 13" << endl;
        json::value j = json::value::object();
        j = system_info.at("Status");

        if(j.as_object().find("State") != j.as_object().end())
            system->status.state = j.at("State").as_string();
        if(j.as_object().find("Health") != j.as_object().end())
            system->status.health = j.at("Health").as_string();
    }
    else
    cout << "System Status is X" << endl;
    // status는 없으면 critical 하라구?

    // if(system_info.as_object().find("HostingRoles") != system_info.as_object().end())
    // {
    //     // cout << "FIRE FIRE 14" << endl;
    //     json::value j = json::value::array();
    //     j = system_info.at("HostingRoles");
    //     cout << "HostingRoles Array의 사이즈 : " << j.size() << endl;
    //     for(int i=0; i<j.size(); i++)
    //     {
    //         system->hosting_roles.push_back(j[i].as_string());
    //     }
    // }
    // else
    // cout << "System HostingRoles is X" << endl;

    if(system_info.as_object().find("IndicatorLED") != system_info.as_object().end())
    {
        // cout << "FIRE FIRE 15" << endl;
        string led = system_info.at("IndicatorLED").as_string();

        if(led == "Off")
            system->indicator_led = LED_OFF;
        else if(led == "Blinking")
            system->indicator_led = LED_BLINKING;
        else if(led == "Lit")
            system->indicator_led = LED_LIT;
    }
    else
    cout << "System IndicatorLED is X" << endl;

    if(system_info.as_object().find("Boot") != system_info.as_object().end())
    {
        // cout << "FIRE FIRE 16" << endl;
        json::value j = json::value::object();
        j = system_info.at("Boot");
        // cout << "FIRE FIRE 여기다" << endl;
        if(j.as_object().find("BootSourceOverrideEnabled") != j.as_object().end())
            system->boot.boot_source_override_enabled = j.at("BootSourceOverrideEnabled").as_string();
        if(j.as_object().find("BootSourceOverrideTarget") != j.as_object().end())
            system->boot.boot_source_override_target = j.at("BootSourceOverrideTarget").as_string();
        if(j.as_object().find("BootSourceOverrideMode") != j.as_object().end())
            system->boot.boot_source_override_mode = j.at("BootSourceOverrideMode").as_string();
        if(j.as_object().find("UefiTargetBootSourceOverride") != j.as_object().end())
            system->boot.uefi_target_boot_source_override = j.at("UefiTargetBootSourceOverride").as_string();
        // cout << "FIRE FIRE 여기냐" << endl;
    } //boot형태가 달라서 터짐
    // 여기까지는 단순 변수에 넣으면 되는거라 else 부분이나 if에 else없는 부분을 처리해줄 필요없음 ㅇㅇ


    // 여기부턴 odata 타고 들어가는 애들 이라 else처리해줘야하고 어떻게 넣을지도 적어놔야함
    // cout << "FIRE FIRE  A  1 " << endl;
    if(system_info.as_object().find("Bios") != system_info.as_object().end())
    {
        cout << "Bios Exist ~~" << endl;
        json::value bios_info;
        string inner_uri;
        if(system_info.at("Bios").as_object().find("@odata.id") != system_info.at("Bios").as_object().end())
        {
            inner_uri = system_info.at("Bios").at("@odata.id").as_string();
            bios_info = get_json_info(inner_uri, _host, _auth_token);
            cout << "받아라 Bios INFO" << endl;
            cout << bios_info << endl;
        }
        else
        {
            // 뭐 Bios 안에 @odata.id가 없으면 없는거지 뭐 안만들면됨
        }

        // 얘는 멤버아니고 바로 정보 사용하면 되네
        
    }
    else
    cout << "System Bios is X" << endl;
    
    // cout << "FIRE FIRE  A  2 " << endl;

    if(system_info.as_object().find("Processors") != system_info.as_object().end())
    {
        cout << "Processors Exist ~~" << endl;
        json::value processor_info;
        string inner_uri;
        if(system_info.at("Processors").as_object().find("@odata.id") != system_info.at("Processors").as_object().end())
            inner_uri = system_info.at("Processors").at("@odata.id").as_string();
        cout << "INNER URI : " << inner_uri << endl;
        processor_info = get_json_info(inner_uri, _host, _auth_token);
        cout << "받아라 Processors INFO" << endl;
        cout << processor_info << endl;

        //vector<string> member_uri;
        json::value array_member = json::value::array();
        if(processor_info.as_object().find("Members") != processor_info.as_object().end())
            array_member = processor_info.at("Members");
        for(int i=0; i<array_member.size(); i++)
        {
            json::value cpu_info;
            string member_uri;
            if(array_member[i].as_object().find("@odata.id") != array_member[i].as_object().end())
                member_uri = array_member[i].at("@odata.id").as_string();
            cout << "MEMBER URI : " << member_uri << endl;
            cpu_info = get_json_info(member_uri, _host, _auth_token);
            cout << "받아라 Processor 안의 CPU INFO" << endl;
            cout << cpu_info << endl;
        }
        
    }
    else
    cout << "System Processors is X" << endl;
    // cout << "FIRE FIRE  A  3 " << endl;

    if(system_info.as_object().find("Memory") != system_info.as_object().end())
    {
        cout << "Memory Exist ~~" << endl;
        json::value memory_info;
        string inner_uri;
        if(system_info.at("Memory").as_object().find("@odata.id") != system_info.at("Memory").as_object().end())
            inner_uri = system_info.at("Memory").at("@odata.id").as_string();
        cout << "INNER URI : " << inner_uri << endl;
        memory_info = get_json_info(inner_uri, _host, _auth_token);
        cout << "받아라 Memory INFO" << endl;
        cout << memory_info << endl;

        json::value array_member = json::value::array();
        if(memory_info.as_object().find("Members") != memory_info.as_object().end())
            array_member = memory_info.at("Members");

        for(int i=0; i<array_member.size(); i++)
        {
            json::value mem_info;
            string member_uri;
            if(array_member[i].as_object().find("@odata.id") != array_member[i].as_object().end())
                member_uri = array_member[i].at("@odata.id").as_string();
            cout << "MEMBER URI : " << member_uri << endl;
            mem_info = get_json_info(member_uri, _host, _auth_token);
            cout << "받아라 Memory 안의 mem INFO" << endl;
            cout << mem_info << endl;
        }


    }
    else
    cout << "System Memory is X" << endl;
    // cout << "FIRE FIRE  A  4 " << endl;

    if(system_info.as_object().find("EthernetInterfaces") != system_info.as_object().end())
    {
        cout << "EthernetInterfaces Exist ~~" << endl;
        json::value ethernet_info;
        string inner_uri;
        if(system_info.at("EthernetInterfaces").as_object().find("@odata.id") != system_info.at("EthernetInterfaces").as_object().end())
            inner_uri = system_info.at("EthernetInterfaces").at("@odata.id").as_string();
        cout << "INNER URI : " << inner_uri << endl;
        ethernet_info = get_json_info(inner_uri, _host, _auth_token);
        cout << "받아라 EthernetInterfaces INFO" << endl;
        cout << ethernet_info << endl;

        json::value array_member = json::value::array();
        if(ethernet_info.as_object().find("Members") != ethernet_info.as_object().end())
            array_member = ethernet_info.at("Members");
        for(int i=0; i<array_member.size(); i++)
        {
            json::value et;
            string member_uri;
            if(array_member[i].as_object().find("@odata.id") != array_member[i].as_object().end())
                member_uri = array_member[i].at("@odata.id").as_string();
            cout << "MEMBER URI : " << member_uri << endl;
            et = get_json_info(member_uri, _host, _auth_token);
            cout << "받아라 EthernetInterfaces 안의 ethernet INFO" << endl;
            cout << et << endl;
        }
    }
    else
    cout << "System EthernetInterfaces is X" << endl;
    // cout << "FIRE FIRE  A  5 " << endl;

    if(system_info.as_object().find("SimpleStorage") != system_info.as_object().end())
    {
        cout << "SimpleStorage Exist ~~" << endl;
        json::value simple_info;
        string inner_uri;
        if(system_info.at("SimpleStorage").as_object().find("@odata.id") != system_info.at("SimpleStorage").as_object().end())
            inner_uri = system_info.at("SimpleStorage").at("@odata.id").as_string();
        cout << "INNER URI : " << inner_uri << endl;
        simple_info = get_json_info(inner_uri, _host, _auth_token);
        cout << "받아라 SimpleStorage INFO" << endl;
        cout << simple_info << endl;

        json::value array_member = json::value::array();
        if(simple_info.as_object().find("Members") != simple_info.as_object().end())
            array_member = simple_info.at("Members");

        for(int i=0; i<array_member.size(); i++)
        {
            json::value sim_info;
            string member_uri;
            if(array_member[i].as_object().find("@odata.id") != array_member[i].as_object().end())
                member_uri = array_member[i].at("@odata.id").as_string();
            cout << "MEMBER URI : " << member_uri << endl;
            sim_info = get_json_info(member_uri, _host, _auth_token);
            cout << "받아라 SimpleStorage 안의 sim INFO" << endl;
            cout << sim_info << endl;
        }
    }
    else
    cout << "System SimpleStorage is X" << endl;
    // cout << "FIRE FIRE  A  6 " << endl;

    // LogService
    if(system_info.as_object().find("LogServices") != system_info.as_object().end())
    {
        cout << "LogServices Exist ~~" << endl;
        json::value service_col_info;
        string inner_uri;
        if(system_info.at("LogServices").as_object().find("@odata.id") != system_info.at("LogServices").as_object().end())
            inner_uri = system_info.at("LogServices").at("@odata.id").as_string();
        cout << "INNER URI : " << inner_uri << endl;
        service_col_info = get_json_info(inner_uri, _host, _auth_token);
        cout << "받아라 LogService Collection INFO" << endl;
        cout << service_col_info << endl;

        json::value array_member = json::value::array();
        if(service_col_info.as_object().find("Members") != service_col_info.as_object().end())
            array_member = service_col_info.at("Members");

        for(int i=0; i<array_member.size(); i++)
        {
            json::value service_info;
            string member_uri;
            if(array_member[i].as_object().find("@odata.id") != array_member[i].as_object().end())
                member_uri = array_member[i].at("@odata.id").as_string();
            cout << "MEMBER URI : " << member_uri << endl;
            service_info = get_json_info(member_uri, _host, _auth_token);
            cout << "받아라 LogService Collection 안의 LogSerivce INFO" << endl;
            cout << service_info << endl;

            json::value entry_col_info;
            string entry_col_uri;
            if(service_info.as_object().find("Entries") != service_info.as_object().end())
            {
                if(service_info.at("Entries").as_object().find("@odata.id") != service_info.at("Entries").as_object().end())
                    entry_col_uri = service_info.at("Entries").at("@odata.id").as_string();
                json::value entry_info;
                cout << "Entry Col URI : " << entry_col_uri << endl;

                entry_info = get_json_info(entry_col_uri, _host, _auth_token);
                cout << "받아라 LogService Collection 안의 LogService 안의 LogEntry Collection INFO" << endl;
                cout << entry_info << endl;

                json::value entry_member = json::value::array();
                // string entry_uri;
                if(entry_info.as_object().find("Members") != entry_info.as_object().end())
                {
                    entry_member = entry_info.at("Members");
                    for(int j=0; j<entry_member.size(); j++)
                    {
                        json::value log_info;
                        string entry_uri;
                        
                        if(entry_member[j].as_object().find("@odata.id") != entry_member[j].as_object().end())
                        {
                            entry_uri = entry_member[j].at("@odata.id").as_string();
                            cout << "Entry URI : " << entry_uri << endl;
                            log_info = get_json_info(entry_uri, _host, _auth_token);
                            cout << "받아라 LogService Collection 안의 LogService 안의 LogEntry Collection 안의 LogEntry INFO" << endl;
                            cout << log_info << endl;
                        }
                        else
                        {
                            //LogEntry Col에서의 Members에 @odata_id가 없는거야 이건 잘못입력되었거나...
                            // 하튼 안만들면됨 
                        }
                    }
                }
                else
                {
                    // LogEntry Col 에서 Members가 없으면 달려있는 LogEntry가 없는거지 안만들면됨
                }
            }
                

            // 요기에서 Entries 다시 들어가서 한번더 얻어와야함
        }


        
    }
    else
    cout << "System LogServices is X" << endl;
    // cout << "FIRE FIRE  A  7 " << endl;

    
}

void add_chassis(string _bmc_id, string _host, string _auth_token)
{
    string uri = _bmc_id;
    vector<string> uri_tokens = string_split(uri, '/');
    string new_uri;
    for(int i=0; i<uri_tokens.size(); i++)
    {
        if(i == 3)
            continue;
        new_uri += "/";
        new_uri += uri_tokens[i];
    }

    cout << " ADD Chassis " << endl;
    cout << "New Uri : " << new_uri << endl;

    http_client client(_host);

    http_request req(methods::GET);
    req.set_request_uri(new_uri);
    req.headers().add("X-Auth-Token", _auth_token);
    http_response response;

    if(req.headers().has("X-Auth-Token"))
        cout << "Token : " << req.headers()["X-Auth-Token"] << endl;

    try
    {
        /* code */
        pplx::task<http_response> responseTask = client.request(req);
        response = responseTask.get();

    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        cout << "BMC Chassis 정보요청 실패" << endl;
        return ;
    }

    json::value chassis_info;
    chassis_info = response.extract_json().get();

    cout<<"일단 CHASSIS INFO "<<endl;
    cout<<chassis_info<<endl;

    // Chassis *chassis = new Chassis(_bmc_id, "BMC Chassis");

    // Chassis 멤버변수 넣어주기
    if(chassis_info.as_object().find("Id") != chassis_info.as_object().end())
    {
        cout << "Chassis Id is O" << endl;
    }
    else
        cout << "Chassis Id is X" << endl;

    if(chassis_info.as_object().find("ChassisType") != chassis_info.as_object().end())
    {
        cout << "Chassis ChassisType is O" << endl;
    }
    else
        cout << "Chassis ChassisType is X" << endl;


    if(chassis_info.as_object().find("Manufacturer") != chassis_info.as_object().end())
    {
        cout << "Chassis Manufacturer is O" << endl;
    }
    else
        cout << "Chassis Manufacturer is X" << endl;


    if(chassis_info.as_object().find("Model") != chassis_info.as_object().end())
    {
        cout << "Chassis Model is O" << endl;
    }
    else
        cout << "Chassis Model is X" << endl;


    if(chassis_info.as_object().find("SerialNumber") != chassis_info.as_object().end())
    {
        cout << "Chassis SerialNumber is O" << endl;
    }
    else
        cout << "Chassis SerialNumber is X" << endl;


    if(chassis_info.as_object().find("PartNumber") != chassis_info.as_object().end())
    {
        cout << "Chassis PartNumber is O" << endl;
    }
    else
        cout << "Chassis PartNumber is X" << endl;


    if(chassis_info.as_object().find("AssetTag") != chassis_info.as_object().end())
    {
        cout << "Chassis AssetTag is O" << endl;
    }
    else
        cout << "Chassis AssetTag is X" << endl;


    if(chassis_info.as_object().find("PowerState") != chassis_info.as_object().end())
    {
        cout << "Chassis PowerState is O" << endl;
    }
    else
        cout << "Chassis PowerState is X" << endl;


    if(chassis_info.as_object().find("HeightMm") != chassis_info.as_object().end())
    {
        cout << "Chassis HeightMm is O" << endl;
    }
    else
        cout << "Chassis HeightMm is X" << endl;


    if(chassis_info.as_object().find("WidthMm") != chassis_info.as_object().end())
    {
        cout << "Chassis WidthMm is O" << endl;
    }
    else
        cout << "Chassis WidthMm is X" << endl;


    if(chassis_info.as_object().find("DepthMm") != chassis_info.as_object().end())
    {
        cout << "Chassis DepthMm is O" << endl;
    }
    else
        cout << "Chassis DepthMm is X" << endl;


    if(chassis_info.as_object().find("WeightKg") != chassis_info.as_object().end())
    {
        cout << "Chassis WeightKg is O" << endl;
    }
    else
        cout << "Chassis WeightKg is X" << endl;

    if(chassis_info.as_object().find("IndicatorLED") != chassis_info.as_object().end())
    {
        cout << "Chassis IndicatorLED is O" << endl;
    }
    else
        cout << "Chassis IndicatorLED is X" << endl;

    if(chassis_info.as_object().find("Status") != chassis_info.as_object().end())
    {
        cout << "Chassis Status is O" << endl;
    }
    else
        cout << "Chassis Status is X" << endl;

    if(chassis_info.as_object().find("Location") != chassis_info.as_object().end())
    {
        cout << "Chassis Location is O" << endl;
    }
    else
        cout << "Chassis Location is X" << endl;

    


    if(chassis_info.as_object().find("Thermal") != chassis_info.as_object().end())
    {
        json::value thermal_info;
        string inner_uri = chassis_info.at("Thermal").at("@odata.id").as_string();
        cout << "INNER URI : " << inner_uri << endl;
        thermal_info = get_json_info(inner_uri, _host, _auth_token);
        cout << "받아라 Thermal INFO" << endl;
        cout << thermal_info << endl;
    }
    else
        cout << "Thermal is X" << endl;

    if(chassis_info.as_object().find("Power") != chassis_info.as_object().end())
    {
        json::value power_info;
        string inner_uri = chassis_info.at("Power").at("@odata.id").as_string();
        cout << "INNER URI : " << inner_uri << endl;
        power_info = get_json_info(inner_uri, _host, _auth_token);
        cout << "받아라 Power INFO" << endl;
        cout << power_info << endl;
    }
    else
        cout << "Power is X" << endl;

    if(chassis_info.as_object().find("Sensors") != chassis_info.as_object().end())
    {
        json::value sensor_info;
        string inner_uri = chassis_info.at("Sensors").at("@odata.id").as_string();
        cout << "INNER URI : " << inner_uri << endl;
        sensor_info = get_json_info(inner_uri, _host, _auth_token);
        cout << "받아라 Sensors INFO" << endl;
        cout << sensor_info << endl;
    }
    else
        cout << "Sensors is X" << endl;

}


void add_manager(string _bmc_id, string _host, string _auth_token)
{
    string uri = _bmc_id;
    vector<string> uri_tokens = string_split(uri, '/');
    string new_uri;
    for(int i=0; i<uri_tokens.size(); i++)
    {
        if(i == 3)
            continue;
        new_uri += "/";
        new_uri += uri_tokens[i];
    }

    cout << " ADD Manager " << endl;
    cout << "New Uri : " << new_uri << endl;

    http_client client(_host);

    http_request req(methods::GET);
    req.set_request_uri(new_uri);
    req.headers().add("X-Auth-Token", _auth_token);
    http_response response;

    if(req.headers().has("X-Auth-Token"))
        cout << "Token : " << req.headers()["X-Auth-Token"] << endl;

    try
    {
        /* code */
        pplx::task<http_response> responseTask = client.request(req);
        response = responseTask.get();

    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        cout << "BMC Manager 정보요청 실패" << endl;
        return ;
    }

    json::value manager_info;
    manager_info = response.extract_json().get();

    cout<<"일단 MANAGER INFO "<<endl;
    cout<<manager_info<<endl;

    // Manager *manager = new Manager(_bmc_id, "BMC Manager");

    // Manager 멤버변수 넣어주기
    if(manager_info.as_object().find("Id") != manager_info.as_object().end())
    {
        cout << "Manager Id is O" << endl;
    }
    else
        cout << "Manager Id is X" << endl;

    if(manager_info.as_object().find("ManagerType") != manager_info.as_object().end())
    {
        cout << "Manager ManagerType is O" << endl;
    }
    else
        cout << "Manager ManagerType is X" << endl;

    if(manager_info.as_object().find("Description") != manager_info.as_object().end())
    {
        cout << "Manager Description is O" << endl;
    }
    else
        cout << "Manager Description is X" << endl;

    if(manager_info.as_object().find("FirmwareVersion") != manager_info.as_object().end())
    {
        cout << "Manager FirmwareVersion is O" << endl;
    }
    else
        cout << "Manager FirmwareVersion is X" << endl;

    if(manager_info.as_object().find("DateTime") != manager_info.as_object().end())
    {
        cout << "Manager DateTime is O" << endl;
    }
    else
        cout << "Manager DateTime is X" << endl;

    if(manager_info.as_object().find("DateTimeLocalOffset") != manager_info.as_object().end())
    {
        cout << "Manager DateTimeLocalOffset is O" << endl;
    }
    else
        cout << "Manager DateTimeLocalOffset is X" << endl;

    if(manager_info.as_object().find("Model") != manager_info.as_object().end())
    {
        cout << "Manager Model is O" << endl;
    }
    else
        cout << "Manager Model is X" << endl;

    if(manager_info.as_object().find("UUID") != manager_info.as_object().end())
    {
        cout << "Manager UUID is O" << endl;
    }
    else
        cout << "Manager UUID is X" << endl;

    if(manager_info.as_object().find("PowerState") != manager_info.as_object().end())
    {
        cout << "Manager PowerState is O" << endl;
    }
    else
        cout << "Manager PowerState is X" << endl;

    if(manager_info.as_object().find("Status") != manager_info.as_object().end())
    {
        cout << "Manager Status is O" << endl;
    }
    else
        cout << "Manager Status is X" << endl;

    if(manager_info.as_object().find("NetworkProtocol") != manager_info.as_object().end())
    {
        json::value network_info;
        string inner_uri = manager_info.at("NetworkProtocol").at("@odata.id").as_string();
        cout << "INNER URI : " << inner_uri << endl;
        network_info = get_json_info(inner_uri, _host, _auth_token);
        cout << "받아라 NetworkProtocol INFO" << endl;
        cout << network_info << endl;
    }
    else
        cout << "NetworkProtocol is X" << endl;
    
    if(manager_info.as_object().find("EthernetInterfaces") != manager_info.as_object().end())
    {
        json::value ethernet_info;
        string inner_uri = manager_info.at("EthernetInterfaces").at("@odata.id").as_string();
        cout << "INNER URI : " << inner_uri << endl;
        ethernet_info = get_json_info(inner_uri, _host, _auth_token);
        cout << "받아라 EthernetInterfaces INFO" << endl;
        cout << ethernet_info << endl;
    }
    else
        cout << "EthernetInterfaces is X" << endl;

    if(manager_info.as_object().find("LogServices") != manager_info.as_object().end())
    {
        json::value log_info;
        string inner_uri = manager_info.at("LogServices").at("@odata.id").as_string();
        cout << "INNER URI : " << inner_uri << endl;
        log_info = get_json_info(inner_uri, _host, _auth_token);
        cout << "받아라 LogServices INFO" << endl;
        cout << log_info << endl;
    }
    else
        cout << "LogServices is X" << endl;
}


json::value get_json_info(string _uri, string _host, string _auth_token)
{
    http_client client(_host);
    http_request req(methods::GET);
    req.set_request_uri(_uri);
    req.headers().add("X-Auth-Token", _auth_token);
    http_response res;
    json::value info;

    try
    {
        /* code */
        pplx::task<http_response> responseTask = client.request(req);
        res = responseTask.get();

    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        cout << "BMC 정보요청 실패" << endl;
        cout << "fail uri : " << _uri << endl;
        return info;
    }

    // if

    // cout << res.status_code() << endl;

    info = res.extract_json().get();

    return info;

}