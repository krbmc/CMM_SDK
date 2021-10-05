#include "chassis_controller.hpp"

extern unordered_map<string, Resource *> g_record;
extern map<string, string> module_id_table;


/**
 * @brief BMC를 등록하는 함수
 * @authors 강
 * 
 * @param _bmc_id BMC의 module id
 * @param _ip  0.0.0.0 형식의 ip주소
 * @param _port 포트번호 :xxx로 붙음
 * @param _is_https http인지 https인지 (true가 https)
 * @param _username root
 * @param _password root계정 패스워드
 */
void add_new_bmc(string _bmc_id, string _ip, string _port, bool _is_https, string _username, string _password)
{
    string odata_chassis, odata_system, odata_manager;

    // check exist
    odata_system = ODATA_SYSTEM_ID;
    odata_system = odata_system + "/" + _bmc_id;
    odata_chassis = ODATA_CHASSIS_ID;
    odata_chassis = odata_chassis + "/" + _bmc_id;
    odata_manager = ODATA_MANAGER_ID;
    odata_manager = odata_manager + "/" + _bmc_id;

    // cout << " 잘 나오나? " << endl;
    // cout << "sys : " << odata_system << endl;
    // cout << "cha : " << odata_chassis << endl;
    // cout << "man : " << odata_manager << endl;
    // cout << "upd : " << odata_update << endl;

    // if(module_id_table.find(_bmc_id) != module_id_table.end())
    // {
    //     log(warning) << "이미 존재하는 BMC_id입니다.";
    //     log(warning) << "추가시도된 BMC_id : " << _bmc_id;
    //     log(warning) << "Module_id_table로 검사함";
    //     return ;
    // }
    // 밖에서 넣고 들어왔는데 당연히 존재하지 ㅇㅇ

    // if(record_is_exist(odata_system) || record_is_exist(odata_chassis) ||
    // record_is_exist(odata_manager) || record_is_exist(odata_update))
    // {
    //     log(warning) << "이미 존재하는 BMC_id입니다.";
    //     log(warning) << "입력한 BMC_id : " << _bmc_id;
    //     // cout << "이미 존재하는 BMC_id입니다." << endl;
    //     // cout << "입력한 BMC_id : " << _bmc_id << endl;
    //     return ;
    // }
    // module_id_table들어와서 빼도될듯

    // host주소
    string host;
    if(_is_https == true)
        host = "https";
    else
        host = "http";
    host = host + "://" + _ip + ":" + _port;
    log(trace) << "!!!! Host : " << host;
    // cout << "Host : " << host << endl;

    // // For Login ----------------------------------------------------------------------------------
    // string session_uri = ODATA_SESSION_ID;//"/redfish/v1/SessionService/Sessions";
    // json::value jv;
    // jv[U("UserName")] = json::value::string(U(_username));
    // jv[U("Password")] = json::value::string(U(_password));
    // http_client client(host);
    // http_request req_login(methods::POST);
    // req_login.set_request_uri(session_uri); // 이렇게 해도 되고 아마 uri까지 host에 붙인걸 host자리에 넣어도 되는듯
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
    // login을 안하고 진행하는 상태로 구현 나중에 로그인 필요시 주석풀고 변형

    // log(info) << "BMC_id : " << _bmc_id << " / Address : " << module_id_table[_bmc_id];

    // auth_token = login_response.headers()["X-Auth-Token"];


    //make system
    add_system(odata_system, host);
    // add_system(odata_system, host, auth_token);

    //make chassis
    // add_chassis(odata_chassis, host);
    // add_chassis(odata_chassis, host, auth_token);

    //make manager
    // add_manager(odata_manager, host);
    // add_manager(odata_manager, host, auth_token);

}

// void add_system(string _bmc_id, string _host, string _auth_token)
void add_system(string _uri, string _host)
{
    // Systems *system = new Systems(_bmc_id, "BMC System");
    string uri = _uri;
    vector<string> uri_tokens = string_split(uri, '/');
    string new_uri;
    for(int i=0; i<uri_tokens.size(); i++)
    {
        if(i == 3)
            continue;
        new_uri += "/";
        new_uri += uri_tokens[i];
    }

    log(trace) << "!@#$ ADD System";
    log(trace) << "!@#$ New Uri : " << new_uri;

    json::value system_info;
    system_info = get_json_info(new_uri, _host);

    log(trace) << "!@#$ SYSTEM INFO START" << endl;
    cout << system_info << endl;
    log(trace) << "!@#$ SYSTEM INFO END" << endl;

    if(system_info == json::value::null())
    {
        log(warning) << "System INFO is Null in Add System";
        return ;
    }


    // Resource 드디어 생성
    Systems *system = new Systems(_uri);
    ((Collection *)g_record[ODATA_SYSTEM_ID]) -> add_member(system);

    bring_system(system, system_info, _host);

    record_save_json();
    // resource_save_json(system);


/**** 임시주석 시작점
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

    // ((Collection *)g_record[ODATA_SYSTEM_ID]) -> add_member(system);
    

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
            // bios_info = get_json_info(inner_uri, _host, _auth_token);
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
            // processor_col_info = get_json_info(inner_uri, _host, _auth_token);
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
                        // processor_info = get_json_info(member_uri, _host, _auth_token);
                        // cout << "받아라 Processor Collection 안의 Processor INFO" << endl;
                        // cout << processor_info << endl;

                        // 프로세서 만들기
                        // Processors *pro = new Processors(member_uri);
                        vector<string> uri_tokens = string_split(member_uri, '/');
                        string my_odata = _uri + "/Processors/" + uri_tokens[uri_tokens.size()-1];
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
            // memory_col_info = get_json_info(inner_uri, _host, _auth_token);
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
                        // memory_info = get_json_info(member_uri, _host, _auth_token);
                        // cout << "받아라 Memory Collection 안의 Memory INFO" << endl;
                        // cout << memory_info << endl;

                        // Memory 만들기
                        vector<string> uri_tokens = string_split(member_uri, '/');
                        string my_odata = _uri + "/Memory/" + uri_tokens[uri_tokens.size()-1];
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
            // ethernet_col_info = get_json_info(inner_uri, _host, _auth_token);
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
                        // ethernet_info = get_json_info(member_uri, _host, _auth_token);
                        // cout << "받아라 EthernetInterfaces Collection 안의 EthernetInterfaces INFO" << endl;
                        // cout << ethernet_info << endl;

                        // EthernetInterfaces 만들기
                        
                        vector<string> uri_tokens = string_split(member_uri, '/');
                        string my_odata = _uri + "/EthernetInterfaces/" + uri_tokens[uri_tokens.size()-1];
                        EthernetInterfaces *eth = new EthernetInterfaces(my_odata);

                        
                        if(ethernet_info.as_object().find("Id") != ethernet_info.as_object().end())
                            eth->id = ethernet_info.at("Id").as_string();

                        if(ethernet_info.as_object().find("Description") != ethernet_info.as_object().end())
                            eth->description = ethernet_info.at("Description").as_string();

                        if(ethernet_info.as_object().find("LinkStatus") != ethernet_info.as_object().end())
                            eth->link_status = ethernet_info.at("LinkStatus").as_string();

                        if(ethernet_info.as_object().find("MACAddress") != ethernet_info.as_object().end())
                            eth->mac_address = ethernet_info.at("MACAddress").as_string();
// cout << " 이쯤 ?? 0 " << endl;
                        if(ethernet_info.as_object().find("MTUSize") != ethernet_info.as_object().end())
                            eth->mtu_size = ethernet_info.at("MTUSize").as_integer();
// cout << " 이쯤 ?? 1 " << endl;
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
// cout << " 이쯤 ?? 2 " << endl;
                                if(ip_array[ip_num].as_object().find("PrefixLength") != ip_array[ip_num].as_object().end())
                                    tmp.prefix_length = ip_array[ip_num].at("PrefixLength").as_integer();
// cout << " 이쯤 ?? 3 " << endl;
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
            // simple_col_info = get_json_info(inner_uri, _host, _auth_token);
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
                        // simple_info = get_json_info(member_uri, _host, _auth_token);
                        // cout << "받아라 SimpleStorage Collection 안의 SimpleStorage INFO" << endl;
                        // cout << simple_info << endl;

                        // Simplestorage 만들기

                        vector<string> uri_tokens = string_split(member_uri, '/');
                        string my_odata = _uri + "/SimpleStorage/" + uri_tokens[uri_tokens.size()-1];
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
            // service_col_info = get_json_info(inner_uri, _host, _auth_token);
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
                    // service_info = get_json_info(member_uri, _host, _auth_token);
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

                            // entry_info = get_json_info(entry_col_uri, _host, _auth_token);
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
                                        // log_info = get_json_info(entry_uri, _host, _auth_token);
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

**/// 임시주석 끝점    
}


// void add_chassis(string _bmc_id, string _host, string _auth_token)
// {
//     string uri = _bmc_id;
//     vector<string> uri_tokens = string_split(uri, '/');
//     string new_uri;
//     for(int i=0; i<uri_tokens.size(); i++)
//     {
//         if(i == 3)
//             continue;
//         new_uri += "/";
//         new_uri += uri_tokens[i];
//     }

//     // cout << " ADD Chassis " << endl;
//     // cout << "New Uri : " << new_uri << endl;

//     http_client client(_host);

//     http_request req(methods::GET);
//     req.set_request_uri(new_uri);
//     req.headers().add("X-Auth-Token", _auth_token);
//     http_response response;

//     // if(req.headers().has("X-Auth-Token"))
//     //     cout << "Token : " << req.headers()["X-Auth-Token"] << endl;

//     try
//     {
//         /* code */
//         pplx::task<http_response> responseTask = client.request(req);
//         response = responseTask.get();

//     }
//     catch(const std::exception& e)
//     {
//         std::cerr << e.what() << '\n';
//         cout << "BMC Chassis 정보요청 실패" << endl;
//         return ;
//     }

//     json::value chassis_info;
//     chassis_info = response.extract_json().get();

//     // cout<<"일단 CHASSIS INFO "<<endl;
//     // cout<<chassis_info<<endl;

//     // Chassis Resource 생성
//     Chassis *chassis = new Chassis(_bmc_id, "BMC Chassis");

//     // Chassis Thermal, Power, Sensor collection
//     chassis->thermal = new Thermal(chassis->odata.id + "/Thermal");
//     chassis->thermal->name = "BMC Chassis Thermal";
//     chassis->thermal->temperatures = new List(chassis->thermal->odata.id + "/Temperatures", TEMPERATURE_TYPE);
//     chassis->thermal->temperatures->name = "Chassis Temperatures";
//     chassis->thermal->fans = new List(chassis->thermal->odata.id + "/Fans", FAN_TYPE);
//     chassis->thermal->fans->name = "Chassis Fans";

//     chassis->power = new Power(chassis->odata.id + "/Power");
//     chassis->power->name = "BMC Chassis Power";
//     chassis->power->power_control = new List(chassis->power->odata.id + "/PowerControl", POWER_CONTROL_TYPE);
//     chassis->power->power_control->name = "Chassis PowerControl";
//     chassis->power->voltages = new List(chassis->power->odata.id + "/Voltages", VOLTAGE_TYPE);
//     chassis->power->voltages->name = "Chassis Voltages";
//     chassis->power->power_supplies = new List(chassis->power->odata.id + "/PowerSupplies", POWER_SUPPLY_TYPE);
//     chassis->power->power_supplies->name = "Chassis PowerSupplies";

//     chassis->sensors = new Collection(chassis->odata.id + "/Sensors", ODATA_SENSOR_COLLECTION_TYPE);
//     chassis->sensors->name = "Chassis Sensor Collection";
//     // 센서 파워는 나중에 쓰임

//     ((Collection *)g_record[ODATA_CHASSIS_ID]) -> add_member(chassis);


//     // Chassis 멤버변수 넣어주기
//     if(chassis_info.as_object().find("Id") != chassis_info.as_object().end())
//     {
//         // cout << "Chassis Id is O" << endl;
//         chassis->id = chassis_info.at("Id").as_string();
//     }
//     // else
//         // cout << "Chassis Id is X" << endl;

//     if(chassis_info.as_object().find("ChassisType") != chassis_info.as_object().end())
//     {
//         // cout << "Chassis ChassisType is O" << endl;
//         chassis->chassis_type = chassis_info.at("ChassisType").as_string();
//     }
//     // else
//     //     cout << "Chassis ChassisType is X" << endl;


//     if(chassis_info.as_object().find("Manufacturer") != chassis_info.as_object().end())
//     {
//         // cout << "Chassis Manufacturer is O" << endl;
//         chassis->manufacturer = chassis_info.at("Manufacturer").as_string();
//     }
//     // else
//         // cout << "Chassis Manufacturer is X" << endl;


//     if(chassis_info.as_object().find("Model") != chassis_info.as_object().end())
//     {
//         // cout << "Chassis Model is O" << endl;
//         chassis->model = chassis_info.at("Model").as_string();
//     }
//     // else
//         // cout << "Chassis Model is X" << endl;


//     if(chassis_info.as_object().find("SerialNumber") != chassis_info.as_object().end())
//     {
//         // cout << "Chassis SerialNumber is O" << endl;
//         chassis->serial_number = chassis_info.at("SerialNumber").as_string();
//     }
//     // else
//         // cout << "Chassis SerialNumber is X" << endl;


//     if(chassis_info.as_object().find("PartNumber") != chassis_info.as_object().end())
//     {
//         // cout << "Chassis PartNumber is O" << endl;
//         chassis->part_number = chassis_info.at("PartNumber").as_string();
//     }
//     // else
//         // cout << "Chassis PartNumber is X" << endl;


//     if(chassis_info.as_object().find("AssetTag") != chassis_info.as_object().end())
//     {
//         // cout << "Chassis AssetTag is O" << endl;
//         chassis->asset_tag = chassis_info.at("AssetTag").as_string();
//     }
//     // else
//         // cout << "Chassis AssetTag is X" << endl;


//     if(chassis_info.as_object().find("PowerState") != chassis_info.as_object().end())
//     {
//         // cout << "Chassis PowerState is O" << endl;
//         chassis->power_state = chassis_info.at("PowerState").as_string();
//     }
//     // else
//         // cout << "Chassis PowerState is X" << endl;


//     // if(chassis_info.as_object().find("HeightMm") != chassis_info.as_object().end())
//     // {
//     //     cout << "Chassis HeightMm is O" << endl;
//     //     chassis->height_mm = chassis_info.at("HeightMm").as_double();
//     // }
//     // else
//     //     cout << "Chassis HeightMm is X" << endl;


//     // if(chassis_info.as_object().find("WidthMm") != chassis_info.as_object().end())
//     // {
//     //     cout << "Chassis WidthMm is O" << endl;
//     //     chassis->width_mm = chassis_info.at("WidthMm").as_double();
//     // }
//     // else
//     //     cout << "Chassis WidthMm is X" << endl;


//     // if(chassis_info.as_object().find("DepthMm") != chassis_info.as_object().end())
//     // {
//     //     cout << "Chassis DepthMm is O" << endl;
//     //     chassis->depth_mm = chassis_info.at("DepthMm").as_double();
//     // }
//     // else
//     //     cout << "Chassis DepthMm is X" << endl;


//     // if(chassis_info.as_object().find("WeightKg") != chassis_info.as_object().end())
//     // {
//     //     cout << "Chassis WeightKg is O" << endl;
//     //     chassis->weight_kg = chassis_info.at("WeightKg").as_double();
//     // }
//     // else
//     //     cout << "Chassis WeightKg is X" << endl;

//     if(chassis_info.as_object().find("IndicatorLED") != chassis_info.as_object().end())
//     {
//         // cout << "Chassis IndicatorLED is O" << endl;
//         string led;
//         led = chassis_info.at("IndicatorLED").as_string();

//         if(led == "Off")
//             chassis->indicator_led = LED_OFF;
//         else if(led == "Blinking")
//             chassis->indicator_led = LED_BLINKING;
//         else if(led == "Lit")
//             chassis->indicator_led = LED_LIT;
//     }
//     // else
//         // cout << "Chassis IndicatorLED is X" << endl;

//     if(chassis_info.as_object().find("Status") != chassis_info.as_object().end())
//     {
//         // cout << "Chassis Status is O" << endl;
//         json::value j = json::value::object();
//         j = chassis_info.at("Status");

//         if(j.as_object().find("State") != j.as_object().end())
//             chassis->status.state = j.at("State").as_string();
//         if(j.as_object().find("Health") != j.as_object().end())
//             chassis->status.health = j.at("Health").as_string();
//     }
//     // else
//         // cout << "Chassis Status is X" << endl;

//     if(chassis_info.as_object().find("Location") != chassis_info.as_object().end())
//     {
//         // cout << "Chassis Location is O" << endl;
//         json::value j = json::value::object();
//         j = chassis_info.at("Location");

//         if(j.as_object().find("PostalAddress") != j.as_object().end())
//         {
//             json::value k = json::value::object();
//             k = j.at("PostalAddress");

//             if(k.as_object().find("Country") != k.as_object().end())
//                 chassis->location.postal_address.country = k.at("Country").as_string();
//             if(k.as_object().find("Territory") != k.as_object().end())
//                 chassis->location.postal_address.territory = k.at("Territory").as_string();
//             if(k.as_object().find("City") != k.as_object().end())
//                 chassis->location.postal_address.city = k.at("City").as_string();
//             if(k.as_object().find("Street") != k.as_object().end())
//                 chassis->location.postal_address.street = k.at("Street").as_string();
//             if(k.as_object().find("HouseNumber") != k.as_object().end())
//                 chassis->location.postal_address.house_number = k.at("HouseNumber").as_string();
//             if(k.as_object().find("Name") != k.as_object().end())
//                 chassis->location.postal_address.name = k.at("Name").as_string();
//             if(k.as_object().find("PostalCode") != k.as_object().end())
//                 chassis->location.postal_address.postal_code = k.at("PostalCode").as_string();

//         }

//         if(j.as_object().find("Placement") != j.as_object().end())
//         {
//             json::value k = json::value::object();
//             k = j.at("Placement");

//             if(k.as_object().find("Row") != k.as_object().end())
//                 chassis->location.placement.row = k.at("Row").as_string();
//             if(k.as_object().find("Rack") != k.as_object().end())
//                 chassis->location.placement.rack = k.at("Rack").as_string();
//             if(k.as_object().find("RackOffsetUnits") != k.as_object().end())
//                 chassis->location.placement.rack_offset_units = k.at("RackOffsetUnits").as_string();
//             if(k.as_object().find("RackOffset") != k.as_object().end())
//                 chassis->location.placement.rack_offset = k.at("RackOffset").as_integer();
//         }
//     }
//     // else
//         // cout << "Chassis Location is X" << endl;

    


//     if(chassis_info.as_object().find("Thermal") != chassis_info.as_object().end())
//     {
//         json::value thermal_info;
//         string inner_uri;
//         if(chassis_info.at("Thermal").as_object().find("@odata.id") != chassis_info.at("Thermal").as_object().end())
//         {
//             inner_uri = chassis_info.at("Thermal").at("@odata.id").as_string();
//             // cout << "INNER URI : " << inner_uri << endl;
//             // thermal_info = get_json_info(inner_uri, _host, _auth_token);
//             // cout << "받아라 Thermal INFO" << endl;
//             // cout << thermal_info << endl;

//             if(thermal_info.as_object().find("Id") != thermal_info.as_object().end())
//                 chassis->thermal->id = thermal_info.at("Id").as_string();
            
//             json::value fan_array = json::value::array();
//             if(thermal_info.as_object().find("Fans") != thermal_info.as_object().end())
//             {
//                 fan_array = thermal_info.at("Fans");
//                 for(int fan_num=0; fan_num<fan_array.size(); fan_num++)
//                 {
//                     // Fan 만들기
//                     string fan_odata;
//                     if(fan_array[fan_num].as_object().find("@odata.id") != fan_array[fan_num].as_object().end())
//                         fan_odata = fan_array[fan_num].at("@odata.id").as_string();
//                     else
//                     {
//                         // else면 팬 정보에 odata.id 가 없다는 거라 이건 Fan리소스를 생성해줄수가없어서 컨티뉴하겠음
//                         continue;
//                     }

//                     vector<string> uri_tokens = string_split(fan_odata, '/');
//                     string my_odata = _bmc_id + "/Thermal/Fans/" + uri_tokens[uri_tokens.size()-1];
//                     Fan *fan = new Fan(my_odata);

//                     if(fan_array[fan_num].as_object().find("Name") != fan_array[fan_num].as_object().end())
//                         fan->name = fan_array[fan_num].at("Name").as_string();
                    
//                     if(fan_array[fan_num].as_object().find("MemberId") != fan_array[fan_num].as_object().end())
//                         fan->member_id = fan_array[fan_num].at("MemberId").as_string();
                    
//                     if(fan_array[fan_num].as_object().find("SensorNumber") != fan_array[fan_num].as_object().end())
//                         fan->sensor_num = fan_array[fan_num].at("SensorNumber").as_integer();
                    
//                     if(fan_array[fan_num].as_object().find("Reading") != fan_array[fan_num].as_object().end())
//                         fan->reading = fan_array[fan_num].at("Reading").as_integer();

//                     if(fan_array[fan_num].as_object().find("ReadingUnits") != fan_array[fan_num].as_object().end())
//                         fan->reading_units = fan_array[fan_num].at("ReadingUnits").as_string();

//                     if(fan_array[fan_num].as_object().find("UpperThresholdNonCritical") != fan_array[fan_num].as_object().end())
//                         fan->upper_threshold_non_critical = fan_array[fan_num].at("UpperThresholdNonCritical").as_integer();
                    
//                     if(fan_array[fan_num].as_object().find("UpperThresholdCritical") != fan_array[fan_num].as_object().end())
//                         fan->upper_threshold_critical = fan_array[fan_num].at("UpperThresholdCritical").as_integer();

//                     if(fan_array[fan_num].as_object().find("UpperThresholdFatal") != fan_array[fan_num].as_object().end())
//                         fan->upper_threshold_fatal = fan_array[fan_num].at("UpperThresholdFatal").as_integer();

//                     if(fan_array[fan_num].as_object().find("LowerThresholdNonCritical") != fan_array[fan_num].as_object().end())
//                         fan->lower_threshold_non_critical = fan_array[fan_num].at("LowerThresholdNonCritical").as_integer();
                    
//                     if(fan_array[fan_num].as_object().find("LowerThresholdCritical") != fan_array[fan_num].as_object().end())
//                         fan->lower_threshold_critical = fan_array[fan_num].at("LowerThresholdCritical").as_integer();

//                     if(fan_array[fan_num].as_object().find("LowerThresholdFatal") != fan_array[fan_num].as_object().end())
//                         fan->lower_threshold_fatal = fan_array[fan_num].at("LowerThresholdFatal").as_integer();

//                     if(fan_array[fan_num].as_object().find("MinReadingRange") != fan_array[fan_num].as_object().end())
//                         fan->min_reading_range = fan_array[fan_num].at("MinReadingRange").as_integer();
                    
//                     if(fan_array[fan_num].as_object().find("MaxReadingRange") != fan_array[fan_num].as_object().end())
//                         fan->max_reading_range = fan_array[fan_num].at("MaxReadingRange").as_integer();

//                     if(fan_array[fan_num].as_object().find("PhysicalContext") != fan_array[fan_num].as_object().end())
//                         fan->physical_context = fan_array[fan_num].at("PhysicalContext").as_string();

//                     if(fan_array[fan_num].as_object().find("Status") != fan_array[fan_num].as_object().end())
//                     {
//                         json::value j = json::value::object();
//                         j = fan_array[fan_num].at("Status");

//                         if(j.as_object().find("State") != j.as_object().end())
//                             fan->status.state = j.at("State").as_string();
//                         if(j.as_object().find("Health") != j.as_object().end())
//                             fan->status.health = j.at("Health").as_string();
//                     }

//                     chassis->thermal->fans->add_member(fan);

//                 }
//             }

//             json::value temp_array = json::value::array();
//             if(thermal_info.as_object().find("Temperatures") != thermal_info.as_object().end())
//             {
//                 temp_array = thermal_info.at("Temperatures");
//                 for(int temp_num=0; temp_num<temp_array.size(); temp_num++)
//                 {
//                     // Temperature 만들기
//                     string temp_odata;
//                     if(temp_array[temp_num].as_object().find("@odata.id") != temp_array[temp_num].as_object().end())
//                         temp_odata = temp_array[temp_num].at("@odata.id").as_string();
//                     else
//                     {
//                         // else면 템퍼러쳐 정보에 odata.id 가 없다는 거라 이건 Temperature리소스를 생성해줄수없어서 컨티뉴
//                         continue;
//                     }

//                     vector<string> uri_tokens = string_split(temp_odata, '/');
//                     string my_odata = _bmc_id + "/Thermal/Temperatures/" + uri_tokens[uri_tokens.size()-1];
//                     Temperature *temp = new Temperature(my_odata);

//                     if(temp_array[temp_num].as_object().find("Name") != temp_array[temp_num].as_object().end())
//                         temp->name = temp_array[temp_num].at("Name").as_string();

//                     if(temp_array[temp_num].as_object().find("MemberId") != temp_array[temp_num].as_object().end())
//                         temp->member_id = temp_array[temp_num].at("MemberId").as_string();

//                     if(temp_array[temp_num].as_object().find("SensorNumber") != temp_array[temp_num].as_object().end())
//                         temp->sensor_num = temp_array[temp_num].at("SensorNumber").as_integer();

//                     if(temp_array[temp_num].as_object().find("ReadingCelsius") != temp_array[temp_num].as_object().end())
//                         temp->reading_celsius = temp_array[temp_num].at("ReadingCelsius").as_double();

//                     if(temp_array[temp_num].as_object().find("UpperThresholdNonCritical") != temp_array[temp_num].as_object().end())
//                         temp->upper_threshold_non_critical = temp_array[temp_num].at("UpperThresholdNonCritical").as_double();

//                     if(temp_array[temp_num].as_object().find("UpperThresholdCritical") != temp_array[temp_num].as_object().end())
//                         temp->upper_threshold_critical = temp_array[temp_num].at("UpperThresholdCritical").as_double();

//                     if(temp_array[temp_num].as_object().find("UpperThresholdFatal") != temp_array[temp_num].as_object().end())
//                         temp->upper_threshold_fatal = temp_array[temp_num].at("UpperThresholdFatal").as_double();

//                     if(temp_array[temp_num].as_object().find("LowerThresholdNonCritical") != temp_array[temp_num].as_object().end())
//                         temp->lower_threshold_non_critical = temp_array[temp_num].at("LowerThresholdNonCritical").as_double();

//                     if(temp_array[temp_num].as_object().find("LowerThresholdCritical") != temp_array[temp_num].as_object().end())
//                         temp->lower_threshold_critical = temp_array[temp_num].at("LowerThresholdCritical").as_double();

//                     if(temp_array[temp_num].as_object().find("LowerThresholdFatal") != temp_array[temp_num].as_object().end())
//                         temp->lower_threshold_fatal = temp_array[temp_num].at("LowerThresholdFatal").as_double();

//                     if(temp_array[temp_num].as_object().find("MinReadingRangeTemp") != temp_array[temp_num].as_object().end())
//                         temp->min_reading_range_temp = temp_array[temp_num].at("MinReadingRangeTemp").as_double();

//                     if(temp_array[temp_num].as_object().find("MaxReadingRangeTemp") != temp_array[temp_num].as_object().end())
//                         temp->max_reading_range_temp = temp_array[temp_num].at("MaxReadingRangeTemp").as_double();

//                     if(temp_array[temp_num].as_object().find("PhysicalContext") != temp_array[temp_num].as_object().end())
//                         temp->physical_context = temp_array[temp_num].at("PhysicalContext").as_string();

//                     if(temp_array[temp_num].as_object().find("Status") != temp_array[temp_num].as_object().end())
//                     {
//                         json::value j = json::value::object();
//                         j = temp_array[temp_num].at("Status");

//                         if(j.as_object().find("State") != j.as_object().end())
//                             temp->status.state = j.at("State").as_string();
//                         if(j.as_object().find("Health") != j.as_object().end())
//                             temp->status.health = j.at("Health").as_string();
//                     }

//                     chassis->thermal->temperatures->add_member(temp);

//                 }
//             }
//         }
//     }
//     // else
//         // cout << "Thermal is X" << endl;

//     if(chassis_info.as_object().find("Power") != chassis_info.as_object().end())
//     {
//         json::value power_info;
//         string inner_uri = chassis_info.at("Power").at("@odata.id").as_string();
//         // cout << "INNER URI : " << inner_uri << endl;
//         // power_info = get_json_info(inner_uri, _host, _auth_token);
//         // cout << "받아라 Power INFO" << endl;
//         // cout << power_info << endl;
//         cout << "  WHY? " << endl;
//     }
//     else
//         cout << "Power is X" << endl;

//     if(chassis_info.as_object().find("Sensors") != chassis_info.as_object().end())
//     {
//         json::value sensor_info;
//         string inner_uri = chassis_info.at("Sensors").at("@odata.id").as_string();
//         // cout << "INNER URI : " << inner_uri << endl;
//         // sensor_info = get_json_info(inner_uri, _host, _auth_token);
//         // cout << "받아라 Sensors INFO" << endl;
//         // cout << sensor_info << endl;
//     }
//     else
//         cout << "Sensors is X" << endl;


//     /**
//      * @todo Power랑 Sensors는 나중에 값 받아오게 되면 구현해야함
//      */
    

// }


// void add_manager(string _bmc_id, string _host, string _auth_token)
// {
//     string uri = _bmc_id;
//     vector<string> uri_tokens = string_split(uri, '/');
//     string new_uri;
//     for(int i=0; i<uri_tokens.size(); i++)
//     {
//         if(i == 3)
//             continue;
//         new_uri += "/";
//         new_uri += uri_tokens[i];
//     }

//     // cout << " ADD Manager " << endl;
//     // cout << "New Uri : " << new_uri << endl;

//     http_client client(_host);

//     http_request req(methods::GET);
//     req.set_request_uri(new_uri);
//     req.headers().add("X-Auth-Token", _auth_token);
//     http_response response;

//     // if(req.headers().has("X-Auth-Token"))
//     //     cout << "Token : " << req.headers()["X-Auth-Token"] << endl;

//     try
//     {
//         /* code */
//         pplx::task<http_response> responseTask = client.request(req);
//         response = responseTask.get();

//     }
//     catch(const std::exception& e)
//     {
//         std::cerr << e.what() << '\n';
//         cout << "BMC Manager 정보요청 실패" << endl;
//         return ;
//     }

//     json::value manager_info;
//     manager_info = response.extract_json().get();

//     cout<<"일단 MANAGER INFO "<<endl;
//     cout<<manager_info<<endl;

//     // Manager Resource 생성
//     Manager *manager = new Manager(_bmc_id, "BMC Manager");

//     // Manager ethernet, logservice, remote_accountservice
//     manager->ethernet = new Collection(manager->odata.id + "/EthernetInterfaces", ODATA_ETHERNET_INTERFACE_COLLECTION_TYPE);
//     manager->ethernet->name = "Manager Ethernet Interface Collection";

//     manager->log_service = new Collection(manager->odata.id + "/LogServices", ODATA_LOG_SERVICE_COLLECTION_TYPE);
//     manager->log_service->name = "Manager Log Service Collection";
//     // 로그엔트리 컬렉션만들어야함

//     manager->remote_account_service = new AccountService(manager->odata.id + "/AccountService");
//     manager->remote_account_service->account_collection = new Collection(manager->remote_account_service->odata.id + "/Accounts", ODATA_ACCOUNT_COLLECTION_TYPE);
//     manager->remote_account_service->account_collection->name = "Remote Accounts Collection";

//     manager->remote_account_service->role_collection = new Collection(manager->remote_account_service->odata.id + "/Roles", ODATA_ROLE_COLLECTION_TYPE);
//     manager->remote_account_service->role_collection->name = "Remote Roles Collection";

//     ((Collection *)g_record[ODATA_MANAGER_ID]) -> add_member(manager);

//     // Manager 멤버변수 넣어주기
//     if(manager_info.as_object().find("Id") != manager_info.as_object().end())
//     {
//         // cout << "Manager Id is O" << endl;
//         manager->id = manager_info.at("Id").as_string();
//     }
//     // else
//         // cout << "Manager Id is X" << endl;

//     if(manager_info.as_object().find("ManagerType") != manager_info.as_object().end())
//     {
//         // cout << "Manager ManagerType is O" << endl;
//         manager->manager_type = manager_info.at("ManagerType").as_string();
//     }
//     // else
//         // cout << "Manager ManagerType is X" << endl;

//     if(manager_info.as_object().find("Description") != manager_info.as_object().end())
//     {
//         // cout << "Manager Description is O" << endl;
//         manager->description = manager_info.at("Description").as_string();
//     }
//     // else
//         // cout << "Manager Description is X" << endl;

//     if(manager_info.as_object().find("FirmwareVersion") != manager_info.as_object().end())
//     {
//         // cout << "Manager FirmwareVersion is O" << endl;
//         manager->firmware_version = manager_info.at("FirmwareVersion").as_string();
//     }
//     // else
//         // cout << "Manager FirmwareVersion is X" << endl;

//     if(manager_info.as_object().find("DateTime") != manager_info.as_object().end())
//     {
//         // cout << "Manager DateTime is O" << endl;
//         manager->datetime = manager_info.at("DateTime").as_string();
//     }
//     // else
//         // cout << "Manager DateTime is X" << endl;

//     if(manager_info.as_object().find("DateTimeLocalOffset") != manager_info.as_object().end())
//     {
//         // cout << "Manager DateTimeLocalOffset is O" << endl;
//         manager->datetime_offset = manager_info.at("DateTimeLocalOffset").as_string();
//     }
//     // else
//         // cout << "Manager DateTimeLocalOffset is X" << endl;

//     if(manager_info.as_object().find("Model") != manager_info.as_object().end())
//     {
//         // cout << "Manager Model is O" << endl;
//         manager->model = manager_info.at("Model").as_string();
//     }
//     // else
//         // cout << "Manager Model is X" << endl;

//     if(manager_info.as_object().find("UUID") != manager_info.as_object().end())
//     {
//         // cout << "Manager UUID is O" << endl;
//         manager->uuid = manager_info.at("UUID").as_string();
//     }
//     // else
//     //     cout << "Manager UUID is X" << endl;

//     if(manager_info.as_object().find("PowerState") != manager_info.as_object().end())
//     {
//         // cout << "Manager PowerState is O" << endl;
//         manager->power_state = manager_info.at("PowerState").as_string();
//     }
//     // else
//         // cout << "Manager PowerState is X" << endl;

//     if(manager_info.as_object().find("Status") != manager_info.as_object().end())
//     {
//         // cout << "Manager Status is O" << endl;
        
//         json::value j = json::value::object();
//         j = manager_info.at("Status");

//         if(j.as_object().find("State") != j.as_object().end())
//             manager->status.state = j.at("State").as_string();
//         if(j.as_object().find("Health") != j.as_object().end())
//             manager->status.health = j.at("Health").as_string();
//     }
//     // else
//         // cout << "Manager Status is X" << endl;

//     if(manager_info.as_object().find("NetworkProtocol") != manager_info.as_object().end())
//     {
//         json::value network_info;
//         string inner_uri;
//         if(manager_info.at("NetworkProtocol").as_object().find("@odata.id") != manager_info.at("NetworkProtocol").as_object().end())
//         {
//             inner_uri = manager_info.at("NetworkProtocol").at("@odata.id").as_string();
//             cout << "INNER URI : " << inner_uri << endl;
//             // network_info = get_json_info(inner_uri, _host, _auth_token);
//             cout << "받아라 NetworkProtocol INFO" << endl;
//             cout << network_info << endl;

//             // NetworkProtocol 만들기

//             vector<string> uri_tokens = string_split(inner_uri, '/');
//             string my_odata = _bmc_id + "/" + uri_tokens[uri_tokens.size()-1];
//             NetworkProtocol *net = new NetworkProtocol(my_odata);

//             if(network_info.as_object().find("Id") != network_info.as_object().end())
//                 net->id = network_info.at("Id").as_string();

//             if(network_info.as_object().find("FQDN") != network_info.as_object().end())
//                 net->fqdn = network_info.at("FQDN").as_string();

//             if(network_info.as_object().find("HostName") != network_info.as_object().end())
//                 net->hostname = network_info.at("HostName").as_string();

//             if(network_info.as_object().find("Description") != network_info.as_object().end())
//                 net->description = network_info.at("Description").as_string();
            
//             if(network_info.as_object().find("Status") != network_info.as_object().end())
//             {
//                 json::value j = json::value::object();
//                 j = network_info.at("Status");

//                 if(j.as_object().find("State") != j.as_object().end())
//                     net->status.state = j.at("State").as_string();
//                 if(j.as_object().find("Health") != j.as_object().end())
//                     net->status.health = j.at("Health").as_string();
//             }

//             if(network_info.as_object().find("SNMP") != network_info.as_object().end())
//             {
//                 json::value j = json::value::object();
//                 j = network_info.at("SNMP");

//                 // if(j.as_object().find("ProtocolEnabled") != j.as_object().end())
//                 //     net->snmp_enabled = j.at("ProtocolEnabled").as_bool();
//                 // if(j.as_object().find("Port") != j.as_object().end())
//                 //     net->snmp_port = j.at("Port").as_integer();
//             }

//             if(network_info.as_object().find("IPMI") != network_info.as_object().end())
//             {
//                 json::value j = json::value::object();
//                 j = network_info.at("IPMI");

//                 if(j.as_object().find("ProtocolEnabled") != j.as_object().end())
//                     net->ipmi_enabled = j.at("ProtocolEnabled").as_bool();
//                 if(j.as_object().find("Port") != j.as_object().end())
//                     net->ipmi_port = j.at("Port").as_integer();
//             }

//             if(network_info.as_object().find("KVMIP") != network_info.as_object().end())
//             {
//                 json::value j = json::value::object();
//                 j = network_info.at("KVMIP");

//                 if(j.as_object().find("ProtocolEnabled") != j.as_object().end())
//                     net->kvmip_enabled = j.at("ProtocolEnabled").as_bool();
//                 if(j.as_object().find("Port") != j.as_object().end())
//                     net->kvmip_port = j.at("Port").as_integer();
//             }

//             if(network_info.as_object().find("HTTP") != network_info.as_object().end())
//             {
//                 json::value j = json::value::object();
//                 j = network_info.at("HTTP");

//                 if(j.as_object().find("ProtocolEnabled") != j.as_object().end())
//                     net->http_enabled = j.at("ProtocolEnabled").as_bool();
//                 if(j.as_object().find("Port") != j.as_object().end())
//                     net->http_port = j.at("Port").as_integer();
//             }

//             if(network_info.as_object().find("HTTPS") != network_info.as_object().end())
//             {
//                 json::value j = json::value::object();
//                 j = network_info.at("HTTPS");

//                 if(j.as_object().find("ProtocolEnabled") != j.as_object().end())
//                     net->https_enabled = j.at("ProtocolEnabled").as_bool();
//                 if(j.as_object().find("Port") != j.as_object().end())
//                     net->https_port = j.at("Port").as_integer();
//             }

//             if(network_info.as_object().find("NTP") != network_info.as_object().end())
//             {
//                 json::value j = json::value::object();
//                 j = network_info.at("NTP");

//                 if(j.as_object().find("ProtocolEnabled") != j.as_object().end())
//                     net->ntp_enabled = j.at("ProtocolEnabled").as_bool();
                
//                 if(j.as_object().find("NTPServers") != j.as_object().end())
//                 {
//                     json::value k = json::value::array();
//                     k = j.at("NTPServers");

//                     for(int ntp_num=0; ntp_num<k.size(); ntp_num++)
//                     {
//                         string server;
//                         server = k[ntp_num].as_string();
//                         net->v_netservers.push_back(server);
//                     }
//                 }
//             }


//             manager->network = net;
//         }
//     }
//     // else
//         // cout << "NetworkProtocol is X" << endl;
    
//     if(manager_info.as_object().find("EthernetInterfaces") != manager_info.as_object().end())
//     {
//         json::value ethernet_col_info;
//         string inner_uri;
//         if(manager_info.at("EthernetInterfaces").as_object().find("@odata.id") != manager_info.at("EthernetInterfaces").as_object().end())
//         {
//             inner_uri = manager_info.at("EthernetInterfaces").at("@odata.id").as_string();
//             // cout << "INNER URI : " << inner_uri << endl;
//             // ethernet_col_info = get_json_info(inner_uri, _host, _auth_token);
//             // cout << "받아라 EthernetInterfaces Collection INFO" << endl;
//             // cout << ethernet_col_info << endl;

//             json::value array_member = json::value::array();
//             if(ethernet_col_info.as_object().find("Members") != ethernet_col_info.as_object().end())
//             {
//                 array_member = ethernet_col_info.at("Members");
//                 for(int i=0; i<array_member.size(); i++)
//                 {
//                     json::value ethernet_info;
//                     string member_uri;
//                     if(array_member[i].as_object().find("@odata.id") != array_member[i].as_object().end())
//                     {
//                         member_uri = array_member[i].at("@odata.id").as_string();
//                         // cout << "MEMBER URI : " << member_uri << endl;
//                         // ethernet_info = get_json_info(member_uri, _host, _auth_token);
//                         // cout << "받아라 EthernetInterfaces Collection 안의 EthernetInterfaces INFO" << endl;
//                         // cout << ethernet_info << endl;

//                         // EthernetInterfaces 만들기
                        
//                         vector<string> uri_tokens = string_split(member_uri, '/');
//                         string my_odata = _bmc_id + "/EthernetInterfaces/" + uri_tokens[uri_tokens.size()-1];
//                         EthernetInterfaces *eth = new EthernetInterfaces(my_odata);

//                         if(ethernet_info.as_object().find("Id") != ethernet_info.as_object().end())
//                             eth->id = ethernet_info.at("Id").as_string();

//                         if(ethernet_info.as_object().find("Description") != ethernet_info.as_object().end())
//                             eth->description = ethernet_info.at("Description").as_string();

//                         if(ethernet_info.as_object().find("LinkStatus") != ethernet_info.as_object().end())
//                             eth->link_status = ethernet_info.at("LinkStatus").as_string();

//                         if(ethernet_info.as_object().find("MACAddress") != ethernet_info.as_object().end())
//                             eth->mac_address = ethernet_info.at("MACAddress").as_string();

//                         if(ethernet_info.as_object().find("MTUSize") != ethernet_info.as_object().end())
//                             eth->mtu_size = ethernet_info.at("MTUSize").as_integer();

//                         if(ethernet_info.as_object().find("HostName") != ethernet_info.as_object().end())
//                             eth->hostname = ethernet_info.at("HostName").as_string();

//                         if(ethernet_info.as_object().find("FQDN") != ethernet_info.as_object().end())
//                             eth->fqdn = ethernet_info.at("FQDN").as_string();

//                         if(ethernet_info.as_object().find("IPv6DefaultGateway") != ethernet_info.as_object().end())
//                             eth->ipv6_default_gateway = ethernet_info.at("IPv6DefaultGateway").as_string();

//                         if(ethernet_info.as_object().find("IPv4Addresses") != ethernet_info.as_object().end())
//                         {
//                             json::value ip_array = json::value::array();
//                             ip_array = ethernet_info.at("IPv4Addresses");
//                             for(int ip_num=0; ip_num<ip_array.size(); ip_num++)
//                             {
//                                 IPv4_Address tmp;
//                                 if(ip_array[ip_num].as_object().find("Address") != ip_array[ip_num].as_object().end())
//                                     tmp.address = ip_array[ip_num].at("Address").as_string();
                                
//                                 if(ip_array[ip_num].as_object().find("AddressOrigin") != ip_array[ip_num].as_object().end())
//                                     tmp.address_origin = ip_array[ip_num].at("AddressOrigin").as_string();

//                                 if(ip_array[ip_num].as_object().find("SubnetMask") != ip_array[ip_num].as_object().end())
//                                     tmp.subnet_mask = ip_array[ip_num].at("SubnetMask").as_string();

//                                 if(ip_array[ip_num].as_object().find("Gateway") != ip_array[ip_num].as_object().end())
//                                     tmp.gateway = ip_array[ip_num].at("Gateway").as_string();

//                                 eth->v_ipv4.push_back(tmp);
//                             }
//                         }

//                         if(ethernet_info.as_object().find("IPv6Addresses") != ethernet_info.as_object().end())
//                         {
//                             json::value ip_array = json::value::array();
//                             ip_array = ethernet_info.at("IPv6Addresses");
//                             for(int ip_num=0; ip_num<ip_array.size(); ip_num++)
//                             {
//                                 IPv6_Address tmp;
//                                 if(ip_array[ip_num].as_object().find("Address") != ip_array[ip_num].as_object().end())
//                                     tmp.address = ip_array[ip_num].at("Address").as_string();
                                
//                                 if(ip_array[ip_num].as_object().find("AddressOrigin") != ip_array[ip_num].as_object().end())
//                                     tmp.address_origin = ip_array[ip_num].at("AddressOrigin").as_string();

//                                 if(ip_array[ip_num].as_object().find("AddressState") != ip_array[ip_num].as_object().end())
//                                     tmp.address_state = ip_array[ip_num].at("AddressState").as_string();

//                                 if(ip_array[ip_num].as_object().find("PrefixLength") != ip_array[ip_num].as_object().end())
//                                     tmp.prefix_length = ip_array[ip_num].at("PrefixLength").as_integer();
//                                 else
//                                     tmp.prefix_length = 0;

//                                 eth->v_ipv6.push_back(tmp);
//                             }
//                         }

//                         manager->ethernet->add_member(eth);
//                     }
//                 }
//             }
//         }
//     }
//     // else
//         // cout << "EthernetInterfaces is X" << endl;

//     if(manager_info.as_object().find("LogServices") != manager_info.as_object().end())
//     {
//         json::value log_info;
//         string inner_uri = manager_info.at("LogServices").at("@odata.id").as_string();
//         // cout << "INNER URI : " << inner_uri << endl;
//         // log_info = get_json_info(inner_uri, _host, _auth_token);
//         // cout << "받아라 LogServices INFO" << endl;
//         // cout << log_info << endl;
//     }
//     else
//         cout << "LogServices is X" << endl;

//     // Remote AccountService(BMC AccountService)
//     json::value remote_info;
//     string remote_uri = ODATA_ACCOUNT_SERVICE_ID;
//     // remote_info = get_json_info(remote_uri, _host, _auth_token);
//     // cout << "받아라 Remote AccountService INFO" << endl;
//     // cout << remote_info << endl;

//     if(remote_info.as_object().find("Id") != remote_info.as_object().end())
//         manager->remote_account_service->id = remote_info.at("Id").as_string();

//     if(remote_info.as_object().find("ServiceEnabled") != remote_info.as_object().end())
//         manager->remote_account_service->service_enabled = remote_info.at("ServiceEnabled").as_bool();

//     if(remote_info.as_object().find("AuthFailureLoggingThreshold") != remote_info.as_object().end())
//         manager->remote_account_service->auth_failure_logging_threshold = remote_info.at("AuthFailureLoggingThreshold").as_integer();

//     if(remote_info.as_object().find("MinPasswordLength") != remote_info.as_object().end())
//         manager->remote_account_service->min_password_length = remote_info.at("MinPasswordLength").as_integer();

//     if(remote_info.as_object().find("AccountLockoutThreshold") != remote_info.as_object().end())
//         manager->remote_account_service->account_lockout_threshold = remote_info.at("AccountLockoutThreshold").as_integer();

//     if(remote_info.as_object().find("AccountLockoutDuration") != remote_info.as_object().end())
//         manager->remote_account_service->account_lockout_duration = remote_info.at("AccountLockoutDuration").as_integer();

//     if(remote_info.as_object().find("AccountLockoutCounterResetAfter") != remote_info.as_object().end())
//         manager->remote_account_service->account_lockout_counter_reset_after = remote_info.at("AccountLockoutCounterResetAfter").as_integer();

//     if(remote_info.as_object().find("AccountLockoutCounterResetEnabled") != remote_info.as_object().end())
//         manager->remote_account_service->account_lockout_counter_reset_enabled = remote_info.at("AccountLockoutCounterResetEnabled").as_integer();

//     if(remote_info.as_object().find("Status") != remote_info.as_object().end())
//     {
//         json::value j = json::value::object();
//         j = remote_info.at("Status");

//         if(j.as_object().find("State") != j.as_object().end())
//             manager->remote_account_service->status.state = j.at("State").as_string();
//         if(j.as_object().find("Health") != j.as_object().end())
//             manager->remote_account_service->status.health = j.at("Health").as_string();
//     }

//     if(remote_info.as_object().find("Roles") != remote_info.as_object().end())
//     {
//         json::value role_col_info;
//         string inner_uri;
//         if(remote_info.at("Roles").as_object().find("@odata.id") != remote_info.at("Roles").as_object().end())
//         {
//             inner_uri = remote_info.at("Roles").at("@odata.id").as_string();
//             // cout << "INNER URI : " << inner_uri << endl;
//             // role_col_info = get_json_info(inner_uri, _host, _auth_token);
//             // cout << "받아라 Remote AccountService 안의 Role Collection INFO" << endl;
//             // cout << role_col_info << endl;

//             json::value array_member = json::value::array();
//             if(role_col_info.as_object().find("Members") != role_col_info.as_object().end())
//             {
//                 array_member = role_col_info.at("Members");
//                 for(int role_num=0; role_num<array_member.size(); role_num++)
//                 {
//                     json::value role_info;
//                     string member_uri;
//                     if(array_member[role_num].as_object().find("@odata.id") != array_member[role_num].as_object().end())
//                     {
//                         member_uri = array_member[role_num].at("@odata.id").as_string();
//                         // cout << "MEMBER URI : " << member_uri << endl;
//                         // role_info = get_json_info(member_uri, _host, _auth_token);
//                         // cout << "받아라 Remote AccountService 안의 Role Collection 안의 Role INFO" << endl;
//                         // cout << role_info << endl;

//                         // Role 만들기
//                         string last_token = get_current_object_name(member_uri, "/");
//                         string my_odata = _bmc_id + "/AccountService/Roles/" + last_token;
//                         // cout << "MY_odata : " << my_odata << endl;

//                         Role *rol = new Role(my_odata);

//                         if(role_info.as_object().find("Id") != role_info.as_object().end())
//                             rol->id = role_info.at("Id").as_string();

//                         if(role_info.as_object().find("IsPredefined") != role_info.as_object().end())
//                             rol->is_predefined = role_info.at("IsPredefined").as_bool();

//                         if(role_info.as_object().find("AssignedPrivileges") != role_info.as_object().end())
//                         {
//                             json::value v_privileges = json::value::array();
//                             v_privileges = role_info.at("AssignedPrivileges");

//                             for(int pre_num=0; pre_num<v_privileges.size(); pre_num++)
//                                 rol->assigned_privileges.push_back(v_privileges[pre_num].as_string());
//                         }

//                         manager->remote_account_service->role_collection->add_member(rol);
//                     }
//                 }
//             }
//         }
//     }

//     if(remote_info.as_object().find("Accounts") != remote_info.as_object().end())
//     {
//         json::value account_col_info;
//         string inner_uri;
//         if(remote_info.at("Accounts").as_object().find("@odata.id") != remote_info.at("Accounts").as_object().end())
//         {
//             inner_uri = remote_info.at("Accounts").at("@odata.id").as_string();
//             // cout << "INNER URI : " << inner_uri << endl;
//             // account_col_info = get_json_info(inner_uri, _host, _auth_token);
//             // cout << "받아라 Remote AccountService 안의 Account Collection INFO" << endl;
//             // cout << account_col_info << endl;

//             json::value array_member = json::value::array();
//             if(account_col_info.as_object().find("Members") != account_col_info.as_object().end())
//             {
//                 array_member = account_col_info.at("Members");
//                 for(int account_num=0; account_num<array_member.size(); account_num++)
//                 {
//                     json::value account_info;
//                     string member_uri;
//                     if(array_member[account_num].as_object().find("@odata.id") != array_member[account_num].as_object().end())
//                     {
//                         member_uri = array_member[account_num].at("@odata.id").as_string();
//                         // cout << "MEMBER URI : " << member_uri << endl;
//                         // account_info = get_json_info(member_uri, _host, _auth_token);
//                         // cout << "받아라 Remote AccountService 안의 Account Collection 안의 Account INFO" << endl;
//                         // cout << account_info << endl;

//                         // Account 만들기
//                         string last_token = get_current_object_name(member_uri, "/");
//                         string my_odata = _bmc_id + "/AccountService/Accounts/" + last_token;
//                         // cout << "MY_odata : " << my_odata << endl;
//                         Account *acc = new Account(my_odata);

//                         if(account_info.as_object().find("Id") != account_info.as_object().end())
//                             acc->id = account_info.at("Id").as_string();

//                         if(account_info.as_object().find("Enabled") != account_info.as_object().end())
//                             acc->enabled = account_info.at("Enabled").as_bool();

//                         if(account_info.as_object().find("UserName") != account_info.as_object().end())
//                             acc->user_name = account_info.at("UserName").as_string();

//                         if(account_info.as_object().find("Password") != account_info.as_object().end())
//                             acc->password = account_info.at("Password").as_string();

//                         if(account_info.as_object().find("Locked") != account_info.as_object().end())
//                             acc->locked = account_info.at("Locked").as_bool();

//                         if(account_info.as_object().find("Links") != account_info.as_object().end())
//                         {
//                             json::value tmp1 = account_info.at("Links");//json::value::object();
//                             // tmp1 = account_info.at("Links");
//                             if(tmp1.as_object().find("Role") != tmp1.as_object().end())
//                             {
//                                 json::value tmp2 = tmp1.at("Role");
//                                 if(tmp2.as_object().find("@odata.id") != tmp2.as_object().end())
//                                 {
//                                     string role_odata, role_id, new_odata;
//                                     role_odata = tmp2.at("@odata.id").as_string();
//                                     role_id = get_current_object_name(role_odata, "/");
//                                     new_odata = _bmc_id + "/AccountService/Roles/" + role_id;
//                                     if(record_is_exist(new_odata))
//                                     {
//                                         acc->role = (Role *)g_record[new_odata];
//                                         // cout << "롤 있어요" << endl;
//                                         // cout << new_odata << endl;
//                                     }
//                                     else
//                                     {
//                                         // cout << "롤 없어요" << endl;
//                                     }
//                                 }
//                             }

//                         }

//                         manager->remote_account_service->account_collection->add_member(acc);
                            
//                             // role_id는 실제 어카운트리소스에는 없고 role객체가 연결되어있으면 json에만 나타나는거네
//                             // role_id가 없으면 롤없는채로 두고 있으면 롤 연결해야할듯 이거 어카운트보다 롤 먼저
//                             // 처리해야할듯
//                             // Role부분인데 이걸 링크들어가서 롤들어가서 odata읽어서 할지
//                             // 롤아이디로 걍 연결시켜줘버릴지 롤 아이디가 제대로 안들어가있으면 문제될텐데
//                             // 그냥 롤 포인터 자체 연결시킴
//                     }


//                 }
                
//             }


//         }

//     }
    
// }

void add_manager(string _uri, string _host)
{
    string uri = _uri;
    vector<string> uri_tokens = string_split(uri, '/');
    string new_uri;
    for(int i=0; i<uri_tokens.size(); i++)
    {
        if(i == 3)
            continue;
        new_uri += "/";
        new_uri += uri_tokens[i];
    }

    log(trace) << "!@#$ ADD Manager";
    log(trace) << "!@#$ New Uri : " << new_uri;

    json::value manager_info;
    manager_info = get_json_info(new_uri, _host);

    log(trace) << "!@#$ MANAGER INFO START" << endl;
    cout << manager_info << endl;
    log(trace) << "!@#$ MANAGER INFO END" << endl;

    if(manager_info == json::value::null())
    {
        log(warning) << "Manager INFO is Null in Add Manager";
        return ;
    }

    // Resource 생성
    Manager *manager = new Manager(_uri);
    ((Collection *)g_record[ODATA_MANAGER_ID]) -> add_member(manager);

    bring_manager(manager, manager_info, _host);

    record_save_json();

}


void bring_system(Systems *_system, json::value _info, string _addr)
{
    // 일반멤버변수
    get_value_from_json_key(_info, "Id", _system->id);
    get_value_from_json_key(_info, "SystemType", _system->system_type);
    get_value_from_json_key(_info, "AssetTag", _system->asset_tag);
    get_value_from_json_key(_info, "Manufacturer", _system->manufacturer);
    get_value_from_json_key(_info, "Model", _system->model);
    get_value_from_json_key(_info, "SerialNumber", _system->serial_number);
    get_value_from_json_key(_info, "PartNumber", _system->part_number);
    get_value_from_json_key(_info, "Description", _system->description);
    get_value_from_json_key(_info, "UUID", _system->uuid);
    get_value_from_json_key(_info, "HostName", _system->hostname);
    get_value_from_json_key(_info, "PowerState", _system->power_state);
    get_value_from_json_key(_info, "BiosVersion", _system->bios_version);

    //status, boot, led
    json::value status_info;
    if(get_value_from_json_key(_info, "Status", status_info))
    {
        get_value_from_json_key(status_info, "State", _system->status.state);
        get_value_from_json_key(status_info, "Health", _system->status.health);
    }

    json::value boot_info;
    if(get_value_from_json_key(_info, "Boot", boot_info))
    {
        get_value_from_json_key(boot_info, "BootSourceOverrideEnabled", _system->boot.boot_source_override_enabled);
        get_value_from_json_key(boot_info, "BootSourceOverrideTarget", _system->boot.boot_source_override_target);
        get_value_from_json_key(boot_info, "BootSourceOverrideMode", _system->boot.boot_source_override_mode);
        get_value_from_json_key(boot_info, "UefiTargetBootSourceOverride", _system->boot.uefi_target_boot_source_override);
    }

    string led;
    if(get_value_from_json_key(_info, "IndicatorLED", led))
    {
        if(led == "Off")
            _system->indicator_led = LED_OFF;
        else if(led == "Lit")
            _system->indicator_led = LED_LIT;
        else if(led == "Blinking")
            _system->indicator_led = LED_BLINKING;
    }

    // 포인터멤버변수
    _system->bios = new Bios(_system->odata.id + "/Bios", "Bios");
    // bring_bios 는 안만들고 걍 여기서 자체처리
    
    _system->storage = new Collection(_system->odata.id + "/Storage", ODATA_STORAGE_COLLECTION_TYPE);
    _system->storage->name = "Computer System Storage Collection";
    // bring_storages 구현완료

    _system->processor = new Collection(_system->odata.id + "/Processors", ODATA_PROCESSOR_COLLECTION_TYPE);
    _system->processor->name = "Computer System Processor Collection";
    // bring_processors 구현완료

    _system->memory = new Collection(_system->odata.id + "/Memory", ODATA_MEMORY_COLLECTION_TYPE);
    _system->memory->name = "Computer System Memory Collection";
    // bring_memories 구현완료

    _system->ethernet = new Collection(_system->odata.id + "/EthernetInterfaces", ODATA_ETHERNET_INTERFACE_COLLECTION_TYPE);
    _system->ethernet->name = "Computer System Ethernet Interface Collection";
    // bring_ethernet_interfaces 구현완료

    _system->log_service = new Collection(_system->odata.id + "/LogServices", ODATA_LOG_SERVICE_COLLECTION_TYPE);
    _system->log_service->name = "Computer System Log Service Collection";
    // bring_logservices 구현완료

    _system->simple_storage = new Collection(_system->odata.id + "/SimpleStorage", ODATA_SIMPLE_STORAGE_COLLECTION_TYPE);
    _system->simple_storage->name = "Computer System Simple Storage Collection";
    // bring_simple_storages 구현중

    json::value bios_odata_info;
    if(get_value_from_json_key(_info, "Bios", bios_odata_info))
    {
        string bios_uri;
        if(read_odata_id(bios_odata_info, bios_uri))
        {
            json::value bios_info;
            bios_info = get_json_info(bios_uri, _addr);

            string odata = _system->bios->odata.id;

            _system->bios->load_json(bios_info);
            _system->bios->odata.id = odata;
        }
    }

    json::value storage_odata_info;
    if(get_value_from_json_key(_info, "Storage", storage_odata_info))
    {
        string storage_col_uri;
        if(read_odata_id(storage_odata_info, storage_col_uri))
        {
            json::value storage_col_info;
            storage_col_info = get_json_info(storage_col_uri, _addr);
            bring_storages(_system->storage, storage_col_info, _addr);
        }
    }

    json::value processor_odata_info;
    if(get_value_from_json_key(_info, "Processors", processor_odata_info))
    {
        string processor_col_uri;
        if(read_odata_id(processor_odata_info, processor_col_uri))
        {
            json::value processor_col_info;
            processor_col_info = get_json_info(processor_col_uri, _addr);
            bring_processors(_system->processor, processor_col_info, _addr);
        }
    }

    json::value memory_odata_info;
    if(get_value_from_json_key(_info, "Memory", memory_odata_info))
    {
        string memory_col_uri;
        if(read_odata_id(memory_odata_info, memory_col_uri))
        {
            json::value memory_col_info;
            memory_col_info = get_json_info(memory_col_uri, _addr);
            bring_memories(_system->memory, memory_col_info, _addr);
        }
    }

    json::value ethernet_odata_info;
    if(get_value_from_json_key(_info, "EthernetInterfaces", ethernet_odata_info))
    {
        string ethernet_col_uri;
        if(read_odata_id(ethernet_odata_info, ethernet_col_uri))
        {
            json::value ethernet_col_info;
            ethernet_col_info = get_json_info(ethernet_col_uri, _addr);
            bring_ethernet_interfaces(_system->ethernet, ethernet_col_info, _addr);
        }
    }

    json::value logservice_odata_info;
    if(get_value_from_json_key(_info, "LogServices", logservice_odata_info))
    {
        string logservice_col_uri;
        if(read_odata_id(logservice_odata_info, logservice_col_uri))
        {
            json::value logservice_col_info;
            logservice_col_info = get_json_info(logservice_col_uri, _addr);
            bring_logservices(_system->log_service, logservice_col_info, _addr);
        }
    }

    json::value simple_storage_odata_info;
    if(get_value_from_json_key(_info, "SimpleStorage", simple_storage_odata_info))
    {
        string simple_storage_col_uri;
        if(read_odata_id(simple_storage_odata_info, simple_storage_col_uri))
        {
            json::value simple_storage_col_info;
            simple_storage_col_info = get_json_info(simple_storage_col_uri, _addr);
            bring_simple_storages(_system->simple_storage, simple_storage_col_info, _addr);
        }
    }
    
    return ;
}

void bring_storages(Collection *_collection, json::value _info, string _addr)
{
    json::value array_member;
    if(get_value_from_json_key(_info, "Members", array_member))
    {
        for(int i=0; i<array_member.size(); i++)
        {
            json::value storage_info;
            string member_uri;
            if(read_odata_id(array_member[i], member_uri))
            {
                storage_info = get_json_info(member_uri, _addr);
            }
            else
                continue;

            // storage info로 storage 만들기
            string member_id = get_current_object_name(member_uri, "/");
            string odata = _collection->odata.id + "/" + member_id;
            Storage *sto = new Storage(odata);

            // cf> 나중에 이 함수들을 갱신반영하는데 써먹으려면 여기에 resource있으면 가져오고
            // 없으면 생성하게끔 바꿔주면 될거같음

            get_value_from_json_key(storage_info, "Id", sto->id);
            get_value_from_json_key(storage_info, "Description", sto->description);

            json::value status_info;
            if(get_value_from_json_key(storage_info, "Status", status_info))
            {
                get_value_from_json_key(status_info, "State", sto->status.state);
                get_value_from_json_key(status_info, "Health", sto->status.health);
            }

            sto->controller = new List(sto->odata.id + "/StorageControllers", STORAGE_CONTROLLER_TYPE);
            sto->controller->name = "Storage Controllers List";
            // bring_storage_controllers 구현완료

            sto->drives = new Collection(sto->odata.id + "/Drives", ODATA_DRIVE_COLLECTION_TYPE);
            sto->drives->name = "Storage Drive Collection";
            // bring_drives 구현완료

            sto->volumes = new Collection(sto->odata.id + "/Volumes", ODATA_VOLUME_COLLECTION_TYPE);
            sto->volumes->name = "Storage Volume Collection";
            // bring_volumes 구현완료

            json::value storage_controller_list_info;
            if(get_value_from_json_key(storage_info, "StorageControllers", storage_controller_list_info))
            {
                // 아이쒸 이거 storage_info(jv) 에서 StorageControllers를 읽어들이면 odata타는게 아니라 List여서
                // array로 목록 좌라락 나오네 그냥 바로 해야할듯
                if(storage_controller_list_info.size() > 0)
                    bring_storage_controllers(sto->controller, storage_controller_list_info, _addr);
            }

            json::value drive_odata_info;
            if(get_value_from_json_key(storage_info, "Drives", drive_odata_info))
            {
                string drive_col_uri;
                if(read_odata_id(drive_odata_info, drive_col_uri))
                {
                    json::value drive_col_info;
                    drive_col_info = get_json_info(drive_col_uri, _addr);
                    bring_drives(sto->drives, drive_col_info, _addr);
                }
            }

            json::value volume_odata_info;
            if(get_value_from_json_key(storage_info, "Volumes", volume_odata_info))
            {
                string volume_col_uri;
                if(read_odata_id(volume_odata_info, volume_col_uri))
                {
                    json::value volume_col_info;
                    volume_col_info = get_json_info(volume_col_uri, _addr);
                    bring_volumes(sto->volumes, volume_col_info, _addr);
                }
            }

            _collection->add_member(sto);
        }
    }

}

void bring_storage_controllers(List *_list, json::value _info, string _addr)
{
    // list들은 인자로 들어오는 info(jv)가 array로 좌라락 들어있음 그래서 바로 반복문돌려
    for(int i=0; i<_info.size(); i++)
    {
        string member_uri;
        json::value storage_controller_info;

        if(read_odata_id(_info[i], member_uri))
        {
            storage_controller_info = get_json_info(member_uri, _addr);
        }
        else
            continue;

        string member_id = get_current_object_name(member_uri, "/");
        string odata = _list->odata.id + "/" + member_id;
        StorageControllers *sto_con = new StorageControllers(odata);

        get_value_from_json_key(storage_controller_info, "Id", sto_con->id);
        get_value_from_json_key(storage_controller_info, "Manufacturer", sto_con->manufacturer);
        get_value_from_json_key(storage_controller_info, "Model", sto_con->model);
        get_value_from_json_key(storage_controller_info, "SerialNumber", sto_con->serial_number);
        get_value_from_json_key(storage_controller_info, "PartNumber", sto_con->part_number);
        get_value_from_json_key(storage_controller_info, "SpeedGbps", sto_con->speed_gbps);
        get_value_from_json_key(storage_controller_info, "FirmwareVersion", sto_con->firmware_version);

        // status, identifier, 벡터 2개 해서 총 4개 블록
        json::value status_info;
        if(get_value_from_json_key(storage_controller_info, "Status", status_info))
        {
            get_value_from_json_key(status_info, "State", sto_con->status.state);
            get_value_from_json_key(status_info, "Health", sto_con->status.health);
        }

        json::value identifier_info;
        if(get_value_from_json_key(storage_controller_info, "Identifiers", identifier_info))
        {
            get_value_from_json_key(identifier_info, "DurableName", sto_con->identifier.durable_name);
            get_value_from_json_key(identifier_info, "DurableNameFormat", sto_con->identifier.durable_name_format);
        }

        json::value support_controller_protocols_info;
        if(get_value_from_json_key(storage_controller_info, "SupportedControllerProtocols", support_controller_protocols_info))
        {
            for(int i=0; i<support_controller_protocols_info.size(); i++)
            {
                sto_con->support_controller_protocols.push_back(support_controller_protocols_info[i].as_string());
            }
        }

        json::value support_device_protocols_info;
        if(get_value_from_json_key(storage_controller_info, "SupportedDeviceProtocols", support_device_protocols_info))
        {
            for(int i=0; i<support_device_protocols_info.size(); i++)
            {
                sto_con->support_device_protocols.push_back(support_device_protocols_info[i].as_string());
            }
        }

        _list->add_member(sto_con);
    }
}

void bring_drives(Collection *_collection, json::value _info, string _addr)
{
    json::value array_member;
    if(get_value_from_json_key(_info, "Members", array_member))
    {
        for(int i=0; i<array_member.size(); i++)
        {
            json::value drive_info;
            string member_uri;
            if(read_odata_id(array_member[i], member_uri))
            {
                drive_info = get_json_info(member_uri, _addr);
            }
            else
                continue;

            // drive_info로 drive만들기
            string member_id = get_current_object_name(member_uri, "/");
            string odata = _collection->odata.id + "/" + member_id;
            Drive *drive = new Drive(odata);

            get_value_from_json_key(drive_info, "Id", drive->id);
            get_value_from_json_key(drive_info, "AssetTag", drive->asset_tag);
            get_value_from_json_key(drive_info, "Description", drive->description);
            get_value_from_json_key(drive_info, "EncryptionAbility", drive->encryption_ability);
            get_value_from_json_key(drive_info, "EncryptionStatus", drive->encryption_status);
            get_value_from_json_key(drive_info, "HotspareType", drive->hotspare_type);
            get_value_from_json_key(drive_info, "Manufacturer", drive->manufacturer);
            get_value_from_json_key(drive_info, "MediaType", drive->media_type);
            get_value_from_json_key(drive_info, "Model", drive->model);
            get_value_from_json_key(drive_info, "Name", drive->name);
            get_value_from_json_key(drive_info, "SKU", drive->sku);
            get_value_from_json_key(drive_info, "StatusIndicator", drive->status_indicator);
            get_value_from_json_key(drive_info, "PartNumber", drive->part_number);
            get_value_from_json_key(drive_info, "Protocol", drive->protocol);
            get_value_from_json_key(drive_info, "Revision", drive->revision);
            get_value_from_json_key(drive_info, "SerialNumber", drive->serial_number);
            get_value_from_json_key(drive_info, "BlockSizeBytes", drive->block_size_bytes);
            get_value_from_json_key(drive_info, "CapableSpeedGbs", drive->capable_speed_Gbs);
            get_value_from_json_key(drive_info, "NegotiatedSpeedGbs", drive->negotiated_speed_Gbs);
            get_value_from_json_key(drive_info, "PredictedMediaLifeLeftPercent", drive->predicted_media_life_left_percent);
            get_value_from_json_key(drive_info, "RotationSpeedRPM", drive->rotation_speed_RPM);
            get_value_from_json_key(drive_info, "FailurePredicted", drive->failure_predicted);

            json::value identifier_vector_info;
            if(get_value_from_json_key(drive_info, "Identifiers", identifier_vector_info))
            {
                for(int i=0; i<identifier_vector_info.size(); i++)
                {
                    Identifier iden;
                    get_value_from_json_key(identifier_vector_info[i], "DurableName", iden.durable_name);
                    get_value_from_json_key(identifier_vector_info[i], "DurableNameFormat", iden.durable_name_format);

                    drive->identifier.push_back(iden);
                }
            }

            json::value physical_location_info;
            if(get_value_from_json_key(drive_info, "PhysicalLocation", physical_location_info))
            {
                get_value_from_json_key(physical_location_info, "Info", drive->physical_location.info);
                get_value_from_json_key(physical_location_info, "InfoFormat", drive->physical_location.info_format);

                json::value part_location_info;
                if(get_value_from_json_key(physical_location_info, "PartLocation", part_location_info))
                {
                    get_value_from_json_key(part_location_info, "LocationType", drive->physical_location.part_location.location_type);
                    get_value_from_json_key(part_location_info, "ServiceLabel", drive->physical_location.part_location.service_label);
                    get_value_from_json_key(part_location_info, "LocationOrdinalValue", drive->physical_location.part_location.location_ordinal_value);
                }
            }

            json::value status_info;
            if(get_value_from_json_key(drive_info, "Status", status_info))
            {
                get_value_from_json_key(status_info, "State", drive->status.state);
                get_value_from_json_key(status_info, "Health", drive->status.health);
            }

            _collection->add_member(drive);
        }
    }
}

void bring_volumes(Collection *_collection, json::value _info, string _addr)
{
    json::value array_member;
    if(get_value_from_json_key(_info, "Members", array_member))
    {
        for(int i=0; i<array_member.size(); i++)
        {
            json::value volume_info;
            string member_uri;
            if(read_odata_id(array_member[i], member_uri))
            {
                volume_info = get_json_info(member_uri, _addr);
            }
            else
                continue;

            // volume_info로 volume만들기
            string member_id = get_current_object_name(member_uri, "/");
            string odata = _collection->odata.id + "/" + member_id;
            Volume *volume = new Volume(odata);

            volume->load_json(volume_info);
            volume->odata.id = odata;

            _collection->add_member(volume);
        }
    }
}

void bring_processors(Collection *_collection, json::value _info, string _addr)
{
    json::value array_member;
    if(get_value_from_json_key(_info, "Members", array_member))
    {
        for(int i=0; i<array_member.size(); i++)
        {
            json::value processor_info;
            string member_uri;
            if(read_odata_id(array_member[i], member_uri))
            {
                processor_info = get_json_info(member_uri, _addr);
            }
            else
                continue;

            // processor_info로 processor 만들기
            string member_id = get_current_object_name(member_uri, "/");
            string odata = _collection->odata.id + "/" + member_id;
            Processors *processor = new Processors(odata);

            processor->load_json(processor_info);
            processor->odata.id = odata;

            _collection->add_member(processor);
        }
    }
}

void bring_memories(Collection *_collection, json::value _info, string _addr)
{
    json::value array_member;
    if(get_value_from_json_key(_info, "Members", array_member))
    {
        for(int i=0; i<array_member.size(); i++)
        {
            json::value memory_info;
            string member_uri;
            if(read_odata_id(array_member[i], member_uri))
            {
                memory_info = get_json_info(member_uri, _addr);
            }
            else
                continue;

            // memory_info로 memory 만들기
            string member_id = get_current_object_name(member_uri, "/");
            string odata = _collection->odata.id + "/" + member_id;
            Memory *memory = new Memory(odata);

            memory->load_json(memory_info);
            memory->odata.id = odata;

            _collection->add_member(memory);
        }
    }
}

void bring_ethernet_interfaces(Collection *_collection, json::value _info, string _addr)
{
    json::value array_member;
    if(get_value_from_json_key(_info, "Members", array_member))
    {
        for(int i=0; i<array_member.size(); i++)
        {
            json::value ethernet_info;
            string member_uri;
            if(read_odata_id(array_member[i], member_uri))
            {
                ethernet_info = get_json_info(member_uri, _addr);
            }
            else
                continue;

            // ethernet_info로 ethernet 만들기
            string member_id = get_current_object_name(member_uri, "/");
            string odata = _collection->odata.id + "/" + member_id;
            EthernetInterfaces *ethernet = new EthernetInterfaces(odata);

            ethernet->load_json(ethernet_info);
            ethernet->odata.id = odata;

            _collection->add_member(ethernet);
        }
    }
}

void bring_logservices(Collection *_collection, json::value _info, string _addr)
{
    // 얘는 엔트리가잇지
    json::value array_member;
    if(get_value_from_json_key(_info, "Members", array_member))
    {
        for(int i=0; i<array_member.size(); i++)
        {
            json::value logservice_info;
            string member_uri;
            if(read_odata_id(array_member[i], member_uri))
            {
                logservice_info = get_json_info(member_uri, _addr);
            }
            else
                continue;

            // logservice_info로 logservice 만들기
            string member_id = get_current_object_name(member_uri, "/");
            string odata = _collection->odata.id + "/" + member_id;
            LogService *logservice = new LogService(odata);

            logservice->load_json(logservice_info);
            logservice->odata.id = odata;

            logservice->entry = new Collection(logservice->odata.id + "/Entries", ODATA_LOG_ENTRY_COLLECTION_TYPE);
            logservice->entry->name = "Computer System Log Entry Collection";
            // bring_logentries 구현완료

            json::value logentry_odata_info;
            if(get_value_from_json_key(logservice_info, "Entries", logentry_odata_info))
            {
                string logentry_col_uri;
                if(read_odata_id(logentry_odata_info, logentry_col_uri))
                {
                    json::value logentry_col_info;
                    logentry_col_info = get_json_info(logentry_col_uri, _addr);
                    bring_logentries(logservice->entry, logentry_col_info, _addr);
                }
            }

            _collection->add_member(logservice);
        }
    }

}

void bring_logentries(Collection *_collection, json::value _info, string _addr)
{
    json::value array_member;
    if(get_value_from_json_key(_info, "Members", array_member))
    {
        for(int i=0; i<array_member.size(); i++)
        {
            json::value logentry_info;
            string member_uri;
            if(read_odata_id(array_member[i], member_uri))
            {
                logentry_info = get_json_info(member_uri, _addr);
            }
            else
                continue;

            // logentry_info로 logentry 만들기
            string member_id = get_current_object_name(member_uri, "/");
            string odata = _collection->odata.id + "/" + member_id;
            LogEntry *logentry = new LogEntry(odata);

            logentry->load_json(logentry_info);
            logentry->odata.id = odata;

            _collection->add_member(logentry);
        }
    }
}

void bring_simple_storages(Collection *_collection, json::value _info, string _addr)
{
    json::value array_member;
    if(get_value_from_json_key(_info, "Members", array_member))
    {
        for(int i=0; i<array_member.size(); i++)
        {
            json::value simple_storage_info;
            string member_uri;
            if(read_odata_id(array_member[i], member_uri))
            {
                simple_storage_info = get_json_info(member_uri, _addr);
            }
            else
                continue;

            // simple_storage_info로 simple_storage 만들기
            string member_id = get_current_object_name(member_uri, "/");
            string odata = _collection->odata.id + "/" + member_id;
            SimpleStorage *simple_storage = new SimpleStorage(odata);

            simple_storage->load_json(simple_storage_info);
            simple_storage->odata.id = odata;

            _collection->add_member(simple_storage);
        }
    }
}

void bring_manager(Manager *_manager, json::value _info, string _addr)
{
    string odata = _manager->odata.id;

    // 일반멤버변수
    _manager->load_json(_info);
    _manager->odata.id = odata;

    // 포인터멤버변수
    _manager->network = new NetworkProtocol(_manager->odata.id + "/NetworkProtocol");
    // 자체 처리중

    // _manager->syslog = new SyslogService(_manager->odata.id + "/SyslogService");
    // syslog처리뒤 넣기

    _manager->ethernet = new Collection(_manager->odata.id + "/EthernetInterfaces", ODATA_ETHERNET_INTERFACE_COLLECTION_TYPE);
    // bring_ethernet_interfaces 호출
    
    _manager->log_service = new Collection(_manager->odata.id + "/LogServices", ODATA_LOG_SERVICE_COLLECTION_TYPE);
    // bring_logservices 호출

    _manager->virtual_media = new Collection(_manager->odata.id + "/VirtualMedia", ODATA_VIRTUAL_MEDIA_COLLECTION_TYPE);
    // bring_virtual_media 구현완료

    // _manager->remote_account_service = new AccountService(_manager->odata.id + "/RemoteAccountService");
    // bring_account_service 구현필요
    // bring_account_service에서 AccountService 인스턴스도 생성하는식으로 감

    json::value network_odata_info;
    if(get_value_from_json_key(_info, "NetworkProtocol", network_odata_info))
    {
        string network_uri;
        if(read_odata_id(network_odata_info, network_uri))
        {
            json::value network_info;
            network_info = get_json_info(network_uri, _addr);

            string odata = _manager->network->odata.id;

            _manager->network->load_json(network_info);
            _manager->network->odata.id = odata;
            // networkprotocol에 certificates 컬렉션 들어오면 처리해줘야함
        }
    }

    json::value ethernet_odata_info;
    if(get_value_from_json_key(_info, "EthernetInterfaces", ethernet_odata_info))
    {
        string ethernet_col_uri;
        if(read_odata_id(ethernet_odata_info, ethernet_col_uri))
        {
            json::value ethernet_col_info;
            ethernet_col_info = get_json_info(ethernet_col_uri, _addr);
            bring_ethernet_interfaces(_manager->ethernet, ethernet_col_info, _addr);
        }
    }

    json::value logservice_odata_info;
    if(get_value_from_json_key(_info, "LogServices", logservice_odata_info))
    {
        string logservice_col_uri;
        if(read_odata_id(logservice_odata_info, logservice_col_uri))
        {
            json::value logservice_col_info;
            logservice_col_info = get_json_info(logservice_col_uri, _addr);
            bring_logservices(_manager->log_service, logservice_col_info, _addr);
        }
    }

    json::value vm_odata_info;
    if(get_value_from_json_key(_info, "VirtualMedia", vm_odata_info))
    {
        string vm_col_uri;
        if(read_odata_id(vm_odata_info, vm_col_uri))
        {
            json::value vm_col_info;
            vm_col_info = get_json_info(vm_col_uri, _addr);
            bring_virtual_media(_manager->virtual_media, vm_col_info, _addr);
        }
    }

    string account_service_uri = ODATA_ACCOUNT_SERVICE_ID;
    json::value account_service_info;
    account_service_info = get_json_info(account_service_uri, _addr);
    bring_account_service(_manager, account_service_info, _addr);


}

void bring_virtual_media(Collection *_collection, json::value _info, string _addr)
{
    json::value array_member;
    if(get_value_from_json_key(_info, "Members", array_member))
    {
        for(int i=0; i<array_member.size(); i++)
        {
            json::value vm_info;
            string member_uri;
            if(read_odata_id(array_member[i], member_uri))
            {
                vm_info = get_json_info(member_uri, _addr);
            }
            else
                continue;

            // vm_info로 vm 만들기
            string member_id = get_current_object_name(member_uri, "/");
            string odata = _collection->odata.id + "/" + member_id;
            VirtualMedia *vm = new VirtualMedia(odata);

            vm->load_json(vm_info);
            vm->odata.id = odata;

            _collection->add_member(vm);
        }
    }
}

void bring_account_service(Manager *_manager, json::value _info, string _addr)
{
    
    AccountService *account_service = new AccountService(_manager->odata.id + "/AccountService");
    string odata = account_service->odata.id;
    _manager->remote_account_service = account_service;

    account_service->load_json(_info);
    account_service->odata.id = odata;

    account_service->account_collection = new Collection(account_service->odata.id + "/Accounts", ODATA_ACCOUNT_COLLECTION_TYPE);
    account_service->account_collection->name = "Accounts Collection";

    account_service->role_collection = new Collection(account_service->odata.id + "/Roles", ODATA_ROLE_COLLECTION_TYPE);
    account_service->role_collection->name = "Roles Collection";

    

}


// json::value get_json_info(string _uri, string _host, string _auth_token)
json::value get_json_info(string _uri, string _host)
{
    http_client client(_host);
    http_request req(methods::GET);
    req.set_request_uri(_uri);
    // req.headers().add("X-Auth-Token", _auth_token);
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
        log(warning) << "BMC 정보요청 실패";
        log(warning) << "fail uri : " << _uri;
        return info;
    }

    // cout << res.status_code() << endl;

    info = res.extract_json().get();

    return info;

}

bool read_odata_id(json::value _jv, string &_uri)
{
    if(get_value_from_json_key(_jv, "@odata.id", _uri))
        return true;
    else
        return false;
}


// 근데 나중에 bmc추가말고 갱신하는거 실시간갱신 방식이라 햇는데 get오면 get해왔을때 달라졋으면 변경 없는게 생겻으면 생성
// 있었는데 없어진거 삭제.... post는 생성위주니까 생성 패치는 변경잉께 get이 문제겟구나
// 근데 그렇다 해도 여기서 쭈욱 타고 내려가서 다읽고 갱신하는거까진 안해도 될ㄷ스 어차피 get같은거
// 요청보내서 받아온정보를 사용하니깐 걍 갱신만 잘하면될듯 아니 근데 그러면 cmm이 bmc정보를 가지고있는 의미가 있나?