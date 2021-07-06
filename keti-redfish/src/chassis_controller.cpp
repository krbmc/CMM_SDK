#include "chassis_controller.hpp"

extern unordered_map<string, Resource *> g_record;
extern map<string, string> module_id_table;


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

    // cout << " 잘 나오나? " << endl;
    // cout << "sys : " << odata_system << endl;
    // cout << "cha : " << odata_chassis << endl;
    // cout << "man : " << odata_manager << endl;
    // cout << "upd : " << odata_update << endl;

    if(module_id_table.find(_bmc_id) != module_id_table.end())
    {
        log(warning) << "이미 존재하는 BMC_id입니다.";
        log(warning) << "입력한 BMC_id : " << _bmc_id;
        log(warning) << "Module_id_table로 검사함";
        return ;
    }

    if(record_is_exist(odata_system) || record_is_exist(odata_chassis) ||
    record_is_exist(odata_manager) || record_is_exist(odata_update))
    {
        log(warning) << "이미 존재하는 BMC_id입니다.";
        log(warning) << "입력한 BMC_id : " << _bmc_id;
        // cout << "이미 존재하는 BMC_id입니다." << endl;
        // cout << "입력한 BMC_id : " << _bmc_id << endl;
        return ;
    }
    // module_id_table들어와서 빼도될듯

    // Login
    string host;
    if(_is_https == true)
        host = "https";
    else
        host = "http";
    host = host + "://" + _ip + ":" + _port;
    cout << "Host : " << host << endl;
    module_id_table.insert({_bmc_id, host});
    log(info) << "BMC_id : " << _bmc_id << " / Address : " << module_id_table[_bmc_id];

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
    // cout << login_response.status_code() << endl;

    auth_token = login_response.headers()["X-Auth-Token"];


    //make system
    add_system(odata_system, host, auth_token);

    //make chassis
    add_chassis(odata_chassis, host, auth_token);

    //make manager
    add_manager(odata_manager, host, auth_token);

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

    // cout << " ADD System " << endl;
    // cout << "New Uri : " << new_uri << endl;


    http_client client(_host);

    http_request req(methods::GET);
    req.set_request_uri(new_uri);
    req.headers().add("X-Auth-Token", _auth_token);
    http_response response;

    // if(req.headers().has("X-Auth-Token"))
    //     cout << "Token : " << req.headers()["X-Auth-Token"] << endl;

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

    // log(warning) << "일단 SYSTEM INFO " << endl;
    // cout << "일단 SYSTEM INFO " << endl;
    // cout <<system_info << endl;


    // Resource 드디어 생성
    Systems *system = new Systems(_bmc_id, "BMC System");

    // System Collection Generate
    system->processor = new Collection(system->odata.id + "/Processors", ODATA_PROCESSOR_COLLECTION_TYPE);
    system->processor->name = "Computer System Processor Collection";

    system->memory = new Collection(system->odata.id + "/Memory", ODATA_MEMORY_COLLECTION_TYPE);
    system->memory->name = "Computer System Memory Collection";

    system->ethernet = new Collection(system->odata.id + "/EthernetInterfaces", ODATA_ETHERNET_INTERFACE_COLLECTION_TYPE);
    system->ethernet->name = "Computer System Ethernet Interface Collection";

    system->log_service = new Collection(system->odata.id + "/LogServices", ODATA_LOG_SERVICE_COLLECTION_TYPE);
    system->log_service->name = "Computer System Log Service Collection";

    system->simple_storage = new Collection(system->odata.id + "/SimpleStorage", ODATA_SIMPLE_STORAGE_COLLECTION_TYPE);
    system->simple_storage->name = "Computer System Simple Storage Collection";
    // 여기에서 한번더 하위로 만드는 리소스가 로그서비스이기 때문에 로그서비스(아직 처리안함) 할때 엔트리컬렉션 만들어주면됨

    system->bios = new Bios(system->odata.id + "/Bios", "Bios");
    // 이게 없어서 광란의 세그먼테이션 폴트 났었음
    // 이런 포인터변수들 잘 만들어주자
    

    // System 멤버변수 넣어주기
    if(system_info.as_object().find("Id") != system_info.as_object().end()) // 존재하면
    {
        // cout << "FIRE FIRE 1" << endl;
        system->id = system_info.at("Id").as_string();
    }
    // else
    // cout << "System Id is X" << endl;
    // >>>> ID는 무시해줘야할듯 

    if(system_info.as_object().find("SystemType") != system_info.as_object().end())
    {
        // cout << "FIRE FIRE 2" << endl;
        system->system_type = system_info.at("SystemType").as_string();
    }
    // else
    // cout << "System SystemType is X" << endl;

    if(system_info.as_object().find("AssetTag") != system_info.as_object().end())
    {
        // cout << "FIRE FIRE 3" << endl;
        system->asset_tag = system_info.at("AssetTag").as_string();
    }
    // else
    // cout << "System AssetTag is X" << endl;

    if(system_info.as_object().find("Manufacturer") != system_info.as_object().end())
    {
        // cout << "FIRE FIRE 4" << endl;
        system->manufacturer = system_info.at("Manufacturer").as_string();
    }
    // else
    // cout << "System Manufacturer is X" << endl;

    if(system_info.as_object().find("Model") != system_info.as_object().end())
    {
        // cout << "FIRE FIRE 5" << endl;
        system->model = system_info.at("Model").as_string();
    }
    // else
    // cout << "System Model is X" << endl;

    if(system_info.as_object().find("SerialNumber") != system_info.as_object().end())
    {
        // cout << "FIRE FIRE 6" << endl;
        system->serial_number = system_info.at("SerialNumber").as_string();
    }
    // else
    // cout << "System SerialNumber is X" << endl;

    if(system_info.as_object().find("PartNumber") != system_info.as_object().end())
    {
        // cout << "FIRE FIRE 7" << endl;
        system->part_number = system_info.at("PartNumber").as_string();
    }
    // else
    // cout << "System PartNumber is X" << endl;

    if(system_info.as_object().find("Description") != system_info.as_object().end())
    {
        // cout << "FIRE FIRE 8" << endl;
        system->description = system_info.at("Description").as_string();
    }
    // else
    // cout << "System Description is X" << endl;

    if(system_info.as_object().find("UUID") != system_info.as_object().end())
    {
        // cout << "FIRE FIRE 9" << endl;
        system->uuid = system_info.at("UUID").as_string();
    }
    // else
    // cout << "System UUID is X" << endl;

    if(system_info.as_object().find("HostName") != system_info.as_object().end())
    {
        // cout << "FIRE FIRE 10" << endl;
        system->hostname = system_info.at("HostName").as_string();
    }
    // else
    // cout << "System HostName is X" << endl;

    if(system_info.as_object().find("PowerState") != system_info.as_object().end())
    {
        // cout << "FIRE FIRE 11" << endl;
        system->power_state = system_info.at("PowerState").as_string();
    }
    // else
    // cout << "System PowerState is X" << endl;

    if(system_info.as_object().find("BiosVersion") != system_info.as_object().end())
    {
        // cout << "FIRE FIRE 12" << endl;
        system->bios_version = system_info.at("BiosVersion").as_string();
    }
    // else
    // cout << "System BiosVersion is X" << endl;



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
    // else
    // cout << "System Status is X" << endl;
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
    // else
    // cout << "System IndicatorLED is X" << endl;

    if(system_info.as_object().find("Boot") != system_info.as_object().end())
    {
        json::value j = json::value::object();
        j = system_info.at("Boot");

        if(j.as_object().find("BootSourceOverrideEnabled") != j.as_object().end())
            system->boot.boot_source_override_enabled = j.at("BootSourceOverrideEnabled").as_string();
        if(j.as_object().find("BootSourceOverrideTarget") != j.as_object().end())
            system->boot.boot_source_override_target = j.at("BootSourceOverrideTarget").as_string();
        if(j.as_object().find("BootSourceOverrideMode") != j.as_object().end())
            system->boot.boot_source_override_mode = j.at("BootSourceOverrideMode").as_string();
        if(j.as_object().find("UefiTargetBootSourceOverride") != j.as_object().end())
            system->boot.uefi_target_boot_source_override = j.at("UefiTargetBootSourceOverride").as_string();
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

            // 여기에서 system->bios = new Bios ~~~ 해줘야하는데 현재 json에 오는 데이터가 없어서 현재 위에 별도로
            // 만들었음 (포인터변수라 터짐)
        }
        else
        {
            // 뭐 Bios 안에 @odata.id가 없으면 없는거지 뭐 안만들면됨
            cout << "Bios 없음" << endl;
        }

        // 얘는 멤버아니고 바로 정보 사용하면 되네
        
    }
    // else
    // cout << "System Bios is X" << endl;

    // cout << "FIRE FIRE  A  2 " << endl;

    if(system_info.as_object().find("Processors") != system_info.as_object().end())
    {
        // cout << "Processors Exist ~~" << endl;
        json::value processor_col_info;
        string inner_uri;
        if(system_info.at("Processors").as_object().find("@odata.id") != system_info.at("Processors").as_object().end())
        {
            // get json Processor collection
            inner_uri = system_info.at("Processors").at("@odata.id").as_string();
            // cout << "INNER URI : " << inner_uri << endl;
            processor_col_info = get_json_info(inner_uri, _host, _auth_token);
            // cout << "받아라 Processor Collection INFO" << endl;
            // cout << processor_col_info << endl;

            // Processor collection 안의 Members
            json::value array_member = json::value::array();
            if(processor_col_info.as_object().find("Members") != processor_col_info.as_object().end())
            {
                array_member = processor_col_info.at("Members");
                for(int i=0; i<array_member.size(); i++)
                {
                    json::value processor_info;
                    string member_uri;
                    if(array_member[i].as_object().find("@odata.id") != array_member[i].as_object().end())
                    {
                        member_uri = array_member[i].at("@odata.id").as_string();
                        // cout << "MEMBER URI : " << member_uri << endl;
                        processor_info = get_json_info(member_uri, _host, _auth_token);
                        // cout << "받아라 Processor Collection 안의 Processor INFO" << endl;
                        // cout << processor_info << endl;

                        // 프로세서 만들기
                        // Processors *pro = new Processors(member_uri);
                        vector<string> uri_tokens = string_split(member_uri, '/');
                        string my_odata = _bmc_id + "/Processors/" + uri_tokens[uri_tokens.size()-1];
                        Processors *pro = new Processors(my_odata);

                        if(processor_info.as_object().find("Id") != processor_info.as_object().end())
                            pro->id = processor_info.at("Id").as_string();

                        if(processor_info.as_object().find("Socket") != processor_info.as_object().end())
                            pro->socket = processor_info.at("Socket").as_string();

                        if(processor_info.as_object().find("ProcessorType") != processor_info.as_object().end())
                            pro->processor_type = processor_info.at("ProcessorType").as_string();

                        if(processor_info.as_object().find("ProcessorArchitecture") != processor_info.as_object().end())
                            pro->processor_architecture = processor_info.at("ProcessorArchitecture").as_string();

                        if(processor_info.as_object().find("InstructionSet") != processor_info.as_object().end())
                            pro->instruction_set = processor_info.at("InstructionSet").as_string();

                        if(processor_info.as_object().find("Manufacturer") != processor_info.as_object().end())
                            pro->manufacturer = processor_info.at("Manufacturer").as_string();

                        if(processor_info.as_object().find("Model") != processor_info.as_object().end())
                            pro->model = processor_info.at("Model").as_string();

                        if(processor_info.as_object().find("MaxSpeedMHz") != processor_info.as_object().end())
                            pro->max_speed_mhz = processor_info.at("MaxSpeedMHz").as_integer();

                        if(processor_info.as_object().find("TotalCores") != processor_info.as_object().end())
                            pro->total_cores = processor_info.at("TotalCores").as_integer();

                        if(processor_info.as_object().find("TotalThreads") != processor_info.as_object().end())
                            pro->total_threads = processor_info.at("TotalThreads").as_integer();

                        if(processor_info.as_object().find("Status") != processor_info.as_object().end())
                        {
                            json::value j = json::value::object();
                            j = processor_info.at("Status");

                            if(j.as_object().find("State") != j.as_object().end())
                                pro->status.state = j.at("State").as_string();

                            if(j.as_object().find("Health") != j.as_object().end())
                                pro->status.health = j.at("Health").as_string();
                        }

                        if(processor_info.as_object().find("ProcessorId") != processor_info.as_object().end())
                        {
                            json::value j = json::value::object();
                            j = processor_info.at("ProcessorId");

                            if(j.as_object().find("VendorId") != j.as_object().end())
                                pro->p_id.vendor_id = j.at("VendorId").as_string();
                            
                            if(j.as_object().find("IdentificationRegisters") != j.as_object().end())
                                pro->p_id.identification_registers = j.at("IdentificationRegisters").as_string();

                            if(j.as_object().find("EffectiveFamily") != j.as_object().end())
                                pro->p_id.effective_family = j.at("EffectiveFamily").as_string();

                            if(j.as_object().find("EffectiveModel") != j.as_object().end())
                                pro->p_id.effective_model = j.at("EffectiveModel").as_string();

                            if(j.as_object().find("Step") != j.as_object().end())
                                pro->p_id.step = j.at("Step").as_string();

                            if(j.as_object().find("MicrocodeInfo") != j.as_object().end())
                                pro->p_id.microcode_info = j.at("MicrocodeInfo").as_string();
                        }

                        system->processor->add_member(pro);
                        // Processor 연결!
                    }
                    else
                    {
                        // 위에 들어가면 processor 하나하나씩 정보 
                        // Processor 하나씩 만들어서 collection에 add_member
                    }
                }
            }
            else
            {
                // 위에 들어가면 processor들 있는거니깐 그거에 맞게 만들고 넣어주면됨
            }
        }
        else
        {
            // 위에 if에 들어가서 있는거면 System안에 Processor Collection은 있는거임
            // System 안에 Processor collection 만들면되고
        }
        
    }
    // else
    // cout << "System Processors is X" << endl;
    // cout << "FIRE FIRE  A  3 " << endl;

    if(system_info.as_object().find("Memory") != system_info.as_object().end())
    {
        // cout << "Memory Exist ~~" << endl;
        json::value memory_col_info;
        string inner_uri;
        if(system_info.at("Memory").as_object().find("@odata.id") != system_info.at("Memory").as_object().end())
        {
            inner_uri = system_info.at("Memory").at("@odata.id").as_string();
            // cout << "INNER URI : " << inner_uri << endl;
            memory_col_info = get_json_info(inner_uri, _host, _auth_token);
            // cout << "받아라 Memory Collection INFO" << endl;
            // cout << memory_col_info << endl;

            json::value array_member = json::value::array();
            if(memory_col_info.as_object().find("Members") != memory_col_info.as_object().end())
            {
                array_member = memory_col_info.at("Members");

                for(int i=0; i<array_member.size(); i++)
                {
                    json::value memory_info;
                    string member_uri;
                    if(array_member[i].as_object().find("@odata.id") != array_member[i].as_object().end())
                    {
                        member_uri = array_member[i].at("@odata.id").as_string();
                        // cout << "MEMBER URI : " << member_uri << endl;
                        memory_info = get_json_info(member_uri, _host, _auth_token);
                        // cout << "받아라 Memory Collection 안의 Memory INFO" << endl;
                        // cout << memory_info << endl;

                        // Memory 만들기
                        vector<string> uri_tokens = string_split(member_uri, '/');
                        string my_odata = _bmc_id + "/Memory/" + uri_tokens[uri_tokens.size()-1];
                        Memory *mem = new Memory(my_odata);

                        if(memory_info.as_object().find("Id") != memory_info.as_object().end())
                            mem->id = memory_info.at("Id").as_string();

                        if(memory_info.as_object().find("CapacityKiB") != memory_info.as_object().end())
                            mem->capacity_kib = memory_info.at("CapacityKiB").as_integer();

                        if(memory_info.as_object().find("Status") != memory_info.as_object().end())
                        {
                            json::value j = json::value::object();
                            j = memory_info.at("Status");
                            
                            if(j.as_object().find("State") != j.as_object().end())
                                mem->status.state = j.at("State").as_string();
                            if(j.as_object().find("Health") != j.as_object().end())
                                mem->status.health = j.at("Health").as_string();
                        }

                        system->memory->add_member(mem);
                            
                    }
                    else
                    {
                        // Memory만들어서 memory collection 에 add_member
                    }
                }
            }
            else
            {
                // 위에 들어가면 Memory들 있는거임 더 들어가는거없어서 바로 만들고 넣어주면됨
            }
        }
        else
        {
            // 위에 들어가면 Memory Collection이 있는거임
            // System안에 Memory collection 만들고
        }


    }
    // else
    // cout << "System Memory is X" << endl;
    // cout << "FIRE FIRE  A  4 " << endl;

    if(system_info.as_object().find("EthernetInterfaces") != system_info.as_object().end())
    {
        // cout << "EthernetInterfaces Exist ~~" << endl;
        json::value ethernet_col_info;
        string inner_uri;
        if(system_info.at("EthernetInterfaces").as_object().find("@odata.id") != system_info.at("EthernetInterfaces").as_object().end())
        {
            inner_uri = system_info.at("EthernetInterfaces").at("@odata.id").as_string();
            // cout << "INNER URI : " << inner_uri << endl;
            ethernet_col_info = get_json_info(inner_uri, _host, _auth_token);
            // cout << "받아라 EthernetInterfaces Collection INFO" << endl;
            // cout << ethernet_col_info << endl;

            json::value array_member = json::value::array();
            if(ethernet_col_info.as_object().find("Members") != ethernet_col_info.as_object().end())
            {
                array_member = ethernet_col_info.at("Members");
                for(int i=0; i<array_member.size(); i++)
                {
                    json::value ethernet_info;
                    string member_uri;
                    if(array_member[i].as_object().find("@odata.id") != array_member[i].as_object().end())
                    {
                        member_uri = array_member[i].at("@odata.id").as_string();
                        // cout << "MEMBER URI : " << member_uri << endl;
                        ethernet_info = get_json_info(member_uri, _host, _auth_token);
                        // cout << "받아라 EthernetInterfaces Collection 안의 EthernetInterfaces INFO" << endl;
                        // cout << ethernet_info << endl;

                        // EthernetInterfaces 만들기
                        
                        vector<string> uri_tokens = string_split(member_uri, '/');
                        string my_odata = _bmc_id + "/EthernetInterfaces/" + uri_tokens[uri_tokens.size()-1];
                        EthernetInterfaces *eth = new EthernetInterfaces(my_odata);

                        
                        if(ethernet_info.as_object().find("Id") != ethernet_info.as_object().end())
                            eth->id = ethernet_info.at("Id").as_string();

                        if(ethernet_info.as_object().find("Description") != ethernet_info.as_object().end())
                            eth->description = ethernet_info.at("Description").as_string();

                        if(ethernet_info.as_object().find("LinkStatus") != ethernet_info.as_object().end())
                            eth->link_status = ethernet_info.at("LinkStatus").as_string();

                        if(ethernet_info.as_object().find("MACAddress") != ethernet_info.as_object().end())
                            eth->mac_address = ethernet_info.at("MACAddress").as_string();

                        if(ethernet_info.as_object().find("MTUSize") != ethernet_info.as_object().end())
                            eth->mtu_size = ethernet_info.at("MTUSize").as_integer();

                        if(ethernet_info.as_object().find("HostName") != ethernet_info.as_object().end())
                            eth->hostname = ethernet_info.at("HostName").as_string();

                        if(ethernet_info.as_object().find("FQDN") != ethernet_info.as_object().end())
                            eth->fqdn = ethernet_info.at("FQDN").as_string();

                        if(ethernet_info.as_object().find("IPv4Addresses") != ethernet_info.as_object().end())
                        {
                            json::value ip_array = json::value::array();
                            ip_array = ethernet_info.at("IPv4Addresses");
                            for(int ip_num=0; ip_num<ip_array.size(); ip_num++)
                            {
                                IPv4_Address tmp;
                                if(ip_array[ip_num].as_object().find("Address") != ip_array[ip_num].as_object().end())
                                    tmp.address = ip_array[ip_num].at("Address").as_string();
                                
                                if(ip_array[ip_num].as_object().find("AddressOrigin") != ip_array[ip_num].as_object().end())
                                    tmp.address_origin = ip_array[ip_num].at("AddressOrigin").as_string();

                                if(ip_array[ip_num].as_object().find("SubnetMask") != ip_array[ip_num].as_object().end())
                                    tmp.subnet_mask = ip_array[ip_num].at("SubnetMask").as_string();

                                if(ip_array[ip_num].as_object().find("Gateway") != ip_array[ip_num].as_object().end())
                                    tmp.gateway = ip_array[ip_num].at("Gateway").as_string();

                                eth->v_ipv4.push_back(tmp);
                            }
                        }

                        if(ethernet_info.as_object().find("IPv6Addresses") != ethernet_info.as_object().end())
                        {
                            json::value ip_array = json::value::array();
                            ip_array = ethernet_info.at("IPv6Addresses");
                            for(int ip_num=0; ip_num<ip_array.size(); ip_num++)
                            {
                                IPv6_Address tmp;
                                if(ip_array[ip_num].as_object().find("Address") != ip_array[ip_num].as_object().end())
                                    tmp.address = ip_array[ip_num].at("Address").as_string();
                                
                                if(ip_array[ip_num].as_object().find("AddressOrigin") != ip_array[ip_num].as_object().end())
                                    tmp.address_origin = ip_array[ip_num].at("AddressOrigin").as_string();

                                if(ip_array[ip_num].as_object().find("AddressState") != ip_array[ip_num].as_object().end())
                                    tmp.address_state = ip_array[ip_num].at("AddressState").as_string();

                                if(ip_array[ip_num].as_object().find("PrefixLength") != ip_array[ip_num].as_object().end())
                                    tmp.prefix_length = ip_array[ip_num].at("PrefixLength").as_integer();

                                eth->v_ipv6.push_back(tmp);
                            }
                        }

                        system->ethernet->add_member(eth);
                    }
                    else
                    {
                        // ethernetinterface 만들어서 collection에 add_member
                    }
                }
            }
        }
        else
        {
            // 위에 들어가면 ethernet collection 있는거임
            // System안에 ehternetinterface collection 만들고
        }
    }
    // else
    // cout << "System EthernetInterfaces is X" << endl;
    // cout << "FIRE FIRE  A  5 " << endl;

    if(system_info.as_object().find("SimpleStorage") != system_info.as_object().end())
    {
        // cout << "SimpleStorage Exist ~~" << endl;
        json::value simple_col_info;
        string inner_uri;
        if(system_info.at("SimpleStorage").as_object().find("@odata.id") != system_info.at("SimpleStorage").as_object().end())
        {
            inner_uri = system_info.at("SimpleStorage").at("@odata.id").as_string();
            // cout << "INNER URI : " << inner_uri << endl;
            simple_col_info = get_json_info(inner_uri, _host, _auth_token);
            // cout << "받아라 SimpleStorage Collection INFO" << endl;
            // cout << simple_col_info << endl;

            json::value array_member = json::value::array();
            if(simple_col_info.as_object().find("Members") != simple_col_info.as_object().end())
            {
                array_member = simple_col_info.at("Members");

                for(int i=0; i<array_member.size(); i++)
                {
                    json::value simple_info;
                    string member_uri;
                    if(array_member[i].as_object().find("@odata.id") != array_member[i].as_object().end())
                    {
                        member_uri = array_member[i].at("@odata.id").as_string();
                        // cout << "MEMBER URI : " << member_uri << endl;
                        simple_info = get_json_info(member_uri, _host, _auth_token);
                        // cout << "받아라 SimpleStorage Collection 안의 SimpleStorage INFO" << endl;
                        // cout << simple_info << endl;

                        // Simplestorage 만들기

                        vector<string> uri_tokens = string_split(member_uri, '/');
                        string my_odata = _bmc_id + "/SimpleStorage/" + uri_tokens[uri_tokens.size()-1];
                        SimpleStorage *sim = new SimpleStorage(my_odata);

                        if(simple_info.as_object().find("Id") != simple_info.as_object().end())
                            sim->id = simple_info.at("Id").as_string();

                        if(simple_info.as_object().find("Description") != simple_info.as_object().end())
                            sim->description = simple_info.at("Description").as_string();

                        if(simple_info.as_object().find("UefiDevicePath") != simple_info.as_object().end())
                            sim->uefi_device_path = simple_info.at("UefiDevicePath").as_string();

                        if(simple_info.as_object().find("Status") != simple_info.as_object().end())
                        {
                            json::value j = json::value::object();
                            j = simple_info.at("Status");

                            if(j.as_object().find("State") != j.as_object().end())
                                sim->status.state = j.at("State").as_string();
                            if(j.as_object().find("Health") != j.as_object().end())
                                sim->status.health = j.at("Health").as_string();
                        }

                        if(simple_info.as_object().find("Devices") != simple_info.as_object().end())
                        {
                            json::value device_array = json::value::array();
                            device_array = simple_info.at("Devices");

                            for(int dev_num=0; dev_num<device_array.size(); dev_num++)
                            {
                                Device_Info tmp;

                                if(device_array[dev_num].as_object().find("Name") != device_array[dev_num].as_object().end())
                                    tmp.name = device_array[dev_num].at("Name").as_string();
                                
                                if(device_array[dev_num].as_object().find("Manufacturer") != device_array[dev_num].as_object().end())
                                    tmp.manufacturer = device_array[dev_num].at("Manufacturer").as_string();
                                
                                if(device_array[dev_num].as_object().find("Model") != device_array[dev_num].as_object().end())
                                    tmp.model = device_array[dev_num].at("Model").as_string();
                                
                                if(device_array[dev_num].as_object().find("CapacityKBytes") != device_array[dev_num].as_object().end())
                                    tmp.capacity_KBytes = device_array[dev_num].at("CapacityKBytes").as_integer();
                                
                                if(device_array[dev_num].as_object().find("FileSystem") != device_array[dev_num].as_object().end())
                                    tmp.file_system = device_array[dev_num].at("FileSystem").as_string();

                                if(device_array[dev_num].as_object().find("Status") != device_array[dev_num].as_object().end())
                                {
                                    json::value j = json::value::object();
                                    j = device_array[dev_num].at("Status");

                                    if(j.as_object().find("State") != j.as_object().end())
                                        tmp.status.state = j.at("State").as_string();
                                    if(j.as_object().find("Health") != j.as_object().end())
                                        tmp.status.health = j.at("Health").as_string();
                                }

                                sim->devices.push_back(tmp);
                            }
                        }

                        system->simple_storage->add_member(sim);
                        
                    }

                }
            }
        }
        else
        {
            // SimpleStorage Collection 만들고
        }
    }
    // else
    // cout << "System SimpleStorage is X" << endl;
    // cout << "FIRE FIRE  A  6 " << endl;

    // LogService
    if(system_info.as_object().find("LogServices") != system_info.as_object().end())
    {
        // cout << "LogServices Exist ~~" << endl;
        json::value service_col_info;
        string inner_uri;
        if(system_info.at("LogServices").as_object().find("@odata.id") != system_info.at("LogServices").as_object().end())
        {
            inner_uri = system_info.at("LogServices").at("@odata.id").as_string();
            // cout << "INNER URI : " << inner_uri << endl;
            service_col_info = get_json_info(inner_uri, _host, _auth_token);
            // cout << "받아라 LogService Collection INFO" << endl;
            // cout << service_col_info << endl;

            json::value array_member = json::value::array();
            if(service_col_info.as_object().find("Members") != service_col_info.as_object().end())
                array_member = service_col_info.at("Members");

            for(int i=0; i<array_member.size(); i++)
            {
                json::value service_info;
                string member_uri;
                if(array_member[i].as_object().find("@odata.id") != array_member[i].as_object().end())
                {
                    member_uri = array_member[i].at("@odata.id").as_string();
                    // cout << "MEMBER URI : " << member_uri << endl;
                    service_info = get_json_info(member_uri, _host, _auth_token);
                    // cout << "받아라 LogService Collection 안의 LogSerivce INFO" << endl;
                    // cout << service_info << endl;

                    json::value entry_col_info;
                    string entry_col_uri;
                    if(service_info.as_object().find("Entries") != service_info.as_object().end())
                    {
                        if(service_info.at("Entries").as_object().find("@odata.id") != service_info.at("Entries").as_object().end())
                        {
                            entry_col_uri = service_info.at("Entries").at("@odata.id").as_string();
                            json::value entry_info;
                            // cout << "Entry Col URI : " << entry_col_uri << endl;

                            entry_info = get_json_info(entry_col_uri, _host, _auth_token);
                            // cout << "받아라 LogService Collection 안의 LogService 안의 LogEntry Collection INFO" << endl;
                            // cout << entry_info << endl;

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
                                        // cout << "Entry URI : " << entry_uri << endl;
                                        log_info = get_json_info(entry_uri, _host, _auth_token);
                                        // cout << "받아라 LogService Collection 안의 LogService 안의 LogEntry Collection 안의 LogEntry INFO" << endl;
                                        // cout << log_info << endl;
                                    }
                                    else
                                    {
                                        //LogEntry Col에서의 Members에 @odata_id가 없는거야 이건 잘못입력되었거나...
                                        // 하튼 안만들면됨 
                                        // LogEntry만들어서 entry col에 add하면 됨
                                    }
                                }
                            }
                        }
                        else
                        {
                            // LogService 안에 Entry collection 만들고
                        }
                    }
                }
                else
                {
                    // LogService collection 안에 Logservice 만들고 add_member
                }
            }
        }
        else
        {
            // System안에 LogService Collection 만들고
        }


        
    }
    // else
    // cout << "System LogServices is X" << endl;
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

    // cout << " ADD Chassis " << endl;
    // cout << "New Uri : " << new_uri << endl;

    http_client client(_host);

    http_request req(methods::GET);
    req.set_request_uri(new_uri);
    req.headers().add("X-Auth-Token", _auth_token);
    http_response response;

    // if(req.headers().has("X-Auth-Token"))
    //     cout << "Token : " << req.headers()["X-Auth-Token"] << endl;

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

    // cout<<"일단 CHASSIS INFO "<<endl;
    // cout<<chassis_info<<endl;

    // Chassis Resource 생성
    Chassis *chassis = new Chassis(_bmc_id, "BMC Chassis");

    // Chassis Thermal, Power, Sensor collection
    chassis->thermal = new Thermal(chassis->odata.id + "/Thermal");
    chassis->thermal->name = "BMC Chassis Thermal";
    chassis->thermal->temperatures = new List(chassis->thermal->odata.id + "/Temperatures", TEMPERATURE_TYPE);
    chassis->thermal->temperatures->name = "Chassis Temperatures";
    chassis->thermal->fans = new List(chassis->thermal->odata.id + "/Fans", FAN_TYPE);
    chassis->thermal->fans->name = "Chassis Fans";

    chassis->power = new Power(chassis->odata.id + "/Power");
    chassis->power->name = "BMC Chassis Power";
    chassis->power->power_control = new List(chassis->power->odata.id + "/PowerControl", POWER_CONTROL_TYPE);
    chassis->power->power_control->name = "Chassis PowerControl";
    chassis->power->voltages = new List(chassis->power->odata.id + "/Voltages", VOLTAGE_TYPE);
    chassis->power->voltages->name = "Chassis Voltages";
    chassis->power->power_supplies = new List(chassis->power->odata.id + "/PowerSupplies", POWER_SUPPLY_TYPE);
    chassis->power->power_supplies->name = "Chassis PowerSupplies";

    chassis->sensors = new Collection(chassis->odata.id + "/Sensors", ODATA_SENSOR_COLLECTION_TYPE);
    chassis->sensors->name = "Chassis Sensor Collection";
    // 센서 파워는 나중에 쓰임


    // Chassis 멤버변수 넣어주기
    if(chassis_info.as_object().find("Id") != chassis_info.as_object().end())
    {
        // cout << "Chassis Id is O" << endl;
        chassis->id = chassis_info.at("Id").as_string();
    }
    // else
        // cout << "Chassis Id is X" << endl;

    if(chassis_info.as_object().find("ChassisType") != chassis_info.as_object().end())
    {
        // cout << "Chassis ChassisType is O" << endl;
        chassis->chassis_type = chassis_info.at("ChassisType").as_string();
    }
    // else
    //     cout << "Chassis ChassisType is X" << endl;


    if(chassis_info.as_object().find("Manufacturer") != chassis_info.as_object().end())
    {
        // cout << "Chassis Manufacturer is O" << endl;
        chassis->manufacturer = chassis_info.at("Manufacturer").as_string();
    }
    // else
        // cout << "Chassis Manufacturer is X" << endl;


    if(chassis_info.as_object().find("Model") != chassis_info.as_object().end())
    {
        // cout << "Chassis Model is O" << endl;
        chassis->model = chassis_info.at("Model").as_string();
    }
    // else
        // cout << "Chassis Model is X" << endl;


    if(chassis_info.as_object().find("SerialNumber") != chassis_info.as_object().end())
    {
        // cout << "Chassis SerialNumber is O" << endl;
        chassis->serial_number = chassis_info.at("SerialNumber").as_string();
    }
    // else
        // cout << "Chassis SerialNumber is X" << endl;


    if(chassis_info.as_object().find("PartNumber") != chassis_info.as_object().end())
    {
        // cout << "Chassis PartNumber is O" << endl;
        chassis->part_number = chassis_info.at("PartNumber").as_string();
    }
    // else
        // cout << "Chassis PartNumber is X" << endl;


    if(chassis_info.as_object().find("AssetTag") != chassis_info.as_object().end())
    {
        // cout << "Chassis AssetTag is O" << endl;
        chassis->asset_tag = chassis_info.at("AssetTag").as_string();
    }
    // else
        // cout << "Chassis AssetTag is X" << endl;


    if(chassis_info.as_object().find("PowerState") != chassis_info.as_object().end())
    {
        // cout << "Chassis PowerState is O" << endl;
        chassis->power_state = chassis_info.at("PowerState").as_string();
    }
    // else
        // cout << "Chassis PowerState is X" << endl;


    // if(chassis_info.as_object().find("HeightMm") != chassis_info.as_object().end())
    // {
    //     cout << "Chassis HeightMm is O" << endl;
    //     chassis->height_mm = chassis_info.at("HeightMm").as_double();
    // }
    // else
    //     cout << "Chassis HeightMm is X" << endl;


    // if(chassis_info.as_object().find("WidthMm") != chassis_info.as_object().end())
    // {
    //     cout << "Chassis WidthMm is O" << endl;
    //     chassis->width_mm = chassis_info.at("WidthMm").as_double();
    // }
    // else
    //     cout << "Chassis WidthMm is X" << endl;


    // if(chassis_info.as_object().find("DepthMm") != chassis_info.as_object().end())
    // {
    //     cout << "Chassis DepthMm is O" << endl;
    //     chassis->depth_mm = chassis_info.at("DepthMm").as_double();
    // }
    // else
    //     cout << "Chassis DepthMm is X" << endl;


    // if(chassis_info.as_object().find("WeightKg") != chassis_info.as_object().end())
    // {
    //     cout << "Chassis WeightKg is O" << endl;
    //     chassis->weight_kg = chassis_info.at("WeightKg").as_double();
    // }
    // else
    //     cout << "Chassis WeightKg is X" << endl;

    if(chassis_info.as_object().find("IndicatorLED") != chassis_info.as_object().end())
    {
        // cout << "Chassis IndicatorLED is O" << endl;
        string led;
        led = chassis_info.at("IndicatorLED").as_string();

        if(led == "Off")
            chassis->indicator_led = LED_OFF;
        else if(led == "Blinking")
            chassis->indicator_led = LED_BLINKING;
        else if(led == "Lit")
            chassis->indicator_led = LED_LIT;
    }
    // else
        // cout << "Chassis IndicatorLED is X" << endl;

    if(chassis_info.as_object().find("Status") != chassis_info.as_object().end())
    {
        // cout << "Chassis Status is O" << endl;
        json::value j = json::value::object();
        j = chassis_info.at("Status");

        if(j.as_object().find("State") != j.as_object().end())
            chassis->status.state = j.at("State").as_string();
        if(j.as_object().find("Health") != j.as_object().end())
            chassis->status.health = j.at("Health").as_string();
    }
    // else
        // cout << "Chassis Status is X" << endl;

    if(chassis_info.as_object().find("Location") != chassis_info.as_object().end())
    {
        // cout << "Chassis Location is O" << endl;
        json::value j = json::value::object();
        j = chassis_info.at("Location");

        if(j.as_object().find("PostalAddress") != j.as_object().end())
        {
            json::value k = json::value::object();
            k = j.at("PostalAddress");

            if(k.as_object().find("Country") != k.as_object().end())
                chassis->location.postal_address.country = k.at("Country").as_string();
            if(k.as_object().find("Territory") != k.as_object().end())
                chassis->location.postal_address.territory = k.at("Territory").as_string();
            if(k.as_object().find("City") != k.as_object().end())
                chassis->location.postal_address.city = k.at("City").as_string();
            if(k.as_object().find("Street") != k.as_object().end())
                chassis->location.postal_address.street = k.at("Street").as_string();
            if(k.as_object().find("HouseNumber") != k.as_object().end())
                chassis->location.postal_address.house_number = k.at("HouseNumber").as_string();
            if(k.as_object().find("Name") != k.as_object().end())
                chassis->location.postal_address.name = k.at("Name").as_string();
            if(k.as_object().find("PostalCode") != k.as_object().end())
                chassis->location.postal_address.postal_code = k.at("PostalCode").as_string();

        }

        if(j.as_object().find("Placement") != j.as_object().end())
        {
            json::value k = json::value::object();
            k = j.at("Placement");

            if(k.as_object().find("Row") != k.as_object().end())
                chassis->location.placement.row = k.at("Row").as_string();
            if(k.as_object().find("Rack") != k.as_object().end())
                chassis->location.placement.rack = k.at("Rack").as_string();
            if(k.as_object().find("RackOffsetUnits") != k.as_object().end())
                chassis->location.placement.rack_offset_units = k.at("RackOffsetUnits").as_string();
            if(k.as_object().find("RackOffset") != k.as_object().end())
                chassis->location.placement.rack_offset = k.at("RackOffset").as_integer();
        }
    }
    // else
        // cout << "Chassis Location is X" << endl;

    


    if(chassis_info.as_object().find("Thermal") != chassis_info.as_object().end())
    {
        json::value thermal_info;
        string inner_uri;
        if(chassis_info.at("Thermal").as_object().find("@odata.id") != chassis_info.at("Thermal").as_object().end())
        {
            inner_uri = chassis_info.at("Thermal").at("@odata.id").as_string();
            // cout << "INNER URI : " << inner_uri << endl;
            thermal_info = get_json_info(inner_uri, _host, _auth_token);
            // cout << "받아라 Thermal INFO" << endl;
            // cout << thermal_info << endl;

            if(thermal_info.as_object().find("Id") != thermal_info.as_object().end())
                chassis->thermal->id = thermal_info.at("Id").as_string();
            
            json::value fan_array = json::value::array();
            if(thermal_info.as_object().find("Fans") != thermal_info.as_object().end())
            {
                fan_array = thermal_info.at("Fans");
                for(int fan_num=0; fan_num<fan_array.size(); fan_num++)
                {
                    // Fan 만들기
                    string fan_odata;
                    if(fan_array[fan_num].as_object().find("@odata.id") != fan_array[fan_num].as_object().end())
                        fan_odata = fan_array[fan_num].at("@odata.id").as_string();
                    else
                    {
                        // else면 팬 정보에 odata.id 가 없다는 거라 이건 Fan리소스를 생성해줄수가없어서 컨티뉴하겠음
                        continue;
                    }

                    vector<string> uri_tokens = string_split(fan_odata, '/');
                    string my_odata = _bmc_id + "/Thermal/Fans/" + uri_tokens[uri_tokens.size()-1];
                    Fan *fan = new Fan(my_odata);

                    if(fan_array[fan_num].as_object().find("Name") != fan_array[fan_num].as_object().end())
                        fan->name = fan_array[fan_num].at("Name").as_string();
                    
                    if(fan_array[fan_num].as_object().find("MemberId") != fan_array[fan_num].as_object().end())
                        fan->member_id = fan_array[fan_num].at("MemberId").as_string();
                    
                    if(fan_array[fan_num].as_object().find("SensorNumber") != fan_array[fan_num].as_object().end())
                        fan->sensor_num = fan_array[fan_num].at("SensorNumber").as_integer();
                    
                    if(fan_array[fan_num].as_object().find("Reading") != fan_array[fan_num].as_object().end())
                        fan->reading = fan_array[fan_num].at("Reading").as_integer();

                    if(fan_array[fan_num].as_object().find("ReadingUnits") != fan_array[fan_num].as_object().end())
                        fan->reading_units = fan_array[fan_num].at("ReadingUnits").as_string();

                    if(fan_array[fan_num].as_object().find("UpperThresholdNonCritical") != fan_array[fan_num].as_object().end())
                        fan->upper_threshold_non_critical = fan_array[fan_num].at("UpperThresholdNonCritical").as_integer();
                    
                    if(fan_array[fan_num].as_object().find("UpperThresholdCritical") != fan_array[fan_num].as_object().end())
                        fan->upper_threshold_critical = fan_array[fan_num].at("UpperThresholdCritical").as_integer();

                    if(fan_array[fan_num].as_object().find("UpperThresholdFatal") != fan_array[fan_num].as_object().end())
                        fan->upper_threshold_fatal = fan_array[fan_num].at("UpperThresholdFatal").as_integer();

                    if(fan_array[fan_num].as_object().find("LowerThresholdNonCritical") != fan_array[fan_num].as_object().end())
                        fan->lower_threshold_non_critical = fan_array[fan_num].at("LowerThresholdNonCritical").as_integer();
                    
                    if(fan_array[fan_num].as_object().find("LowerThresholdCritical") != fan_array[fan_num].as_object().end())
                        fan->lower_threshold_critical = fan_array[fan_num].at("LowerThresholdCritical").as_integer();

                    if(fan_array[fan_num].as_object().find("LowerThresholdFatal") != fan_array[fan_num].as_object().end())
                        fan->lower_threshold_fatal = fan_array[fan_num].at("LowerThresholdFatal").as_integer();

                    if(fan_array[fan_num].as_object().find("MinReadingRange") != fan_array[fan_num].as_object().end())
                        fan->min_reading_range = fan_array[fan_num].at("MinReadingRange").as_integer();
                    
                    if(fan_array[fan_num].as_object().find("MaxReadingRange") != fan_array[fan_num].as_object().end())
                        fan->max_reading_range = fan_array[fan_num].at("MaxReadingRange").as_integer();

                    if(fan_array[fan_num].as_object().find("PhysicalContext") != fan_array[fan_num].as_object().end())
                        fan->physical_context = fan_array[fan_num].at("PhysicalContext").as_string();

                    if(fan_array[fan_num].as_object().find("Status") != fan_array[fan_num].as_object().end())
                    {
                        json::value j = json::value::object();
                        j = fan_array[fan_num].at("Status");

                        if(j.as_object().find("State") != j.as_object().end())
                            fan->status.state = j.at("State").as_string();
                        if(j.as_object().find("Health") != j.as_object().end())
                            fan->status.health = j.at("Health").as_string();
                    }

                    chassis->thermal->fans->add_member(fan);

                }
            }

            json::value temp_array = json::value::array();
            if(thermal_info.as_object().find("Temperatures") != thermal_info.as_object().end())
            {
                temp_array = thermal_info.at("Temperatures");
                for(int temp_num=0; temp_num<temp_array.size(); temp_num++)
                {
                    // Temperature 만들기
                    string temp_odata;
                    if(temp_array[temp_num].as_object().find("@odata.id") != temp_array[temp_num].as_object().end())
                        temp_odata = temp_array[temp_num].at("@odata.id").as_string();
                    else
                    {
                        // else면 템퍼러쳐 정보에 odata.id 가 없다는 거라 이건 Temperature리소스를 생성해줄수없어서 컨티뉴
                        continue;
                    }

                    vector<string> uri_tokens = string_split(temp_odata, '/');
                    string my_odata = _bmc_id + "/Thermal/Temperatures/" + uri_tokens[uri_tokens.size()-1];
                    Temperature *temp = new Temperature(my_odata);

                    if(temp_array[temp_num].as_object().find("Name") != temp_array[temp_num].as_object().end())
                        temp->name = temp_array[temp_num].at("Name").as_string();

                    if(temp_array[temp_num].as_object().find("MemberId") != temp_array[temp_num].as_object().end())
                        temp->member_id = temp_array[temp_num].at("MemberId").as_string();

                    if(temp_array[temp_num].as_object().find("SensorNumber") != temp_array[temp_num].as_object().end())
                        temp->sensor_num = temp_array[temp_num].at("SensorNumber").as_integer();

                    if(temp_array[temp_num].as_object().find("ReadingCelsius") != temp_array[temp_num].as_object().end())
                        temp->reading_celsius = temp_array[temp_num].at("ReadingCelsius").as_double();

                    if(temp_array[temp_num].as_object().find("UpperThresholdNonCritical") != temp_array[temp_num].as_object().end())
                        temp->upper_threshold_non_critical = temp_array[temp_num].at("UpperThresholdNonCritical").as_double();

                    if(temp_array[temp_num].as_object().find("UpperThresholdCritical") != temp_array[temp_num].as_object().end())
                        temp->upper_threshold_critical = temp_array[temp_num].at("UpperThresholdCritical").as_double();

                    if(temp_array[temp_num].as_object().find("UpperThresholdFatal") != temp_array[temp_num].as_object().end())
                        temp->upper_threshold_fatal = temp_array[temp_num].at("UpperThresholdFatal").as_double();

                    if(temp_array[temp_num].as_object().find("LowerThresholdNonCritical") != temp_array[temp_num].as_object().end())
                        temp->lower_threshold_non_critical = temp_array[temp_num].at("LowerThresholdNonCritical").as_double();

                    if(temp_array[temp_num].as_object().find("LowerThresholdCritical") != temp_array[temp_num].as_object().end())
                        temp->lower_threshold_critical = temp_array[temp_num].at("LowerThresholdCritical").as_double();

                    if(temp_array[temp_num].as_object().find("LowerThresholdFatal") != temp_array[temp_num].as_object().end())
                        temp->lower_threshold_fatal = temp_array[temp_num].at("LowerThresholdFatal").as_double();

                    if(temp_array[temp_num].as_object().find("MinReadingRangeTemp") != temp_array[temp_num].as_object().end())
                        temp->min_reading_range_temp = temp_array[temp_num].at("MinReadingRangeTemp").as_double();

                    if(temp_array[temp_num].as_object().find("MaxReadingRangeTemp") != temp_array[temp_num].as_object().end())
                        temp->max_reading_range_temp = temp_array[temp_num].at("MaxReadingRangeTemp").as_double();

                    if(temp_array[temp_num].as_object().find("PhysicalContext") != temp_array[temp_num].as_object().end())
                        temp->physical_context = temp_array[temp_num].at("PhysicalContext").as_string();

                    if(temp_array[temp_num].as_object().find("Status") != temp_array[temp_num].as_object().end())
                    {
                        json::value j = json::value::object();
                        j = temp_array[temp_num].at("Status");

                        if(j.as_object().find("State") != j.as_object().end())
                            temp->status.state = j.at("State").as_string();
                        if(j.as_object().find("Health") != j.as_object().end())
                            temp->status.health = j.at("Health").as_string();
                    }

                    chassis->thermal->temperatures->add_member(temp);

                }
            }
        }
    }
    // else
        // cout << "Thermal is X" << endl;

    if(chassis_info.as_object().find("Power") != chassis_info.as_object().end())
    {
        json::value power_info;
        string inner_uri = chassis_info.at("Power").at("@odata.id").as_string();
        // cout << "INNER URI : " << inner_uri << endl;
        power_info = get_json_info(inner_uri, _host, _auth_token);
        // cout << "받아라 Power INFO" << endl;
        // cout << power_info << endl;
        cout << "  WHY? " << endl;
    }
    else
        cout << "Power is X" << endl;

    if(chassis_info.as_object().find("Sensors") != chassis_info.as_object().end())
    {
        json::value sensor_info;
        string inner_uri = chassis_info.at("Sensors").at("@odata.id").as_string();
        // cout << "INNER URI : " << inner_uri << endl;
        sensor_info = get_json_info(inner_uri, _host, _auth_token);
        // cout << "받아라 Sensors INFO" << endl;
        // cout << sensor_info << endl;
    }
    else
        cout << "Sensors is X" << endl;


    /**
     * @todo Power랑 Sensors는 나중에 값 받아오게 되면 구현해야함
     */
    

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

    // cout << " ADD Manager " << endl;
    // cout << "New Uri : " << new_uri << endl;

    http_client client(_host);

    http_request req(methods::GET);
    req.set_request_uri(new_uri);
    req.headers().add("X-Auth-Token", _auth_token);
    http_response response;

    // if(req.headers().has("X-Auth-Token"))
    //     cout << "Token : " << req.headers()["X-Auth-Token"] << endl;

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

    // Manager Resource 생성
    Manager *manager = new Manager(_bmc_id, "BMC Manager");

    // Manager ethernet, logservice, remote_accountservice
    manager->ethernet = new Collection(manager->odata.id + "/EthernetInterfaces", ODATA_ETHERNET_INTERFACE_COLLECTION_TYPE);
    manager->ethernet->name = "Manager Ethernet Interface Collection";

    manager->log_service = new Collection(manager->odata.id + "/LogServices", ODATA_LOG_SERVICE_COLLECTION_TYPE);
    manager->log_service->name = "Manager Log Service Collection";
    // 로그엔트리 컬렉션만들어야함

    manager->remote_account_service = new AccountService(manager->odata.id + "/AccountService");
    manager->remote_account_service->account_collection = new Collection(manager->remote_account_service->odata.id + "/Accounts", ODATA_ACCOUNT_COLLECTION_TYPE);
    manager->remote_account_service->account_collection->name = "Remote Accounts Collection";

    manager->remote_account_service->role_collection = new Collection(manager->remote_account_service->odata.id + "/Roles", ODATA_ROLE_COLLECTION_TYPE);
    manager->remote_account_service->role_collection->name = "Remote Roles Collection";

    // Manager 멤버변수 넣어주기
    if(manager_info.as_object().find("Id") != manager_info.as_object().end())
    {
        // cout << "Manager Id is O" << endl;
        manager->id = manager_info.at("Id").as_string();
    }
    // else
        // cout << "Manager Id is X" << endl;

    if(manager_info.as_object().find("ManagerType") != manager_info.as_object().end())
    {
        // cout << "Manager ManagerType is O" << endl;
        manager->manager_type = manager_info.at("ManagerType").as_string();
    }
    // else
        // cout << "Manager ManagerType is X" << endl;

    if(manager_info.as_object().find("Description") != manager_info.as_object().end())
    {
        // cout << "Manager Description is O" << endl;
        manager->description = manager_info.at("Description").as_string();
    }
    // else
        // cout << "Manager Description is X" << endl;

    if(manager_info.as_object().find("FirmwareVersion") != manager_info.as_object().end())
    {
        // cout << "Manager FirmwareVersion is O" << endl;
        manager->firmware_version = manager_info.at("FirmwareVersion").as_string();
    }
    // else
        // cout << "Manager FirmwareVersion is X" << endl;

    if(manager_info.as_object().find("DateTime") != manager_info.as_object().end())
    {
        // cout << "Manager DateTime is O" << endl;
        manager->datetime = manager_info.at("DateTime").as_string();
    }
    // else
        // cout << "Manager DateTime is X" << endl;

    if(manager_info.as_object().find("DateTimeLocalOffset") != manager_info.as_object().end())
    {
        // cout << "Manager DateTimeLocalOffset is O" << endl;
        manager->datetime_offset = manager_info.at("DateTimeLocalOffset").as_string();
    }
    // else
        // cout << "Manager DateTimeLocalOffset is X" << endl;

    if(manager_info.as_object().find("Model") != manager_info.as_object().end())
    {
        // cout << "Manager Model is O" << endl;
        manager->model = manager_info.at("Model").as_string();
    }
    // else
        // cout << "Manager Model is X" << endl;

    if(manager_info.as_object().find("UUID") != manager_info.as_object().end())
    {
        // cout << "Manager UUID is O" << endl;
        manager->uuid = manager_info.at("UUID").as_string();
    }
    // else
    //     cout << "Manager UUID is X" << endl;

    if(manager_info.as_object().find("PowerState") != manager_info.as_object().end())
    {
        // cout << "Manager PowerState is O" << endl;
        manager->power_state = manager_info.at("PowerState").as_string();
    }
    // else
        // cout << "Manager PowerState is X" << endl;

    if(manager_info.as_object().find("Status") != manager_info.as_object().end())
    {
        // cout << "Manager Status is O" << endl;
        
        json::value j = json::value::object();
        j = manager_info.at("Status");

        if(j.as_object().find("State") != j.as_object().end())
            manager->status.state = j.at("State").as_string();
        if(j.as_object().find("Health") != j.as_object().end())
            manager->status.health = j.at("Health").as_string();
    }
    // else
        // cout << "Manager Status is X" << endl;

    if(manager_info.as_object().find("NetworkProtocol") != manager_info.as_object().end())
    {
        json::value network_info;
        string inner_uri;
        if(manager_info.at("NetworkProtocol").as_object().find("@odata.id") != manager_info.at("NetworkProtocol").as_object().end())
        {
            inner_uri = manager_info.at("NetworkProtocol").at("@odata.id").as_string();
            cout << "INNER URI : " << inner_uri << endl;
            network_info = get_json_info(inner_uri, _host, _auth_token);
            cout << "받아라 NetworkProtocol INFO" << endl;
            cout << network_info << endl;

            // NetworkProtocol 만들기

            vector<string> uri_tokens = string_split(inner_uri, '/');
            string my_odata = _bmc_id + "/" + uri_tokens[uri_tokens.size()-1];
            NetworkProtocol *net = new NetworkProtocol(my_odata);

            if(network_info.as_object().find("Id") != network_info.as_object().end())
                net->id = network_info.at("Id").as_string();

            if(network_info.as_object().find("FQDN") != network_info.as_object().end())
                net->fqdn = network_info.at("FQDN").as_string();

            if(network_info.as_object().find("HostName") != network_info.as_object().end())
                net->hostname = network_info.at("HostName").as_string();

            if(network_info.as_object().find("Description") != network_info.as_object().end())
                net->description = network_info.at("Description").as_string();
            
            if(network_info.as_object().find("Status") != network_info.as_object().end())
            {
                json::value j = json::value::object();
                j = network_info.at("Status");

                if(j.as_object().find("State") != j.as_object().end())
                    net->status.state = j.at("State").as_string();
                if(j.as_object().find("Health") != j.as_object().end())
                    net->status.health = j.at("Health").as_string();
            }

            if(network_info.as_object().find("SNMP") != network_info.as_object().end())
            {
                json::value j = json::value::object();
                j = network_info.at("SNMP");

                if(j.as_object().find("ProtocolEnabled") != j.as_object().end())
                    net->snmp_enabled = j.at("ProtocolEnabled").as_bool();
                if(j.as_object().find("Port") != j.as_object().end())
                    net->snmp_port = j.at("Port").as_integer();
            }

            if(network_info.as_object().find("IPMI") != network_info.as_object().end())
            {
                json::value j = json::value::object();
                j = network_info.at("IPMI");

                if(j.as_object().find("ProtocolEnabled") != j.as_object().end())
                    net->ipmi_enabled = j.at("ProtocolEnabled").as_bool();
                if(j.as_object().find("Port") != j.as_object().end())
                    net->ipmi_port = j.at("Port").as_integer();
            }

            if(network_info.as_object().find("KVMIP") != network_info.as_object().end())
            {
                json::value j = json::value::object();
                j = network_info.at("KVMIP");

                if(j.as_object().find("ProtocolEnabled") != j.as_object().end())
                    net->kvmip_enabled = j.at("ProtocolEnabled").as_bool();
                if(j.as_object().find("Port") != j.as_object().end())
                    net->kvmip_port = j.at("Port").as_integer();
            }

            if(network_info.as_object().find("HTTP") != network_info.as_object().end())
            {
                json::value j = json::value::object();
                j = network_info.at("HTTP");

                if(j.as_object().find("ProtocolEnabled") != j.as_object().end())
                    net->http_enabled = j.at("ProtocolEnabled").as_bool();
                if(j.as_object().find("Port") != j.as_object().end())
                    net->http_port = j.at("Port").as_integer();
            }

            if(network_info.as_object().find("HTTPS") != network_info.as_object().end())
            {
                json::value j = json::value::object();
                j = network_info.at("HTTPS");

                if(j.as_object().find("ProtocolEnabled") != j.as_object().end())
                    net->https_enabled = j.at("ProtocolEnabled").as_bool();
                if(j.as_object().find("Port") != j.as_object().end())
                    net->https_port = j.at("Port").as_integer();
            }

            if(network_info.as_object().find("NTP") != network_info.as_object().end())
            {
                json::value j = json::value::object();
                j = network_info.at("NTP");

                if(j.as_object().find("ProtocolEnabled") != j.as_object().end())
                    net->ntp_enabled = j.at("ProtocolEnabled").as_bool();
                
                if(j.as_object().find("NTPServers") != j.as_object().end())
                {
                    json::value k = json::value::array();
                    k = j.at("NTPServers");

                    for(int ntp_num=0; ntp_num<k.size(); ntp_num++)
                    {
                        string server;
                        server = k[ntp_num].as_string();
                        net->v_netservers.push_back(server);
                    }
                }
            }


            manager->network = net;
        }
    }
    // else
        // cout << "NetworkProtocol is X" << endl;
    
    if(manager_info.as_object().find("EthernetInterfaces") != manager_info.as_object().end())
    {
        json::value ethernet_col_info;
        string inner_uri;
        if(manager_info.at("EthernetInterfaces").as_object().find("@odata.id") != manager_info.at("EthernetInterfaces").as_object().end())
        {
            inner_uri = manager_info.at("EthernetInterfaces").at("@odata.id").as_string();
            // cout << "INNER URI : " << inner_uri << endl;
            ethernet_col_info = get_json_info(inner_uri, _host, _auth_token);
            // cout << "받아라 EthernetInterfaces Collection INFO" << endl;
            // cout << ethernet_col_info << endl;

            json::value array_member = json::value::array();
            if(ethernet_col_info.as_object().find("Members") != ethernet_col_info.as_object().end())
            {
                array_member = ethernet_col_info.at("Members");
                for(int i=0; i<array_member.size(); i++)
                {
                    json::value ethernet_info;
                    string member_uri;
                    if(array_member[i].as_object().find("@odata.id") != array_member[i].as_object().end())
                    {
                        member_uri = array_member[i].at("@odata.id").as_string();
                        // cout << "MEMBER URI : " << member_uri << endl;
                        ethernet_info = get_json_info(member_uri, _host, _auth_token);
                        // cout << "받아라 EthernetInterfaces Collection 안의 EthernetInterfaces INFO" << endl;
                        // cout << ethernet_info << endl;

                        // EthernetInterfaces 만들기
                        
                        vector<string> uri_tokens = string_split(member_uri, '/');
                        string my_odata = _bmc_id + "/EthernetInterfaces/" + uri_tokens[uri_tokens.size()-1];
                        EthernetInterfaces *eth = new EthernetInterfaces(my_odata);

                        if(ethernet_info.as_object().find("Id") != ethernet_info.as_object().end())
                            eth->id = ethernet_info.at("Id").as_string();

                        if(ethernet_info.as_object().find("Description") != ethernet_info.as_object().end())
                            eth->description = ethernet_info.at("Description").as_string();

                        if(ethernet_info.as_object().find("LinkStatus") != ethernet_info.as_object().end())
                            eth->link_status = ethernet_info.at("LinkStatus").as_string();

                        if(ethernet_info.as_object().find("MACAddress") != ethernet_info.as_object().end())
                            eth->mac_address = ethernet_info.at("MACAddress").as_string();

                        if(ethernet_info.as_object().find("MTUSize") != ethernet_info.as_object().end())
                            eth->mtu_size = ethernet_info.at("MTUSize").as_integer();

                        if(ethernet_info.as_object().find("HostName") != ethernet_info.as_object().end())
                            eth->hostname = ethernet_info.at("HostName").as_string();

                        if(ethernet_info.as_object().find("FQDN") != ethernet_info.as_object().end())
                            eth->fqdn = ethernet_info.at("FQDN").as_string();

                        if(ethernet_info.as_object().find("IPv6DefaultGateway") != ethernet_info.as_object().end())
                            eth->ipv6_default_gateway = ethernet_info.at("IPv6DefaultGateway").as_string();

                        if(ethernet_info.as_object().find("IPv4Addresses") != ethernet_info.as_object().end())
                        {
                            json::value ip_array = json::value::array();
                            ip_array = ethernet_info.at("IPv4Addresses");
                            for(int ip_num=0; ip_num<ip_array.size(); ip_num++)
                            {
                                IPv4_Address tmp;
                                if(ip_array[ip_num].as_object().find("Address") != ip_array[ip_num].as_object().end())
                                    tmp.address = ip_array[ip_num].at("Address").as_string();
                                
                                if(ip_array[ip_num].as_object().find("AddressOrigin") != ip_array[ip_num].as_object().end())
                                    tmp.address_origin = ip_array[ip_num].at("AddressOrigin").as_string();

                                if(ip_array[ip_num].as_object().find("SubnetMask") != ip_array[ip_num].as_object().end())
                                    tmp.subnet_mask = ip_array[ip_num].at("SubnetMask").as_string();

                                if(ip_array[ip_num].as_object().find("Gateway") != ip_array[ip_num].as_object().end())
                                    tmp.gateway = ip_array[ip_num].at("Gateway").as_string();

                                eth->v_ipv4.push_back(tmp);
                            }
                        }

                        if(ethernet_info.as_object().find("IPv6Addresses") != ethernet_info.as_object().end())
                        {
                            json::value ip_array = json::value::array();
                            ip_array = ethernet_info.at("IPv6Addresses");
                            for(int ip_num=0; ip_num<ip_array.size(); ip_num++)
                            {
                                IPv6_Address tmp;
                                if(ip_array[ip_num].as_object().find("Address") != ip_array[ip_num].as_object().end())
                                    tmp.address = ip_array[ip_num].at("Address").as_string();
                                
                                if(ip_array[ip_num].as_object().find("AddressOrigin") != ip_array[ip_num].as_object().end())
                                    tmp.address_origin = ip_array[ip_num].at("AddressOrigin").as_string();

                                if(ip_array[ip_num].as_object().find("AddressState") != ip_array[ip_num].as_object().end())
                                    tmp.address_state = ip_array[ip_num].at("AddressState").as_string();

                                if(ip_array[ip_num].as_object().find("PrefixLength") != ip_array[ip_num].as_object().end())
                                    tmp.prefix_length = ip_array[ip_num].at("PrefixLength").as_integer();
                                else
                                    tmp.prefix_length = 0;

                                eth->v_ipv6.push_back(tmp);
                            }
                        }

                        manager->ethernet->add_member(eth);
                    }
                }
            }
        }
    }
    // else
        // cout << "EthernetInterfaces is X" << endl;

    if(manager_info.as_object().find("LogServices") != manager_info.as_object().end())
    {
        json::value log_info;
        string inner_uri = manager_info.at("LogServices").at("@odata.id").as_string();
        // cout << "INNER URI : " << inner_uri << endl;
        log_info = get_json_info(inner_uri, _host, _auth_token);
        // cout << "받아라 LogServices INFO" << endl;
        // cout << log_info << endl;
    }
    else
        cout << "LogServices is X" << endl;

    // Remote AccountService(BMC AccountService)
    json::value remote_info;
    string remote_uri = ODATA_ACCOUNT_SERVICE_ID;
    remote_info = get_json_info(remote_uri, _host, _auth_token);
    // cout << "받아라 Remote AccountService INFO" << endl;
    // cout << remote_info << endl;

    if(remote_info.as_object().find("Id") != remote_info.as_object().end())
        manager->remote_account_service->id = remote_info.at("Id").as_string();

    if(remote_info.as_object().find("ServiceEnabled") != remote_info.as_object().end())
        manager->remote_account_service->service_enabled = remote_info.at("ServiceEnabled").as_bool();

    if(remote_info.as_object().find("AuthFailureLoggingThreshold") != remote_info.as_object().end())
        manager->remote_account_service->auth_failure_logging_threshold = remote_info.at("AuthFailureLoggingThreshold").as_integer();

    if(remote_info.as_object().find("MinPasswordLength") != remote_info.as_object().end())
        manager->remote_account_service->min_password_length = remote_info.at("MinPasswordLength").as_integer();

    if(remote_info.as_object().find("AccountLockoutThreshold") != remote_info.as_object().end())
        manager->remote_account_service->account_lockout_threshold = remote_info.at("AccountLockoutThreshold").as_integer();

    if(remote_info.as_object().find("AccountLockoutDuration") != remote_info.as_object().end())
        manager->remote_account_service->account_lockout_duration = remote_info.at("AccountLockoutDuration").as_integer();

    if(remote_info.as_object().find("AccountLockoutCounterResetAfter") != remote_info.as_object().end())
        manager->remote_account_service->account_lockout_counter_reset_after = remote_info.at("AccountLockoutCounterResetAfter").as_integer();

    if(remote_info.as_object().find("AccountLockoutCounterResetEnabled") != remote_info.as_object().end())
        manager->remote_account_service->account_lockout_counter_reset_enabled = remote_info.at("AccountLockoutCounterResetEnabled").as_integer();

    if(remote_info.as_object().find("Status") != remote_info.as_object().end())
    {
        json::value j = json::value::object();
        j = remote_info.at("Status");

        if(j.as_object().find("State") != j.as_object().end())
            manager->remote_account_service->status.state = j.at("State").as_string();
        if(j.as_object().find("Health") != j.as_object().end())
            manager->remote_account_service->status.health = j.at("Health").as_string();
    }

    if(remote_info.as_object().find("Roles") != remote_info.as_object().end())
    {
        json::value role_col_info;
        string inner_uri;
        if(remote_info.at("Roles").as_object().find("@odata.id") != remote_info.at("Roles").as_object().end())
        {
            inner_uri = remote_info.at("Roles").at("@odata.id").as_string();
            // cout << "INNER URI : " << inner_uri << endl;
            role_col_info = get_json_info(inner_uri, _host, _auth_token);
            // cout << "받아라 Remote AccountService 안의 Role Collection INFO" << endl;
            // cout << role_col_info << endl;

            json::value array_member = json::value::array();
            if(role_col_info.as_object().find("Members") != role_col_info.as_object().end())
            {
                array_member = role_col_info.at("Members");
                for(int role_num=0; role_num<array_member.size(); role_num++)
                {
                    json::value role_info;
                    string member_uri;
                    if(array_member[role_num].as_object().find("@odata.id") != array_member[role_num].as_object().end())
                    {
                        member_uri = array_member[role_num].at("@odata.id").as_string();
                        // cout << "MEMBER URI : " << member_uri << endl;
                        role_info = get_json_info(member_uri, _host, _auth_token);
                        // cout << "받아라 Remote AccountService 안의 Role Collection 안의 Role INFO" << endl;
                        // cout << role_info << endl;

                        // Role 만들기
                        string last_token = get_current_object_name(member_uri, "/");
                        string my_odata = _bmc_id + "/AccountService/Roles/" + last_token;
                        // cout << "MY_odata : " << my_odata << endl;

                        Role *rol = new Role(my_odata);

                        if(role_info.as_object().find("Id") != role_info.as_object().end())
                            rol->id = role_info.at("Id").as_string();

                        if(role_info.as_object().find("IsPredefined") != role_info.as_object().end())
                            rol->is_predefined = role_info.at("IsPredefined").as_bool();

                        if(role_info.as_object().find("AssignedPrivileges") != role_info.as_object().end())
                        {
                            json::value v_privileges = json::value::array();
                            v_privileges = role_info.at("AssignedPrivileges");

                            for(int pre_num=0; pre_num<v_privileges.size(); pre_num++)
                                rol->assigned_privileges.push_back(v_privileges[pre_num].as_string());
                        }

                        manager->remote_account_service->role_collection->add_member(rol);
                    }
                }
            }
        }
    }

    if(remote_info.as_object().find("Accounts") != remote_info.as_object().end())
    {
        json::value account_col_info;
        string inner_uri;
        if(remote_info.at("Accounts").as_object().find("@odata.id") != remote_info.at("Accounts").as_object().end())
        {
            inner_uri = remote_info.at("Accounts").at("@odata.id").as_string();
            // cout << "INNER URI : " << inner_uri << endl;
            account_col_info = get_json_info(inner_uri, _host, _auth_token);
            // cout << "받아라 Remote AccountService 안의 Account Collection INFO" << endl;
            // cout << account_col_info << endl;

            json::value array_member = json::value::array();
            if(account_col_info.as_object().find("Members") != account_col_info.as_object().end())
            {
                array_member = account_col_info.at("Members");
                for(int account_num=0; account_num<array_member.size(); account_num++)
                {
                    json::value account_info;
                    string member_uri;
                    if(array_member[account_num].as_object().find("@odata.id") != array_member[account_num].as_object().end())
                    {
                        member_uri = array_member[account_num].at("@odata.id").as_string();
                        // cout << "MEMBER URI : " << member_uri << endl;
                        account_info = get_json_info(member_uri, _host, _auth_token);
                        // cout << "받아라 Remote AccountService 안의 Account Collection 안의 Account INFO" << endl;
                        // cout << account_info << endl;

                        // Account 만들기
                        string last_token = get_current_object_name(member_uri, "/");
                        string my_odata = _bmc_id + "/AccountService/Accounts/" + last_token;
                        // cout << "MY_odata : " << my_odata << endl;
                        Account *acc = new Account(my_odata);

                        if(account_info.as_object().find("Id") != account_info.as_object().end())
                            acc->id = account_info.at("Id").as_string();

                        if(account_info.as_object().find("Enabled") != account_info.as_object().end())
                            acc->enabled = account_info.at("Enabled").as_bool();

                        if(account_info.as_object().find("UserName") != account_info.as_object().end())
                            acc->user_name = account_info.at("UserName").as_string();

                        if(account_info.as_object().find("Password") != account_info.as_object().end())
                            acc->password = account_info.at("Password").as_string();

                        if(account_info.as_object().find("Locked") != account_info.as_object().end())
                            acc->locked = account_info.at("Locked").as_bool();

                        if(account_info.as_object().find("Links") != account_info.as_object().end())
                        {
                            json::value tmp1 = account_info.at("Links");//json::value::object();
                            // tmp1 = account_info.at("Links");
                            if(tmp1.as_object().find("Role") != tmp1.as_object().end())
                            {
                                json::value tmp2 = tmp1.at("Role");
                                if(tmp2.as_object().find("@odata.id") != tmp2.as_object().end())
                                {
                                    string role_odata, role_id, new_odata;
                                    role_odata = tmp2.at("@odata.id").as_string();
                                    role_id = get_current_object_name(role_odata, "/");
                                    new_odata = _bmc_id + "/AccountService/Roles/" + role_id;
                                    if(record_is_exist(new_odata))
                                    {
                                        acc->role = (Role *)g_record[new_odata];
                                        // cout << "롤 있어요" << endl;
                                        // cout << new_odata << endl;
                                    }
                                    else
                                    {
                                        // cout << "롤 없어요" << endl;
                                    }
                                }
                            }

                        }

                        manager->remote_account_service->account_collection->add_member(acc);
                            
                            // role_id는 실제 어카운트리소스에는 없고 role객체가 연결되어있으면 json에만 나타나는거네
                            // role_id가 없으면 롤없는채로 두고 있으면 롤 연결해야할듯 이거 어카운트보다 롤 먼저
                            // 처리해야할듯
                            // Role부분인데 이걸 링크들어가서 롤들어가서 odata읽어서 할지
                            // 롤아이디로 걍 연결시켜줘버릴지 롤 아이디가 제대로 안들어가있으면 문제될텐데
                            // 그냥 롤 포인터 자체 연결시킴
                    }


                }
                
            }


        }

    }
    
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