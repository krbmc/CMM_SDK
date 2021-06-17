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
    case ACTIONS_TYPE:
        j = ((Actions *)g_record[_uri])->get_json();
        break;
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
 * 
 * @return true 
 * @return false 
 */
bool record_load_json(void)
{
    for (auto it = g_record.begin(); it != g_record.end(); it++)
        switch (it->second->type)
        {
        case SERVICE_ROOT_TYPE:
            ((ServiceRoot *)it->second)->load_json();
            break;
        case COLLECTION_TYPE:
            ((Collection *)it->second)->load_json();
            break;
        case LIST_TYPE:
            // ((List *)it->second)->load_json();
            break;
        case SYSTEM_TYPE:
            break;
        case PROCESSOR_TYPE:
            break;
        case SIMPLE_STORAGE_TYPE:
            break;
        case CHASSIS_TYPE:
            // ((Chassis *)it->second)->load_json();
            break;
        case THERMAL_TYPE:
            // ((Thermal *)it->second)->load_json();   
            break;
        case TEMPERATURE_TYPE:
            // ((Temperature *)it->second)->load_json();   
            break;
        case FAN_TYPE:
            // ((Fan *)it->second)->load_json();   
            break;
        case POWER_TYPE:
            break;
        case MANAGER_TYPE:
            break;
        case ETHERNET_INTERFACE_TYPE:
            break;
        case LOG_SERVICE_TYPE:
            break;
        case LOG_ENTRY_TYPE:
            break;
        case TASK_SERVICE_TYPE:
            break;
        case SESSION_SERVICE_TYPE:
            // ((SessionService *)it->second)->load_json();
            break;
        case SESSION_TYPE:
            // ((Session *)it->second)->load_json();
            break;
        case ACCOUNT_SERVICE_TYPE:
            // ((AccountService *)it->second)->load_json();
            break;
        case ACCOUNT_TYPE:
            // ((Account *)it->second)->load_json();
            break;
        case ROLE_TYPE:
            // ((Role *)it->second)->load_json();
            break;
        case EVENT_SERVICE_TYPE:
            break;
        case DESTINATION_TYPE:
            break;
        default:
            break;
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
 * @authors 강
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
        // cout << namelist[i]->d_name << endl;
        // cout << "namepath : " << str << endl;
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
            // cout << "sub : " << sub << endl;
            if(tmp == ".json") // .json으로 끝나는지를 확인해줘야함 만들어야할듯
            {
                if(record_is_exist(sub))
                    continue;
                else
                {
                    string uri = _path;
                    // cout << "URI : " << uri << endl;
                    if(uri == ODATA_ACCOUNT_ID)
                    {
                        Resource *res = new Resource(ACCOUNT_TYPE, sub, ODATA_ACCOUNT_ID);
                        g_record[str] = res;
                        cout << "Here is Account : " << name << endl;
                        cout << record_get_json(ODATA_ACCOUNT_ID) << endl;
                        record_print();
                        // 레코드 만들어졋는지 보려고 출력해봄
                    }
                    else if(uri == ODATA_SESSION_ID)
                    {
                        Resource *res = new Resource(SESSION_TYPE, sub, ODATA_SESSION_ID);
                        g_record[str] = res;
                        cout << "Here is Session : " << name << endl;
                    }
                    else if(uri == ODATA_CHASSIS_ID)
                    {
                        Resource *res = new Resource(CHASSIS_TYPE, sub, ODATA_CHASSIS_ID);
                        g_record[str] = res;
                        cout << "Here is Chassis : " << name << endl;
                    }
                    else if(uri == ODATA_MANAGER_ID)
                    {
                        Resource *res = new Resource(MANAGER_TYPE, sub, ODATA_MANAGER_ID);
                        g_record[str] = res;
                        cout << "Here is Manager : " << name << endl;
                    }
                    else if(uri == ODATA_SYSTEM_ID)
                    {
                        Resource *res = new Resource(SYSTEM_TYPE, sub, ODATA_SYSTEM_ID);
                        g_record[str] = res;
                        cout << "Here is System : " << name << endl;
                    }

                    // 여기는 종류 늘어나면 추가될 수 있고..
                    
                    // Resource형으로 odata.id odata.type만 넣어서 레코드 만들고 등록해놓은다음에
                    // 나중에 load_json에서 레코드 돌면서 제이슨파일 읽어서 정보수정할때 리소스 지우고 실제 해당하는
                    // 리소스로(account,session 등) 만들어서 연결시켜줄 예정

                }

                /**
                 * @details
                 * /redfish 디렉토리를 돌면서 디렉토리면 하위로 들어가고 파일이면 .json파일인지를 검사함
                 * 검사해서 json파일인데 현재 레코드에 없는녀석이라면 임시방편으로 Resource 형으로 레코드를 넣어줌
                 * 이 때 uri나 타입은 해당 리소스에 맞게 넣어줌
                 * @todo
                 * 그래서 load_json 구현할 때 레코드 돌면서 이렇게 생성된 레코드들은 다시 해당 리소스 타입으로 만들어서 
                 * 연결시키고 json읽어서 정보도 수정해줘야함
                 */
            }

            // 여기엔 이제 *.json 인지 파악해서 그거면 추출해서 정보 저장하고 그걸로 레코드생성
            // 그 정보 추출할때 json::value받아서 뽑는함수 만들면 나중에 keti-edge에서 정보 받아올때도
            // 사용가능할듯 
            // 여기서 레코드를 이미 만들어주기때문에 실제 초기화할때 레코드 존재하는지를 검사해줘야 중복안됨
        }
    }

    for(int i=0; i<count; i++)
    {
        free(namelist[i]);
    }

    free(namelist);

    return ;
}