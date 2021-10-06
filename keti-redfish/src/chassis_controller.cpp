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
// void add_new_bmc(string _bmc_id, string _ip, string _port, bool _is_https, string _username, string _password)
void add_new_bmc(string _bmc_id, string _address, string _username, string _password)
// 현재 address 통째로 들어옴
// do_task_bmc 부분에서 요청 수행 후 실시간 갱신작업 추가 필요
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
    // string host;
    // if(_is_https == true)
    //     host = "https";
    // else
    //     host = "http";
    // host = host + "://" + _ip + ":" + _port;
    // log(trace) << "!!!! Host : " << host;
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
    add_system(odata_system, _address);
    // add_system(odata_system, host);
    // add_system(odata_system, host, auth_token);

    //make chassis
    add_chassis(odata_chassis, _address);
    // add_chassis(odata_chassis, host);
    // add_chassis(odata_chassis, host, auth_token);

    //make manager
    add_manager(odata_manager, _address);
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

}

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

void add_chassis(string _uri, string _host)
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

    log(trace) << "!@#$ ADD Chassis";
    log(trace) << "!@#$ New Uri : " << new_uri;

    json::value chassis_info;
    chassis_info = get_json_info(new_uri, _host);

    log(trace) << "!@#$ CHASSIS INFO START" << endl;
    cout << chassis_info << endl;
    log(trace) << "!@#$ CHASSIS INFO END" << endl;

    if(chassis_info == json::value::null())
    {
        log(warning) << "Chassis INFO is Null in Add Chassis";
        return ;
    }

    // Resource 생성
    Chassis *chassis = new Chassis(_uri);
    ((Collection *)g_record[ODATA_CHASSIS_ID]) -> add_member(chassis);

    bring_chassis(chassis, chassis_info, _host);

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
    // bring_account_service 구현완료
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

    json::value account_odata_info;
    if(get_value_from_json_key(_info, "Accounts", account_odata_info))
    {
        string account_col_uri;
        if(read_odata_id(account_odata_info, account_col_uri))
        {
            json::value account_col_info;
            account_col_info = get_json_info(account_col_uri, _addr);
            bring_accounts(account_service->account_collection, account_col_info, _addr);
        }
    }

    json::value role_odata_info;
    if(get_value_from_json_key(_info, "Roles", role_odata_info))
    {
        string role_col_uri;
        if(read_odata_id(role_odata_info, role_col_uri))
        {
            json::value role_col_info;
            role_col_info = get_json_info(role_col_uri, _addr);
            bring_roles(account_service->role_collection, role_col_info, _addr);
        }
    }

}

void bring_accounts(Collection *_collection, json::value _info, string _addr)
{
    json::value array_member;
    if(get_value_from_json_key(_info, "Members", array_member))
    {
        for(int i=0; i<array_member.size(); i++)
        {
            json::value account_info;
            string member_uri;
            if(read_odata_id(array_member[i], member_uri))
            {
                account_info = get_json_info(member_uri, _addr);
            }
            else
                continue;

            // account_info로 account 만들기
            string member_id = get_current_object_name(member_uri, "/");
            string odata = _collection->odata.id + "/" + member_id;
            Account *account = new Account(odata);

            account->load_json(account_info);
            account->odata.id = odata;

            _collection->add_member(account);
        }
    }
}

void bring_roles(Collection *_collection, json::value _info, string _addr)
{
    json::value array_member;
    if(get_value_from_json_key(_info, "Members", array_member))
    {
        for(int i=0; i<array_member.size(); i++)
        {
            json::value role_info;
            string member_uri;
            if(read_odata_id(array_member[i], member_uri))
            {
                role_info = get_json_info(member_uri, _addr);
            }
            else
                continue;

            // role_info로 role 만들기
            string member_id = get_current_object_name(member_uri, "/");
            string odata = _collection->odata.id + "/" + member_id;
            Role *role = new Role(odata);

            role->load_json(role_info);
            role->odata.id = odata;

            _collection->add_member(role);
        }
    }
}

void bring_chassis(Chassis *_chassis, json::value _info, string _addr)
{
    string odata = _chassis->odata.id;

    // 일반멤버변수
    _chassis->load_json(_info);
    _chassis->odata.id = odata;

    // 포인터멤버변수
    _chassis->thermal = new Thermal(_chassis->odata.id + "/Thermal");
    _chassis->thermal->name = "Chassis Thermal";

    _chassis->power = new Power(_chassis->odata.id + "/Power");
    _chassis->power->name = "Chassis Power";

    _chassis->storage = new Collection(_chassis->odata.id + "/Storage", ODATA_STORAGE_COLLECTION_TYPE);
    _chassis->storage->name = "Chassis Storage Collection";

    _chassis->sensors = new Collection(_chassis->odata.id + "/Sensors", ODATA_SENSOR_COLLECTION_TYPE);
    _chassis->sensors->name = "Sensor Collection";

    _chassis->log_service = new Collection(_chassis->odata.id + "/LogServices", ODATA_LOG_SERVICE_COLLECTION_TYPE);
    _chassis->log_service->name = "Chassis Log Service Collection";

    json::value thermal_odata_info;
    if(get_value_from_json_key(_info, "Thermal", thermal_odata_info))
    {
        string thermal_uri;
        if(read_odata_id(thermal_odata_info, thermal_uri))
        {
            json::value thermal_info;
            thermal_info = get_json_info(thermal_uri, _addr);

            string odata = _chassis->thermal->odata.id;

            _chassis->thermal->load_json(thermal_info);
            _chassis->thermal->odata.id = odata;
            _chassis->thermal->name = "Chassis Thermal";

            _chassis->thermal->temperatures = new List(_chassis->thermal->odata.id + "/Temperatures", TEMPERATURE_TYPE);
            _chassis->thermal->temperatures->name = "Chassis Temperatures";

            _chassis->thermal->fans = new List(_chassis->thermal->odata.id + "/Fans", FAN_TYPE);
            _chassis->thermal->fans->name = "Chassis Fans";

            json::value temperatures_list_info;
            if(get_value_from_json_key(thermal_info, "Temperatures", temperatures_list_info))
            {
                if(temperatures_list_info.size() > 0)
                    bring_temperatures(_chassis->thermal->temperatures, temperatures_list_info, _addr);
            }

            json::value fans_list_info;
            if(get_value_from_json_key(thermal_info, "Fans", fans_list_info))
            {
                if(fans_list_info.size() > 0)
                    bring_fans(_chassis->thermal->fans, fans_list_info, _addr);
            }

        }
    }

    json::value power_odata_info;
    if(get_value_from_json_key(_info, "Power", power_odata_info))
    {
        string power_uri;
        if(read_odata_id(power_odata_info, power_uri))
        {
            json::value power_info;
            power_info = get_json_info(power_uri, _addr);

            string odata = _chassis->power->odata.id;

            _chassis->power->load_json(power_info);
            _chassis->power->odata.id = odata;
            _chassis->power->name = "Chassis Power";

            _chassis->power->power_control = new List(_chassis->power->odata.id + "/PowerControl", POWER_CONTROL_TYPE);
            _chassis->power->power_control->name = "Chassis PowerControl";

            _chassis->power->voltages = new List(_chassis->power->odata.id + "/Voltages", VOLTAGE_TYPE);
            _chassis->power->voltages->name = "Chassis Voltages";

            _chassis->power->power_supplies = new List(_chassis->power->odata.id + "/PowerSupplies", POWER_SUPPLY_TYPE);
            _chassis->power->power_supplies->name = "Chassis PowerSupplies";

            json::value power_control_list_info;
            if(get_value_from_json_key(power_info, "PowerControl", power_control_list_info))
            {
                if(power_control_list_info.size() > 0)
                    bring_power_controls(_chassis->power->power_control, power_control_list_info, _addr);
            }

            json::value voltage_list_info;
            if(get_value_from_json_key(power_info, "Voltages", voltage_list_info))
            {
                if(voltage_list_info.size() > 0)
                    bring_voltages(_chassis->power->voltages, voltage_list_info, _addr);
            }

            json::value power_supply_list_info;
            if(get_value_from_json_key(power_info, "PowerSupplies", power_supply_list_info))
            {
                if(power_supply_list_info.size() > 0)
                    bring_power_supplies(_chassis->power->power_supplies, power_supply_list_info, _addr);
            }

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
            bring_storages(_chassis->storage, storage_col_info, _addr);
        }
    }

    json::value sensor_odata_info;
    if(get_value_from_json_key(_info, "Sensors", sensor_odata_info))
    {
        string sensor_col_uri;
        if(read_odata_id(sensor_odata_info, sensor_col_uri))
        {
            json::value sensor_col_info;
            sensor_col_info = get_json_info(sensor_col_uri, _addr);
            bring_sensors(_chassis->sensors, sensor_col_info, _addr);
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
            bring_logservices(_chassis->log_service, logservice_col_info, _addr);
        }
    }
}

void bring_temperatures(List *_list, json::value _info, string _addr)
{
    for(int i=0; i<_info.size(); i++)
    {
        string member_uri;
        json::value temperature_info;

        if(read_odata_id(_info[i], member_uri))
        {
            temperature_info = get_json_info(member_uri, _addr);
        }
        else
            continue;

        string member_id = get_current_object_name(member_uri, "/");
        string odata = _list->odata.id + "/" + member_id;
        Temperature *temp = new Temperature(odata);

        temp->load_json(temperature_info);
        temp->odata.id = odata;

        _list->add_member(temp);
    }
}

void bring_fans(List *_list, json::value _info, string _addr)
{
    for(int i=0; i<_info.size(); i++)
    {
        string member_uri;
        json::value fan_info;

        if(read_odata_id(_info[i], member_uri))
        {
            fan_info = get_json_info(member_uri, _addr);
        }
        else
            continue;

        string member_id = get_current_object_name(member_uri, "/");
        string odata = _list->odata.id + "/" + member_id;
        Fan *fan = new Fan(odata);

        fan->load_json(fan_info);
        fan->odata.id = odata;

        _list->add_member(fan);
    }
}

void bring_power_controls(List *_list, json::value _info, string _addr)
{
    for(int i=0; i<_info.size(); i++)
    {
        string member_uri;
        json::value power_control_info;

        if(read_odata_id(_info[i], member_uri))
        {
            power_control_info = get_json_info(member_uri, _addr);
        }
        else
            continue;

        string member_id = get_current_object_name(member_uri, "/");
        string odata = _list->odata.id + "/" + member_id;
        PowerControl *pc = new PowerControl(odata);

        pc->load_json(power_control_info);
        pc->odata.id = odata;

        _list->add_member(pc);
    }
}

void bring_voltages(List *_list, json::value _info, string _addr)
{
    for(int i=0; i<_info.size(); i++)
    {
        string member_uri;
        json::value voltage_info;

        if(read_odata_id(_info[i], member_uri))
        {
            voltage_info = get_json_info(member_uri, _addr);
        }
        else
            continue;

        string member_id = get_current_object_name(member_uri, "/");
        string odata = _list->odata.id + "/" + member_id;
        Voltage *vol = new Voltage(odata);

        vol->load_json(voltage_info);
        vol->odata.id = odata;

        _list->add_member(vol);
    }
}

void bring_power_supplies(List *_list, json::value _info, string _addr)
{
    for(int i=0; i<_info.size(); i++)
    {
        string member_uri;
        json::value power_supply_info;

        if(read_odata_id(_info[i], member_uri))
        {
            power_supply_info = get_json_info(member_uri, _addr);
        }
        else
            continue;

        string member_id = get_current_object_name(member_uri, "/");
        string odata = _list->odata.id + "/" + member_id;
        PowerSupply *ps = new PowerSupply(odata);

        ps->load_json(power_supply_info);
        ps->odata.id = odata;

        _list->add_member(ps);
    }

}

void bring_sensors(Collection *_collection, json::value _info, string _addr)
{
    json::value array_member;
    if(get_value_from_json_key(_info, "Members", array_member))
    {
        for(int i=0; i<array_member.size(); i++)
        {
            json::value sensor_info;
            string member_uri;
            if(read_odata_id(array_member[i], member_uri))
            {
                sensor_info = get_json_info(member_uri, _addr);
            }
            else
                continue;

            // sensor_info로 sensor 만들기
            string member_id = get_current_object_name(member_uri, "/");
            string odata = _collection->odata.id + "/" + member_id;
            Sensor *sensor = new Sensor(odata);

            sensor->load_json(sensor_info);
            sensor->odata.id = odata;

            _collection->add_member(sensor);
        }
    }
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