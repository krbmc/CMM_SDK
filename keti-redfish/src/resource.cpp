#include "resource.hpp"

extern unordered_map<string, Resource *> g_record;
extern ServiceRoot *g_service_root;

#define BMC_PORT "443"

/**
 * @brief Resource initialization
 */
bool init_resource(void)
{
    g_service_root = new ServiceRoot();
    
    // record_load_json();
    log(info) << "record load json complete";
    
    add_new_bmc("1", "10.0.6.104", BMC_PORT, false, "TEST_ONE", "PASS_ONE");
    add_new_bmc("500", "10.0.6.104", BMC_PORT, false, "TEST_ONE", "PASS_ONE");
    // cout << "\t\t dy : add new bmc complete" << endl;
    
    // cout << " 갑자기? " << endl;
    record_save_json();
    // cout << " 갑자기?2 " << endl;
    log(info) << "record save json complete";
    
    return true;
}

// void init_record_bmc(void) 
// {
//     string odata_id = ODATA_SYSTEM_ID;
//     odata_id = odata_id + "/2";
//     Systems *system = new Systems(odata_id, "2~");
//     system->name = "EdgeServer BMC 2 System";
//     /* System 정보 넣기 필요 */

//     odata_id = ODATA_CHASSIS_ID;
//     odata_id = odata_id + "/2";
//     Chassis *chassis = new Chassis(odata_id, "2~");
//     chassis->name = "EdgeServer BMC 2 Chassis";
//     chassis->chassis_type = "Blade";
//     /* Chassis 정보 넣기 필요
//     서비스루트 만들때처럼 temperature 5개 만드는건 안했음아직*/

//     odata_id = ODATA_MANAGER_ID;
//     odata_id = odata_id + "/2";
//     Manager *manager = new Manager(odata_id, "2~");
//     manager->name = "EdgeServer BMC 2 Manager";
//     manager->manager_type = "Blade";
//     manager->firmware_version = "v1";
//     /* Manager 정보 넣기 필요 */ 

//     /**
//      * @todo 현재
//      * /redfish/v1/Systems/2
//      * /redfish/v1/Managers/2
//      * /redfish/v1/Chassis/2 3개만 해놧는데
//      * /redfish/v1/Systems/2/~~~~ 뒤에 붙는거까지 다 정보요청해서 읽고 레코드화 json화 해야함.
//      */
    
    
// }

// Resource start
json::value Resource::get_json(void)
{
    json::value j;
    try{
        // log(info) << "oid : " << this->odata.id << ", otype : " << this->odata.type << " type : " << this->type << "name : " << this->name << endl;
        j[U("@odata.type")] = json::value::string(U(this->odata.type));
        j[U("Name")] = json::value::string(U(this->name));
        j[U("@odata.id")] = json::value::string(U(this->odata.id));
        j["type"] = json::value::string(to_string(this->type)); // uint_8
    }
    catch (json::json_exception &e){
        return j = json::value::null();
    }
    return j;
}

json::value Resource::get_json(int type)
{
    json::value j;
    
    switch (type){
        case NO_DATA_TYPE:{
            try{
                j[U("Name")] = json::value::string(U(this->name));
                j[U("@odata.id")] = json::value::string(U(this->odata.id));
                j["type"] = json::value::string(to_string(this->type)); // uint_8
            }
            catch (json::json_exception &e){
                return j = json::value::null();
            }
            return j;
            break;
        }
        default:
            cout << "unknown type get json" << endl;
            return json::value::null();
            break;
    }
}

json::value Resource::get_odata_id_json(void)
{
    json::value j;
    if(this == nullptr)
    {
        log(error) << "null pointer";
        return j;
    }
    try{
        j[U("@odata.id")] = json::value::string(U(this->odata.id));
    }
    catch (json::json_exception &e)
    {
        log(warning) << this->odata.id << "error invoked!";
    }
    catch (exception &std_e)
    {
        log(warning) << this << "error invoked!";
    }
    return j;
}

bool Resource::save_json(void)
{
    string json_content;
    // cout << "어딘데 ? start" << endl;
// cout << this->odata.id << endl;
    json_content = record_get_json(this->odata.id).serialize();

    // cout << "어딘데 ? 0" << endl;
    // cout << this->odata.id << endl;
    
    if (json_content == "null"){
        log(warning) << "Something Wrong in save json : " << this->odata.id << endl;
        return false;
    }
    // log(info) << "file " << this->odata.id << " : " << json_content;
    
    // cout << "어딘데 ? 1" << endl;
    vector<string> tokens = string_split(this->odata.id, '/');
    string sub_path = "/";
    for (unsigned int i = 0; i < tokens.size() - 1; i++)
    {
        if (i == 0)
            sub_path = sub_path + tokens[i];
        else
            sub_path = sub_path + '/' + tokens[i];
        
        mkdir(sub_path.c_str(), 0755);
    }
    // cout << "어딘데 ? 2" << endl;

    // Save json file to path
    ofstream out(this->odata.id + ".json");
    out << json_content << endl;
    out.close();
    // log(info) << "save complete : " << this->odata.id + ".json" << endl << endl;
    // cout << "어딘데 ? 3" << endl;

    return true;
}

bool Resource::load_json(json::value &j)
{

    try{
        this->name = j.at("Name").as_string();
        this->type = atoi(j.at("type").as_string().c_str());
        this->odata.id = j.at("@odata.id").as_string();
        this->odata.type = j.at("@odata.type").as_string();
    }
    catch (json::json_exception &e)
    {
        return false;
    }
    return true;
}

bool Resource::load_json(json::value &j, int type)
{
    try{
        switch (type){
            case NO_DATA_TYPE:{
                this->name = j.at("Name").as_string();
                this->type = atoi(j.at("type").as_string().c_str());
                this->odata.id = j.at("@odata.id").as_string();
                break;
            }
            default:{
                cout << "unknown type load json" << endl;
                break;
            }
        }
    }
    catch (json::json_exception &e)
    {
        return false;
    }
    return true;
}

bool Resource::load_json_from_file(json::value &j)
{
    try
    {
        ifstream target_file(this->odata.id + ".json");
        stringstream string_stream;

        string_stream << target_file.rdbuf();
        target_file.close();

        j = json::value::parse(string_stream);
        auto j_obj = j.as_object();

        if (j_obj.find("@odata.type") == j_obj.end()){
            Resource::load_json(j, NO_DATA_TYPE);
        }else{
            Resource::load_json(j);
        }
    }
    catch (json::json_exception &e)
    {
        cout << "error : no Resource Data in " << this->odata.id << endl;
        return false;
    }
    
    return true;
}
// Resource end

// Collection start
json::value Collection::get_json(void)
{
    auto j = this->Resource::get_json();
    if (j.is_null())
        return j;
    
    j[U("Members")] = json::value::array();
    for (unsigned int i = 0; i < this->members.size(); i++){
        j[U("Members")][i] = this->members[i]->get_odata_id_json();
    }
    j[U("Members@odata.count")] = json::value::number(U(this->members.size()));
    return j;
}

void Collection::add_member(Resource *_resource)
{
    this->members.push_back(_resource);
}

bool Collection::load_json(json::value &j)
{
    try
    {
        Resource::load_json(j);
    }
    catch (json::json_exception &e)
    {
        return false;
    }

    return true;
}
// Collection end

// List start
json::value List::get_json(void)
{
    auto j = this->Resource::get_json();
    if (j.is_null())
        return j;
    
    j["MemberType"] = json::value::number(this->member_type);
    j[U(this->name)] = json::value::array();
    j["MemberType"] = json::value::number(this->member_type);
    for (unsigned int i = 0; i < this->members.size(); i++)
        switch (this->member_type)
        {
        case TEMPERATURE_TYPE:
            j[U("Temperatures")][i] = ((Temperature *)this->members[i])->get_json();
            break;
        case FAN_TYPE:
            j[U("Fans")][i] = ((Fan *)this->members[i])->get_json();
            break;
        case STORAGE_CONTROLLER_TYPE:
            j[U("StorageControllers")][i] = ((StorageControllers *)this->members[i])->get_json();
            break;
        case POWER_CONTROL_TYPE:
            j[U("PowerControl")][i] = ((PowerControl *)this->members[i])->get_json();
            break;
        case VOLTAGE_TYPE:
            j[U("Voltages")][i] = ((Voltage *)this->members[i])->get_json();
            break;
        case POWER_SUPPLY_TYPE:
            j[U("PowerSupplies")][i] = ((PowerSupply *)this->members[i])->get_json();
            break;
        }
        // 위에 this->name으로 이름을 만들었기 때문에 name하고 이름 똑같아야 새로 안만들고 잘 들어감ㅇㅇ
    return j;
}

void List::add_member(Resource *_resource)
{
    this->members.push_back(_resource);
}

// resource가 이미 존재하면 g_record에는 따로 저장하지 않음..?
bool List::load_json(json::value &j)
{
    json::value members;

    try{
        Resource::load_json(j);
        this->member_type = j.at("MemberType").as_integer();
        // members = j.at(this->name);
        // for (json::value item : members.as_array()){
        //     switch (this->member_type){
        //         case TEMPERATURE_TYPE:
        //             Temperature *t = new Temperature(item.at("@odata.id").as_string(), item.at("MemberId"));
        //             if (!t.load_json(item))
        //                 log(warning) << "temperature" << this->member_id << " load failed";
        //             this->add_member(t);
        //             break;
        //         case FAN_TYPE:
        //             Fan *f = new Fan(item.at("@odata.id").as_string(), item.at("MemberId"));
        //             if (!f.load_json(item))
        //                 log(warning) << "Fan" << this->member_id << " load failed";
        //             this->add_member(f);
        //             break;
        //         case STORAGE_CONTROLLER_TYPE:
        //             StorageControllers *sc = new StorageControllers(item.at("@odata.id").as_string(), item.at("MemberId"));
        //             if (!sc.load_json(item))
        //                 log(warning) << "Storage Controller" << this->member_id << " load failed";
        //             this->add_member(sc);
        //             break;
        //         case POWER_CONTROL_TYPE:
        //             PowerControl *pc = new PowerControl(item.at("@odata.id").as_string(), item.at("MemberId"));
        //             if (!pc.load_json(item))
        //                 log(warning) << "Power Control" << this->member_id << " load failed";
        //             this->add_member(pc);
        //             break;
        //         case VOLTAGE_TYPE:
        //             Voltage *v = new Voltage(item.at("@odata.id").as_string(), item.at("MemberId"));
        //             if (!v.load_json(item))
        //                 log(warning) << "Voltage" << this->member_id << " load failed";
        //             this->add_member(v);
        //             break;
        //         case POWER_SUPPLY_TYPE:
        //             PowerSupply *ps = new PowerSupply(item.at("@odata.id").as_string(), item.at("MemberId"));
        //             if (!ps.load_json(item))
        //                 log(warning) << "Power Supply" << this->member_id << " load failed";
        //             this->add_member(ps);
        //             break;
        //     }
        // }
    }
    catch (json::json_exception &e)
    {
        return false;
    }

    return true;    
}
// List end

// Actions start
json::value Actions::get_json(void)
{
    json::value j;
    
    string act = "#";
    act = act + this->action_by + this->action_what;

    json::value k = json::value::object();
    k[U("target")] = json::value::string(U(this->target));
    if(this->act_type == "Reset")
    {
        k[U("ResetType@Redfish.AllowableValues")] = json::value::array();
        for(int i=0; i<this->action_info.size(); i++)
            k[U("ResetType@Redfish.AllowableValues")][i] = json::value::string(this->action_info[i]);
    }
    
    //j[U(act)] = k;
    return k;
    // return j;
}

// Actions end

// Account start
json::value Account::get_json(void)
{
    auto j = this->Resource::get_json();
    if (j.is_null())
        return j;
    
    json::value k;
    j[U("Id")] = json::value::string(U(this->id));
    j[U("Enabled")] = json::value::boolean(U(this->enabled));
    j[U("Password")] = json::value::string(U(this->password));
    j[U("UserName")] = json::value::string(U(this->user_name));
    j[U("Locked")] = json::value::boolean(U(this->locked));
    j["RoleOdataId"] = json::value::string(this->role_id);
    
    if (this->role == nullptr)
    {
        j[U("RoleId")] = json::value::null();
        k = json::value::object();
    }
    else
    {
        j[U("RoleId")] = json::value::string(U(this->role->id));
        k[U("Role")] = this->role->get_odata_id_json();
    }
    j[U("Links")] = k;

    return j;
}

bool Account::load_json(json::value &j)
{
    try{
        Resource::load_json(j);
        this->id = j.at("Id").as_string();
        this->enabled = j.at("Enabled").as_bool();
        this->password = j.at("Password").as_string();
        this->user_name = j.at("UserName").as_string();
        this->locked = j.at("Locked").as_bool();
        this->role_id = j.at("RoleOdataId").as_string();
    }
    catch (json::json_exception &e)
    {
        return false;
    }

    return true;
}
// Account end

// Role start
json::value Role::get_json(void)
{
    auto j = this->Resource::get_json();
    if (j.is_null())
        return j;
    
    j[U("Id")] = json::value::string(U(this->id));
    j[U("IsPredefined")] = json::value::boolean(U(this->is_predefined));
    for (unsigned int i = 0; i < this->assigned_privileges.size(); i++)
        j[U("AssignedPrivileges")][i] = json::value::string(U(this->assigned_privileges[i]));
    return j;
}

bool Role::load_json(json::value &j)
{
    json::value role;

    try{
        Resource::load_json(j);
        this->id = j.at("Id").as_string();
        this->is_predefined = j.at("IsPredefined").as_bool();

        role = j.at("AssignedPrivileges");
        for(auto str : role.as_array())
            this->assigned_privileges.push_back(str.as_string());
    }
    catch (json::json_exception &e)
    {
        return false;
    }

    return true;

}
// Role end

// AccountService start
json::value AccountService::get_json(void)
{
    auto j = this->Resource::get_json();
    if (j.is_null())
        return j;
    
    json::value k;
    j[U("Id")] = json::value::string(U(this->id));
    k[U("State")] = json::value::string(U(this->status.state));
    k[U("Health")] = json::value::string(U(this->status.health));
    j[U("Status")] = k;
    j[U("ServiceEnabled")] = json::value::boolean(U(this->service_enabled));
    j[U("AuthFailureLoggingThreshold")] = json::value::number(U(this->auth_failure_logging_threshold));
    j[U("MinPasswordLength")] = json::value::number(U(this->min_password_length));
    j[U("AccountLockoutThreshold")] = json::value::number(U(this->account_lockout_threshold));
    j[U("AccountLockoutDuration")] = json::value::number(U(this->account_lockout_duration));
    j[U("AccountLockoutCounterResetAfter")] = json::value::number(U(this->account_lockout_counter_reset_after));
    j[U("AccountLockoutCounterResetEnabled")] = json::value::boolean(U(this->account_lockout_counter_reset_enabled));
    j[U("Accounts")] = this->account_collection->get_odata_id_json();
    j[U("Roles")] = this->role_collection->get_odata_id_json();
    return j;
}

bool AccountService::load_json(json::value &j)
{
    json::value status;
    
    try{
        Resource::load_json(j);
        this->id = j.at("Id").as_string();
        this->service_enabled = j.at("ServiceEnabled").as_bool();
        this->auth_failure_logging_threshold = j.at("AuthFailureLoggingThreshold").as_integer();
        this->min_password_length = j.at("MinPasswordLength").as_integer();
        this->account_lockout_threshold = j.at("AccountLockoutThreshold").as_integer();
        this->account_lockout_duration = j.at("AccountLockoutDuration").as_integer();
        this->account_lockout_counter_reset_after = j.at("AccountLockoutCounterResetAfter").as_integer();
        this->account_lockout_counter_reset_enabled = j.at("AccountLockoutCounterResetEnabled").as_bool();

        status = j.at("Status");
        this->status.state = status.at("State").as_string();
        this->status.health = status.at("Health").as_string();
    }
    catch (json::json_exception &e)
    {
        return false;
    }

    return true;
}
// AccountService end

// SessionService start
json::value SessionService::get_json(void)
{
    auto j = this->Resource::get_json();
    if (j.is_null())
        return j;
    
    json::value k;
    j[U("Id")] = json::value::string(U(this->id));
    k[U("State")] = json::value::string(U(this->status.state));
    k[U("Health")] = json::value::string(U(this->status.health));
    j[U("Status")] = k;
    j[U("ServiceEnabled")] = json::value::boolean(U(this->service_enabled));
    j[U("SessionTimeout")] = json::value::number(U(this->session_timeout));
    j[U("Sessions")] = this->session_collection->get_odata_id_json();
    return j;
}

bool SessionService::load_json(json::value &j)
{
    json::value status;
    
    try{
        Resource::load_json(j);
        this->service_enabled = j.at("ServiceEnabled").as_bool();
        this->session_timeout = j.at("SessionTimeout").as_integer();
        this->id = j.at("Id").as_string();
        
        status = j.at("Status");
        this->status.state = status.at("State").as_string();
        this->status.health = status.at("Health").as_string();
    }
    catch (json::json_exception &e)
    {
        return false;
    }

    return true;
}
// SessionService end

// Session start
json::value Session::get_json(void)
{
    auto j = this->Resource::get_json();
    if (j.is_null())
        return j;
    
    j[U("Id")] = json::value::string(U(this->id));
    // 연결된 account 없으면 session 소멸?
    j[U("UserName")] = json::value::string(U(this->account->user_name));
    // j[U("UserName")] = json::value::string("");
    j[U("AccountId")] = json::value::string(this->account_id);
    
    return j;
}

bool Session::load_json(json::value &j)
{
    try{
        Resource::load_json(j);
        this->id = j.at("Id").as_string();
        this->account_id = j.at("AccountId").as_string();
    }
    catch (json::json_exception &e)
    {
        return false;
    }

    return true;
}

pplx::task<void> Session::start(void)
{
    Session *session = this;
    unsigned int *remain_expires_time = &this->_remain_expires_time;
    /*
    * @ authors 강
    * @ TODO : 세션종료(로그아웃시) cancel하는 방식으로 해야함
    * 현재 세션만료시간 조정으로 종료함
    */
    // pplx::cancellation_token *c_token = &this->c_token;
    return pplx::create_task([remain_expires_time] {
               boost::asio::io_service io;
               boost::asio::deadline_timer session_timer(io, boost::posix_time::seconds(1));
               session_timer.async_wait(boost::bind(timer, &session_timer, remain_expires_time));
               io.run();

                // @@@@@
            //    if(pplx::is_task_cancellation_requested())
            //    {
            //     //    cout << session->id << "is Canceled" << endl;
            //        pplx::cancel_current_task();
            //        cout << "cancel??" << endl;
            //    }
           }) //, *c_token)
           // 여기 *c_token 추가했음
        .then([session] {
            // @@@@@
            cout << "IN START : " << session->id << endl;
            // @TODO : 세션종료되면서 세션.json삭제랑 레코드json들 수정작업 해야함

            string path = ODATA_SESSION_ID;
            path = path + '/' + session->id;
            cout << path << endl;

            g_record.erase(path); // 레코드자체 지운거

            cout << "지우기 전" << endl;
            cout << record_get_json(ODATA_SESSION_ID) << endl;

            Collection *col = (Collection *)g_record[ODATA_SESSION_ID];
            std::vector<Resource *>::iterator iter;
            for(iter=col->members.begin(); iter!=col->members.end(); iter++)
            {
                if(((Session *)(*iter))->id == session->id)
                {
                    col->members.erase(iter);
                    break;
                }
            } // 컬렉션에서 지운거

            record_save_json(); // 레코드 json파일 갱신
            string json_path = path;
            json_path = json_path + ".json";
            if(remove(json_path.c_str()) < 0)
            {
                cout << "delete error in session remove" << endl;
            }
            // json파일 제거

            // delete session;
            cout << "지운 후" << endl;
            cout << record_get_json(ODATA_SESSION_ID) << endl;
            cout << "LOGOUT!!!!" << endl;
        });
}
// Session end


// Log Service & Log Entry start
json::value LogService::get_json(void)
{
    auto j = this->Resource::get_json();
    if (j.is_null())
        return j;
    
    j[U("Id")] = json::value::string(U(this->id));
    j[U("Description")] = json::value::string(U(this->description));
    j[U("MaxNumberOfRecords")] = json::value::number(U(this->max_number_of_records));
    j[U("OverWritePolicy")] = json::value::string(U(this->overwrite_policy));
    j[U("DateTime")] = json::value::string(U(this->datetime));
    j[U("DateTimeLocalOffset")] = json::value::string(U(this->datetime_local_offset));
    j[U("ServiceEnabled")] = json::value::boolean(U(this->service_enabled));
    j[U("LogEntryType")] = json::value::string(U(this->log_entry_type));

    json::value k;
    k[U("State")] = json::value::string(U(this->status.state));
    k[U("Health")] = json::value::string(U(this->status.health));
    j[U("Status")] = k;

    j[U("Entries")] = this->entry->get_odata_id_json();

    json::value j_act;
    for(int i=0; i<this->actions.size(); i++)
    {
        string act = "#";
        act = act + this->actions[i]->action_name;
        j_act[U(act)] = this->actions[i]->get_json();
    }
    // std::vector<Resource *>::iterator iter;
    // for(iter = this->actions->members.begin(); iter != this->actions->members.end(); iter++)
    // {
    //     string act = "#";
    //     act = act + ((Actions *)(*iter))->action_name;
    //     // act = act + ((Actions *)*iter)->action_by + "." + ((Actions *)*iter)->action_what;
    //     j_act[U(act)] = ((Actions *)(*iter))->get_json();
    // }
    j[U("Actions")] = j_act;


    return j;
}

bool LogService::load_json(json::value &j)
{
    json::value status;
    
    try{
        Resource::load_json(j);
        this->id = j.at("Id").as_string();
        this->description = j.at("Description").as_string();
        this->datetime = j.at("DateTime").as_string();
        this->datetime_local_offset = j.at("DateTimeLocalOffset").as_string();
        this->max_number_of_records = j.at("MaxNumberOfRecords").as_integer();
        this->log_entry_type = j.at("LogEntryType").as_string();
        this->overwrite_policy = j.at("OverWritePolicy").as_string();
        this->service_enabled = j.at("ServiceEnabled").as_bool();

        status = j.at("Status");
        this->status.state = status.at("State").as_string();
        this->status.health = status.at("Health").as_string();
    }
    catch (json::json_exception &e)
    {
        return false;
    }

    return true;
}

json::value LogEntry::get_json(void)
{
    auto j = this->Resource::get_json();
    if (j.is_null())
        return j;
    
    j[U("Id")] = json::value::string(U(this->id));
    j[U("EntryType")] = json::value::string(U(this->entry_type));
    j[U("Severity")] = json::value::string(U(this->severity));
    j[U("Created")] = json::value::string(U(this->created));
    j[U("SensorNumber")] = json::value::number(U(this->sensor_number));
    j[U("Message")] = json::value::string(U(this->message));
    j[U("MessageId")] = json::value::string(U(this->message_id));

    j[U("MessageArgs")] = json::value::array();
    for(int i=0; i<this->message_args.size(); i++)
        j[U("MessageArgs")][i] = json::value::string(U(this->message_args[i]));

    return j;
}

bool LogEntry::load_json(json::value &j)
{
    json::value message_args;

    try{
        Resource::load_json(j);
        this->id = j.at("Id").as_string();
        this->entry_type = j.at("EntryType").as_string();
        this->severity = j.at("Severity").as_string();
        this->created = j.at("Created").as_string();
        this->sensor_number = j.at("SensorNumber").as_integer();
        this->message = j.at("Message").as_string();
        this->message_id = j.at("MessageId").as_string();

        message_args = j.at("MessageArgs");
        for(auto str : message_args.as_array())
            this->message_args.push_back(str.as_string());
    }
    catch (json::json_exception &e)
    {
        return false;
    }

    return true;
}
// Log Service & Log Entry end

// Event Service & Event Destination start
json::value EventDestination::get_json(void)
{
    auto j = this->Resource::get_json();
    if (j.is_null())
        return j;
    
    j[U("Id")] = json::value::string(U(this->id));
    j[U("Destination")] = json::value::string(U(this->destination));
    j[U("SubscriptionType")] = json::value::string(U(this->subscription_type));
    j[U("DeliveryRetryPolicy")] = json::value::string(U(this->delivery_retry_policy));
    j[U("Context")] = json::value::string(U(this->context));
    j[U("Protocol")] = json::value::string(U(this->protocol));

    json::value k;
    k[U("State")] = json::value::string(U(this->status.state));
    k[U("Health")] = json::value::string(U(this->status.health));
    j[U("Status")] = k;

    j[U("EventTypes")] = json::value::array();
    for(int i=0; i<this->event_types.size(); i++)
        j[U("EventTypes")][i] = json::value::string(U(this->event_types[i]));


    return j;
}

bool EventDestination::load_json(json::value &j)
{
    json::value status;
    json::value event_types;

    try{
        Resource::load_json(j);
        this->id = j.at("Id").as_string();
        this->destination = j.at("Destination").as_string();
        this->subscription_type = j.at("SubscriptionType").as_string();
        this->delivery_retry_policy = j.at("DeliveryRetryPolicy").as_string();
        this->context = j.at("Context").as_string();
        this->protocol = j.at("Protocol").as_string();
        
        event_types = j.at("EventTypes");
        for(auto str : event_types.as_array())
            this->event_types.push_back(str.as_string());
        
        status = j.at("Status");
        this->status.state = status.at("State").as_string();
        this->status.health = status.at("Health").as_string();

    }
    catch (json::json_exception &e)
    {
        return false;
    }

    return true;
}

json::value EventService::get_json(void)
{
    auto j = this->Resource::get_json();
    if (j.is_null())
        return j;
    
    j[U("Id")] = json::value::string(U(this->id));
    j[U("ServiceEnabled")] = json::value::boolean(U(this->service_enabled));
    j[U("DeliveryRetryAttempts")] = json::value::number(U(this->delivery_retry_attempts));
    j[U("DeliveryRetryIntervalSeconds")] = json::value::number(U(this->delivery_retry_interval_seconds));
    j[U("ServerSentEventUri")] = json::value::string(U(this->serversent_event_uri));

    json::value k;
    k[U("State")] = json::value::string(U(this->status.state));
    k[U("Health")] = json::value::string(U(this->status.health));
    j[U("Status")] = k;

    j[U("EventTypesForSubscription")] = json::value::array();
    for(int i=0; i<this->event_types_for_subscription.size(); i++)
        j[U("EventTypesForSubscription")][i] = json::value::string(U(this->event_types_for_subscription[i]));

    json::value j_sse;
    j_sse[U("EventType")] = json::value::boolean(U(this->sse.event_type));
    j_sse[U("MetricReportDefinition")] = json::value::boolean(U(this->sse.metric_report_definition));
    j_sse[U("RegistryPrefix")] = json::value::boolean(U(this->sse.registry_prefix));
    j_sse[U("ResourceType")] = json::value::boolean(U(this->sse.resource_type));
    j_sse[U("EventFormatType")] = json::value::boolean(U(this->sse.event_format_type));
    j_sse[U("MessageId")] = json::value::boolean(U(this->sse.message_id));
    j_sse[U("OriginResource")] = json::value::boolean(U(this->sse.origin_resource));
    j_sse[U("SubordinateResources")] = json::value::boolean(U(this->sse.subordinate_resources));
    j[U("SSEFilterPropertiesSupported")] = j_sse;

    j[U("Subscriptions")] = this->subscriptions->get_odata_id_json();

    return j;
}

bool EventService::load_json(json::value &j)
{
    json::value status;
    json::value event_types_for_subscription, sse;

    try{
        Resource::load_json(j);
        this->id = j.at("Id").as_string();
        this->service_enabled = j.at("ServiceEnabled").as_bool();
        this->delivery_retry_attempts = j.at("DeliveryRetryAttempts").as_integer();
        this->delivery_retry_interval_seconds = j.at("DeliveryRetryIntervalSeconds").as_integer();
        this->serversent_event_uri = j.at("ServerSentEventUri").as_string();

        status = j.at("Status");
        this->status.state = status.at("State").as_string();
        this->status.health = status.at("Health").as_string();

        event_types_for_subscription = j.at("EventTypesForSubscription");
        for (auto str : event_types_for_subscription.as_array())
            this->event_types_for_subscription.push_back(str.as_string());
        
        sse = j.at("SSEFilterPropertiesSupported");
        this->sse.event_type = sse.at("EventType").as_bool();
        this->sse.metric_report_definition = sse.at("MetricReportDefinition").as_bool();
        this->sse.registry_prefix = sse.at("RegistryPrefix").as_bool();
        this->sse.resource_type = sse.at("ResourceType").as_bool();
        this->sse.event_format_type = sse.at("EventFormatType").as_bool();
        this->sse.message_id = sse.at("MessageId").as_bool();
        this->sse.origin_resource = sse.at("OriginResource").as_bool();
        this->sse.subordinate_resources = sse.at("SubordinateResources").as_bool();
    }
    catch (json::json_exception &e)
    {
        return false;
    }

    return true;
}
// Event Service & Event Destination end

// Update Service & Software Inventory start

json::value UpdateService::get_json(void)
{
    json::value j = this->Resource::get_json();
    j[U("Id")] = json::value::string(U(this->id));
    j[U("ServiceEnabled")] = json::value::boolean(U(this->service_enabled));
    j[U("HttpPushUri")] = json::value::string(U(this->http_push_uri));

    json::value k;
    k[U("State")] = json::value::string(U(this->status.state));
    k[U("Health")] = json::value::string(U(this->status.health));
    j[U("Status")] = k;

    j[U("FirmwareInventory")] = this->firmware_inventory->get_odata_id_json();
    j[U("SoftwareInventory")] = this->software_inventory->get_odata_id_json();

    return j;
}

bool UpdateService::load_json(json::value &j)
{
    json::value status;
    
    try{
        Resource::load_json(j);
        this->id = j.at("Id").as_string();
        this->service_enabled = j.at("ServiceEnabled").as_bool();
        this->http_push_uri = j.at("HttpPushUri").as_string();

        status = j.at("Status");
        this->status.state = status.at("State").as_string();
        this->status.health = status.at("Health").as_string();

    }
    catch (json::json_exception &e)
    {
        return false;
    }

    return true;
}

json::value SoftwareInventory::get_json(void)
{
    json::value j = this->Resource::get_json();
    j[U("Id")] = json::value::string(U(this->id));
    j[U("Updatable")] = json::value::boolean(U(this->updatable));
    j[U("Manufacturer")] = json::value::string(U(this->manufacturer));
    j[U("ReleaseDate")] = json::value::string(U(this->release_date));
    j[U("Version")] = json::value::string(U(this->version));
    j[U("SoftwareId")] = json::value::string(U(this->software_id));
    j[U("LowestSupportedVersion")] = json::value::string(U(this->lowest_supported_version));

    json::value k;
    k[U("State")] = json::value::string(U(this->status.state));
    k[U("Health")] = json::value::string(U(this->status.health));
    j[U("Status")] = k;

    j[U("UefiDevicePaths")] = json::value::array();
    for(int i=0; i<this->uefi_device_paths.size(); i++)
        j[U("UefiDevicePaths")][i] = json::value::string(U(this->uefi_device_paths[i]));

    return j;
}

bool SoftwareInventory::load_json(json::value &j)
{
    json::value status;
    json::value uefi_device_paths;

    try{
        Resource::load_json(j);
        this->id = j.at("Id").as_string();
        this->updatable = j.at("Updatable").as_bool();
        this->manufacturer = j.at("Manufacturer").as_string();
        this->release_date = j.at("ReleaseDate").as_string();
        this->version = j.at("Version").as_string();
        this->software_id = j.at("SoftwareId").as_string();
        this->lowest_supported_version = j.at("LowestSupportedVersion").as_string();

        uefi_device_paths = j.at("UefiDevicePaths");
        for(auto str : uefi_device_paths.as_array())
            this->uefi_device_paths.push_back(str.as_string());
        
        status = j.at("Status");
        this->status.state = status.at("State").as_string();
        this->status.health = status.at("Health").as_string();

    }
    catch (json::json_exception &e)
    {
        return false;
    }

    return true;

}

// Update Service & Software Inventory end

// Temperature start
json::value Temperature::get_json(void)
{
    auto j = this->Resource::get_json();
    if (j.is_null())
        return j;
    
    json::value k;
    
    j[U("MemberId")] = json::value::string(U(this->member_id));
    k[U("State")] = json::value::string(U(this->status.state));
    k[U("Health")] = json::value::string(U(this->status.health));
    j[U("Status")] = k;

    j[U("SensorNumber")] = json::value::number(U(this->sensor_num));
    j[U("ReadingCelsius")] = json::value::number(U(this->reading_celsius));
    j[U("UpperThresholdNonCritical")] = json::value::number(U(this->upper_threshold_non_critical));
    j[U("UpperThresholdCritical")] = json::value::number(U(this->upper_threshold_critical));
    j[U("UpperThresholdFatal")] = json::value::number(U(this->upper_threshold_fatal));
    j[U("LowerThresholdNonCritical")] = json::value::number(U(this->lower_threshold_non_critical));
    j[U("LowerThresholdCritical")] = json::value::number(U(this->lower_threshold_critical));
    j[U("LowerThresholdFatal")] = json::value::number(U(this->lower_threshold_fatal));

    j[U("MinReadingRangeTemp")] = json::value::number(U(this->min_reading_range_temp));
    j[U("MaxReadingRangeTemp")] = json::value::number(U(this->max_reading_range_temp));
    j[U("PhysicalContext")] = json::value::string(U(this->physical_context));

    return j;
}

bool Temperature::load_json(json::value &j)
{
    json::value status;

    try{
        Resource::load_json(j);
        this->member_id = j.at("MemberId").as_string();

        status = j.at("Status");
        this->status.state = status.at("State").as_string();
        this->status.health = status.at("Health").as_string();

        this->sensor_num = j.at("SensorNumber").as_integer();
        this->reading_celsius = j.at("ReadingCelsius").as_double();
        this->upper_threshold_non_critical = j.at("UpperThresholdNonCritical").as_double();
        this->upper_threshold_critical = j.at("UpperThresholdCritical").as_double();
        this->upper_threshold_fatal = j.at("UpperThresholdFatal").as_double();
        this->lower_threshold_non_critical = j.at("LowerThresholdNonCritical").as_double();
        this->lower_threshold_critical = j.at("LowerThresholdCritical").as_double();
        this->lower_threshold_fatal = j.at("LowerThresholdFatal").as_double();
        
        this->min_reading_range_temp = j.at("MinReadingRangeTemp").as_double();
        this->max_reading_range_temp = j.at("MaxReadingRangeTemp").as_double();
        this->physical_context = j.at("PhysicalContext").as_string();
    }
    catch (json::json_exception &e)
    {
        return false;
    }

    return true;
}

pplx::task<void> Temperature::read(uint8_t _sensor_index, uint8_t _sensor_context)
{
    double *reading_celsius = &this->reading_celsius;
    bool *thread = &this->thread;
    *thread = true;

    switch (_sensor_context)
    {
    case INTAKE_CONTEXT:
        return pplx::create_task([thread, reading_celsius, _sensor_index] {
            while (*thread)
            {
                *reading_celsius = round(get_intake_temperature(_sensor_index));
                sleep(1);
            }
        });
    case CPU_CONTEXT:
        return pplx::create_task([thread, reading_celsius, _sensor_index] {
            while (*thread)
            {
                *reading_celsius = round(get_cpu_temperature(_sensor_index));
                sleep(1);
            }
        });
    }
    return pplx::create_task([]{});
}
// Temperature end

// Fan start
json::value Fan::get_json(void)
{
    auto j = this->Resource::get_json();
    if (j.is_null())
        return j;
    
    json::value k;

    j[U("MemberId")] = json::value::string(U(this->member_id));

    k[U("State")] = json::value::string(U(this->status.state));
    k[U("Health")] = json::value::string(U(this->status.health));
    j[U("Status")] = k;

    j[U("SensorNumber")] = json::value::number(U(this->sensor_num));
    j[U("Reading")] = json::value::number(U(this->reading));
    j[U("ReadingUnits")] = json::value::string(U(this->reading_units));

    j[U("UpperThresholdNonCritical")] = json::value::number(U(this->upper_threshold_non_critical));
    j[U("UpperThresholdCritical")] = json::value::number(U(this->upper_threshold_critical));
    j[U("UpperThresholdFatal")] = json::value::number(U(this->upper_threshold_fatal));
    j[U("LowerThresholdNonCritical")] = json::value::number(U(this->lower_threshold_non_critical));
    j[U("LowerThresholdCritical")] = json::value::number(U(this->lower_threshold_critical));
    j[U("LowerThresholdFatal")] = json::value::number(U(this->lower_threshold_fatal));

    j[U("MinReadingRange")] = json::value::number(U(this->min_reading_range));
    j[U("MaxReadingRange")] = json::value::number(U(this->max_reading_range));
    j[U("PhysicalContext")] = json::value::string(U(this->physical_context));

    return j;
}

bool Fan::load_json(json::value &j)
{
    json::value status;

    try{
        Resource::load_json(j);
        this->member_id = j.at("MemberId").as_string();

        status = j.at("Status");
        this->status.state = status.at("State").as_string();
        this->status.health = status.at("Health").as_string();

        this->sensor_num = j.at("SensorNumber").as_integer();
        this->reading = j.at("Reading").as_integer();
        this->reading_units = j.at("ReadingUnits").as_string();

        this->upper_threshold_non_critical = j.at("UpperThresholdNonCritical").as_integer();
        this->upper_threshold_critical = j.at("UpperThresholdCritical").as_integer();
        this->upper_threshold_fatal = j.at("UpperThresholdFatal").as_integer();
        this->lower_threshold_non_critical = j.at("LowerThresholdNonCritical").as_integer();
        this->lower_threshold_critical = j.at("LowerThresholdCritical").as_integer();
        this->lower_threshold_fatal = j.at("LowerThresholdFatal").as_integer();
        
        this->min_reading_range = j.at("MinReadingRange").as_integer();
        this->max_reading_range = j.at("MaxReadingRange").as_integer();
        this->physical_context = j.at("PhysicalContext").as_string();
    }
    catch (json::json_exception &e)
    {
        return false;
    }

    return true;

}
// Fan end

// Thermal start
json::value Thermal::get_json(void)
{
    unsigned int i;
    auto j = this->Resource::get_json();
    if (j.is_null())
        return j;
    

    j[U("Id")] = json::value::string(U(this->id));
    j[U("Temperatures")] = json::value::array();
    for (i = 0; i < this->temperatures->members.size(); i++)
        j[U("Temperatures")][i] = ((Temperature *)this->temperatures->members[i])->get_json();
    j[U("Fans")] = json::value::array();
    for (i = 0; i < this->fans->members.size(); i++)
        j[U("Fans")][i] = ((Fan *)this->fans->members[i])->get_json();

    return j;
}

bool Thermal::load_json(json::value &j)
{
    json::value status;
    
    try{
        Resource::load_json(j);
        this->id = j.at("Id").as_string();
        
        // temperature list, fan list 
    }
    catch (json::json_exception &e)
    {
        return false;
    }

    return true;
}
// Thermal end

// Sensor start
json::value Sensor::get_json(void)
{
    json::value j;
    j = this->Resource::get_json();
    j[U("Id")] = json::value::string(U(this->id));
    j[U("ReadingType")] = json::value::string(U(this->reading_type));
    j[U("ReadingTime")] = json::value::string(U(this->reading_time));

    ostringstream o;
    o << this->reading;
    j[U("Reading")] = json::value::string(U(o.str()));
    // 이렇게 하면 소수점 다 안나오게 할 수 있네 double형인거? 나중에 참고
    // j[U("Reading")] = json::value::number(U(this->reading));
    
    j[U("ReadingUnits")] = json::value::string(U(this->reading_units));
    j[U("ReadingRangeMin")] = json::value::number(U(this->reading_range_min));
    j[U("ReadingRangeMax")] = json::value::number(U(this->reading_range_max));
    j[U("Accuracy")] = json::value::number(U(this->accuracy));
    j[U("Precision")] = json::value::number(U(this->precision));
    j[U("SensingInterval")] = json::value::string(U(this->sensing_interval));
    j[U("PhysicalContext")] = json::value::string(U(this->physical_context));

    json::value k;
    k[U("State")] = json::value::string(U(this->status.state));
    k[U("Health")] = json::value::string(U(this->status.health));
    j[U("Status")] = k;

    json::value thresh;
    json::value i;
    i[U("Reading")] = json::value::number(U(this->thresh.upper_critical.reading));
    i[U("Activation")] = json::value::string(U(this->thresh.upper_critical.activation));
    thresh[U("UpperCritical")] = i;

    i[U("Reading")] = json::value::number(U(this->thresh.upper_caution.reading));
    i[U("Activation")] = json::value::string(U(this->thresh.upper_caution.activation));
    thresh[U("UpperCaution")] = i;

    i[U("Reading")] = json::value::number(U(this->thresh.upper_fatal.reading));
    i[U("Activation")] = json::value::string(U(this->thresh.upper_fatal.activation));
    thresh[U("UpperFatal")] = i;

    i[U("Reading")] = json::value::number(U(this->thresh.lower_critical.reading));
    i[U("Activation")] = json::value::string(U(this->thresh.lower_critical.activation));
    thresh[U("LowerCritical")] = i;

    i[U("Reading")] = json::value::number(U(this->thresh.lower_caution.reading));
    i[U("Activation")] = json::value::string(U(this->thresh.lower_caution.activation));
    thresh[U("LowerCaution")] = i;

    i[U("Reading")] = json::value::number(U(this->thresh.lower_fatal.reading));
    i[U("Activation")] = json::value::string(U(this->thresh.lower_fatal.activation));
    thresh[U("LowerFatal")] = i;

    j[U("Thresholds")] = thresh;

    return j;
}

bool Sensor::load_json(json::value &j)
{
    json::value status, threshold;
    json::value upper_critical, upper_caution, upper_fatal, lower_caution, lower_critical, lower_fatal;

    try{
        Resource::load_json(j);
        this->id = j.at("Id").as_string();
        this->reading_type = j.at("ReadingType").as_string();
        this->reading_time = j.at("ReadingTime").as_string();

        this->reading = j.at("Reading").as_double();
        this->reading_units = j.at("ReadingUnits").as_string();
        this->reading_range_max = j.at("ReadingRangeMax").as_double();
        this->reading_range_min = j.at("ReadingRangeMin").as_double();
        this->accuracy = j.at("Accuracy").as_double();
        this->precision = j.at("Precision").as_double();
        this->physical_context = j.at("PhysicalContext").as_string();
        this->sensing_interval = j.at("SensingInterval").as_string();

        threshold = j.at("Thresholds");
        upper_critical = threshold.at("UpperCritical");
        this->thresh.upper_critical.reading = upper_critical.at("Reading").as_double();
        this->thresh.upper_critical.activation = upper_critical.at("Activation").as_string();
        upper_caution = threshold.at("UpperCaution");
        this->thresh.upper_caution.reading = upper_caution.at("Reading").as_double();
        this->thresh.upper_caution.activation = upper_caution.at("Activation").as_string();
        upper_fatal = threshold.at("UpperFatal");
        this->thresh.upper_fatal.reading = upper_fatal.at("Reading").as_double();
        this->thresh.upper_fatal.activation = upper_fatal.at("Activation").as_string();
        lower_critical = threshold.at("LowerCritical");
        this->thresh.lower_critical.reading = lower_critical.at("Reading").as_double();
        this->thresh.lower_critical.activation = lower_critical.at("Activation").as_string();
        lower_caution = threshold.at("LowerCaution");
        this->thresh.lower_caution.reading = lower_caution.at("Reading").as_double();
        this->thresh.lower_caution.activation = lower_caution.at("Activation").as_string();
        lower_fatal = threshold.at("LowerFatal");
        this->thresh.lower_fatal.reading = lower_fatal.at("Reading").as_double();
        this->thresh.lower_fatal.activation = lower_fatal.at("Activation").as_string();
        
        status = j.at("Status");
        this->status.state = status.at("State").as_string();
        this->status.health = status.at("Health").as_string();

    }
    catch (json::json_exception &e)
    {
        log(warning) << "read something failed in sensor";
        return true;
    }

    return true;
}
// Sensor end

// Power &  PowerControl, Voltage, PowerSupply start

json::value PowerSupply::get_json(void)
{
    auto j = this->Resource::get_json(NO_DATA_TYPE);
    if (j.is_null())
        return j;
    
    json::value k;

    j[U("MemberId")] = json::value::string(U(this->member_id));
    
    j[U("PowerSupplyType")] = json::value::string(U(this->power_supply_type));
    j[U("LineInputVoltageType")] = json::value::string(U(this->line_input_voltage_type));
    j[U("LineInputVoltage")] = json::value::number(U(this->line_input_voltage));
    j[U("PowerCapacityWatts")] = json::value::number(U(this->power_capacity_watts));
    j[U("LastPowerOutputWatts")] = json::value::number(U(this->last_power_output_watts));

    j[U("Model")] = json::value::string(U(this->model));
    j[U("Manufacturer")] = json::value::string(U(this->manufacturer));
    j[U("FirmwareVersion")] = json::value::string(U(this->firmware_version));
    j[U("SerialNumber")] = json::value::string(U(this->serial_number));
    j[U("PartNumber")] = json::value::string(U(this->part_number));
    j[U("SparePartNumber")] = json::value::string(U(this->spare_part_number));

    k[U("State")] = json::value::string(U(this->status.state));
    k[U("Health")] = json::value::string(U(this->status.health));
    j[U("Status")] = k;

    j[U("InputRanges")] = json::value::array();
    for(int i=0; i<this->input_ranges.size(); i++)
    {
        json::value l = json::value::object();
        l[U("InputType")] = json::value::string(U(this->input_ranges[i].input_type));
        l[U("MinimumVoltage")] = json::value::number(U(this->input_ranges[i].minimum_voltage));
        l[U("MaximumVoltage")] = json::value::number(U(this->input_ranges[i].maximum_voltage));
        l[U("OutputWattage")] = json::value::number(U(this->input_ranges[i].output_wattage));
        j[U("InputRanges")][i] = l;

    }

    return j;
}

bool PowerSupply::load_json(json::value &j)
{
    json::value status, input_ranges;

    try{
        Resource::load_json(j, NO_DATA_TYPE);
        this->member_id = j.at("MemberId").as_string();

        this->power_supply_type = j.at("PowerSupplyType").as_string();
        this->line_input_voltage_type = j.at("LineInputVoltageType").as_string();
        this->line_input_voltage = j.at("LineInputVoltage").as_double();
        this->power_capacity_watts = j.at("PowerCapacityWatts").as_double();
        this->last_power_output_watts = j.at("LastPowerOutputWatts").as_double();

        this->model = j.at("Model").as_string();
        this->manufacturer = j.at("Manufacturer").as_string();
        this->firmware_version = j.at("FirmwareVersion").as_string();
        this->serial_number = j.at("SerialNumber").as_string();
        this->part_number = j.at("PartNumber").as_string();
        this->spare_part_number = j.at("SparePartNumber").as_string();

        status = j.at("Status");
        this->status.state = status.at("State").as_string();
        this->status.health = status.at("Health").as_string();

        input_ranges = j.at("InputRanges");
        for (auto item : input_ranges.as_array()){
            InputRange temp;
            
            temp.input_type = item.at("InputType").as_string();
            temp.minimum_voltage = item.at("MinimumVoltage").as_double();
            temp.maximum_voltage = item.at("MaximumVoltage").as_double();
            temp.output_wattage = item.at("OutputWattage").as_double();
            
            this->input_ranges.push_back(temp);
        }
    }
    catch (json::json_exception &e)
    {
        return false;
    }

    return true;

}

json::value Voltage::get_json(void)
{
    auto j = this->Resource::get_json(NO_DATA_TYPE);
    if (j.is_null())
        return j;
    
    json::value k;

    j[U("MemberId")] = json::value::string(U(this->member_id));
    j[U("SensorNumber")] = json::value::number(U(this->sensor_num));
    j[U("ReadingVolts")] = json::value::number(U(this->reading_volts));
    j[U("UpperThresholdNonCritical")] = json::value::number(U(this->upper_threshold_non_critical));
    j[U("UpperThresholdCritical")] = json::value::number(U(this->upper_threshold_critical));
    j[U("UpperThresholdFatal")] = json::value::number(U(this->upper_threshold_fatal));
    j[U("LowerThresholdNonCritical")] = json::value::number(U(this->lower_threshold_non_critical));
    j[U("LowerThresholdCritical")] = json::value::number(U(this->lower_threshold_critical));
    j[U("LowerThresholdFatal")] = json::value::number(U(this->lower_threshold_fatal));
    j[U("MinReadingRange")] = json::value::number(U(this->min_reading_range));
    j[U("MaxReadingRange")] = json::value::number(U(this->max_reading_range));
    j[U("PhysicalContext")] = json::value::string(U(this->physical_context));

    k[U("State")] = json::value::string(U(this->status.state));
    k[U("Health")] = json::value::string(U(this->status.health));
    j[U("Status")] = k;

    return j;
}

bool Voltage::load_json(json::value &j)
{
    json::value status;

    try{
        Resource::load_json(j, NO_DATA_TYPE);
        this->member_id = j.at("MemberId").as_string();

        status = j.at("Status");
        this->status.state = status.at("State").as_string();
        this->status.health = status.at("Health").as_string();

        this->sensor_num = j.at("SensorNumber").as_integer();
        this->reading_volts = j.at("ReadingVolts").as_double();

        this->upper_threshold_non_critical = j.at("UpperThresholdNonCritical").as_double();
        this->upper_threshold_critical = j.at("UpperThresholdCritical").as_double();
        this->upper_threshold_fatal = j.at("UpperThresholdFatal").as_double();
        this->lower_threshold_non_critical = j.at("LowerThresholdNonCritical").as_double();
        this->lower_threshold_critical = j.at("LowerThresholdCritical").as_double();
        this->lower_threshold_fatal = j.at("LowerThresholdFatal").as_double();
        
        this->min_reading_range = j.at("MinReadingRange").as_double();
        this->max_reading_range = j.at("MaxReadingRange").as_double();
        this->physical_context = j.at("PhysicalContext").as_string();
    }
    catch (json::json_exception &e)
    {
        return false;
    }

    return true;

}

json::value PowerControl::get_json(void)
{
    auto j = this->Resource::get_json(NO_DATA_TYPE);
    if (j.is_null())
        return j;
    
    json::value k;

    j[U("MemberId")] = json::value::string(U(this->member_id));
    j[U("PowerConsumedWatts")] = json::value::number(U(this->power_consumed_watts));
    j[U("PowerRequestedWatts")] = json::value::number(U(this->power_requested_watts));
    j[U("PowerAvailableWatts")] = json::value::number(U(this->power_available_watts));
    j[U("PowerCapacityWatts")] = json::value::number(U(this->power_capacity_watts));
    j[U("PowerAllocatedWatts")] = json::value::number(U(this->power_allocated_watts));

    k[U("State")] = json::value::string(U(this->status.state));
    k[U("Health")] = json::value::string(U(this->status.health));
    j[U("Status")] = k;

    json::value metric, limit;
    metric[U("IntervalInMin")] = json::value::number(U(this->power_metrics.interval_in_min));
    metric[U("MinConsumedWatts")] = json::value::number(U(this->power_metrics.min_consumed_watts));
    metric[U("MaxConsumedWatts")] = json::value::number(U(this->power_metrics.max_consumed_watts));
    metric[U("AverageConsumedWatts")] = json::value::number(U(this->power_metrics.avg_consumed_watts));
    j[U("PowerMetrics")] = metric;

    limit[U("LimitInWatts")] = json::value::number(U(this->power_limit.limit_in_watts));
    limit[U("LimitException")] = json::value::string(U(this->power_limit.limit_exception));
    limit[U("CorrectionInMs")] = json::value::number(U(this->power_limit.correction_in_ms));
    j[U("PowerLimit")] = limit;

    return j;
}

bool PowerControl::load_json(json::value &j)
{
    json::value status, power_limit, power_metrics;

    try{
        Resource::load_json(j, NO_DATA_TYPE);
        this->member_id = j.at("MemberId").as_string();
        this->power_consumed_watts = j.at("PowerConsumedWatts").as_double();
        this->power_requested_watts = j.at("PowerRequestedWatts").as_double();
        this->power_available_watts = j.at("PowerAvailableWatts").as_double();
        this->power_capacity_watts = j.at("PowerCapacityWatts").as_double();
        this->power_allocated_watts = j.at("PowerAvailableWatts").as_double();
        
        status = j.at("Status");
        this->status.state = status.at("State").as_string();
        this->status.health = status.at("Health").as_string();

        power_metrics = j.at("PowerMetrics");
        this->power_metrics.interval_in_min = power_metrics.at("IntervalInMin").as_integer();
        this->power_metrics.min_consumed_watts = power_metrics.at("MinConsumedWatts").as_double();
        this->power_metrics.max_consumed_watts = power_metrics.at("MaxConsumedWatts").as_double();
        this->power_metrics.avg_consumed_watts = power_metrics.at("AverageConsumedWatts").as_double();

        power_limit = j.at("PowerLimit");
        this->power_limit.limit_in_watts = power_limit.at("LimitInWatts").as_double();
        this->power_limit.limit_exception = power_limit.at("LimitException").as_string();
        this->power_limit.correction_in_ms = power_limit.at("CorrectionInMs").as_integer();
    }
    catch (json::json_exception &e)
    {
        return false;
    }

    return true;

}

json::value Power::get_json(void)
{
    unsigned int i;
    auto j = this->Resource::get_json();
    if (j.is_null())
        return j;
    

    j[U("Id")] = json::value::string(U(this->id));

    j[U("PowerControl")] = json::value::array();
    for (i = 0; i < this->power_control->members.size(); i++)
        j[U("PowerControl")][i] = ((PowerControl *)this->power_control->members[i])->get_json();
    j[U("Voltages")] = json::value::array();
    for (i = 0; i < this->voltages->members.size(); i++)
        j[U("Voltages")][i] = ((Voltage *)this->voltages->members[i])->get_json();
    j[U("PowerSupplies")] = json::value::array();
    for (i = 0; i < this->power_supplies->members.size(); i++)
        j[U("PowerSupplies")][i] = ((PowerSupply *)this->power_supplies->members[i])->get_json();

    return j;
}

bool Power::load_json(json::value &j)
{
    json::value status;
    
    try{
        Resource::load_json(j);
        this->id = j.at("Id").as_string();    
    }
    catch (json::json_exception &e)
    {
        return false;
    }

    return true;
}
// Power &  PowerControl, Voltage, PowerSupply end

// Chassis start
json::value Chassis::get_json(void)
{
    auto j = this->Resource::get_json();
    if (j.is_null())
        return j;
    
    json::value k;
    json::value l;
    j[U("Id")] = json::value::string(U(this->id));
    j[U("ChassisType")] = json::value::string(U(this->chassis_type));
    j[U("Manufacturer")] = json::value::string(U(this->manufacturer));
    j[U("Model")] = json::value::string(U(this->model));
    j[U("SerialNumber")] = json::value::string(U(this->serial_number));
    j[U("PartNumber")] = json::value::string(U(this->part_number));
    j[U("AssetTag")] = json::value::string(U(this->asset_tag));
    j[U("PowerState")] = json::value::string(U(this->power_state));

    switch (this->indicator_led)
    {
    case LED_OFF:
        j[U("IndicatorLED")] = json::value::string(U("Off"));
        break;
    case LED_BLINKING:
        j[U("IndicatorLED")] = json::value::string(U("Blinking"));
        break;
    case LED_LIT:
        j[U("IndicatorLED")] = json::value::string(U("Lit"));
        break;
    }

    k[U("State")] = json::value::string(U(this->status.state));
    k[U("Health")] = json::value::string(U(this->status.health));
    j[U("Status")] = k;

    k = json::value::object();
    l[U("Country")] = json::value::string(U(this->location.postal_address.country));
    l[U("Territory")] = json::value::string(U(this->location.postal_address.territory));
    l[U("City")] = json::value::string(U(this->location.postal_address.city));
    l[U("Street")] = json::value::string(U(this->location.postal_address.street));
    l[U("HouseNumber")] = json::value::string(U(this->location.postal_address.house_number));
    l[U("Name")] = json::value::string(U(this->location.postal_address.name));
    l[U("PostalCode")] = json::value::string(U(this->location.postal_address.postal_code));
    k[U("PostalAddress")] = l;
    l = json::value::object();
    l[U("Row")] = json::value::string(U(this->location.placement.row));
    l[U("Rack")] = json::value::string(U(this->location.placement.rack));
    l[U("RackOffsetUnits")] = json::value::string(U(this->location.placement.rack_offset_units));
    l[U("RackOffset")] = json::value::number(U(this->location.placement.rack_offset));
    k[U("Placement")] = l;
    j[U("Location")] = k;

    // TODO Thermal, Power 추가 필요
    j[U("Thermal")] = this->thermal->get_odata_id_json();
    j[U("Power")] = this->power->get_odata_id_json();
    j[U("Sensors")] = this->sensors->get_odata_id_json();
    return j;
}

bool Chassis::load_json(json::value &j)
{
    json::value status, loc, postal_address, placement;
    string s_led;

    try{
        Resource::load_json(j);
        this->id = j.at("Id").as_string();
        this->chassis_type = j.at("ChassisType").as_string();
        this->manufacturer = j.at("Manufacturer").as_string();
        this->model = j.at("Model").as_string();
        this->serial_number = j.at("SerialNumber").as_string();
        this->part_number = j.at("PartNumber").as_string();
        this->asset_tag = j.at("AssetTag").as_string();
        this->power_state = j.at("PowerState").as_string();

        s_led = j.at("IndicatorLED").as_string();
        if (s_led == "Off")
            this->indicator_led = LED_OFF;
        else if (s_led == "Lit")
            this->indicator_led = LED_LIT;
        else if (s_led == "Blinking")
            this->indicator_led = LED_BLINKING;
        else
            log(warning) << "led value is strange..";
        
        status = j.at("Status");
        this->status.state = status.at("State").as_string();
        this->status.health = status.at("Health").as_string();

        loc = j.at("Location");
        postal_address = loc.at("PostalAddress");
        this->location.postal_address.country = postal_address.at("Country").as_string();
        this->location.postal_address.territory = postal_address.at("Territory").as_string();
        this->location.postal_address.city = postal_address.at("City").as_string();
        this->location.postal_address.street = postal_address.at("Street").as_string();
        this->location.postal_address.house_number = postal_address.at("HouseNumber").as_string();
        this->location.postal_address.name = postal_address.at("Name").as_string();
        this->location.postal_address.postal_code = postal_address.at("PostalCode").as_string();

        placement = loc.at("Placement");
        this->location.placement.row = placement.at("Row").as_string();
        this->location.placement.rack = placement.at("Rack").as_string();
        this->location.placement.rack_offset_units = placement.at("RackOffsetUnits").as_string();
        this->location.placement.rack_offset = placement.at("RackOffset").as_integer();

        // Thermal, Power, Sensor 주소 추가 필요
    }
    catch (json::json_exception &e)
    {
        return false;
    }

    return true;

}

pplx::task<void> Chassis::led_off(uint8_t _led_index)
{
    uint8_t *indicator_led = &this->indicator_led;
    *indicator_led = LED_OFF;
    log(info) << this->name << ": LED off";
    return pplx::create_task([indicator_led, _led_index] {
        GPIO_CLR = 1 << _led_index;
    });
}

pplx::task<void> Chassis::led_lit(uint8_t _led_index)
{
    uint8_t *indicator_led = &this->indicator_led;
    *indicator_led = LED_LIT;
    log(info) << this->name << ": LED lit";
    return pplx::create_task([indicator_led, _led_index] {
        GPIO_SET = 1 << _led_index;
    });
}

pplx::task<void> Chassis::led_blinking(uint8_t _led_index)
{

    uint8_t *indicator_led = &this->indicator_led;
    *indicator_led = LED_BLINKING;
    log(info) << this->name << ": LED blinking";
    return pplx::create_task([indicator_led, _led_index] {
        while (*indicator_led == LED_BLINKING)
        {
            GPIO_CLR = 1 << _led_index;
            usleep(30000);
            GPIO_SET = 1 << _led_index;
            usleep(30000);
        }
    });
}
// Chassis end

// Manager start
json::value Manager::get_json(void)
{
    auto j = this->Resource::get_json();
    if (j.is_null())
        return j;
    
    j[U("Id")] = json::value::string(U(this->id));
    j[U("ManagerType")] = json::value::string(U(this->manager_type));
    j[U("Description")] = json::value::string(U(this->description));
    j[U("FirmwareVersion")] = json::value::string(U(this->firmware_version));
    j[U("DateTime")] = json::value::string(U(this->datetime));
    j[U("DateTimeLocalOffset")] = json::value::string(U(this->datetime_offset));
    j[U("Model")] = json::value::string(U(this->model));
    j[U("UUID")] = json::value::string(U(this->uuid));
    j[U("PowerState")] = json::value::string(U(this->power_state));

    // j[U("NetworkProtocol")] = this->network->get_odata_id_json();
    j[U("EthernetInterfaces")] = this->ethernet->get_odata_id_json();
    j[U("LogServices")] = this->log_service->get_odata_id_json();

    json::value k;
    k[U("State")] = json::value::string(U(this->status.state));
    k[U("Health")] = json::value::string(U(this->status.health));
    j[U("Status")] = k;

    // json::value j_act;
    // for(int i=0; i<this->actions.size(); i++)
    // {
    //     string act = "#";
    //     act = act + this->actions[i]->action_name;
    //     j_act[U(act)] = this->actions[i]->get_json();
    // }
    
    // j[U("Actions")] = j_act;

    return j;
}

bool Manager::load_json(json::value &j)
{
    json::value status;
    
    try{
        Resource::load_json(j);
        this->id = j.at("Id").as_string();
        this->manager_type = j.at("ManagerType").as_string();
        this->description = j.at("Description").as_string();
        this->uuid = j.at("UUID").as_string();
        this->model = j.at("Model").as_string();
        this->firmware_version = j.at("FirmwareVersion").as_string();
        this->datetime = j.at("DateTime").as_string();
        this->datetime_offset = j.at("DateTimeLocalOffset").as_string();
        this->power_state = j.at("PowerState").as_string();

        status = j.at("Status");
        this->status.state = status.at("State").as_string();
        this->status.health = status.at("Health").as_string();        
    }
    catch (json::json_exception &e)
    {
        return false;
    }

    return true;

}

json::value EthernetInterfaces::get_json(void)
{
    auto j = this->Resource::get_json();
    if (j.is_null())
        return j;
    
    j[U("Id")] = json::value::string(U(this->id));
    j[U("Description")] = json::value::string(U(this->description));
    j[U("LinkStatus")] = json::value::string(U(this->link_status));
    j[U("PermanentMACAddress")] = json::value::string(U(this->permanent_mac_address));
    j[U("MACAddress")] = json::value::string(U(this->mac_address));
    j[U("SpeedMbps")] = json::value::number(U(this->speed_Mbps));
    j[U("AutoNeg")] = json::value::boolean(U(this->autoneg));
    j[U("FullDuplex")] = json::value::boolean(U(this->full_duplex));
    j[U("MTUSize")] = json::value::number(U(this->mtu_size));
    j[U("HostName")] = json::value::string(U(this->hostname));
    j[U("FQDN")] = json::value::string(U(this->fqdn));
    j[U("IPv6DefaultGateway")] = json::value::string(U(this->ipv6_default_gateway));
    
    json::value k;
    k[U("State")] = json::value::string(U(this->status.state));
    k[U("Health")] = json::value::string(U(this->status.health));
    j[U("Status")] = k;

    j[U("NameServers")] = json::value::array(); // 네임서버
    for(int i=0; i<this->name_servers.size(); i++)
        j[U("NameServers")][i] = json::value::string(U((this->name_servers)[i]));

    json::value dh_v4, dh_v6;
    json::value v_ip4, v_ip6;
    json::value o_ip4, o_ip6;

    dh_v4[U("DHCPEnabled")] = json::value::boolean(U(this->dhcp_v4.dhcp_enabled));
    dh_v4[U("UseDNSServers")] = json::value::boolean(U(this->dhcp_v4.use_dns_servers));
    dh_v4[U("UseGateway")] = json::value::boolean(U(this->dhcp_v4.use_gateway));
    dh_v4[U("UseNTPServers")] = json::value::boolean(U(this->dhcp_v4.use_ntp_servers));
    dh_v4[U("UseStaticRoutes")] = json::value::boolean(U(this->dhcp_v4.use_static_routes));
    dh_v4[U("UseDomainName")] = json::value::boolean(U(this->dhcp_v4.use_domain_name));
    j[U("DHCPv4")] = dh_v4;

    dh_v6[U("OperatingMode")] = json::value::string(U(this->dhcp_v6.operating_mode));
    dh_v6[U("UseDNSServers")] = json::value::boolean(U(this->dhcp_v6.use_dns_servers));
    dh_v6[U("UseDomainName")] = json::value::boolean(U(this->dhcp_v6.use_domain_name));
    dh_v6[U("UseNTPServers")] = json::value::boolean(U(this->dhcp_v6.use_ntp_servers));
    dh_v6[U("UseRapidCommit")] = json::value::boolean(U(this->dhcp_v6.use_rapid_commit));
    j[U("DHCPv6")] = dh_v6;

    v_ip4 = json::value::array();
    for(int i=0; i<this->v_ipv4.size(); i++)
    {
        o_ip4 = json::value::object();
        o_ip4[U("Address")] = json::value::string(U(this->v_ipv4[i].address));
        o_ip4[U("SubnetMask")] = json::value::string(U(this->v_ipv4[i].subnet_mask));
        o_ip4[U("AddressOrigin")] = json::value::string(U(this->v_ipv4[i].address_origin));
        o_ip4[U("Gateway")] = json::value::string(U(this->v_ipv4[i].gateway));
        v_ip4[i] = o_ip4;
    }
    j[U("IPv4Addresses")] = v_ip4;

    v_ip6 = json::value::array();
    for(int i=0; i<this->v_ipv6.size(); i++)
    {
        o_ip6 = json::value::object();
        o_ip6[U("Address")] = json::value::string(U(this->v_ipv6[i].address));
        o_ip6[U("AddressState")] = json::value::string(U(this->v_ipv6[i].address_state));
        o_ip6[U("AddressOrigin")] = json::value::string(U(this->v_ipv6[i].address_origin));
        o_ip6[U("PrefixLength")] = json::value::number(U(this->v_ipv6[i].prefix_length));
        v_ip6[i] = o_ip6;
    }
    j[U("IPv6Addresses")] = v_ip6;

    return j;
}

bool EthernetInterfaces::load_json(json::value &j)
{
    json::value status;
    json::value name_servers, dhcp_v4, dhcp_v6, v_ipv4, v_ipv6, vlan; 
    
    try{
        Resource::load_json(j);
        this->id = j.at("Id").as_string();
        this->description = j.at("Description").as_string();
        this->link_status = j.at("LinkStatus").as_string();
        this->permanent_mac_address = j.at("PermanentMACAddress").as_string();
        this->mac_address = j.at("MACAddress").as_string();
        this->speed_Mbps = j.at("SpeedMbps").as_integer();
        this->autoneg = j.at("AutoNeg").as_bool();
        this->full_duplex = j.at("FullDuplex").as_bool();
        this->mtu_size = j.at("MTUSize").as_integer();
        this->hostname = j.at("HostName").as_string();
        this->fqdn = j.at("FQDN").as_string();
        
        status = j.at("Status");
        this->status.state = status.at("State").as_string();
        this->status.health = status.at("Health").as_string();

        name_servers = j.at("NameServers");
        for (auto item : name_servers.as_array())
            this->name_servers.push_back(item.as_string());

        dhcp_v4 = j.at("DHCPv4");
        this->dhcp_v4.dhcp_enabled = dhcp_v4.at("DHCPEnabled").as_bool();
        this->dhcp_v4.use_dns_servers = dhcp_v4.at("UseDNSServers").as_bool();
        this->dhcp_v4.use_domain_name = dhcp_v4.at("UseDomainName").as_bool();
        this->dhcp_v4.use_gateway = dhcp_v4.at("UseGateway").as_bool();
        this->dhcp_v4.use_ntp_servers = dhcp_v4.at("UseNTPServers").as_bool();
        this->dhcp_v4.use_static_routes = dhcp_v4.at("UseStaticRoutes").as_bool();

        dhcp_v6 = j.at("DHCPv6");
        this->dhcp_v6.operating_mode = dhcp_v6.at("OperatingMode").as_string();
        this->dhcp_v6.use_dns_servers = dhcp_v6.at("UseDNSServers").as_bool();
        this->dhcp_v6.use_domain_name = dhcp_v6.at("UseDomainName").as_bool();
        this->dhcp_v6.use_ntp_servers = dhcp_v6.at("UseNTPServers").as_bool();
        this->dhcp_v6.use_rapid_commit = dhcp_v6.at("UseRapidCommit").as_bool();

        v_ipv4 = j.at("IPv4Addresses");
        for (auto item : v_ipv4.as_array()){
            IPv4_Address temp;
            temp.address = v_ipv4.at("Address").as_string();
            temp.address_origin = v_ipv4.at("AddressOrigin").as_string();
            temp.subnet_mask = v_ipv4.at("SubnetMask").as_string();
            temp.gateway = v_ipv4.at("Gateway").as_string();
        }
        
        v_ipv6 = j.at("IPv6Addresses");
        for (auto item : v_ipv6.as_array()){
            IPv6_Address temp;
            temp.address = v_ipv6.at("Address").as_string();
            temp.address_origin = v_ipv6.at("AddressOrigin").as_string();
            temp.address_state = v_ipv6.at("AddressState").as_string();
            temp.prefix_length = v_ipv6.at("PrefixLength").as_integer();
        }
    }
    catch (json::json_exception &e)
    {
        log(warning) << "read something failed in ethernet interfaces" << endl;
        return true;
    }

    return true;
}

json::value NetworkProtocol::get_json(void)
{
    auto j = this->Resource::get_json();
    if (j.is_null())
        return j;
    
    json::value snmp,ipmi,ntp,kvmip,https,http;
    j[U("FQDN")] = json::value::string(U(this->fqdn));
    j[U("Id")] = json::value::string(U(this->id));
    j[U("HostName")] = json::value::string(U(this->hostname));
    j[U("Description")] = json::value::string(U(this->description));

    json::value k = json::value::object();
    k[U("State")] = json::value::string(U(this->status.state));
    k[U("Health")] = json::value::string(U(this->status.health));
    j[U("Status")] = k;

    snmp[U("ProtocolEnabled")] = json::value::boolean(this->snmp_enabled);
    snmp[U("Port")] = json::value::number(U(this->snmp_port));
    j[U("SNMP")]=snmp;
    
    ipmi[U("ProtocolEnabled")] = json::value::boolean(this->ipmi_enabled);
    ipmi[U("Port")] = json::value::number(U(this->ipmi_port));
    j[U("IPMI")]=ipmi;

    kvmip[U("ProtocolEnabled")] = json::value::boolean(this->kvmip_enabled);
    kvmip[U("Port")] = json::value::number(U(this->kvmip_port));
    j[U("KVMIP")]=kvmip;

    http[U("ProtocolEnabled")] = json::value::boolean(this->http_enabled);
    http[U("Port")] = json::value::number(U(this->http_port));
    j[U("HTTP")]=http;

    https[U("ProtocolEnabled")] = json::value::boolean(this->https_enabled);
    https[U("Port")] = json::value::number(U(this->https_port));
    j[U("HTTPS")]=https;

    ntp[U("ProtocolEnabled")] = json::value::boolean(this->ntp_enabled);
    for (unsigned int i = 0; i < this->v_netservers.size(); i++)
        ntp[U("NTPServers")][i] = json::value::string(this->v_netservers[i]);
    j[U("NTP")]=ntp;
    return j;
}
// Manager end

// bool NetworkProtocol::load_json(json::value &j)
// {
//     json::value status;
//     json::value http, https, ipmi, ssh, snmp, virtual_media, ssdp, telnet, kvmip, 
//     try{
//         Resource::load_json(j);
//         this->id = j.at("Id").as_string();
//         this->fqdn = j.at("FQDN").as_string();
//         this->hostname = j.at("HostName").as_string();
//         this->description = j.at("Description").as_string();
        

//         status = j.at("Status");
//         this->status.state = status.at("State").as_string();
//         this->status.health = status.at("Health").as_string();

//     }
//     catch (json::json_exception &e)
//     {
//         return false;
//     }

//     return true;
// }

// Task Service start
json::value TaskService::get_json(void)
{
    auto j = this->Resource::get_json();
    if (j.is_null())
        return j;
    
    json::value k;
    j[U("Id")] = json::value::string(U(this->id));
    k[U("State")] = json::value::string(U(this->status.state));
    k[U("Health")] = json::value::string(U(this->status.health));
    j[U("Status")] = k;
    j[U("ServiceEnabled")] = json::value::boolean(U(this->service_enabled));
    j[U("DateTime")] = json::value::string(U(this->datetime));
    j[U("Tasks")] = this->task_collection->get_odata_id_json();

    return j;
}

bool TaskService::load_json(json::value &j)
{
    json::value status;
    
    try{
        Resource::load_json(j);
        this->id = j.at("Id").as_string();
        this->datetime = j.at("DateTime").as_string();
        this->service_enabled = j.at("ServiceEnabled").as_bool();
        
        status = j.at("Status");
        this->status.state = status.at("State").as_string();
        this->status.health = status.at("Health").as_string();
    }
    catch (json::json_exception &e)
    {
        return false;
    }

    return true;
    
}
// Task Service end

// Task start
json::value Task::get_json(void)
{
    auto j = this->Resource::get_json();
    if (j.is_null())
        return j;

    j[U("Id")] = json::value::string(U(this->id));
    j[U("StartTime")] = json::value::string(U(this->start_time));
    j[U("EndTime")] = json::value::string(U(this->end_time));
    j[U("TaskState")] = json::value::string(U(this->task_state));
    j[U("TaskStatus")] = json::value::string(U(this->task_status));

    json::value k;
    json::value l;
    std::map<string,string>::iterator iter;
    for(iter = this->payload.http_headers.begin(); iter!=this->payload.http_headers.end(); iter++)
    {
        l[U(iter->first)] = json::value::string(U(iter->second));
    }
    
    k[U("HttpHeaders")] = l;
    k[U("HttpOperation")] = json::value::string(U(this->payload.http_operation));
    k[U("JsonBody")] = this->payload.json_body;
    k[U("TargetUri")] = json::value::string(U(this->payload.target_uri));
    j[U("Payload")] = k;

    return j;
}

bool Task::load_json(json::value &j)
{
    json::value payload, http_headers;

    try{
        Resource::load_json(j);
        this->id = j.at("Id").as_string();
        this->start_time = j.at("StartTime").as_string();
        this->end_time = j.at("EndTime").as_string();
        this->task_state = j.at("TaskState").as_string();
        this->task_status = j.at("TaskStatus").as_string();

        payload = j.at("Payload");
        this->payload.http_operation = payload.at("HttpOperation").as_string();
        this->payload.json_body = payload.at("JsonBody");
        this->payload.target_uri = payload.at("TargetUri").as_string();
        
        http_headers = payload.at("HttpHeaders");
        for(auto iter = http_headers.as_object().begin(); iter != http_headers.as_object().end(); ++iter)
        {
            this->payload.http_headers[iter->first] = iter->second.as_string();
        }
    }
    catch (json::json_exception &e)
    {
        log(warning) << "read something failed in Task";
        return true;
    }

    return true;
}

void Task::set_payload(http_headers _header, string _operation, json::value _json, string _target_uri)
{
    web::http::http_headers::iterator iter;
    for(iter = _header.begin(); iter!=_header.end(); iter++)
    {
        this->payload.http_headers[iter->first] = iter->second; //.insert(make_pair(iter->first,iter->second));
    }

    this->payload.http_operation = _operation;
    this->payload.json_body = _json;
    this->payload.target_uri = _target_uri;

}
// Task end

// System start
json::value Systems::get_json(void)
{
    auto j = this->Resource::get_json();
    if (j.is_null())
        return j;    

    j[U("Id")] = json::value::string(this->id);
    j[U("SystemType")] = json::value::string(this->system_type);
    j[U("AssetTag")] = json::value::string(this->asset_tag);
    j[U("Manufacturer")] = json::value::string(this->manufacturer);
    j[U("Model")] = json::value::string(this->model);
    j[U("SerialNumber")] = json::value::string(this->serial_number);
    j[U("PartNumber")] = json::value::string(this->part_number);
    j[U("Description")] = json::value::string(this->description);
    j[U("UUID")] = json::value::string(this->uuid);
    j[U("HostName")] = json::value::string(this->hostname);

    json::value k = json::value::object();
    k[U("State")] = json::value::string(U(this->status.state));
    k[U("Health")] = json::value::string(U(this->status.health));
    j[U("Status")] = k;

    switch (this->indicator_led)
    {
    case LED_OFF:
        j[U("IndicatorLED")] = json::value::string(U("Off"));
        break;
    case LED_BLINKING:
        j[U("IndicatorLED")] = json::value::string(U("Blinking"));
        break;
    case LED_LIT:
        j[U("IndicatorLED")] = json::value::string(U("Lit"));
        break;
    }
    
    j[U("PowerState")] = json::value::string(this->power_state);
    j[U("BiosVersion")] = json::value::string(this->bios_version);

    json::value j_boot;
    j_boot[U("BootSourceOverrideEnabled")] = json::value::string(U(this->boot.boot_source_override_enabled));
    j_boot[U("BootSourceOverrideTarget")] = json::value::string(U(this->boot.boot_source_override_target));
    j_boot[U("BootSourceOverrideMode")] = json::value::string(U(this->boot.boot_source_override_mode));
    j_boot[U("UefiTargetBootSourceOverride")] = json::value::string(U(this->boot.uefi_target_boot_source_override));
    j[U("Boot")] = j_boot;

    j[U("Bios")] = this->bios->get_odata_id_json();
    j[U("Processors")] = this->processor->get_odata_id_json();
    j[U("Memory")] = this->memory->get_odata_id_json();
    j[U("EthernetInterfaces")] = this->ethernet->get_odata_id_json();
    j[U("SimpleStorage")] = this->simple_storage->get_odata_id_json();
    j[U("LogServices")] = this->log_service->get_odata_id_json();
    // json::value j_act;
    // for(int i=0; i<this->actions.size(); i++)
    // {
    //     string act = "#";
    //     act = act + this->actions[i]->action_name;
    //     j_act[U(act)] = this->actions[i]->get_json();
    // }
    // j[U("Actions")] = j_act;
    
    return j;
}

bool Systems::load_json(json::value &j)
{
    json::value status, boot; // 구조체
    json::value storage, processor, memory, ethernet, log_service, simple_storage; // 컬렉션

    try{
        Resource::load_json(j);
        this->id = j.at("Id").as_string();
        this->system_type = j.at("SystemType").as_string();
        this->manufacturer = j.at("Manufacturer").as_string();
        this->model = j.at("Model").as_string();
        this->serial_number = j.at("SerialNumber").as_string();
        this->part_number = j.at("PartNumber").as_string();
        this->description = j.at("Description").as_string();
        this->hostname = j.at("HostName").as_string();
        this->asset_tag = j.at("AssetTag").as_string();
        this->power_state = j.at("PowerState").as_string();
        this->uuid = j.at("UUID").as_string();

        status = j.at("Status");
        this->status.state = status.at("State").as_string();
        this->status.health = status.at("Health").as_string();

        string led_status = j.at("IndicatorLED").as_string();
        if (led_status == "Off")
            this->indicator_led = LED_OFF;
        else if (led_status == "Lit")
            this->indicator_led = LED_LIT;
        else if (led_status == "Blinking")
            this->indicator_led = LED_BLINKING;
        else
            log(warning) << "led value is strange..";
        
        boot = j.at("Boot");
        this->boot.boot_source_override_enabled = boot.at("BootSourceOverrideEnabled").as_string();
        this->boot.boot_source_override_mode = boot.at("BootSourceOverrideMode").as_string();
        this->boot.boot_source_override_target = boot.at("BootSourceOverrideTarget").as_string();
        this->boot.uefi_target_boot_source_override = boot.at("UefiTargetBootSourceOverride").as_string();
    }
    catch (json::json_exception &e)
    {
        return false;
    }

    return true;    
}
// System end

// in System start
json::value Processors::get_json(void)
{
    auto j = this->Resource::get_json();
    if (j.is_null())
        return j;
    
    j[U("Id")] = json::value::string(U(this->id));
    j[U("Socket")] = json::value::string(U(this->socket));
    j[U("ProcessorType")] = json::value::string(U(this->processor_type));
    j[U("ProcessorArchitecture")] = json::value::string(U(this->processor_architecture));
    j[U("InstructionSet")] = json::value::string(U(this->instruction_set));
    j[U("Manufacturer")] = json::value::string(U(this->manufacturer));
    j[U("Model")] = json::value::string(U(this->model));
    j[U("MaxSpeedMHz")] = json::value::number(U(this->max_speed_mhz));
    j[U("TotalCores")] = json::value::number(U(this->total_cores));
    j[U("TotalThreads")] = json::value::number(U(this->total_threads));

    json::value k = json::value::object();
    k[U("State")] = json::value::string(U(this->status.state));
    k[U("Health")] = json::value::string(U(this->status.health));
    j[U("Status")] = k;

    json::value l;
    l[U("VendorId")] = json::value::string(U(this->p_id.vendor_id));
    l[U("IdentificationRegisters")] = json::value::string(U(this->p_id.identification_registers));
    l[U("EffectiveFamily")] = json::value::string(U(this->p_id.effective_family));
    l[U("EffectiveModel")] = json::value::string(U(this->p_id.effective_model));
    l[U("Step")] = json::value::string(U(this->p_id.step));
    l[U("MicrocodeInfo")] = json::value::string(U(this->p_id.microcode_info));
    j[U("ProcessorId")] = l;

    return j;
}

bool Processors::load_json(json::value &j)
{
    json::value status, p_id;

    try{
        Resource::load_json(j);
        this->id = j.at("Id").as_string();
        this->socket = j.at("Socket").as_string();
        this->processor_type = j.at("ProcessorType").as_string();
        this->processor_architecture = j.at("ProcessorArchitecture").as_string();
        this->instruction_set = j.at("InstructionSet").as_string();
        this->manufacturer = j.at("Manufacturer").as_string();
        this->model = j.at("Model").as_string();
        this->max_speed_mhz = j.at("MaxSpeedMHz").as_integer();
        this->total_cores = j.at("TotalCores").as_integer();
        this->total_threads = j.at("TotalThreads").as_integer();
        
        status = j.at("Status");
        this->status.state = status.at("State").as_string();
        this->status.health = status.at("Health").as_string();

        p_id = j.at("ProcessorId");
        this->p_id.vendor_id = p_id.at("VendorId").as_string();
        this->p_id.identification_registers = p_id.at("IdentificationRegisters").as_string();
        this->p_id.effective_family = p_id.at("EffectiveFamily").as_string();
        this->p_id.effective_model = p_id.at("EffectiveModel").as_string();
        this->p_id.step = p_id.at("Step").as_string();
        this->p_id.microcode_info = p_id.at("MicrocodeInfo").as_string();
    }
    catch (json::json_exception &e)
    {
        return false;
    }

    return true;
}
// json::value ProcessorSummary::get_json(void)
// {
//     auto j = this->Resource::get_json();
    // if (j.is_null())
    //     return j;
    
//     j[U("Id")] = json::value::string(U(this->id));
//     j[U("Count")] = json::value::number(U(this->count));
//     j[U("LogicalProcessorCount")] = json::value::number(U(this->logical_processor_count));
//     j[U("Model")] = json::value::string(U(this->model));

//     json::value k;
//     k[U("State")] = json::value::string(U(this->status.state));
//     k[U("Health")] = json::value::string(U(this->status.health));
//     j[U("Status")] = k;

//     return j;
// }

json::value Storage::get_json(void)
{
    auto j = this->Resource::get_json();
    if (j.is_null())
        return j;
    
    j[U("Id")] = json::value::string(U(this->id));
    j[U("Description")] = json::value::string(U(this->description));

    json::value k;
    k[U("State")] = json::value::string(U(this->status.state));
    k[U("Health")] = json::value::string(U(this->status.health));
    j[U("Status")] = k;

    j[U("StorageControllers")] = json::value::array();
    for(int i=0; i<this->controller->members.size(); i++)
        j[U("StorageControllers")][i] = ((StorageControllers *)this->controller->members[i])->get_json();


    return j;
}

bool Storage::load_json(json::value &j)
{
    json::value status;

    try{
        Resource::load_json(j);
        
        this->id = j.at("Id").as_string();
        this->description = j.at("Description").as_string();

        status = j.at("Status");
        this->status.state = status.at("State").as_string();
        this->status.health = status.at("Health").as_string();
    }
    catch (json::json_exception &e)
    {
        return false;
    }

    return true;

}

json::value StorageControllers::get_json(void)
{
    auto j = this->Resource::get_json();
    if (j.is_null())
        return j;
    
    j[U("Id")] = json::value::string(U(this->id));
    j[U("Manufacturer")] = json::value::string(U(this->manufacturer));
    j[U("Model")] = json::value::string(U(this->model));
    j[U("SerialNumber")] = json::value::string(U(this->serial_number));
    j[U("PartNumber")] = json::value::string(U(this->part_number));
    j[U("SpeedGbps")] = json::value::number(U(this->speed_gbps));
    j[U("FirmwareVersion")] = json::value::string(U(this->firmware_version));

    json::value k;
    k[U("DurableName")] = json::value::string(U(this->identifier.durable_name));
    k[U("DurableNameFormat")] = json::value::string(U(this->identifier.durable_name_format));
    j[U("Identifiers")] = k;

    json::value l;
    l[U("State")] = json::value::string(U(this->status.state));
    l[U("Health")] = json::value::string(U(this->status.health));
    j[U("Status")] = l;

    j[U("SupportedControllerProtocols")] = json::value::array();
    for(int i=0; i<this->support_controller_protocols.size(); i++)
        j[U("SupportedControllerProtocols")][i] = json::value::string(U((this->support_controller_protocols)[i]));

    j[U("SupportedDeviceProtocols")] = json::value::array();
    for(int i=0; i<this->support_device_protocols.size(); i++)
        j[U("SupportedDeviceProtocols")][i] = json::value::string(U((this->support_device_protocols)[i]));


    return j;
}

bool StorageControllers::load_json(json::value &j)
{
    json::value status, identifier;
    json::value support_controller_protocols, support_device_protocols;

    try{
        Resource::load_json(j);
        this->id = j.at("Id").as_string();
        this->manufacturer = j.at("Manufacturer").as_string();
        this->model = j.at("Model").as_string();
        this->serial_number = j.at("SerialNumber").as_string();
        this->part_number = j.at("PartNumber").as_string();
        this->speed_gbps = j.at("SpeedGbps").as_double();
        this->firmware_version = j.at("FirmwareVersion").as_string();

        status = j.at("Status");
        this->status.state = status.at("State").as_string();
        this->status.health = status.at("Health").as_string();

        identifier = j.at("Identifier");
        this->identifier.durable_name = identifier.at("DurableName").as_string();
        this->identifier.durable_name_format = identifier.at("DurableNameFormat").as_string();

        support_controller_protocols = j.at("SupportedControllerProtocols");
        for (auto str : support_controller_protocols.as_array())
            this->support_controller_protocols.push_back(str.as_string());

        support_device_protocols = j.at("SupportedDeviceProtocols");
        for (auto str : support_device_protocols.as_array())
            this->support_device_protocols.push_back(str.as_string());   
    }   
    catch (json::json_exception &e)
    {
        log(warning) << "read something failed in storage controllers";
        return true;
    }

    return true;

}

json::value SimpleStorage::get_json(void)
{
    auto j = this->Resource::get_json();
    if (j.is_null())
        return j;
    
    j[U("Id")] = json::value::string(U(this->id));
    j[U("Description")] = json::value::string(U(this->description));
    j[U("UefiDevicePath")] = json::value::string(U(this->uefi_device_path));
    // j[U("FileSystem")] = json::value::string(U(this->file_system));


    json::value k;
    k[U("State")] = json::value::string(U(this->status.state));
    k[U("Health")] = json::value::string(U(this->status.health));
    j[U("Status")] = k;

    j[U("Devices")] = json::value::array();
    for(int i=0; i<this->devices.size(); i++)
    {
        json::value o = json::value::object();
        o[U("Name")] = json::value::string(U(this->devices[i].name));
        o[U("Manufacturer")] = json::value::string(U(this->devices[i].manufacturer));
        o[U("Model")] = json::value::string(U(this->devices[i].model));
        o[U("CapacityKBytes")] = json::value::number(U(this->devices[i].capacity_KBytes));
        o[U("FileSystem")] = json::value::string(U(this->devices[i].file_system));

        json::value ok;
        ok[U("State")] = json::value::string(U(this->devices[i].status.state));
        ok[U("Health")] = json::value::string(U(this->devices[i].status.health));
        o[U("Status")] = ok;
        j[U("Devices")][i] = o;
    }

    return j;
}

bool SimpleStorage::load_json(json::value &j)
{
    json::value status, devices, dev_status;

    try{
        Resource::load_json(j);
        this->id = j.at("Id").as_string();
        this->description = j.at("Description").as_string();
        this->uefi_device_path = j.at("UefiDevicePath").as_string();
        
        status = j.at("Status");
        this->status.state = status.at("State").as_string();
        this->status.health = status.at("Health").as_string();

        devices = j.at("Devices");
        for(auto item : devices.as_array()){
            Device_Info temp;
            temp.name = item.at("Name").as_string();
            temp.manufacturer = item.at("Manufacturer").as_string();
            temp.model = item.at("Model").as_string();
            temp.capacity_KBytes = item.at("CapacityKBytes").as_integer();

            dev_status = item.at("Status");
            temp.status.state = dev_status.at("State").as_string();
            temp.status.health = dev_status.at("Health").as_string();
            
            this->devices.push_back(temp);
        }
    }
    catch (json::json_exception &e)
    {
        return false;
    }

    return true;

}

json::value Bios::get_json(void)
{
    auto j = this->Resource::get_json();
    if (j.is_null()){
        cout << "j is null!" << endl;
        return j;
    }
    j[U("Id")] = json::value::string(U(this->id));
    j[U("AttributeRegistry")] = json::value::string(U(this->attribute_registry));

    json::value k;
    k[U("BootMode")] = json::value::string(U(this->attribute.boot_mode));
    k[U("EmbeddedSata")] = json::value::string(U(this->attribute.embedded_sata));
    k[U("NicBoot1")] = json::value::string(U(this->attribute.nic_boot1));
    k[U("NicBoot2")] = json::value::string(U(this->attribute.nic_boot2));
    k[U("PowerProfile")] = json::value::string(U(this->attribute.power_profile));
    k[U("ProcCoreDisable")] = json::value::number(U(this->attribute.proc_core_disable));
    k[U("ProcHyperthreading")] = json::value::string(U(this->attribute.proc_hyper_threading));
    k[U("ProcTurboMode")] = json::value::string(U(this->attribute.proc_turbo_mode));
    k[U("UsbControl")] = json::value::string(U(this->attribute.usb_control));
    j[U("Attributes")] = k;


    return j;
}

bool Bios::load_json(json::value &j)
{
    json::value attribute;
    try{
        Resource::load_json(j);
        this->id = j.at("Id").as_string();
        this->attribute_registry = j.at("AttributeRegistry").as_string();

        attribute = j.at("Attributes");
        this->attribute.boot_mode = attribute.at("BootMode").as_string();
        this->attribute.embedded_sata = attribute.at("EmbeddedSata").as_string();
        this->attribute.nic_boot1 = attribute.at("NicBoot1").as_string();
        this->attribute.nic_boot2 = attribute.at("NicBoot2").as_string();
        this->attribute.power_profile = attribute.at("PowerProfile").as_string();
        this->attribute.proc_core_disable = attribute.at("ProcCoreDisable").as_integer();
        this->attribute.proc_hyper_threading = attribute.at("ProcHyperthreading").as_string();
        this->attribute.proc_turbo_mode = attribute.at("ProcTurboMode").as_string();
        this->attribute.usb_control = attribute.at("UsbControl").as_string();
    }
    catch (json::json_exception &e)
    {
        return false;
    }

    return true;
}

json::value Memory::get_json(void)
{
    auto j = this->Resource::get_json();
    if (j.is_null())
        return j;
    
    j[U("Id")] = json::value::string(U(this->id));
    j[U("RankCount")] = json::value::number(U(this->rank_count));
    j[U("CapacityKiB")] = json::value::number(U(this->capacity_kib));
    j[U("DataWidthBits")] = json::value::number(U(this->data_width_bits));
    j[U("BusWidthBits")] = json::value::number(U(this->bus_width_bits));
    j[U("ErrorCorrection")] = json::value::string(U(this->error_correction));
    j[U("MemoryType")] = json::value::string(U(this->memory_type));
    j[U("MemoryDeviceType")] = json::value::string(U(this->memory_device_type));
    j[U("BaseModuleType")] = json::value::string(U(this->base_module_type));

    json::value k;
    k[U("State")] = json::value::string(U(this->status.state));
    k[U("Health")] = json::value::string(U(this->status.health));
    j[U("Status")] = k;

    j[U("MaxTDPMilliWatts")] = json::value::array();
    for(int i=0; i<this->max_TDP_milliwatts.size(); i++)
        j[U("MaxTDPMilliWatts")][i] = json::value::number(U(this->max_TDP_milliwatts[i]));

    json::value loc = json::value::object();
    loc[U("Socket")] = json::value::number(U(this->m_location.socket));
    loc[U("MemoryController")] = json::value::number(U(this->m_location.memory_controller));
    loc[U("Channel")] = json::value::number(U(this->m_location.channel));
    loc[U("Slot")] = json::value::number(U(this->m_location.slot));
    j[U("MemoryLocation")] = loc;


    j[U("MemoryMedia")] = json::value::array();
    for(int i=0; i<this->memory_media.size(); i++)
        j[U("MemoryMedia")][i] = json::value::string(U(this->memory_media[i]));




    return j;
}

bool Memory::load_json(json::value &j)
{
    json::value status, m_location, max_TDP_milliwatts, memory_media;

    try{
        Resource::load_json(j);
        this->id = j.at("Id").as_string();
        this->rank_count = j.at("RankCount").as_integer();
        this->capacity_kib = j.at("CapacityKiB").as_integer();
        this->data_width_bits = j.at("DataWidthBits").as_integer();
        this->bus_width_bits = j.at("BusWidthBits").as_integer();
        this->error_correction = j.at("ErrorCorrection").as_string();
        this->memory_type = j.at("MemoryType").as_string();
        this->memory_device_type = j.at("MemoryDeviceType").as_string();
        this->base_module_type = j.at("BaseModuleType").as_string();
        
        status = j.at("Status");
        this->status.state = status.at("State").as_string();
        this->status.health = status.at("Health").as_string();

        m_location = j.at("MemoryLocation");
        this->m_location.socket = m_location.at("Socket").as_integer();
        this->m_location.memory_controller = m_location.at("MemoryController").as_integer();
        this->m_location.channel = m_location.at("Channel").as_integer();
        this->m_location.slot = m_location.at("Slot").as_integer();

        max_TDP_milliwatts = j.at("MaxTDPMilliWatts");
        for(auto item : max_TDP_milliwatts.as_array())
            this->max_TDP_milliwatts.push_back(item.as_integer());
        
        memory_media = j.at("MemoryMedia");
        for(auto item : memory_media.as_array())
            this->memory_media.push_back(item.as_string());
    }
    catch (json::json_exception &e)
    {
        return false;
    }

    return true;
}
// in System end

// dy : certificate start
json::value Certificate::get_json(void)
{
    auto j = this->Resource::get_json();
    if (j.is_null())
        return j;
    
    j["Id"] = json::value::string(this->id);
    j["CertificateString"] = json::value::string(this->certificateString);
    j["CertificateType"] = json::value::string(this->certificateType);
    
    json::value Issuer = json::value::object();
    Issuer["Country"] = json::value::string(this->issuer.country);
    Issuer["State"] = json::value::string(this->issuer.state);
    Issuer["City"] = json::value::string(this->issuer.city);
    Issuer["Organization"] = json::value::string(this->issuer.organization);
    Issuer["OrganizationUnit"] = json::value::string(this->issuer.organizationUnit);
    Issuer["CommonName"] = json::value::string(this->issuer.commonName);
    j["Issuer"] = Issuer;

    json::value Subject = json::value::object();
    Subject["Country"] = json::value::string(this->subject.country);
    Subject["State"] = json::value::string(this->subject.state);
    Subject["City"] = json::value::string(this->subject.city);
    Subject["Organization"] = json::value::string(this->subject.organization);
    Subject["OrganizationUnit"] = json::value::string(this->subject.organizationUnit);
    Subject["CommonName"] = json::value::string(this->subject.commonName);
    j["Subject"] = Subject;

    j["ValidNotBefore"] = json::value::string(this->validNotBefore);
    j["ValidNotAfter"] = json::value::string(this->validNotAfter);
    
    j["KeyUsage"] = json::value::array();
    for(int i = 0; i < this->keyUsage.size(); i++)
        j["KeyUsage"][i] = json::value::string(this->keyUsage[i]);
    
    return j;
}

bool Certificate::load_json(json::value &j)
{
    json::value issuer, subject;
    json::value key_usage;

    try {
        this->certificateString = j.at("CertificateString").as_string();
        this->certificateType = j.at("CertificateType").as_string();
        this->id = j.at("Id").as_string();
        this->validNotAfter = j.at("ValidNotAfter").as_string();
        this->validNotBefore = j.at("ValidNotBefore").as_string();

        key_usage = j.at("KeyUsage");
        for(json::value item : key_usage.as_array())
            this->keyUsage.push_back(item.at("@odata.id").as_string());
        
        issuer = j.at("Issuer");
        this->issuer.city = issuer.at("City").as_string();
        this->issuer.commonName = issuer.at("CommonName").as_string();
        this->issuer.country = issuer.at("Country").as_string();
        this->issuer.organization = issuer.at("Organization").as_string();
        this->issuer.organizationUnit = issuer.at("OrganizationUnit").as_string();
        this->issuer.state = issuer.at("State").as_string();

        subject = j.at("Subject");
        this->subject.city = subject.at("City").as_string();
        this->subject.commonName = subject.at("CommonName").as_string();
        this->subject.country = subject.at("Country").as_string();
        this->subject.organization = subject.at("Organization").as_string();
        this->subject.organizationUnit = subject.at("OrganizationUnit").as_string();
        this->subject.state = subject.at("State").as_string();
    }
    catch (json::json_exception &e)
    {
        return false;
    }

    return true;
}

json::value CertificateService::get_json(void)
{
    auto j = this->Resource::get_json();
    if (j.is_null())
        return j;
    
    j["Id"] = json::value::string(this->id);
    
    j["CertificateLocations"] = json::value::array();
    for(int i=0; i<this->certificate_location->members.size(); i++)
        j["CertificateLocations"][i] = this->certificate_location->members[i]->get_odata_id_json();

    return j;
}

bool CertificateService::load_json(json::value &j)
{
    try{
        Resource::load_json(j);
        this->id = j.at("Id").as_string();
    }
    catch (json::json_exception &e)
    {
        return false;
    }

    return true;
}
// dy : certificate end


// // 틀틀틀 복붙
// json::value ProcessorSummary::get_json(void)
// {
//     auto j = this->Resource::get_json();
    // if (j.is_null())
    //     return j;
    
//     j[U("Id")] = json::value::string(U(this->id));


//     return j;
// }




// ServiceRoot start
json::value ServiceRoot::get_json(void)
{
    auto j = this->Resource::get_json();
    if (j.is_null())
        return j;
    
    j[U("Id")] = json::value::string(U(this->id));
    j[U("RedfishVersion")] = json::value::string(U(this->redfish_version));
    j[U("UUID")] = json::value::string(U(this->uuid));
    j[U("Systems")] = this->system_collection->get_odata_id_json();
    j[U("Chassis")] = this->chassis_collection->get_odata_id_json();
    j[U("Managers")] = this->manager_collection->get_odata_id_json();
    j[U("AccountService")] = this->account_service->get_odata_id_json();
    j[U("SessionService")] = this->session_service->get_odata_id_json();
    // j[U("Tasks")] = this->task->get_odata_id_json();
    // j[U("EventService")] = this->event_service->get_odata_id_json();
    return j;
}

// bool ServiceRoot::load_json(json::value &j)
// {
//     try
//     {
//         this->name = j.at("Name").as_string();
//         this->id = j.at("Id").as_string();
//         this->redfish_version = j.at("RedfishVersion").as_string();
//         this->uuid = j.at("UUID").as_string();

//     }
//     catch (json::json_exception &e)
//     {
//         cout << "Error Parsing JSON file " + this->odata.id << endl;
//         return false;
//     }

//     return true;
// }
// ServiceRoot end

bool is_session_valid(const string _token_id)
{
    Session *session;
    string odata_id = ODATA_SESSION_ID;
    odata_id = odata_id + '/' + _token_id;
    if (!record_is_exist(odata_id))
        return false;
    session = (Session *)g_record[odata_id];
    if (session->_remain_expires_time <= 0)
        return false;
    return true;
}

const std::string currentDateTime(void)
{
    time_t     now = time(0); //현재 시간을 time_t 타입으로 저장
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct); // YYYY-MM-DD.HH:mm:ss 형태의 스트링

    return buf;
}