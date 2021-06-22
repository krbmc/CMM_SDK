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
    
    record_load_json();
    cout << "\t\t dy : load json complete" << endl;
    
    // init_record_bmc();

    // add_new_bmc("1", "10.0.6.104", BMC_PORT, false, "TEST_ONE", "PASS_ONE");
    // add_new_bmc("500", "10.0.6.104", BMC_PORT, false, "TEST_ONE", "PASS_ONE");
    // cout << "\t\t dy : add new bmc complete" << endl;
    
    record_save_json();
    cout << "\t\t dy : save json complete" << endl;
    
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
    j[U("@odata.type")] = json::value::string(U(this->odata.type));
    j[U("Name")] = json::value::string(U(this->name));
    j[U("@odata.id")] = json::value::string(U(this->odata.id));
    j["type"] = json::value::string(to_string(this->type)); // uint_8
    return j;
}

json::value Resource::get_odata_id_json(void)
{
    json::value j;
    j[U("@odata.id")] = json::value::string(U(this->odata.id));
    return j;
}

bool Resource::save_json(void)
{

    // Make all of sub directories
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

    // Save json file to path
    ofstream out(this->odata.id + ".json");
    out << record_get_json(this->odata.id).serialize() << endl;
    out.close();
    cout << "\t\t dy : save complete : " << this->odata.id + ".json" << endl;

    return true;
}

bool Resource::load_resource_json(json::value &j)
{
    int type;
    try
    {
        ifstream target_file(this->odata.id + ".json");
        stringstream string_stream;

        string_stream << target_file.rdbuf();
        target_file.close();

        j = json::value::parse(string_stream);
        // cout << "\t\t dy in load resource : " << j << endl;

        this->name = j.at("Name").as_string();
        this->type = atoi(j.at("type").as_string().c_str());
        this->odata.id = j.at("@odata.id").as_string();
        this->odata.type = j.at("@odata.type").as_string();
        g_record[this->odata.id] = this;
    }
    catch (json::json_exception &e)
    {
        throw json::json_exception("Error Parsing JSON file " + this->odata.id);
        return false;
    }

    
    return true;
}
// Resource end

// Collection start
json::value Collection::get_json(void)
{
    auto j = this->Resource::get_json();
    j[U("Members")] = json::value::array();
    for (unsigned int i = 0; i < this->members.size(); i++)
        j[U("Members")][i] = this->members[i]->get_odata_id_json();
    j[U("Members@odata.count")] = json::value::number(U(this->members.size()));
    return j;
}

void Collection::add_member(Resource *_resource)
{
    this->members.push_back(_resource);
}

bool Collection::load_json(void)
{
    json::value j;

    try
    {
        ifstream target_file(this->odata.id + ".json");
        stringstream string_stream;

        string_stream << target_file.rdbuf();
        target_file.close();

        j = json::value::parse(string_stream);
        this->name = j.at("Name").as_string();
        log(info) << this->name;
    }
    catch (json::json_exception &e)
    {
        throw json::json_exception("Error Parsing JSON file " + this->odata.id);
        return false;
    }

    return true;
}
// Collection end

// List start
json::value List::get_json(void)
{
    auto j = this->Resource::get_json();

    j[U(this->name)] = json::value::array();
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
    json::value k;
    j[U("Id")] = json::value::string(U(this->id));
    j[U("Enabled")] = json::value::boolean(U(this->enabled));
    j[U("Password")] = json::value::string(U(this->password));
    j[U("UserName")] = json::value::string(U(this->user_name));
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
    j[U("Locked")] = json::value::boolean(U(this->locked));
    j[U("Links")] = k;
    return j;
}

// Account end

// Role start
json::value Role::get_json(void)
{
    auto j = this->Resource::get_json();
    j[U("Id")] = json::value::string(U(this->id));
    j[U("IsPredefined")] = json::value::boolean(U(this->is_predefined));
    for (unsigned int i = 0; i < this->assigned_privileges.size(); i++)
        j[U("AssignedPrivileges")][i] = json::value::string(U(this->assigned_privileges[i]));
    return j;
}
// Role end

// AccountService start
json::value AccountService::get_json(void)
{
    auto j = this->Resource::get_json();
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
    j[U("AccountLockoutCounterResetEnabled")] = json::value::number(U(this->account_lockout_counter_reset_enabled));
    j[U("Accounts")] = this->account_collection->get_odata_id_json();
    j[U("Roles")] = this->role_collection->get_odata_id_json();
    return j;
}
// AccountService end

// SessionService start
json::value SessionService::get_json(void)
{
    auto j = this->Resource::get_json();
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
// SessionService end

// Session start
json::value Session::get_json(void)
{
    auto j = this->Resource::get_json();
    j[U("Id")] = json::value::string(U(this->id));
    j[U("UserName")] = json::value::string(U(this->account->user_name));
    return j;
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
            g_record.erase(path); // 레코드 지운거
            record_save_json(); // 레코드 json파일 갱신
            string json_path = path;
            json_path = json_path + ".json";
            if(remove(json_path.c_str()) < 0)
            {
                cout << "delete error in session remove" << endl;
            }
            // 세션 json파일 제거
            delete session;
            cout << record_get_json(ODATA_SESSION_ID) << endl;
            cout << "LOGOUT!!!!" << endl;
        });
}
// Session end


// Log Service & Log Entry start
json::value LogService::get_json(void)
{
    auto j = this->Resource::get_json();
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

json::value LogEntry::get_json(void)
{
    auto j = this->Resource::get_json();
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

// Log Service & Log Entry end

// Event Service & Event Destination start
json::value EventDestination::get_json(void)
{
    auto j = this->Resource::get_json();
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

json::value EventService::get_json(void)
{
    auto j = this->Resource::get_json();
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



// Update Service & Software Inventory end

// Temperature start
json::value Temperature::get_json(void)
{
    auto j = this->Resource::get_json();
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
// Fan end

// Thermal start
json::value Thermal::get_json(void)
{
    unsigned int i;
    auto j = this->Resource::get_json();

    j[U("Id")] = json::value::string(U(this->id));
    j[U("Temperatures")] = json::value::array();
    for (i = 0; i < this->temperatures->members.size(); i++)
        j[U("Temperatures")][i] = ((Temperature *)this->temperatures->members[i])->get_json();
    j[U("Fans")] = json::value::array();
    for (i = 0; i < this->fans->members.size(); i++)
        j[U("Fans")][i] = ((Fan *)this->fans->members[i])->get_json();

    return j;
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

    // k.null();
    // cout << "null k" << endl;
    // cout << k << endl;

    json::value thresh;
    json::value i;
    i[U("Reading")] = json::value::number(U(this->thresh.upper_critical.reading));
    i[U("Activation")] = json::value::string(U(this->thresh.upper_critical.activation));
    thresh[U("UpperCritical")] = i;

    // i.null();
    i[U("Reading")] = json::value::number(U(this->thresh.upper_caution.reading));
    i[U("Activation")] = json::value::string(U(this->thresh.upper_caution.activation));
    thresh[U("UpperCaution")] = i;

    // i.null();
    i[U("Reading")] = json::value::number(U(this->thresh.upper_fatal.reading));
    i[U("Activation")] = json::value::string(U(this->thresh.upper_fatal.activation));
    thresh[U("UpperFatal")] = i;

    // i.null();
    i[U("Reading")] = json::value::number(U(this->thresh.lower_critical.reading));
    i[U("Activation")] = json::value::string(U(this->thresh.lower_critical.activation));
    thresh[U("LowerCritical")] = i;

    // i.null();
    i[U("Reading")] = json::value::number(U(this->thresh.lower_caution.reading));
    i[U("Activation")] = json::value::string(U(this->thresh.lower_caution.activation));
    thresh[U("LowerCaution")] = i;

    // i.null();
    i[U("Reading")] = json::value::number(U(this->thresh.lower_fatal.reading));
    i[U("Activation")] = json::value::string(U(this->thresh.lower_fatal.activation));
    thresh[U("LowerFatal")] = i;

    j[U("Thresholds")] = thresh;


    return j;
}


// Sensor end

// Power &  PowerControl, Voltage, PowerSupply start

json::value PowerSupply::get_json(void)
{
    // json::value j, k;
    auto j = this->Resource::get_json();
    json::value k;

    j[U("MemberId")] = json::value::string(U(this->member_id));
    j[U("@odata.id")] = json::value::string(U(this->odata.id));
    j[U("Name")] = json::value::string(U(this->name));

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

json::value Voltage::get_json(void)
{
    // json::value j, k;
    auto j = this->Resource::get_json();
    json::value k;

    j[U("MemberId")] = json::value::string(U(this->member_id));
    j[U("@odata.id")] = json::value::string(U(this->odata.id));
    j[U("Name")] = json::value::string(U(this->name));
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


json::value PowerControl::get_json(void)
{
    // json::value j, k;
    auto j = this->Resource::get_json();
    json::value k;

    j[U("MemberId")] = json::value::string(U(this->member_id));
    j[U("@odata.id")] = json::value::string(U(this->odata.id));
    j[U("Name")] = json::value::string(U(this->name));
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

json::value Power::get_json(void)
{
    unsigned int i;
    auto j = this->Resource::get_json();

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

// Power &  PowerControl, Voltage, PowerSupply end

// Chassis start
json::value Chassis::get_json(void)
{
    auto j = this->Resource::get_json();
    json::value k;
    json::value l;
    j[U("Id")] = json::value::string(U(this->id));
    j[U("ChassisType")] = json::value::string(U(this->chassis_type));
    j[U("Manufacturer")] = json::value::string(U(this->manufacturer));
    j[U("Model")] = json::value::string(U(this->model));
    // j[U("SKU")] = json::value::string(U(this->sku));
    j[U("SerialNumber")] = json::value::string(U(this->serial_number));
    j[U("PartNumber")] = json::value::string(U(this->part_number));
    j[U("AssetTag")] = json::value::string(U(this->asset_tag));
    j[U("PowerState")] = json::value::string(U(this->power_state));

    // j[U("HeightMm")] = json::value::number(U(this->height_mm));
    // j[U("WidthMm")] = json::value::number(U(this->width_mm));
    // j[U("DepthMm")] = json::value::number(U(this->depth_mm));
    // j[U("WeightKg")] = json::value::number(U(this->weight_kg));

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
    j[U("Id")] = json::value::string(U(this->id));
    j[U("ManagerType")] = json::value::string(U(this->manager_type));
    j[U("Description")] = json::value::string(U(this->description));
    j[U("FirmwareVersion")] = json::value::string(U(this->firmware_version));
    j[U("DateTime")] = json::value::string(U(this->datetime));
    j[U("DateTimeLocalOffset")] = json::value::string(U(this->datetime_offset));
    j[U("Model")] = json::value::string(U(this->model));
    j[U("UUID")] = json::value::string(U(this->uuid));
    j[U("PowerState")] = json::value::string(U(this->power_state));

    j[U("NetworkProtocol")] = this->network->get_odata_id_json();
    j[U("EthernetInterfaces")] = this->ethernet->get_odata_id_json();
    j[U("LogServices")] = this->log_service->get_odata_id_json();

    json::value k;
    k[U("State")] = json::value::string(U(this->status.state));
    k[U("Health")] = json::value::string(U(this->status.health));
    j[U("Status")] = k;

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
    //     act = act + ((Actions *)*iter)->action_by + "." + ((Actions *)*iter)->action_what;
    //     j_act[U(act)] = ((Actions *)(*iter))->get_json();
    // }
    j[U("Actions")] = j_act;

    return j;
}

json::value EthernetInterfaces::get_json(void)
{
    auto j = this->Resource::get_json();
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


    

    // json::value snmp,ipmi,ntp,kvmip,https,http;
    // j[U("FQDN")] = json::value::string(U(this->fqdn));
    // j[U("IPv6DefaultGateway")] = json::value::string(U("::"));
    // j[U("MACAddress")] = json::value::string(U(this->macaddress));
    // j[U("MACAddress")] = json::value::string(U(this->macaddress));
    // j[U("Name")] = json::value::string(U(this->name));
    // j[U("Description")] = json::value::string(U(this->description));
    // j[U("MTUSize")] = json::value::string(U(this->mtusize));
    // j[U("LinkStatus")] = json::value::string(U(this->linkstatus));
    
    // json::value ipv4=json::value::object();
    // json::value ipv4address =json::value::array();
    // ipv4[U("Address")]=json::value::string(U(this->address));
    // ipv4[U("SubnetMask")]=json::value::string(U(this->subnetMask));
    // ipv4[U("Gateway")]=json::value::string(U(this->gateway));
    // ipv4[U("AddressOrigin")]=json::value::string(U(this->addressOrigin));
    // ipv4address[0]=ipv4;
    // j[U("IPv4Addresses")] =ipv4address;
    
    // j[U("HostName")] = json::value::string(U(this->hostname));

    // json::value ipv6=json::value::object();
    // json::value ipv6address =json::value::array();
    // ipv6[U("Address")]=json::value::string(U(this->addressv6));
    // ipv6[U("PrefixLength")]=json::value::string(U(this->prefixLength));
    // ipv6[U("AddressOrigin")]=json::value::string(U(this->addressOriginv6));
    // ipv6address[0]=ipv6;
    // j[U("IPv6Addresses")] =ipv6address;
    // j[U("IPv6DefaultGateway")] =json::value::string(U(this->gatewayv6));

    return j;
}

json::value NetworkProtocol::get_json(void)
{
    auto j = this->Resource::get_json();
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

// Task Service start
json::value TaskService::get_json(void)
{
    auto j = this->Resource::get_json();
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
// Task Service end

// Task start
json::value Task::get_json(void)
{
    auto j = this->Resource::get_json();
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
    
    
    
    // payload 추가

    return j;
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
    // log(info)<<"1";
    auto j = this->Resource::get_json();
    // j[U("NAME")] = json::value::string(this->name);
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

    // j[U("HostingRoles")] = json::value::array();
    // for(int i=0; i<this->hosting_roles.size(); i++)
    //     j[U("HostingRoles")][i] = json::value::string(U(this->hosting_roles[i]));

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

    // cout << " 여기일거야 " << endl;

    // if(this->bios)
    // {
        j[U("Bios")] = this->bios->get_odata_id_json();
    //     cout << "있어서 트루" << endl;
    // }
    // else
    //     cout << "없어서 폴스" << endl;
    // cout << " 그치? " << endl;
    
    j[U("Processors")] = this->processor->get_odata_id_json();
    j[U("Memory")] = this->memory->get_odata_id_json();
    j[U("EthernetInterfaces")] = this->ethernet->get_odata_id_json();
    j[U("SimpleStorage")] = this->simple_storage->get_odata_id_json();
    j[U("LogServices")] = this->log_service->get_odata_id_json();

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

// System end

// in System start
json::value Processors::get_json(void)
{
    auto j = this->Resource::get_json();
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

// json::value ProcessorSummary::get_json(void)
// {
//     auto j = this->Resource::get_json();
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

json::value StorageControllers::get_json(void)
{
    auto j = this->Resource::get_json();
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

json::value SimpleStorage::get_json(void)
{
    auto j = this->Resource::get_json();
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

json::value Bios::get_json(void)
{
    auto j = this->Resource::get_json();
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

json::value Memory::get_json(void)
{
    auto j = this->Resource::get_json();
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

// dy : certificate start
json::value Certificate::get_json(void)
{
    json::value j = this->Resource::get_json();
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
        throw json::json_exception("json parsing error in Certificate");
        return false;
    }

    g_record[this->odata.id] = this;
    cout << "\t\t dy : certificate load check : " << get_json() << endl;
    
    return true;
}

json::value CertificateService::get_json(void)
{
    json::value j = this->Resource::get_json();
    j["Id"] = json::value::string(this->id);

    j["CertificateLocations"] = json::value::array();
    for(int i=0; i<this->certificate_location->members.size(); i++)
        j["CertificateLocations"][i] = this->certificate_location->members[i]->get_odata_id_json();

    return j;
}

bool CertificateService::load_json(json::value &j)
{
    json::value certificates;
    
    if (record_is_exist(this->odata.id))
    if (!Resource::load_resource_json(j)){
        fprintf(stderr, "load resource error in Certificate Service\n");
        return false;    
    }

    try{
        certificates = j.at("CertificateLocations");
        
        for (json::value item : certificates.as_array())
            certificate_location->s_members.push_back(item.at("@odata.id").as_string());
    }
    catch (json::json_exception &e)
    {
        throw json::json_exception("json parsing error in Certificate Service");
        return false;
    }

    g_record[this->odata.id] = this;
    cout << "\t\t dy : certificate service load check : " << get_json() << endl;
    return true;
}
// dy : certificate end


// // 틀틀틀 복붙
// json::value ProcessorSummary::get_json(void)
// {
//     auto j = this->Resource::get_json();
//     j[U("Id")] = json::value::string(U(this->id));


//     return j;
// }


// in System end


// ServiceRoot start
json::value ServiceRoot::get_json(void)
{
    auto j = this->Resource::get_json();
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

bool ServiceRoot::load_json(void)
{
    json::value j; // JSON read from input file

    try
    {
        ifstream target_file(this->odata.id + ".json");
        stringstream string_stream;

        string_stream << target_file.rdbuf();
        target_file.close();

        j = json::value::parse(string_stream);
    }
    catch (json::json_exception &e)
    {
        throw json::json_exception("Error Parsing JSON file " + this->odata.id);
        return false;
    }

    this->name = j.at("Name").as_string();
    this->id = j.at("Id").as_string();
    this->redfish_version = j.at("RedfishVersion").as_string();
    this->uuid = j.at("UUID").as_string();
    return true;
}
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