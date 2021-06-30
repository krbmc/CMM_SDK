#include "stdafx.hpp"
#include "resource.hpp"

extern unordered_map<string, Resource *> g_record;

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
    case STORAGE_TYPE:
        j = ((Storage *)g_record[_uri])->get_json();
        break;
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
 * @details ServiceRoot에서 init 되지 않고 기존에 존재하던 데이터들을 디스크에 존재하는 .json파일을 통해 load한다.
 *          json 형식으로 저장되있는 모든 데이터를 우선 resource형식으로 읽어 type값과 odata.id를 얻어내고
 *          이러한 값을 토대로 각각의 객체로 분기하여 생성한다.
 * @return true 
 * @return false 
 */
bool record_load_json(void)
{
    record_init_load("/redfish");
    cout << "init load complete" << endl;

    for (auto it = g_record.begin(); it != g_record.end(); it++){
        json::value j;
        
        // Actions 예외처리
        string uri = it->second->odata.id;
        string resource_type1 = get_last_str(uri, "/");
        string resource_type2 = get_last_str(uri.substr(0, uri.length() - (resource_type1.length() + 1)), "/");
                    
        if(resource_type2 == "Actions")
            continue;

        it->second->load_resource_json(j);
        
        switch (it->second->type)
        {
        // case SERVICE_ROOT_TYPE:
        //     if (!((ServiceRoot *)it->second)->load_json())
        //         fprintf(stderr, "load ServiceRoot failed\n");
        //     break;
        // case COLLECTION_TYPE:
        //     if (!((Collection *)it->second)->load_json())
        //         fprintf(stderr, "load Collection failed\n");
        //     break
        // case LIST_TYPE:
        //     if (!((List *)it->second)->load_json())
        //         fprintf(stderr, "load List failed\n");
        //     break;
        // case SYSTEM_TYPE:
        //     if (!((System *)it->second)->load_json())
        //         fprintf(stderr, "load System failed\n");
        //     break;
        // case PROCESSOR_TYPE:
        //     if (!((Processor *)it->second)->load_json())
        //         fprintf(stderr, "load Processor failed\n");
        //     break;
        // case SIMPLE_STORAGE_TYPE:
        //     if (!((SimpleStorage *)it->second)->load_json())
        //         fprintf(stderr, "load SimpleStorage failed\n");
        //     break;
        // case CHASSIS_TYPE:
        //     if (!((Chassis *)it->second)->load_json())
        //         fprintf(stderr, "load Chassis failed\n");
        //     break;
        // case THERMAL_TYPE:
        //     if (!((Thermal *)it->second)->load_json())
        //         fprintf(stderr, "load Thermal failed\n");
        //     break;
        // case TEMPERATURE_TYPE:
        //     if (!((Temperature *)it->second)->load_json())
        //         fprintf(stderr, "load Temperature failed\n");
        //     break;
        // case FAN_TYPE:
        //     if (!((Fan *)it->second)->load_json())
        //         fprintf(stderr, "load Fan failed\n");
        //     break;
        // case POWER_TYPE:
        //     if (!((Power *)it->second)->load_json())
        //         fprintf(stderr, "load Power failed\n");
        //     break;
        // case MANAGER_TYPE:
        //     if (!((Manager *)it->second)->load_json())
        //         fprintf(stderr, "load Manager failed\n");
        //     break;
        // case ETHERNET_INTERFACE_TYPE:
        //     if (!((EthernetInterface *)it->second)->load_json())
        //         fprintf(stderr, "load Ethernet interface failed\n");
        //     break;
        // case LOG_SERVICE_TYPE:
        //     if (!((LogService *)it->second)->load_json())
        //         fprintf(stderr, "load Log Service failed\n");
        //     break;
        // case LOG_ENTRY_TYPE:
        //     if (!((LogEntry *)it->second)->load_json())
        //         fprintf(stderr, "load Log Entry failed\n");
        //     break;
        // case TASK_SERVICE_TYPE:
        //     if (!((TaskService *)it->second)->load_json())
        //         fprintf(stderr, "load Task Service failed\n");
        //     break;
        // case SESSION_SERVICE_TYPE:
        //     if (!((SessionService *)it->second)->load_json())
        //         fprintf(stderr, "load Session Service failed\n");
        //     break;
        // case SESSION_TYPE:
        //     if (!((Session *)it->second)->load_json())
        //         fprintf(stderr, "load Session failed\n");
        //     break;
        // case ACCOUNT_SERVICE_TYPE:
        //     if (!((AccountService *)it->second)->load_json())
        //         fprintf(stderr, "load Account Service failed\n");
        //     break;
        // case ACCOUNT_TYPE:
        //     if (!((Account *)it->second)->load_json())
        //         fprintf(stderr, "load Account failed\n");
        //     break;
        // case ROLE_TYPE:
        //     if (!((Role *)it->second)->load_json())
        //         fprintf(stderr, "load Role failed\n");
        //     break;
        // case EVENT_SERVICE_TYPE:
        //     if (!((EventService *)it->second)->load_json())
        //         fprintf(stderr, "load EventService failed\n");
        //     break;
        // case DESTINATION_TYPE:
        //     if (!((Destination1 *)it->second)->load_json())
        //         fprintf(stderr, "load Session failed\n");
        //     break;
        case CERTIFICATE_TYPE:{
            Certificate *cert = new Certificate(it->second->odata.id);
            if (!cert->load_json(j))
                fprintf(stderr, "load Certificate failed\n");
            break;
        } 
        default:
            break;
        }
    }
    return true;
}

bool record_save_json(void)
{
    for (auto it = g_record.begin(); it != g_record.end(); it++)
    {
        
        // cout << "cur record : " << it->second->odata.id << endl;
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
 * @brief read existed json file & load record
 * @details 존재하는 .json 파일중 현재 g_record에 존재하지 않는 객체를 얻어 type값과 odata.id를 얻어 Resource 생성. g_record에 저장. 
 * @authors 강, 김
 */
void record_init_load(string _path)
{
    struct dirent **namelist;
    struct stat statbuf;
    int count;

    if((count = scandir(_path.c_str(), &namelist, NULL, alphasort)) == -1)
    {
        cout << "Scandir Error in " << _path << endl; 
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
            // cout << str << " is directory" << endl;
            record_init_load(str);
        }
        else
        {
            string tmp;
            string sub;
            if(str.length() < 6)
            {
                // 해당 파일은 .json이 붙을수없음
                continue;
            }
            tmp = str.substr(str.length()-5);
            sub = str.substr(0, str.length()-5);
            if(tmp == ".json") // .json으로 끝나는지를 확인해줘야함 만들어야할듯
            {
                if(record_is_exist(sub))
                    continue;
                else
                { 
                    cout << sub << endl;
                    
                    Resource *res = new Resource(sub);
                    g_record[sub] = res;   
                }
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