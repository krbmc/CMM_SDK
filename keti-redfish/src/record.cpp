#include "stdafx.hpp"
#include "resource.hpp"

extern unordered_map<string, Resource *> g_record;
vector<Resource*> gc;

/**
 * @brief Find uri in to record(unordered_map)
 * 
 * @param _uri Open data id of resource
 * @return true 
 * @return false 
 */
bool record_is_exist(const string _uri)
{
    if (g_record.find(_uri) != g_record.end())
        return true;
    return false;
}

json::value record_get_json(const string _uri)
{
    json::value j;
    if(!record_is_exist(_uri))
        return j;

    // log(info) << "record_get_json : " <<_uri;

    switch (g_record[_uri]->type)
    {
    case SERVICE_ROOT_TYPE:
        j = ((ServiceRoot *)g_record[_uri])->get_json();
        break;
    case COLLECTION_TYPE:
        j = ((Collection *)g_record[_uri])->get_json();
        break;
    case LIST_TYPE:
        j = ((List *)g_record[_uri])->get_json();
        break;
    // case ACTIONS_TYPE:
    //     j = ((Actions *)g_record[_uri])->get_json();
    //     break;
    case SYSTEM_TYPE:
        j = ((Systems *)g_record[_uri])->get_json();
        break;
    case PROCESSOR_TYPE:
        j = ((Processors *)g_record[_uri])->get_json();
        break;
    // case PROCESSOR_SUMMARY_TYPE:
    //     j = ((ProcessorSummary *)g_record[_uri])->get_json();
    //     break;
    // case STORAGE_TYPE:
    //     j = ((Storage *)g_record[_uri])->get_json();
    //     break;
    case STORAGE_CONTROLLER_TYPE:
        j = ((StorageControllers *)g_record[_uri])->get_json();
        break;
    case BIOS_TYPE:
        j = ((Bios *)g_record[_uri])->get_json();
        break;
    case MEMORY_TYPE:
        j = ((Memory *)g_record[_uri])->get_json();
        break;
    case SIMPLE_STORAGE_TYPE:
        j = ((SimpleStorage *)g_record[_uri])->get_json();
        break;
    case CHASSIS_TYPE:
        j = ((Chassis *)g_record[_uri])->get_json();
        break;
    case THERMAL_TYPE:
        j = ((Thermal *)g_record[_uri])->get_json();
        break;
    case TEMPERATURE_TYPE:
        j = ((Temperature *)g_record[_uri])->get_json();
        break;
    case FAN_TYPE:
        j = ((Fan *)g_record[_uri])->get_json();
        break;
    case SENSOR_TYPE:
        j = ((Sensor *)g_record[_uri])->get_json();
        break;
    case POWER_TYPE:
        j = ((Power *)g_record[_uri])->get_json();
        break;
    case POWER_CONTROL_TYPE:
        j = ((PowerControl *)g_record[_uri])->get_json();
        break;
    case VOLTAGE_TYPE:
        j = ((Voltage *)g_record[_uri])->get_json();
        break;
    case POWER_SUPPLY_TYPE:
        j = ((PowerSupply *)g_record[_uri])->get_json();
        break;
    case MANAGER_TYPE:
        j = ((Manager *)g_record[_uri])->get_json();
        break;
    case ETHERNET_INTERFACE_TYPE:
        j = ((EthernetInterfaces *)g_record[_uri])->get_json();
        break;
    case NETWORK_PROTOCOL_TYPE:
        j = ((NetworkProtocol *)g_record[_uri])->get_json();
        break;
    case LOG_SERVICE_TYPE:
        j = ((LogService *)g_record[_uri])->get_json();
        break;
    case LOG_ENTRY_TYPE:
        j = ((LogEntry *)g_record[_uri])->get_json();
        break;
    case TASK_SERVICE_TYPE:
        j = ((TaskService *)g_record[_uri])->get_json();
        break;
    case TASK_TYPE:
        j = ((Task *)g_record[_uri])->get_json();
        break;
    case SESSION_SERVICE_TYPE:
        j = ((SessionService *)g_record[_uri])->get_json();
        break;
    case SESSION_TYPE:
        j = ((Session *)g_record[_uri])->get_json();
        break;
    case ACCOUNT_SERVICE_TYPE:
        j = ((AccountService *)g_record[_uri])->get_json();
        break;
    case ACCOUNT_TYPE:
        j = ((Account *)g_record[_uri])->get_json();
        break;
    case ROLE_TYPE:
        j = ((Role *)g_record[_uri])->get_json();
        break;
    case EVENT_SERVICE_TYPE:
        j = ((EventService *)g_record[_uri])->get_json();
        break;
    case EVENT_DESTINATION_TYPE:
        j = ((EventDestination *)g_record[_uri])->get_json();
        break;
    case UPDATE_SERVICE_TYPE:
        j = ((UpdateService *)g_record[_uri])->get_json();
        break;
    case SOFTWARE_INVENTORY_TYPE:
        j = ((SoftwareInventory *)g_record[_uri])->get_json();
        break;
    case CERTIFICATE_TYPE:
        j = ((Certificate *)g_record[_uri])->get_json();
        break;
    case CERTIFICATE_SERVICE_TYPE:
        j = ((CertificateService *)g_record[_uri])->get_json();
        break;
    default:
        break;
    }
    return j;
}

/**
 * @brief Load json file and assign to target resource
 * @details #1 모든 .json 파일을 scan하여 Resource* Object로 생성.
 *          #2 ServiceRoot init으로 생성되었지만, 해당 함수에서 덮어쓰기 될 Object들 delete.
 *          #3 생성한 Resource* Object들의 type값에 따라 상응하는 Object로 변환.
 *          #4 의존성 있는 Object들 연결.
 *          #5 Resource* Object들 모두 delete
 * @return true 
 * @return false 
 */
bool record_load_json(void)
{
    vector<Resource*> dependency_object;

    record_init_load("/redfish");
    log(info) << "init load complete"; // #1
    
    // #2. => service root init 하기 전에 record init 하고, record가 없는 객체만 service root에서 생성하도록 구현하는 것이 효율적으로 보입니다.. 네~ 효율이 중요하죠..           

    for (auto it = g_record.begin(); it != g_record.end(); it++){
        json::value j;
        
        // 파일을 읽어 기본 resource 정보를 읽음. type에 한하여 없는 경우, 읽지 않음.
        if (!(it->second->load_json_from_file(j))){
            log(warning) << "invalid file";
            gc.push_back(it->second);
            continue;
        }
        // else{
        //     cout << "file read complete" << endl;
        // }
        
        // Actions 예외처리
        string uri = it->second->odata.id;
        string resource_type1 = get_current_object_name(uri, "/");
        string resource_type2 = get_current_object_name(get_parent_object_uri(uri, "/"), "/");
        
        // cout << "type 1 : " << resource_type1 << endl << "type 2 : " << resource_type2 << endl;

        if(resource_type2 == "Actions"){
            // cout << "Action skip" << endl;
            continue;
        }else{
            // cout << "Action check complete" << endl;
        }
        
        // #3
        switch (it->second->type)
        {
            case SERVICE_ROOT_TYPE:{
                break;
            }
            case COLLECTION_TYPE:{
                string this_odata_id = it->second->odata.id;
                gc.push_back(it->second);
                Collection *collection = new Collection(this_odata_id);
                if (!collection->load_json(j))
                    log(warning) << "load collection failed";
                else
                    dependency_object.push_back(collection);
                break;
            }
            case LIST_TYPE:{
                string this_odata_id = it->second->odata.id;
                gc.push_back(it->second);
                List *list = new List(this_odata_id);
                if (!list->load_json(j))
                    log(warning) << "load List failed";
                else
                    dependency_object.push_back(list);
                break;
            }
            case SYSTEM_TYPE:{
                string this_odata_id = it->second->odata.id;
                gc.push_back(it->second);
                Systems *systems = new Systems(this_odata_id);
                if (!systems->load_json(j))
                    fprintf(stderr, "load System failed\n");
                else
                    dependency_object.push_back(systems);
                break;
            }
            // case STORAGE_TYPE:{
            //     string this_odata_id = it->second->odata.id;
            //     gc.push_back(it->second);
            //     Storage *storage = new Storage(this_odata_id);
            //     if (!storage->load_json(j))
            //         log(warning) << "load Storage failed";
            //     else
            //         dependency_object.push_back(storage);
            //     break;
            // }
            // case STORAGE_CONTROLLER_TYPE:{
            //     string this_odata_id = it->second->odata.id;
            //     gc.push_back(it->second);
            //     StorageControllers *storage_controllers = new StorageControllers(this_odata_id);
            //     if (!storage_controllers->load_json(j))
            //         log(warning) << "load Storage Controllers failed";
            //     else
            //         dependency_object.push_back(storage_controllers);
            //     break;
            // }
            case SIMPLE_STORAGE_TYPE:{
                string this_odata_id = it->second->odata.id;
                gc.push_back(it->second);
                SimpleStorage *simple_storage = new SimpleStorage(this_odata_id);
                if (!simple_storage->load_json(j))
                    log(warning) << "load Simple Storage failed";
                else
                    dependency_object.push_back(simple_storage);
                break;
            }
            case BIOS_TYPE:{
                string this_odata_id = it->second->odata.id;
                gc.push_back(it->second);
                Bios *bios = new Bios(this_odata_id);
                if (!bios->load_json(j))
                    log(warning) << "load Bios failed";
                else
                    dependency_object.push_back(bios);
                break;
            }
            case PROCESSOR_TYPE:{
                string this_odata_id = it->second->odata.id;
                gc.push_back(it->second);
                Processors *processors = new Processors(this_odata_id);
                if (!processors->load_json(j))
                    log(warning) << "load Processor failed";
                else
                    dependency_object.push_back(processors);
                break;
            }
            case MEMORY_TYPE:{
                string this_odata_id = it->second->odata.id;
                gc.push_back(it->second);
                Memory *memory = new Memory(this_odata_id);
                if (!memory->load_json(j))
                    log(warning) << "load Memory failed";
                else
                    dependency_object.push_back(memory);
                break;    
            }
            // case PROCESSOR_SUMMARY_TYPE:{

            // }
            // case NETWORK_INTERFACE_TYPE:{
            // }
            case CHASSIS_TYPE:{
                string this_odata_id = it->second->odata.id;
                gc.push_back(it->second);
                Chassis *chassis = new Chassis(this_odata_id);
                if (!chassis->load_json(j))
                    log(warning) << "load Chassis failed";
                else
                    dependency_object.push_back(chassis);
                break;
            }
            case THERMAL_TYPE:{
                string this_odata_id = it->second->odata.id;
                gc.push_back(it->second);
                Thermal *thermal = new Thermal(this_odata_id);
                if (!thermal->load_json(j))
                    log(warning) << "load Thermal failed";
                else
                    dependency_object.push_back(thermal);
                break;
            }
            case TEMPERATURE_TYPE:{
                string this_odata_id = it->second->odata.id;
                gc.push_back(it->second);
                Temperature *temperature = new Temperature(this_odata_id);
                if (!temperature->load_json(j))
                    log(warning) << "load Temperature failed";
                else
                    dependency_object.push_back(temperature);
                break;
            }
            case FAN_TYPE:{
                string this_odata_id = it->second->odata.id;
                gc.push_back(it->second);
                Fan *fan = new Fan(this_odata_id);
                if (!fan->load_json(j))
                    log(warning) << "load Fan failed";
                else
                    dependency_object.push_back(fan);
                break;
            }
            case SENSOR_TYPE:{
                string this_odata_id = it->second->odata.id;
                gc.push_back(it->second);
                Sensor *sensor = new Sensor(this_odata_id);
                if (!sensor->load_json(j))
                    log(warning) << "load Sensor failed";
                else
                    dependency_object.push_back(sensor);
                break;    
            }
            case POWER_TYPE:{
                string this_odata_id = it->second->odata.id;
                gc.push_back(it->second);
                Power *power = new Power(this_odata_id);
                if (!power->load_json(j))
                    log(warning) << "load Power failed";
                else
                    dependency_object.push_back(power);
                break;
            }
            case POWER_CONTROL_TYPE:{
                string this_odata_id = it->second->odata.id;
                gc.push_back(it->second);
                PowerControl *power_control = new PowerControl(this_odata_id);
                if (!power_control->load_json(j))
                    log(warning) << "load Power Control failed";
                else
                    dependency_object.push_back(power_control);
                break;
            }
            case VOLTAGE_TYPE:{
                string this_odata_id = it->second->odata.id;
                gc.push_back(it->second);
                Voltage *voltage = new Voltage(this_odata_id);
                if (!voltage->load_json(j))
                    log(warning) << "load Voltage failed";
                else
                    dependency_object.push_back(voltage);
                break;    
            }
            case POWER_SUPPLY_TYPE:{
                string this_odata_id = it->second->odata.id;
                gc.push_back(it->second);
                PowerSupply *power_supply = new PowerSupply(this_odata_id);
                if (!power_supply->load_json(j))
                    log(warning) << "load Power Supply failed";
                else
                    dependency_object.push_back(power_supply);
                break;    
            }
            case MANAGER_TYPE:{
                string this_odata_id = it->second->odata.id;
                gc.push_back(it->second);
                Manager *manager = new Manager(this_odata_id);
                if (!manager->load_json(j))
                    log(warning) << "load Manager failed";
                else
                    dependency_object.push_back(manager);
                break;
            }
            // case NETWORK_PROTOCOL_TYPE:{
            //     string this_odata_id = it->second->odata.id;
            //     gc.push_back(it->second);
            //     NetworkProtocol *network_protocol = new NetworkProtocol(this_odata_id);
            //     if (!network_protocol->load_json(j))
            //         log(warning) << "load Network Protocol failed";
            //     break;    
            // }
            case ETHERNET_INTERFACE_TYPE:{
                string this_odata_id = it->second->odata.id;
                gc.push_back(it->second);
                EthernetInterfaces *ether_interfaces = new EthernetInterfaces(this_odata_id);
                if (!ether_interfaces->load_json(j))
                    log(warning) << "load Ethernet interfaces failed";
                else
                    dependency_object.push_back(ether_interfaces);
                break;
            }
            case LOG_SERVICE_TYPE:{
                string this_odata_id = it->second->odata.id;
                gc.push_back(it->second);
                LogService *log_service = new LogService(this_odata_id);
                if (!log_service->load_json(j))
                    log(warning) << "load Log Service failed";
                else
                    dependency_object.push_back(log_service);
                break;    
            }
            case LOG_ENTRY_TYPE:{
                string this_odata_id = it->second->odata.id;
                gc.push_back(it->second);
                LogEntry *log_entry = new LogEntry(this_odata_id);
                if (!log_entry->load_json(j))
                    log(warning) << "load Log Entry failed";
                else
                    dependency_object.push_back(log_entry);
                break;
            }
            case TASK_SERVICE_TYPE:{
                gc.push_back(it->second);
                TaskService *task_service = new TaskService();
                if (!task_service->load_json(j))
                    log(warning) << "load Task Service failed";
                else
                    dependency_object.push_back(task_service);
                break;    
            }
            case TASK_TYPE:{
                string this_odata_id = it->second->odata.id;
                gc.push_back(it->second);
                Task *task = new Task(this_odata_id);
                if (!task->load_json(j))
                    log(warning) << "load Task failed";
                else
                    dependency_object.push_back(task);
                break;    
            }
            case SESSION_SERVICE_TYPE:{
                gc.push_back(it->second);
                SessionService *session_service = new SessionService();
                if (!session_service->load_json(j))
                    log(warning) << "load Session Service failed";
                else
                    dependency_object.push_back(session_service);
                break;    
            }
            case SESSION_TYPE:{
                string this_odata_id = it->second->odata.id;
                gc.push_back(it->second);
                Session *session = new Session(this_odata_id);
                if (!session->load_json(j))
                    log(warning) << "load Session failed";
                else
                    dependency_object.push_back(session);
                break;
            }
            case ACCOUNT_SERVICE_TYPE:{
                gc.push_back(it->second);
                AccountService *account_service = new AccountService();
                if (!account_service->load_json(j))
                    log(warning) << "load Account Service failed";
                else
                    dependency_object.push_back(account_service);
                break;    
            }
            case ACCOUNT_TYPE:{
                string this_odata_id = it->second->odata.id;
                gc.push_back(it->second);
                Account *account = new Account(this_odata_id);
                if (!account->load_json(j))
                    log(warning) << "load Account failed";
                else
                    dependency_object.push_back(account);
                break;
            }
            case ROLE_TYPE:{
                string this_odata_id = it->second->odata.id;
                gc.push_back(it->second);
                Role *role = new Role(this_odata_id);
                if (!role->load_json(j))
                    log(warning) << "load Role failed";
                else
                    dependency_object.push_back(role);
                break;
            }
            case EVENT_SERVICE_TYPE:{
                gc.push_back(it->second);
                EventService *event_service = new EventService();
                if (!event_service->load_json(j))
                    log(warning) << "load Event Service failed";
                else
                    dependency_object.push_back(event_service);
                break;    
            }
            case EVENT_DESTINATION_TYPE:{
                string this_odata_id = it->second->odata.id;
                gc.push_back(it->second);
                EventDestination *event_destination = new EventDestination(this_odata_id);
                if (!event_destination->load_json(j))
                    log(warning) << "load Event Destination failed";
                else
                    dependency_object.push_back(event_destination);
                break;
            }
            case UPDATE_SERVICE_TYPE:{
                string this_odata_id = it->second->odata.id;
                gc.push_back(it->second);
                UpdateService *update_service = new UpdateService(this_odata_id);
                if (!update_service->load_json(j))
                    log(warning) << "load Update Service failed";
                else
                    dependency_object.push_back(update_service);
                break;
            }
            case SOFTWARE_INVENTORY_TYPE:{
                string this_odata_id = it->second->odata.id;
                gc.push_back(it->second);
                SoftwareInventory *software_inventory = new SoftwareInventory(this_odata_id);
                if (!software_inventory->load_json(j))
                    log(warning) << "load Software Inventory failed";
                else
                    dependency_object.push_back(software_inventory);
                break;
            }
            case CERTIFICATE_SERVICE_TYPE:{
                gc.push_back(it->second);
                CertificateService *certificate_service = new CertificateService();
                if (!certificate_service->load_json(j))
                    log(warning) << "load Certificate Service failed";
                else
                    dependency_object.push_back(certificate_service);
                break;
            }
            case CERTIFICATE_TYPE:{
                string this_odata_id = it->second->odata.id;
                gc.push_back(it->second);
                Certificate *cert = new Certificate(this_odata_id);
                if (!cert->load_json(j))
                    log(warning) << "load Certificate failed";
                else
                    dependency_object.push_back(cert);
                break;
            } 
            default:
                log(warning) << "NOT IMPLEMETED IN LOAD JSON : " << it->second->odata.id;
                gc.push_back(it->second);
                break;
        }
    }

    // #4
    for (auto object : dependency_object)
        dependency_injection(object);
    
    log(info) << "after 4";
    // #5
    for (auto garbage : gc)
        delete(garbage);
    
    log(info) << "after 5";
    
    log(info) << "garbage collection complete";
    return true;
}

bool record_save_json(void)
{
    for (auto it = g_record.begin(); it != g_record.end(); it++)
    {
        log(info) << "uri : " << it->first << ", resource address : " << it->second << endl;
        it->second->save_json();
    }
    return true;
}

/**
 * @brief Print sorted keys of record
 * 
 */
void record_print(void)
{
    vector<string> keys;
    for (auto it = g_record.begin(); it != g_record.end(); it++)
        keys.push_back(it->first);
    sort(keys.begin(), keys.end(), comp);
    for (unsigned int i = 0; i < keys.size(); i++)
        log(info) << keys[i];
}

/**
 * @brief g_record init
 * @details 존재하는 .json 파일 전부를 읽어 type값과 odata.id를 얻어 각자의 Resource 생성. g_record에 저장.
 * @authors 강, 김
 */
void record_init_load(string _path)
{
    struct dirent **namelist;
    struct stat statbuf;
    int count;

    if((count = scandir(_path.c_str(), &namelist, NULL, alphasort)) == -1)
    {
        log(warning) << "Scandir Error in " << _path; 
        return ;
    }

    for(int i=0; i<count; i++)
    {
        if(strcmp(namelist[i]->d_name, ".") == 0 || strcmp(namelist[i]->d_name, "..") == 0)
            continue;

        string str = _path;
        string name = namelist[i]->d_name;
        str = str + "/" + name;
        
        
        stat(str.c_str(), &statbuf);
        if(S_ISDIR(statbuf.st_mode))
        {
            record_init_load(str);
        }
        else
        {
            string tmp;
            string sub;
            string list_check;
            // 해당 파일은 .json이 붙을수없음
            if(str.length() < 6)
            {
                continue;
            }

            tmp = str.substr(str.length()-5);
            sub = str.substr(0, str.length()-5);
            list_check = get_parent_object_uri(sub, "/");
            
            if(tmp == ".json") 
            {
                // service root 무시
                if (sub == ODATA_SERVICE_ROOT_ID)
                    continue;

                // 모든 파일 resource로 생성
                // log(info) << sub << " : 파일 명";   
                Resource *res = new Resource(sub);
                g_record[sub] = res;   
            }
        }
    }

    for(int i=0; i<count; i++)
    {
        free(namelist[i]);
    }

    free(namelist);

    return ;
}

/**
 * @brief Dependency Injection to all Resource
 * @details 모든 연결된 Resource들을 연결시키는 작업 수행
 * @authors 김
 */
void dependency_injection(Resource *res)
{
    string id = res->odata.id;
    string parent_object_id = get_parent_object_uri(id, "/");
    vector<Resource *> collection;
    vector<Resource *> list;

    switch (res->type)
    {
        //serviceroot
        case ACCOUNT_SERVICE_TYPE: // BMC Manager && ServiceRoot
            ((ServiceRoot *)g_record[parent_object_id])->account_service = (AccountService *)res;
            break;
        case SESSION_SERVICE_TYPE:
            ((ServiceRoot *)g_record[parent_object_id])->session_service = (SessionService *)res;
            break;
        case TASK_SERVICE_TYPE:
            ((ServiceRoot *)g_record[parent_object_id])->task_service = (TaskService *)res;
            break;
        case CERTIFICATE_SERVICE_TYPE:
            ((ServiceRoot *)g_record[parent_object_id])->certificate_service = (CertificateService *)res;
            break;
        case EVENT_SERVICE_TYPE:
            ((ServiceRoot *)g_record[parent_object_id])->event_service = (EventService *)res;
            break;
        case COLLECTION_TYPE:{
        /*  == Collection location == 
            service root collection : systems, chassis, manager, update
            systems collection : network_interfaces, storage, processor, memory, ethernet, log_service, simple_storage
            chassis collection : sensors
            manager collection : ethernet_interfaces, log_service
            log_service collection : log_entry 
            task_service collection : task
            session_service collection : session
            account_service collection : account, role
            event_service collection : event_destination
            update_service collection : software_inventory, frimware_inventory
            certificate_service collection : certificate
        */
            switch (g_record[parent_object_id]->type){
                case SERVICE_ROOT_TYPE:
                    if (res->odata.type == ODATA_SYSTEM_COLLECTION_TYPE){
                        ((ServiceRoot *)g_record[parent_object_id])->system_collection = (Collection *)res;
                    }else if (res->odata.type == ODATA_CHASSIS_COLLECTION_TYPE){
                        ((ServiceRoot *)g_record[parent_object_id])->chassis_collection = (Collection *)res;
                    }else if (res->odata.type == ODATA_MANAGER_COLLECTION_TYPE){
                        ((ServiceRoot *)g_record[parent_object_id])->manager_collection = (Collection *)res;
                    }else if (res->odata.type == ODATA_UPDATE_SERVICE_COLLECTION_TYPE){
                        ((ServiceRoot *)g_record[parent_object_id])->update_service = (Collection *)res;
                    }else{
                        log(warning) << "\t\t dy : what is this in service root? : " << res->odata.id << " type : " << res->odata.type;
                    }
                    break;
                case SYSTEM_TYPE:
                    if (res->odata.type == ODATA_NETWORK_INTERFACE_TYPE){
                        ((Systems *)g_record[parent_object_id])->network = (Collection *)res;
                    }else if (res->odata.type == ODATA_STORAGE_COLLECTION_TYPE){
                        ((Systems *)g_record[parent_object_id])->storage = (Collection *)res;
                    }else if (res->odata.type == ODATA_PROCESSOR_COLLECTION_TYPE){
                        ((Systems *)g_record[parent_object_id])->processor = (Collection *)res;
                    }else if (res->odata.type == ODATA_MEMORY_COLLECTION_TYPE){
                        ((Systems *)g_record[parent_object_id])->memory = (Collection *)res;
                    }else if (res->odata.type == ODATA_ETHERNET_INTERFACE_COLLECTION_TYPE){
                        ((Systems *)g_record[parent_object_id])->ethernet = (Collection *)res;
                    }else if (res->odata.type == ODATA_LOG_SERVICE_COLLECTION_TYPE){
                        ((Systems *)g_record[parent_object_id])->log_service = (Collection *)res;
                    }else if (res->odata.type == ODATA_SIMPLE_STORAGE_COLLECTION_TYPE){
                        ((Systems *)g_record[parent_object_id])->simple_storage = (Collection *)res;
                    }else{
                        log(warning) << "\t\t dy : what is this in system? : " << res->odata.id << " type : " << res->odata.type;
                    }
                    break;
                case CHASSIS_TYPE:
                    if (res->odata.type == ODATA_SENSOR_COLLECTION_TYPE){
                        ((Chassis *)g_record[parent_object_id])->sensors = (Collection *)res;
                    }else{
                        log(warning) << "\t\t dy : what is this in chassis? : " << res->odata.id << " type : " << res->odata.type;
                    }
                    break;
                case MANAGER_TYPE:
                    if (res->odata.type == ODATA_ETHERNET_INTERFACE_COLLECTION_TYPE){
                        ((Manager *)g_record[parent_object_id])->ethernet = (Collection *)res;
                    }else if (res->odata.type == ODATA_LOG_SERVICE_COLLECTION_TYPE){
                        ((Manager *)g_record[parent_object_id])->log_service = (Collection *)res;
                    }else{
                        log(warning) << "\t\t dy : what is this in manager? : " << res->odata.id << " type : " << res->odata.type;
                    }
                    break;
                case LOG_SERVICE_TYPE:
                    if (res->odata.type == ODATA_LOG_ENTRY_COLLECTION_TYPE){
                        ((LogService *)g_record[parent_object_id])->entry = (Collection *)res;
                    }else{
                        log(warning) << "\t\t dy : what is this in log_service? : " << res->odata.id << " type : " << res->odata.type;
                    }
                    break;
                case TASK_SERVICE_TYPE:
                    if (res->odata.type == ODATA_TASK_COLLECTION_TYPE){
                        ((TaskService *)g_record[parent_object_id])->task_collection = (Collection *)res;
                    }else{
                        log(warning) << "\t\t dy : what is this in task_service? : " << res->odata.id << " type : " << res->odata.type;
                    }
                    break;
                case SESSION_SERVICE_TYPE:
                    if (res->odata.type == ODATA_SESSION_COLLECTION_TYPE){
                        ((SessionService *)g_record[parent_object_id])->session_collection = (Collection *)res;
                    }else{
                        log(warning) << "\t\t dy : what is this in session_service? : " << res->odata.id << " type : " << res->odata.type;
                    }
                    break;
                case ACCOUNT_SERVICE_TYPE:
                    if (res->odata.type == ODATA_ACCOUNT_COLLECTION_TYPE){
                        ((AccountService *)g_record[parent_object_id])->account_collection = (Collection *)res;
                    }else if (res->odata.type == ODATA_ROLE_COLLECTION_TYPE){
                        ((AccountService *)g_record[parent_object_id])->role_collection = (Collection *)res;
                    }else{
                        log(warning) << "\t\t dy : what is this in account_service? : " << res->odata.id << " type : " << res->odata.type;
                    }
                    break;
                case EVENT_SERVICE_TYPE:
                    if (res->odata.type == ODATA_EVENT_DESTINATION_COLLECTION_TYPE){
                        ((EventService *)g_record[parent_object_id])->subscriptions = (Collection *)res;
                    }else{
                        log(warning) << "\t\t dy : what is this in event_service? : " << res->odata.id << " type : " << res->odata.type;
                    }
                    break;
                case UPDATE_SERVICE_TYPE:
                    if (res->name == "Software Inventory Collection"){
                        ((UpdateService *)g_record[parent_object_id])->firmware_inventory = (Collection *)res;
                    }else if (res->name == "Firmware Inventory Collection"){
                        ((UpdateService *)g_record[parent_object_id])->software_inventory = (Collection *)res;
                    }else{
                        log(warning) << "\t\t dy : what is this in update_service? : " << res->odata.id << " type : " << res->odata.type;
                    }
                    break;
                case CERTIFICATE_SERVICE_TYPE:    
                    if (res->odata.type == ODATA_CERTIFICATE_LOCATION_TYPE){
                        ((CertificateService *)g_record[parent_object_id])->certificate_location = (Collection *)res;
                    }else{
                        log(warning) << "\t\t dy : what is this in certificate_service? : " << res->odata.id << " type : " << res->odata.type;
                    }
                    break;
                default:
                    log(warning) << "\t\t dy : what is this in collection? : " << res->odata.id << " type : " << res->odata.type;
                    break;
            }
            break;
        }
        case SYSTEM_TYPE:
            ((Collection *)g_record[parent_object_id])->add_member((Systems *)res);
            break;
        case CHASSIS_TYPE:
            ((Collection *)g_record[parent_object_id])->add_member((Chassis *)res);
            break;
        case MANAGER_TYPE:
            ((Collection *)g_record[parent_object_id])->add_member((Manager *)res);
            break;
        case UPDATE_SERVICE_TYPE:
            ((Collection *)g_record[parent_object_id])->add_member((UpdateService *)res);
            break;
        // case NETWORK_INTERFACE_TYPE:
        // case STORAGE_TYPE:
        //     ((Collection *)g_record[parent_object_id])->add_member((Storage *)res);
        //     break;
        case PROCESSOR_TYPE:
            ((Collection *)g_record[parent_object_id])->add_member((Processors *)res);
            break;
        case MEMORY_TYPE:
            ((Collection *)g_record[parent_object_id])->add_member((Memory *)res);
            break;
        case SIMPLE_STORAGE_TYPE:
            ((Collection *)g_record[parent_object_id])->add_member((SimpleStorage *)res);
            break;
        case LOG_SERVICE_TYPE: // BMC Manager && Systems
            ((Collection *)g_record[parent_object_id])->add_member((LogService *)res);
            break;
        case ETHERNET_INTERFACE_TYPE: // BMC Manager && Systems
            ((Collection *)g_record[parent_object_id])->add_member((EthernetInterfaces *)res);
            break;
        case SENSOR_TYPE:
            ((Collection *)g_record[parent_object_id])->add_member((Sensor *)res);
            break;
        case LOG_ENTRY_TYPE:
            ((Collection *)g_record[parent_object_id])->add_member((LogEntry *)res);
            break;
        case TASK_TYPE:
            ((Collection *)g_record[parent_object_id])->add_member((Task *)res);
            break;
        case SESSION_TYPE:{
            ((Session *)res)->account = ((Account *)g_record[((Session *)res)->account_id]);
            ((Collection *)g_record[parent_object_id])->add_member((Session *)res);
            break;
        }
        case ROLE_TYPE:
            ((Collection *)g_record[parent_object_id])->add_member((Role *)res);
            break;
        case ACCOUNT_TYPE: 
            ((Account *)res)->role = ((Role *)g_record[((Account *)res)->role_id]);
            ((Collection *)g_record[parent_object_id])->add_member((Account *)res);
            break;
        case EVENT_DESTINATION_TYPE:
            ((Collection *)g_record[parent_object_id])->add_member((EventDestination *)res);
            break;
        case SOFTWARE_INVENTORY_TYPE:
            ((Collection *)g_record[parent_object_id])->add_member((SoftwareInventory *)res);
            break;
        case CERTIFICATE_TYPE:
            log(warning) << "CERTIFICATE NOT IMPLEMETED YET";
            // cout << "\t\tdy : child object : " << id << endl;     
            // cout << "\t\tdy : parent object = list : " << parent_object_id << endl << endl;
            // ((Collection *)g_record[parent_object_id])->add_member((Certificate *)res);
            break;
        
        case LIST_TYPE:{
        /*  == List location ==
            storage list : storage_controller
            thermal list : temperatures, fans
            power list : power_control, voltages, power_supplies
        */
            switch (((List *)res)->member_type){
                case STORAGE_CONTROLLER_TYPE:
                    ((Storage *)g_record[parent_object_id])->controller = (List *)res;
                    break;
                case TEMPERATURE_TYPE:
                    ((Thermal *)g_record[parent_object_id])->temperatures = (List *)res;
                    break;
                case FAN_TYPE:
                    ((Thermal *)g_record[parent_object_id])->fans = (List *)res;
                    break;
                case POWER_CONTROL_TYPE:
                    ((Power *)g_record[parent_object_id])->power_control = (List *)res;
                    break;
                case VOLTAGE_TYPE:
                    ((Power *)g_record[parent_object_id])->voltages = (List *)res;
                    break;
                case POWER_SUPPLY_TYPE:
                    ((Power *)g_record[parent_object_id])->power_supplies = (List *)res;
                    break;
                default:
                    log(warning) << "\t\t dy : what is this in list? : " << res->odata.id << " type : " << res->odata.type;
                    break;
            }
            break;
        }
        case STORAGE_CONTROLLER_TYPE:
            ((List *)g_record[parent_object_id])->add_member((StorageControllers *)res);
            break;            
        case TEMPERATURE_TYPE:
            ((List *)g_record[parent_object_id])->add_member((Temperature *)res);
            break;            
        case FAN_TYPE:
            ((List *)g_record[parent_object_id])->add_member((Fan *)res);
            break;            
        case POWER_CONTROL_TYPE:
            ((List *)g_record[parent_object_id])->add_member((PowerControl *)res);
            break;            
        case VOLTAGE_TYPE:
            ((List *)g_record[parent_object_id])->add_member((Voltage *)res);
            break;            
        case POWER_SUPPLY_TYPE:
            ((List *)g_record[parent_object_id])->add_member((PowerSupply *)res);
            break;            
        
        // systems
        case BIOS_TYPE:
            ((Systems *)g_record[parent_object_id])->bios = (Bios *)res;
            break;
        
        // chassis
        case THERMAL_TYPE:
            ((Chassis *)g_record[parent_object_id])->thermal = (Thermal *)res;
            break;
        case POWER_TYPE:
            ((Chassis *)g_record[parent_object_id])->power = (Power *)res;
            break;
    
        // manager
        // case NETWORK_PROTOCOL_TYPE:
    
        // case PROCESSOR_SUMMARY_TYPE:
        default:
            log(warning) << "NOT IMPLEMETED IN DEPENDENCY INJECTION : " << res->odata.id << " type : " << res->odata.type;
            break;
    }
}