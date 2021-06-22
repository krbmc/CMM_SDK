#ifndef RESOURCE_H
#define RESOURCE_H

#include "stdafx.hpp"
#include "hwcontrol.hpp"
#include "chassis_controller.hpp"

/**
 * @brief Redfish information
 */
#define REDFISH_VERSION "v1"

/**
 * @brief Open data protocol information
 */
#define ODATA_TYPE_VERSION REDFISH_VERSION "_0_0"
// Open data protocol path
#define ODATA_SERVICE_ROOT_ID "/redfish/" REDFISH_VERSION
#define ODATA_SYSTEM_ID ODATA_SERVICE_ROOT_ID "/Systems"
#define ODATA_CHASSIS_ID ODATA_SERVICE_ROOT_ID "/Chassis"
#define ODATA_MANAGER_ID ODATA_SERVICE_ROOT_ID "/Managers"

#define ODATA_ETHERNET_INTERFACE_ID ODATA_MANAGER_ID "/EthernetInterfaces" // @@@@ 추가

#define ODATA_TASK_SERVICE_ID ODATA_SERVICE_ROOT_ID "/TaskService"

#define ODATA_TASK_ID ODATA_TASK_SERVICE_ID "/Tasks" // @@@@추가

#define ODATA_SESSION_SERVICE_ID ODATA_SERVICE_ROOT_ID "/SessionService"
#define ODATA_SESSION_ID ODATA_SESSION_SERVICE_ID "/Sessions"
#define ODATA_ACCOUNT_SERVICE_ID ODATA_SERVICE_ROOT_ID "/AccountService"
#define ODATA_ACCOUNT_ID ODATA_ACCOUNT_SERVICE_ID "/Accounts"
#define ODATA_ROLE_ID ODATA_ACCOUNT_SERVICE_ID "/Roles"
#define ODATA_EVENT_SERVICE_ID ODATA_SERVICE_ROOT_ID "/EventService"
#define ODATA_EVENT_DESTINATION_ID ODATA_EVENT_SERVICE_ID "/Subscriptions"
#define ODATA_UPDATE_SERVICE_ID ODATA_SERVICE_ROOT_ID "/UpdateService"

#define ODATA_HEARTBEAT ODATA_SYSTEM_ID "/Heartbeat" // @@@@추가

// Open data protocol resource type
#define ODATA_RESOURCE_TYPE "#Resource." ODATA_TYPE_VERSION ".Resource"
#define ODATA_SERVICE_ROOT_TYPE "#ServiceRoot." ODATA_TYPE_VERSION ".ServiceRoot"
#define ODATA_COLLECTION_TYPE "#Collection.Collection"
#define ODATA_SYSTEM_COLLECTION_TYPE "#ComputerSystemCollection.ComputerSystemCollection"
#define ODATA_SYSTEM_TYPE "#ComputerSystem." ODATA_TYPE_VERSION ".ComputerSystem"
#define ODATA_PROCESSOR_COLLECTION_TYPE "#ProcessorCollection.ProcessorCollection"
#define ODATA_PROCESSOR_TYPE "#Processor." ODATA_TYPE_VERSION ".Processor"
#define ODATA_MEMORY_COLLECTION_TYPE "#MemoryCollection.MemoryCollection" // @@@@ 추가
#define ODATA_MEMORY_TYPE "#Memory." ODATA_TYPE_VERSION ".Memory" // @@@@ 추가
#define ODATA_PROCESSOR_SUMMARY_TYPE "#ProcessorSummary." ODATA_TYPE_VERSION ".ProcessorSummary" // @@@@ 추가
#define ODATA_NETWORK_INTERFACE_TYPE "#NetworkInterface." ODATA_TYPE_VERSION ".NetworkInterface" // @@@@ 추가
#define ODATA_STORAGE_TYPE "#Storage." ODATA_TYPE_VERSION ".Storage" // @@@@ 추가
#define ODATA_STORAGE_CONTROLLER_TYPE "#StorageController." ODATA_TYPE_VERSION ".StorageController" // @@@@ 추가
#define ODATA_STORAGE_COLLECTION_TYPE "#StorageCollection.StorageCollection" // @@@@@ 추가
#define ODATA_BIOS_TYPE "#Bios." ODATA_TYPE_VERSION ".Bios" // @@@@ 추가
#define ODATA_SIMPLE_STORAGE_COLLECTION_TYPE "#SimpleStorageCollection.SimpleStorageCollection"
#define ODATA_SIMPLE_STORAGE_TYPE "#SimpleStorage." ODATA_TYPE_VERSION ".SimpleStorage"
#define ODATA_CHASSIS_COLLECTION_TYPE "#ChassisCollection.ChassisCollection"
#define ODATA_CHASSIS_TYPE "#Chassis." ODATA_TYPE_VERSION ".Chassis"
#define ODATA_SENSOR_TYPE "#Sensor." ODATA_TYPE_VERSION ".Sensor" // @@@@ 추가
#define ODATA_SENSOR_COLLECTION_TYPE "#SensorCollection.SensorCollection" // @@@@ 추가
#define ODATA_THERMAL_TYPE "#Thermal." ODATA_TYPE_VERSION ".Thermal"
#define ODATA_POWER_TYPE "#Power." ODATA_TYPE_VERSION ".Power"
#define ODATA_MANAGER_COLLECTION_TYPE "#ManagerCollection.ManagerCollection"
#define ODATA_MANAGER_TYPE "#Manager." ODATA_TYPE_VERSION ".Manager"

#define ODATA_ACTIONS_TYPE "#Actions." ODATA_TYPE_VERSION ".Actions"
#define ODATA_ACTIONS_COLLECTION_TYPE "#ActionsCollection.ActionsCollection"

#define ODATA_NETWORK_PROTOCOL_TYPE "#NetworkProtocol." ODATA_TYPE_VERSION ".NetworkProtocol" // @@@@ 추가

#define ODATA_ETHERNET_INTERFACE_COLLECTION_TYPE "#EthernetInterfaceCollection.EthernetInterfaceCollection"
#define ODATA_ETHERNET_INTERFACE_TYPE "#EthernetInterface." ODATA_TYPE_VERSION ".EthernetInterface"
#define ODATA_LOG_SERVICE_COLLECTION_TYPE "#LogServiceCollection.LogServiceCollection"
#define ODATA_LOG_SERVICE_TYPE "#LogService." ODATA_TYPE_VERSION ".LogService"
#define ODATA_LOG_ENTRY_COLLECTION_TYPE "#LogEntryCollection.LogEntryCollection"
#define ODATA_LOG_ENRTY_TYPE "#LogEntry." ODATA_TYPE_VERSION ".LogEntry"
#define ODATA_TASK_SERVICE_TYPE "#TaskService." ODATA_TYPE_VERSION ".TaskService"
#define ODATA_TASK_COLLECTION_TYPE "#TaskCollection.TaskCollection"

#define ODATA_TASK_TYPE "#Task." ODATA_TYPE_VERSION ".Task" // @@@@추가

#define ODATA_SESSION_SERVICE_TYPE "#SessionService." ODATA_TYPE_VERSION ".SessionService"
#define ODATA_SESSION_COLLECTION_TYPE "#SessionCollection.SessionCollection"
#define ODATA_SESSION_TYPE "#Session." ODATA_TYPE_VERSION ".Session"
#define ODATA_ACCOUNT_SERVICE_TYPE "#AccountService." ODATA_TYPE_VERSION ".AccountService"
#define ODATA_ACCOUNT_COLLECTION_TYPE "#ManagerAccountCollection.ManagerAccountCollection"
#define ODATA_ACCOUNT_TYPE "#ManagerAccount." ODATA_TYPE_VERSION ".ManagerAccount"
#define ODATA_ROLE_COLLECTION_TYPE "#RoleCollection.RoleCollection"
#define ODATA_ROLE_TYPE "#Role." ODATA_TYPE_VERSION ".Role"
#define ODATA_EVENT_SERVICE_TYPE "#EventService." ODATA_TYPE_VERSION ".EventService"
#define ODATA_EVENT_DESTINATION_TYPE "#EventDestination." ODATA_TYPE_VERSION ".EventDestination" // @@@@ 추가
#define ODATA_EVENT_DESTINATION_COLLECTION_TYPE "#EventDestinationCollection.EventDestinationCollection" // @@@@ 추가
// #define ODATA_DESTINATION_COLLECTION_TYPE "#EventDestinationCollection.EventDestinationCollection"
// #define ODATA_DESTINATION_TYPE "#EventDestination." ODATA_TYPE_VERSION ".EventDestination"
#define ODATA_UPDATE_SERVICE_TYPE "#UpdateService." ODATA_TYPE_VERSION ".UpdateService" // @@@@ 추가
#define ODATA_UPDATE_SERVICE_COLLECTION_TYPE "#UpdateServiceCollection.UpdateServiceCollection" // @@@@ 추가
#define ODATA_SOFTWARE_INVENTORY_TYPE "#SoftwareInventory." ODATA_TYPE_VERSION ".SoftwareInventory" // @@@@ 추가
#define ODATA_SOFTWARE_INVENTORY_COLLECTION_TYPE "#SoftwareInventoryCollection.SoftwareInventoryCollection" // @@@@ 추가

/**
 * @brief Redfish resource status element
 */
#define STATUS_STATE_ENABLED "Enabled"
#define STATUS_STATE_DISABLED "Disabled"
#define STATUS_STATE_STAND_BY_OFFLINE "StandbyOffline"
#define STATUS_STATE_STARTING "Starting"
#define STATUS_HEALTH_OK "OK"
#define STATUS_HEALTH_CRITICAL "Critical"
#define STATUS_HEALTH_WARNING "Warning"

/**
 * @brief Redfish chassis resource indicator led state element
 */
#define LED_OFF 0
#define LED_BLINKING 1
#define LED_LIT 2

/**
 * @brief Redfish chassis resource power state element
 */
#define POWER_STATE_OFF "Off"
#define POWER_STATE_ON "On"
#define POWER_STATE_POWERING_OFF "PoweringOff"
#define POWER_STATE_POWERING_ON "PoweringOn"

/**
 * @brief Redfish Task State & Task Status element
 */
#define TASK_STATE_NEW "New"
#define TASK_STATE_RUNNING "Running"
#define TASK_STATE_COMPLETED "Completed"
#define TASK_STATE_CANCELLED "Cancelled"
#define TASK_STATE_SUSPENDED "Suspended"
#define TASK_STATE_KILLED "Killed"

#define TASK_STATUS_CRITICAL "Critical"
#define TASK_STATUS_OK "OK"
#define TASK_STATUS_WARNING "Warning"

const std::string currentDateTime();

/**
 * @brief CMM_ID / CMM_ADDRESS
 * 
 */
#define CMM_ID "1"
#define CMM_ADDRESS "https://10.0.6.107:443"

/**
 * @brief Resource class type
 */
enum RESOURCE_TYPE
{
    SUPER_TYPE,
    SERVICE_ROOT_TYPE,
    COLLECTION_TYPE,
    LIST_TYPE,
    ACTIONS_TYPE,
    SYSTEM_TYPE,
    STORAGE_TYPE,
    STORAGE_CONTROLLER_TYPE,
    SIMPLE_STORAGE_TYPE,
    BIOS_TYPE,
    PROCESSOR_TYPE,
    MEMORY_TYPE,
    PROCESSOR_SUMMARY_TYPE,
    NETWORK_INTERFACE_TYPE,
    CHASSIS_TYPE,
    THERMAL_TYPE,
    TEMPERATURE_TYPE,
    FAN_TYPE,
    SENSOR_TYPE,
    POWER_TYPE,
    POWER_CONTROL_TYPE,
    VOLTAGE_TYPE,
    POWER_SUPPLY_TYPE,
    MANAGER_TYPE,
    NETWORK_PROTOCOL_TYPE,
    ETHERNET_INTERFACE_TYPE,
    LOG_SERVICE_TYPE,
    LOG_ENTRY_TYPE,
    TASK_SERVICE_TYPE,
    TASK_TYPE,
    SESSION_SERVICE_TYPE,
    SESSION_TYPE,
    ACCOUNT_SERVICE_TYPE,
    ACCOUNT_TYPE,
    ROLE_TYPE,
    EVENT_SERVICE_TYPE,
    EVENT_DESTINATION_TYPE,
    UPDATE_SERVICE_TYPE,
    SOFTWARE_INVENTORY_TYPE,
    DESTINATION_TYPE
};

/**
 * @brief Sensor context
 */
enum SENSOR_CONTEXT
{
    INTAKE_CONTEXT,
    CPU_CONTEXT
};

/**
 * @brief Open data protocol information
 *        @odata.id
 *        @odata.type
 */
typedef struct _OData
{
    string id;
    string type;
} OData;

/**
 * @brief Status of redfish resource
 */
typedef struct _Status
{
    string state;
    string health;
} Status;

/**
 * @brief PostalAddress information of machine
 */
typedef struct _PostalAddress
{
    string country;
    string territory;
    string city;
    string street;
    string house_number;
    string name;
    string postal_code;
} PostalAddress;

/**
 * @brief Placement information of machine
 */
typedef struct _Placement
{
    string row;
    string rack;
    string rack_offset_units;
    unsigned int rack_offset;
} Placement;

/**
 * @brief Location information of machine
 */
typedef struct _Location
{
    PostalAddress postal_address;
    Placement placement;
} Location;

/**
 * @brief struct addition
 * @authors 강
 */
// typedef struct _Actions
// {
//     std::vector<string> actions;
//     string type;
//     string target;
//     string actioncommand;
// } Actions;

typedef struct _Boot
{
    // BootSOE(Source Override Enabled)
    string boot_source_override_enabled;
    string boot_source_override_target;
    string boot_source_override_mode;
    string uefi_target_boot_source_override;

    // 여기에 enabled, target, mode 3가지 들어갈수있는 Allowable value를 넣어놓아야하나?

    // string BootSourceOverrideEnabled;
    // string BootSourceOverrideTarget;
} Boot;

typedef struct _MemoryLocation
{
    unsigned int channel;
    unsigned int memory_controller;
    unsigned int slot;
    unsigned int socket;

} MemoryLocation;

// typedef struct _MemorySummary
// {
//     double total_system_memory_GiB;
//     double total_system_persistent_memory_GiB;
//     string memory_mirroring;
//     Status status;

// } MemorySummary; // 뺄듯

typedef struct _DHCP_v4
{
    bool dhcp_enabled;
    bool use_dns_servers;
    bool use_gateway;
    bool use_ntp_servers;
    bool use_static_routes;
    bool use_domain_name;

} DHCP_v4;

typedef struct _DHCP_v6
{
    string operating_mode;
    bool use_dns_servers;
    bool use_ntp_servers;
    bool use_domain_name;
    bool use_rapid_commit;

} DHCP_v6;

typedef struct _IPv4_Address
{
    string address;
    string address_origin;
    string subnet_mask;
    string gateway;

} IPv4_Address;

typedef struct _IPv6_Address
{
    string address;
    string address_origin;
    string address_state;
    int prefix_length;

} IPv6_Address;

typedef struct _VLAN
{
    bool vlan_enable;
    int vlan_id;
} Vlan;


typedef struct _Device_Info
{
    unsigned int capacity_KBytes;
    string file_system;
    string manufacturer;
    string model;
    string name;
    Status status;

} Device_Info;



/**
 * @brief Task Resource에 들어가는 Payload
 * @author 강
 * @details http_hearders에 들어온 요청의 헤더, http_operation에 요청의 종류(get,post..)
 * json_body에 json값, target_uri에 api list값
 */
typedef struct _Payload
{
    map<string, string> http_headers;
    string http_operation;
    json::value json_body;
    string target_uri;

} Payload;

typedef struct _Identifier
{
    string durable_name;
    string durable_name_format;
} Identifier;

typedef struct _Attribute
{
    string boot_mode;
    string embedded_sata;
    string nic_boot1;
    string nic_boot2;
    string power_profile;
    int proc_core_disable;
    string proc_hyper_threading;
    string proc_turbo_mode;
    string usb_control;
} Attribute;

typedef struct _ProcessorId
{
    string identification_registers;
    string effective_family;
    string effective_model;
    string step;
    string microcode_info;
    string vendor_id;
} ProcessorId;

// typedef struct _ProcessorSummary
// {
//     int count;
//     int logical_processor_count;
//     // Metrics *metrics; // class 만들어야함
//     string model;
//     Status status;

// } ProcessorSummary;

typedef struct _SSE_Filter_Properties_Supported
{
    bool event_type;
    bool metric_report_definition;
    bool registry_prefix;
    bool resource_type;
    bool event_format_type;
    bool message_id;
    bool origin_resource;
    bool subordinate_resources;

} SSE_filter;

typedef struct _PowerLimit
{
    int correction_in_ms;
    string limit_exception;
    double limit_in_watts;

} PowerLimit; // in PowerControl

typedef struct _PowerMetrics
{
    int interval_in_min;
    double min_consumed_watts;
    double max_consumed_watts;
    double avg_consumed_watts;

} PowerMetrics; // in PowerControl

typedef struct _InputRange
{
    string input_type;
    double minimum_voltage;
    double maximum_voltage;
    double output_wattage;

} InputRange; // in PowerSupply

typedef struct _Info_Threshold
{
    string activation;
    double reading;
    // string dwelltime;

} Info_Threshold; // in struct Thresholds (for Sensor)

typedef struct _Thresholds
{
    Info_Threshold lower_caution;
    Info_Threshold lower_critical;
    Info_Threshold lower_fatal;
    Info_Threshold upper_caution;
    Info_Threshold upper_critical;
    Info_Threshold upper_fatal;

} Thresholds; // in Sensor

typedef struct _Value_About_HA
{
    string peer_primary_address;
    int primary_port;
    string peer_secondary_address;
    int second_port;
    bool origin;
    bool enabled;
} Value_About_HA;


/**
 * @brief Redfish resource of Actions
 * @authors 강
 * @details Actions 클래스가 해당하는 건 /Actions가 아니라 뒤에 더 붙는 /Actions/~~~~.#### 에 해당함
 * 앞에 Actions는 그냥 그걸 담고있는 리소스 안에 컬렉션으로 만들어놓을거임
 */

class Actions
{
    public:
    string id;
    string target; //uri
    string act_type; // ---ActionInfo
    string action_name; // action_by.action_what
    string action_by; 
    string action_what; // action_by.action_what 의 형태
    vector<string> action_info;

    // for parsing
    string uri;
    vector<string> uri_tokens;

    Actions(const string _target)
    {
        // cout << "odata : " << _odata_id << endl;
        this->uri = _target;
        this->uri_tokens = string_split(this->uri, '/');
        string last = uri_tokens[uri_tokens.size()-1];
        this->action_name = last;
        vector<string> v = string_split(last, '.');
        this->action_by = v.at(0);
        this->action_what = v.at(1);
        this->target = _target;

        // g_record[_odata_id] = this;

    }
    Actions(const string _target, const string _act_type) : Actions(_target)
    {
        this->act_type = _act_type;

        if(act_type == "Reset")
        {
            this->action_info.push_back("On");
            this->action_info.push_back("ForceOff");
            this->action_info.push_back("ForceRestart");
            this->action_info.push_back("ForceOn");
            this->action_info.push_back("GracefulShutdown");
            this->action_info.push_back("GracefulRestart");
            this->action_info.push_back("Nmi");
            this->action_info.push_back("PushPowerButton");
        }

    };
    ~Actions()
    {
        // g_record.erase(this->odata.id);

    };

    json::value get_json(void);
    bool load_json(void);
};

// class Actions : public Resource
// {
//     public:
//     string id;
//     string target; //uri
//     string act_type; // ---ActionInfo
//     string action_name; // action_by.action_what
//     string action_by; 
//     string action_what; // action_by.action_what 의 형태
//     vector<string> action_info;

//     // for parsing
//     string uri;
//     vector<string> uri_tokens;

//     Actions(const string _odata_id) : Resource(ACTIONS_TYPE, _odata_id, ODATA_ACTIONS_TYPE)
//     {
//         // cout << "odata : " << _odata_id << endl;
//         this->uri = _odata_id;
//         this->uri_tokens = string_split(this->uri, '/');
//         string last = uri_tokens[uri_tokens.size()-1];
//         this->action_name = last;
//         vector<string> v = string_split(last, '.');
//         this->action_by = v.at(0);
//         this->action_what = v.at(1);
//         this->target = _odata_id;

        

//         g_record[_odata_id] = this;

//     }
//     Actions(const string _odata_id, const string _act_type) : Actions(_odata_id)
//     {
//         this->act_type = _act_type;

//         if(act_type == "Reset")
//         {
//             this->action_info.push_back("On");
//             this->action_info.push_back("ForceOff");
//             this->action_info.push_back("ForceRestart");
//             this->action_info.push_back("ForceOn");
//             this->action_info.push_back("GracefulShutdown");
//             this->action_info.push_back("GracefulRestart");
//             this->action_info.push_back("Nmi");
//             this->action_info.push_back("PushPowerButton");
//         }

//     };
//     ~Actions()
//     {
//         g_record.erase(this->odata.id);

//     };

//     json::value get_json(void);
//     bool load_json(void);

// };


/**
 * @brief Resource of redfish schema
 */
class Resource
{
public:
    OData odata;
    string name;
    uint8_t type;

    // Class constructor, destructor oveloading
    Resource(const uint8_t _type, const string _odata_id)
    {
        this->name = "";
        this->type = _type;
        this->odata.id = _odata_id;
        this->odata.type = "";
    };
    Resource(const uint8_t _type, const string _odata_id, const string _odata_type) : Resource(_type, _odata_id)
    {
        this->odata.type = _odata_type;
    };
    ~Resource(){};

    json::value get_json(void);
    json::value get_odata_id_json(void);
    bool save_json(void);
    bool load_json(void);
};

/**
 * @brief Resource map of resource
 */
extern unordered_map<string, Resource *> g_record;
extern map<string, string> module_id_table;
// extern unordered_map<string, unordered_map<string, Task *> > task_map;


/**
 * @brief Root of resource
 *        /redfish/v1/Systems
 *        /redfish/v1/Chassis
 *        /redfish/v1/Managers
 *        /redfish/v1/AccountService/Accounts
 *        /redfish/v1/SessionService/Sessions
 */
class Collection : public Resource
{
public:
    vector<Resource *> members;

    // Class constructor, destructor oveloading
    Collection(const string _odata_id, const string _odata_type) : Resource(COLLECTION_TYPE, _odata_id, _odata_type)
    {
        // switch(_odata_type)
        // {
        //     case ODATA_PROCESSOR_COLLECTION_TYPE:
        //         this->name = "Processor Collection";
        //         break;
        //     case ODATA_SIMPLE_STORAGE_COLLECTION_TYPE:
        //         this->name = "Simple Storage Collection";
        //         break;
        //     case ODATA_ETHERNET_INTERFACE_COLLECTION_TYPE:
        //         this->name = "Ethernet Network Interface Collection";
        //         break;
        //     case ODATA_LOG_SERVICE_COLLECTION_TYPE:
        //         this->name = "Log Service Collection";
        //         break;
        //     case ODATA_LOG_ENTRY_COLLECTION_TYPE:
        //         this->name = "Log Entry Collection";
        //         break;
        //     case ODATA_DESTINATION_COLLECTION_TYPE:
        //         this->name = "Event Subscriptions Collection";
        //         break;
        // }
        g_record[_odata_id] = this;
    };
    ~Collection()
    {
        g_record.erase(this->odata.id);
    };

    void add_member(Resource *);
    json::value get_json(void);
    bool load_json(void);
};

/**
 * @brief List of resource
 *        /redfish/v1/Chassis/#/Thermal/Temperatures
 *        /redfish/v1/Chassis/#/Thermal/Fans
 */
class List : public Resource
{
public:
    vector<Resource *> members;
    uint8_t member_type;

    // Class constructor, destructor oveloading
    List(const string _odata_id, const uint8_t _member_type) : Resource(LIST_TYPE, _odata_id)
    {
        this->member_type = _member_type;
        g_record[_odata_id] = this;
    };

    ~List()
    {
        g_record.erase(this->odata.id);
    };

    void add_member(Resource *);
    json::value get_json(void);
    bool load_json(void);
};

/**
 * @brief Redfish resource of account
 */
class Role : public Resource
{
public:
    string id;
    bool is_predefined;
    vector<string> assigned_privileges;

    Role(const string _odata_id) : Resource(ROLE_TYPE, _odata_id, ODATA_ROLE_TYPE)
    {
        this->id = "";
        this->is_predefined = false;
        assigned_privileges.push_back("test");

        ((Collection *)g_record[ODATA_ROLE_ID])->add_member(this);
        g_record[_odata_id] = this;
    };
    ~Role()
    {
        g_record.erase(this->odata.id);
    };

    json::value get_json(void);
    bool load_json(void);
};

/**
 * @brief Redfish resource of account
 */
class Account : public Resource
{
public:
    string id;
    bool enabled;
    string password;
    string user_name;
    bool locked;
    Role *role;

    Account(const string _odata_id, const string _role_id) : Resource(ACCOUNT_TYPE, _odata_id, ODATA_ACCOUNT_TYPE)
    {
        string odata_id;
        odata_id = ODATA_ROLE_ID;
        odata_id = odata_id + '/' + _role_id;

        this->id = "";
        this->enabled = false;
        this->password = "";
        this->user_name = "";
        if (record_is_exist(odata_id))
            this->role = (Role *)g_record[odata_id];
        else
            this->role = nullptr;
        this->locked = false;

        ((Collection *)g_record[ODATA_ACCOUNT_ID])->add_member(this);
        g_record[_odata_id] = this;
    };
    ~Account()
    {
        g_record.erase(this->odata.id);
    };

    json::value get_json(void);
    bool load_json(void);
};

/**
 * @brief Redfish resource of account service
 **/
class AccountService : public Resource
{
public:
    string id;
    Status status;
    bool service_enabled;
    unsigned int auth_failure_logging_threshold;
    unsigned int min_password_length;
    unsigned int account_lockout_threshold;
    unsigned int account_lockout_duration;
    unsigned int account_lockout_counter_reset_after;
    unsigned int account_lockout_counter_reset_enabled;
    Collection *account_collection;
    Collection *role_collection;

    // Class constructor, destructor oveloading
    AccountService() : Resource(ACCOUNT_SERVICE_TYPE, ODATA_ACCOUNT_SERVICE_ID, ODATA_ACCOUNT_SERVICE_TYPE)
    {
        // AccountService configuration
        this->name = "Account Service";
        this->id = "AccountService";
        this->status.state = STATUS_STATE_DISABLED;
        this->status.health = STATUS_HEALTH_OK;
        this->service_enabled = true;
        this->auth_failure_logging_threshold = 0;
        this->min_password_length = 6;
        this->account_lockout_threshold = 0;
        this->account_lockout_duration = 0;
        this->account_lockout_counter_reset_after = 0;
        this->account_lockout_counter_reset_enabled = 0;

        // Role collection configure
        this->role_collection = new Collection(ODATA_ROLE_ID, ODATA_ROLE_COLLECTION_TYPE);
        this->role_collection->name = "Roles Collection";
        // Account collection configure
        this->account_collection = new Collection(ODATA_ACCOUNT_ID, ODATA_ACCOUNT_COLLECTION_TYPE);
        this->account_collection->name = "Accounts Collection";

        // Administrator role configuration
        Role *_administrator = new Role(this->role_collection->odata.id + "/Administrator");
        _administrator->id = "Administrator";
        _administrator->name = "User Role";
        _administrator->is_predefined = true;
        _administrator->assigned_privileges.push_back("Login");
        _administrator->assigned_privileges.push_back("ConfigureManager");
        _administrator->assigned_privileges.push_back("ConfigureUsers");
        _administrator->assigned_privileges.push_back("ConfigureSelf");
        _administrator->assigned_privileges.push_back("ConfigureComponents");

        // Operator role configuration
        Role *_operator = new Role(this->role_collection->odata.id + "/Operator");
        _operator->id = "Operator";
        _operator->name = "User Role";
        _operator->is_predefined = true;
        _operator->assigned_privileges.push_back("Login");
        _operator->assigned_privileges.push_back("ConfigureSelf");
        _operator->assigned_privileges.push_back("ConfigureComponents");

        // ReadOnly role configuration
        Role *_read_only = new Role(this->role_collection->odata.id + "/ReadOnly");
        _read_only->id = "ReadOnly";
        _read_only->name = "User Role";
        _read_only->is_predefined = true;
        _read_only->assigned_privileges.push_back("Login");
        _read_only->assigned_privileges.push_back("ConfigureSelf");

        // Root account configure
        Account *_root = new Account(this->account_collection->odata.id + "/root", "Administrator");
        _root->id = "root";
        _root->name = "User Account";
        _root->user_name = "root";
        _root->password = "ketilinux";
        _root->enabled = true;
        _root->locked = false;

        g_record[ODATA_ACCOUNT_SERVICE_ID] = this;
    };
    // AccountService(const string _odata_id) : Resource(ACCOUNT_SERVICE_TYPE, _odata_id, ODATA_ACCOUNT_SERVICE_TYPE)
    // {
    //     this->account_collection = new Collection(_odata_id + "/Accounts", ODATA_ACCOUNT_COLLECTION_TYPE);
    //     this->account_collection->name = "Remote Accounts Collection";

    //     this->role_collection = new Collection(_odata_id + "/Roles", ODATA_ROLE_COLLECTION_TYPE);
    //     this->role_collection->name = "Roles Collection";
    //     // 매니저에 들어가는 리모트어카운트서비스 하는중

    // };
    ~AccountService()
    {
        g_record.erase(this->odata.id);
    };

    json::value get_json(void);
    bool load_json(void);
};



/**
 * @brief Redfish resource of Log Service
 * @authors 강
 */

class LogEntry : public Resource
{
    public:
    string id;
    string entry_type;
    string severity;
    string created;
    unsigned int sensor_number;
    string message;
    string message_id;
    vector<string> message_args;

    LogEntry(const string _odata_id) : Resource(LOG_ENTRY_TYPE, _odata_id, ODATA_LOG_ENRTY_TYPE)
    {
        this->id = "";
        this->entry_type = "Event";
        this->severity = "OK";
        this->created = currentDateTime();
        this->sensor_number = 1;
        this->message = "Temperature threshold exceeded";
        this->message_id = "Contoso.1.0.TempAssert";
        this->message_args.push_back("42");

        g_record[_odata_id] = this;

    }
    LogEntry(const string _odata_id, const string _entry_id) : LogEntry(_odata_id)
    {
        this->id = _entry_id;
    };
    ~LogEntry()
    {
        g_record.erase(this->odata.id);
    };

    json::value get_json(void);
    bool load_json(void);

};

class LogService : public Resource
{
    public:
    string id;
    string description;
    string datetime;
    string datetime_local_offset;
    unsigned int max_number_of_records;
    string log_entry_type;
    string overwrite_policy;
    bool service_enabled;
    Status status;

    Collection *entry;
    // Collection *actions;
    vector<Actions *> actions;

    LogService(const string _odata_id) : Resource(LOG_SERVICE_TYPE, _odata_id, ODATA_LOG_SERVICE_TYPE)
    {
        this->id = "";
        this->description = "This Log contains entries related to the operation of the host Computer System";
        this->max_number_of_records = 1000;
        this->overwrite_policy = "WrapsWhenFull";
        this->datetime = currentDateTime();
        this->datetime_local_offset = "+06:00";
        this->status.state = STATUS_STATE_ENABLED;
        this->status.health = STATUS_HEALTH_OK;
        this->log_entry_type = "Event";
        this->service_enabled = true;

        this->entry = new Collection(_odata_id + "/Entries", ODATA_LOG_ENTRY_COLLECTION_TYPE);
        this->entry->name = "Log Entry Collection";


        Actions *act = new Actions(_odata_id + "/Actions/LogService.ClearLog");
        actions.push_back(act);
        // this->actions = new Collection(_odata_id + "/Actions", ODATA_ACTIONS_COLLECTION_TYPE);
        // this->actions->name = "LogService Actions Collection";
        // Actions *act = new Actions(_odata_id + "/Actions/LogService.ClearLog");
        // this->actions->add_member(act);

        g_record[_odata_id] = this;

    }
    LogService(const string _odata_id, const string _log_service_id) : LogService(_odata_id)
    {
        this->id = _log_service_id;
    };
    ~LogService()
    {
        g_record.erase(this->odata.id);
    };

    json::value get_json(void);
    bool load_json(void);
    

};

/**
 * @brief Redfish resource of Event Service
 * @authors 강
 */

class EventDestination : public Resource
{
    public:
    string id;
    string destination;
    string subscription_type;
    string delivery_retry_policy;
    vector<string> event_types;
    string protocol;
    string context;

    Status status;

    EventDestination(const string _odata_id) : Resource(EVENT_DESTINATION_TYPE, _odata_id, ODATA_EVENT_DESTINATION_TYPE)
    {
        this->id = "";
        this->destination = "http://www.dnsname.com/Destination1";
        this->subscription_type = "RedfishEvent";
        this->delivery_retry_policy = "TerminateAfterRetries";
        this->context = "WebUser3";
        this->protocol = "Redfish";

        this->event_types.push_back("Alert");

        this->status.state = STATUS_STATE_ENABLED;
        this->status.health = STATUS_HEALTH_OK;
        
        ((Collection *)g_record[ODATA_EVENT_DESTINATION_ID])->add_member(this);
        g_record[_odata_id] = this;

    }
    EventDestination(const string _odata_id, const string _dest_id) : EventDestination(_odata_id)
    {
        this->id = _dest_id;

    };
    ~EventDestination()
    {
        g_record.erase(this->odata.id);
    };

    json::value get_json(void);
    bool load_json(void);

};

class EventService : public Resource
{
    public:
    string id;
    bool service_enabled;
    int delivery_retry_attempts;
    unsigned int delivery_retry_interval_seconds;
    vector<string> event_types_for_subscription;
    string serversent_event_uri;

    SSE_filter sse;
    Status status;
    Collection *subscriptions;

    EventService() : Resource(EVENT_SERVICE_TYPE, ODATA_EVENT_SERVICE_ID, ODATA_EVENT_SERVICE_TYPE)
    {
        this->id = "";
        this->service_enabled = true;
        this->delivery_retry_attempts = 3;
        this->delivery_retry_interval_seconds = 60;

        this->event_types_for_subscription.push_back("StatusChange");
        this->event_types_for_subscription.push_back("ResourceUpdated");
        this->event_types_for_subscription.push_back("ResourceAdded");
        this->event_types_for_subscription.push_back("ResourceRemoved");
        this->event_types_for_subscription.push_back("Alert");

        this->serversent_event_uri = "/redfish/v1/EventService/SSE";

        this->sse.event_type = true;
        this->sse.metric_report_definition = false;
        this->sse.registry_prefix = true;
        this->sse.resource_type = true;
        this->sse.event_format_type = false;
        this->sse.message_id = true;
        this->sse.origin_resource = true;
        this->sse.subordinate_resources = true;

        this->status.state = STATUS_STATE_ENABLED;
        this->status.health = STATUS_HEALTH_OK;

        this->subscriptions = new Collection(ODATA_EVENT_DESTINATION_ID, ODATA_EVENT_DESTINATION_COLLECTION_TYPE);
        this->subscriptions->name = "Subscription Collection";

        EventDestination *_test = new EventDestination(this->subscriptions->odata.id + "/1", "Subscriber 1");

        g_record[ODATA_EVENT_SERVICE_ID] = this;

    };
    ~EventService()
    {
        g_record.erase(this->odata.id);
    };

    json::value get_json(void);
    bool load_json(void);

};

/**
 * @brief Redfish resource of Update Service
 * @authors 강
 */

class SoftwareInventory : public Resource
{
    public:
    string id;
    bool updatable;
    string manufacturer;
    string release_date;
    string version;
    string software_id;
    string lowest_supported_version;
    vector<string> uefi_device_paths;

    Status status;

    SoftwareInventory(const string _odata_id) : Resource(SOFTWARE_INVENTORY_TYPE, _odata_id, ODATA_SOFTWARE_INVENTORY_TYPE)
    {
        this->id = "";
        this->updatable = true;
        this->manufacturer = "";
        this->release_date = "";
        this->version = "";
        this->software_id = "";
        this->lowest_supported_version = "";
        this->uefi_device_paths.push_back("BMC(0x1, 0x0ABCDEF)");

        this->status.state = STATUS_STATE_ENABLED;
        this->status.health = STATUS_HEALTH_OK;

        g_record[_odata_id] = this;

    }
    SoftwareInventory(const string _odata_id, const string _soft_id) : SoftwareInventory(_odata_id)
    {
        this->id = _soft_id;
    };
    ~SoftwareInventory()
    {
        g_record.erase(this->odata.id);
    };

    json::value get_json(void);
    bool load_json(void);

};

class UpdateService : public Resource
{
    public:
    string id;
    bool service_enabled;
    string http_push_uri;

    Status status;
    Collection *firmware_inventory;
    Collection *software_inventory;
    Collection *actions;

    UpdateService(const string _odata_id) : Resource(UPDATE_SERVICE_TYPE, _odata_id, ODATA_UPDATE_SERVICE_TYPE)
    {
        this->id = "";
        this->service_enabled = true;
        this->http_push_uri = "/FWUpdate";

        this->status.state = STATUS_STATE_ENABLED;
        this->status.health = STATUS_HEALTH_OK;

        this->firmware_inventory = new Collection(_odata_id + "/FirmwareInventory", ODATA_SOFTWARE_INVENTORY_COLLECTION_TYPE);
        this->firmware_inventory->name = "Firmware Inventory Collection";
        this->software_inventory = new Collection(_odata_id + "/SoftwareInventory", ODATA_SOFTWARE_INVENTORY_COLLECTION_TYPE);
        this->software_inventory->name = "Software Inventory Collection";

        this->actions = new Collection(_odata_id + "/Actions", ODATA_ACTIONS_COLLECTION_TYPE);
        this->actions->name = "UpdateService Actions Collection";
        Actions *act = new Actions(_odata_id + "/Actions/UpdateService.SimpleUpdate");

        ((Collection *)(g_record[ODATA_UPDATE_SERVICE_ID]))->add_member(this);

        g_record[_odata_id] = this;
    }
    UpdateService(const string _odata_id, const string _update_id) : UpdateService(_odata_id)
    {
        this->id = _update_id;
    };
    ~UpdateService()
    {
        g_record.erase(this->odata.id);
    };

    json::value get_json(void);
    bool load_json(void);

};

// /**
//  * @brief Redfish resource of Certificate Service
//  * @authors 강
//  */

// class CertificateService : public Resource
// {
//     public:

// };





/**
 * @brief Redfish resource of managers
 * @authors 강
 * @details 
 */

class NetworkProtocol : public Resource
{
    public:
    string id;
    string hostname;
    string description;
    string fqdn;
    bool snmp_enabled;
    bool ipmi_enabled;
    bool ntp_enabled;
    bool kvmip_enabled;
    bool https_enabled;
    bool http_enabled;
    int snmp_port;
    int ipmi_port;
    int ntp_port;
    int kvmip_port;
    int https_port;
    int http_port;
    vector<string> v_netservers;

    Status status;

    NetworkProtocol(const string _odata_id) : Resource(NETWORK_PROTOCOL_TYPE, _odata_id, ODATA_NETWORK_PROTOCOL_TYPE)
    {
        this->id = "";
        this->fqdn = "web483-bmc.dmtf.org";
        this->description = "Manager Network Service";
        this->hostname = "web483-bmc";

        this->status.state = STATUS_STATE_ENABLED;
        this->status.health = STATUS_HEALTH_OK;

        this->http_enabled = true;
        this->http_port = 80;
        this->https_enabled = true;
        this->https_port = 443;

        this->ipmi_enabled = true;
        this->ipmi_port = 623;
        this->snmp_enabled = true;
        this->snmp_port = 161;

        this->kvmip_enabled = true;
        this->kvmip_port = 5288;
        this->ntp_enabled = true;
        this->ntp_port = 77;

        g_record[_odata_id] = this;
    }
    NetworkProtocol(const string _odata_id, const string _network_id) : NetworkProtocol(_odata_id)
    {
        this->id = _network_id;

    };
    ~NetworkProtocol()
    {
        g_record.erase(this->odata.id);
    };

    json::value get_json(void);
    bool load_json(void);
};

class EthernetInterfaces : public Resource
{
public:
    string id;
    string description;
    string link_status;
    string permanent_mac_address;
    string mac_address;
    unsigned int speed_Mbps;
    bool autoneg;
    bool full_duplex;
    unsigned int mtu_size;
    string hostname;
    string fqdn;
    vector<string> name_servers;
    DHCP_v4 dhcp_v4;
    DHCP_v6 dhcp_v6;
    vector<IPv4_Address> v_ipv4;
    vector<IPv6_Address> v_ipv6;
    Vlan vlan;

    Status status;
    // string address;
    // string subnetMask;
    // string gateway;
    // string addressOrigin;
    

    // string addressv6;
    // string prefixLength;
    // string addressOriginv6;
    // string gatewayv6;
    // int vlanid;
    // bool vlan_enable;
    
    
    int index;
    
    // vector<string> v_netservers;

    EthernetInterfaces(const string _odata_id) : Resource(ETHERNET_INTERFACE_TYPE, _odata_id, ODATA_ETHERNET_INTERFACE_TYPE)
    {
        // this->id = "";
        // this->description = "Manager NIC 1";
        // this->status.state = STATUS_STATE_ENABLED;
        // this->status.health = STATUS_HEALTH_OK;
        // this->link_status = "LinkUp";
        // this->permanent_mac_address = "12:44:6A:3B:04:11";
        // this->mac_address = "12:44:6A:3B:04:11";
        // this->speed_Mbps = 1000;
        // this->autoneg = true;
        // this->full_duplex = true;
        // this->mtu_size = 1500;
        // this->hostname = "web483";
        // this->fqdn = "web483.contoso.com";
        // this->name_servers.push_back("names.contoso.com");

        // IPv4_Address add4;
        // add4.address = "192.168.0.10";
        // add4.subnet_mask = "255.255.252.0";
        // add4.address_origin = "DHCP";
        // add4.gateway = "192.168.0.1";
        // this->v_ipv4.push_back(add4);

        // IPv6_Address add6;
        // add6.address = "fe80::1ec1:deff:fe6f:1e24";
        // add6.address_origin = "SLAAC";
        // add6.address_state = "Preferred";
        // add6.prefix_length = 64;
        // this->v_ipv6.push_back(add6);

        // this->dhcp_v4.dhcp_enabled = true;
        // this->dhcp_v4.use_dns_servers = true;
        // this->dhcp_v4.use_ntp_servers = false;
        // this->dhcp_v4.use_gateway = true;
        // this->dhcp_v4.use_static_routes = true;
        // this->dhcp_v4.use_domain_name = true;

        // this->dhcp_v6.operating_mode = "Stateful";
        // this->dhcp_v6.use_dns_servers = true;
        // this->dhcp_v6.use_ntp_servers = false;
        // this->dhcp_v6.use_domain_name = false;
        // this->dhcp_v6.use_rapid_commit = false;

        // this->vlan.vlan_enable = true;
        // this->vlan.vlan_id = 101;


        // 기존
        // this->macaddress = "";
        // this->description = "";
        // this->mtusize = "";
        // this->hostname = "";
        // this->address = "";
        // this->subnetMask = "";
        // this->gateway = "";
        // this->addressOrigin = "";
        // this->linkstatus = "";

        // this->addressv6 = "";
        // this->prefixLength = "";
        // this->addressOriginv6 = "";
        // this->gatewayv6 = "";
        

        g_record[_odata_id] = this;
    }
    EthernetInterfaces(const string _odata_id, const string _ether_id) : EthernetInterfaces(_odata_id)
    {
        this->id = _ether_id;
    };
    ~EthernetInterfaces()
    {
        g_record.erase(this->odata.id);
    };

    json::value get_json(void);
    bool load_json(void);


};

class Manager : public Resource
{
public:
    string id;
    string manager_type;
    string description;
    string uuid;
    string model;
    string firmware_version;
    string datetime;
    string datetime_offset;
    string power_state;

    // Actions actions;
    NetworkProtocol *network;
    
    Status status;
    Collection *ethernet;
    Collection *log_service;
    // Collection *actions;
    vector<Actions *> actions;
    AccountService *account_service;


    Manager(const string _odata_id) : Resource(MANAGER_TYPE, _odata_id, ODATA_MANAGER_TYPE)
    {
        this->name = "";
        // this->id = _manager_id;
        this->manager_type = "CMM";
        this->description = "CMM Manager";
        this->status.state = STATUS_STATE_ENABLED;
        this->status.health = STATUS_HEALTH_OK;
        this->firmware_version = "Manager Firmware Version";
        this->uuid = "Manager UUID";
        this->model = "Manager Model";
        this->power_state = "On";

        this->network = new NetworkProtocol(this->odata.id + "/NetworkProtocol", "NetworkProtocol");

        string oodata = _odata_id;
        // cout << "oodata : " << oodata << endl;
        oodata = oodata + "/EthernetInterfaces";
        // cout << "oodata2 : " << oodata << endl;
        this->ethernet = new Collection(oodata, ODATA_ETHERNET_INTERFACE_COLLECTION_TYPE);
        // 여기에 이더넷 4개 만드는거 걍 넣어놓기 -> 서비스루트에서 이닛할때로 만듬.

        this->datetime = currentDateTime();
        this->datetime_offset = "+06:00";

        this->log_service = new Collection(_odata_id + "/LogServices", ODATA_LOG_SERVICE_COLLECTION_TYPE);
        this->log_service->name = "Log Service Collection";
        string res_id = _odata_id + "/LogServices";
        res_id = res_id + "/Log1";
        LogService *logservice = new LogService(res_id, "Log Service 1~");
        this->log_service->add_member(logservice);
        res_id = res_id + "/Entries";
        res_id = res_id + "/0";
        LogEntry *log = new LogEntry(res_id, "Log Entry 0~");
        logservice->entry->add_member(log);

        Actions *act = new Actions(_odata_id + "/Actions/Manager.Reset", "Reset");
        actions.push_back(act);

        // this->actions = new Collection(_odata_id + "/Actions", ODATA_ACTIONS_COLLECTION_TYPE);
        // this->actions->name = "Managers Actions Collection";
        // Actions *act = new Actions(_odata_id + "/Actions/Manager.Reset", "Reset");
        // this->actions->add_member(act);



        ((Collection *)g_record[ODATA_MANAGER_ID])->add_member(this);

        g_record[_odata_id] = this;
    }
    Manager(const string _odata_id, const string _manager_id) : Manager(_odata_id)
    {
        this->id = _manager_id;
    };
    ~Manager()
    {
        delete network;
        g_record.erase(this->odata.id);
    };

    json::value get_json(void); // 여기 가서 수정해줘야함
    bool load_json(void);


};


/**
 * @brief Redfish Resource of Task
 * 
 */
class Task : public Resource
{
    public:
    string id;
    string start_time;
    string end_time;
    string task_state; 
    string task_status;
    Payload payload;

    Task(const string _odata_id, const string _task_id) : Resource(TASK_TYPE, _odata_id, ODATA_TASK_TYPE)
    {
        this->name = "Task";
        this->id = _task_id;
        this->start_time = currentDateTime();
        this->end_time = "";
        this->task_state = TASK_STATE_NEW;
        this->task_status = TASK_STATUS_OK;
        // starttime, state, status는 임의로 넣어놓음

        ((Collection *)g_record[ODATA_TASK_ID])->add_member(this);
        g_record[_odata_id] = this;

    };
    ~Task()
    {
        g_record.erase(this->odata.id);

    };

    json::value get_json(void);
    bool load_json(void);
    void set_payload(http_headers _header, string _operation, json::value _json, string _target_uri);
    // pplx가 필요한구조인가..?

};

/**
 * @brief Redfish Resource of Task Service
 * @author 강
 * 
 */
class TaskService : public Resource
{
public:
    string id;
    string datetime;
    bool service_enabled;
    Status status;
    Collection *task_collection;
    // string overwrite_policy; // enum , 완료된 task overwrite
    // bool event_on_change; // task state바뀌었을 때 이벤트 전송여부

    TaskService() : Resource(TASK_SERVICE_TYPE, ODATA_TASK_SERVICE_ID, ODATA_TASK_SERVICE_TYPE)
    {
        this->name = "Task Service";
        this->id = "TaskService";
        this->status.state = STATUS_STATE_ENABLED;
        this->status.health = STATUS_HEALTH_OK;
        this->service_enabled = true;
        this->datetime = currentDateTime();

        this->task_collection = new Collection(ODATA_TASK_ID, ODATA_TASK_TYPE);
        task_collection->name = "Task Collection";

        Task *_test = new Task(this->task_collection->odata.id + "/test", "Test Task");

        g_record[ODATA_TASK_SERVICE_ID] = this;
    };
    ~TaskService()
    {
        g_record.erase(this->odata.id);
    };

    json::value get_json(void);
    bool load_json(void);

};




/**
 * @brief Redfish resource of session service
 */
class SessionService : public Resource
{
public:
    string id;
    Status status;
    bool service_enabled;
    unsigned int session_timeout;
    Collection *session_collection;

    // Class constructor, destructor oveloading
    SessionService() : Resource(SESSION_SERVICE_TYPE, ODATA_SESSION_SERVICE_ID, ODATA_SESSION_SERVICE_TYPE)
    {
        // AccountService configuration
        this->name = "Session Service";
        this->id = "SessionService";
        this->status.state = STATUS_STATE_ENABLED;
        this->status.health = STATUS_HEALTH_OK;
        this->service_enabled = true;
        this->session_timeout = 86400; // 30sec to 86400sec

        // AccountCollection configuration
        this->session_collection = new Collection(ODATA_SESSION_ID, ODATA_SESSION_TYPE);
        session_collection->name = "Session Collection";

        g_record[ODATA_SESSION_SERVICE_ID] = this;
    };
    ~SessionService()
    {
        g_record.erase(this->odata.id);
    };

    json::value get_json(void);
    bool load_json(void);
};

/**
 * @brief Redfish resource of session
 */
class Session : public Resource
{
public:
    string id;
    Account *account;
    unsigned int _remain_expires_time;
    // @@@@@@@@ authors 강
    pplx::cancellation_token_source cts;
    pplx::cancellation_token c_token = cts.get_token();

    // Class constructor, destructor oveloading
    Session(const string _odata_id, const string _session_id, Account *_account) : Resource(SESSION_TYPE, _odata_id, ODATA_SESSION_TYPE)
    {
        this->name = "User Session";
        this->id = _session_id;
        this->account = _account;
        this->_remain_expires_time = ((SessionService *)g_record[ODATA_SESSION_SERVICE_ID])->session_timeout;

        ((Collection *)g_record[ODATA_SESSION_ID])->add_member(this);
        g_record[_odata_id] = this;
    };
    ~Session()
    {
        g_record.erase(this->odata.id);
    };
    json::value get_json(void);
    bool load_json(void);
    pplx::task<void> start(void);

private:
};

/**
 * @brief Redfish resource of temperature
 */
class Temperature : public Resource
{
public:
    string member_id;
    Status status;
    int sensor_num;
    double reading_celsius;
    double upper_threshold_non_critical;
    double upper_threshold_critical;
    double upper_threshold_fatal;
    double lower_threshold_non_critical;
    double lower_threshold_critical;
    double lower_threshold_fatal;
    double min_reading_range_temp;
    double max_reading_range_temp;
    string physical_context;

    // Class constructor, destructor oveloading
    Temperature(const string _odata_id) : Resource(TEMPERATURE_TYPE, _odata_id, ODATA_THERMAL_TYPE)
    {
        this->member_id = "";
        this->status.state = STATUS_STATE_ENABLED;
        this->status.health = STATUS_HEALTH_OK;

        this->sensor_num = 0;
        this->reading_celsius = 0;
        this->upper_threshold_non_critical = 0;
        this->upper_threshold_critical = 0;
        this->upper_threshold_fatal = 0;
        this->lower_threshold_non_critical = 0;
        this->lower_threshold_critical = 0;
        this->lower_threshold_fatal = 0;
        this->min_reading_range_temp = 0;
        this->max_reading_range_temp = 0;
        this->physical_context = "CPU";
        this->thread = false;

        g_record[_odata_id] = this;
    }
    Temperature(const string _odata_id, const string _member_id) : Temperature(_odata_id)
    {
        this->member_id = _member_id;
    };
    ~Temperature()
    {
        this->thread = false;
        g_record.erase(this->odata.id);
    }

    json::value get_json(void);
    bool load_json(void);
    pplx::task<void> read(uint8_t _sensor_index, uint8_t _sensor_context);

private:
    bool thread;
};

/**
 * @brief Redfish resource of temperature
 */

class Sensor : public Resource
{
    public:
    string id;
    string reading_type; // 센서타입
    string reading_time;

    double reading;
    string reading_units; // 측정단위
    double reading_range_max;
    double reading_range_min;
    double accuracy; // 오차율(%)
    double precision; // reading값의 유효숫자인듯
    string physical_context;
    string sensing_interval; // 센서값의 시간간격이라는데 왜 스트링?

    Thresholds thresh;
    Status status;

    Sensor(const string _odata_id) : Resource(SENSOR_TYPE, _odata_id, ODATA_SENSOR_TYPE)
    {
        this->id = "";
        this->reading_type = "Such as Temperature";
        this->reading_time = "Reading Time";
        this->reading = 30.6;

        this->reading_units = "C";
        this->reading_range_min = 0;
        this->reading_range_max = 70;
        this->accuracy = 0.25;
        this->precision = 1;
        this->sensing_interval = "PT3S ???";
        this->physical_context = "Chassis";
        this->thresh.upper_caution.activation = "Increasing";
        this->thresh.upper_caution.reading = 35;
        this->thresh.upper_critical.activation = "Increasing";
        this->thresh.upper_critical.reading = 40;
        this->thresh.lower_caution.activation = "Increasing";
        this->thresh.lower_caution.reading = 10;

        this->status.state = STATUS_STATE_ENABLED;
        this->status.health = STATUS_HEALTH_OK;

        g_record[_odata_id] = this;
    }
    Sensor(const string _odata_id, const string _sensor_id) : Sensor(_odata_id)
    {
        this->id = _sensor_id;
    };
    ~Sensor()
    {
        g_record.erase(this->odata.id);
    };

    json::value get_json(void);
    bool load_json(void);

};


class Fan : public Resource
{
public:
    string member_id;
    Status status;
    int sensor_num;
    
    int reading;
    string reading_units;
    
    int upper_threshold_non_critical;
    int upper_threshold_critical;
    int upper_threshold_fatal;
    int lower_threshold_non_critical;
    int lower_threshold_critical;
    int lower_threshold_fatal;
    int min_reading_range;
    int max_reading_range;
    string physical_context;

    // Class constructor, destructor oveloading
    Fan(const string _odata_id) : Resource(FAN_TYPE, _odata_id)
    {
        this->member_id = "";
        this->status.state = STATUS_STATE_ENABLED;
        this->status.health = STATUS_HEALTH_OK;

        this->sensor_num = 0;
        this->reading = 0;
        this->reading_units = "RPM";

        this->lower_threshold_fatal = 0;
        this->lower_threshold_critical = 0;
        this->lower_threshold_non_critical = 0;
        this->upper_threshold_fatal = 0;
        this->upper_threshold_critical = 0;
        this->upper_threshold_non_critical = 0;

        this->min_reading_range = 0;
        this->max_reading_range = 0;
        this->physical_context = "CPU";

        g_record[_odata_id] = this;
    }
    Fan(const string _odata_id, const string _member_id) : Fan(_odata_id)
    {
        this->member_id = _member_id;
    };
    ~Fan()
    {
        g_record.erase(this->odata.id);
    }

    json::value get_json(void);
    bool load_json(void);
};

/**
 * @brief Redfish resource of thermal
 * 
 */
class Thermal : public Resource
{
public:
    string id;
    List *temperatures;
    List *fans;

    // Class constructor, destructor oveloading
    Thermal(const string _odata_id) : Resource(THERMAL_TYPE, _odata_id, ODATA_THERMAL_TYPE)
    {
        this->id = "Thermal";

        // Temperatures configuration
        this->temperatures = new List(this->odata.id + "/Temperatures", TEMPERATURE_TYPE);
        this->temperatures->name = "Temperatures";

        // Fans configuration
        this->fans = new List(this->odata.id + "/Fans", FAN_TYPE);
        this->fans->name = "Fans";

        g_record[_odata_id] = this;
    };
    ~Thermal()
    {
        g_record.erase(this->odata.id);
    };

    json::value get_json(void);
    bool load_json(void);
};

/**
 * @brief Power 관련 resource
 * @authors 강
 */

class Voltage : public Resource
{
    public:
    string member_id;
    int sensor_num;
    double reading_volts;
    double upper_threshold_non_critical;
    double upper_threshold_critical;
    double upper_threshold_fatal;
    double lower_threshold_non_critical;
    double lower_threshold_critical;
    double lower_threshold_fatal;
    double min_reading_range;
    double max_reading_range;
    string physical_context;
    
    Status status;

    Voltage(const string _odata_id) : Resource(VOLTAGE_TYPE, _odata_id, ODATA_POWER_TYPE)
    {
        this->member_id = "";
        this->status.state = STATUS_STATE_ENABLED;
        this->status.health = STATUS_HEALTH_OK;

        this->sensor_num = 11;
        this->reading_volts = 12;
        this->upper_threshold_non_critical = 12.5;
        this->upper_threshold_critical = 13;
        this->upper_threshold_fatal = 15;
        this->lower_threshold_non_critical = 11.5;
        this->lower_threshold_critical = 11;
        this->lower_threshold_fatal = 10;
        this->min_reading_range = 0;
        this->max_reading_range = 20;
        this->physical_context = "VoltageRegulator";

        g_record[_odata_id] = this;

    }
    Voltage(const string _odata_id, const string _member_id) : Voltage(_odata_id)
    {
        this->member_id = _member_id;
    };
    ~Voltage()
    {
        g_record.erase(this->odata.id);
    };

    json::value get_json(void);
    bool load_json(void);

};

class PowerSupply : public Resource
{
    public:
    string member_id;
    string power_supply_type;
    string line_input_voltage_type;
    double line_input_voltage;
    double power_capacity_watts;
    double last_power_output_watts;

    string model;
    string manufacturer;
    string firmware_version;
    string serial_number;
    string part_number;
    string spare_part_number;

    vector<InputRange> input_ranges;

    Status status;

    PowerSupply(const string _odata_id) : Resource(POWER_SUPPLY_TYPE, _odata_id, ODATA_POWER_TYPE)
    {
        this->member_id = "";
        this->status.state = STATUS_STATE_ENABLED;
        this->status.health = STATUS_HEALTH_OK;
        
        this->power_supply_type = "AC";
        this->line_input_voltage_type = "ACWideRange";
        this->line_input_voltage = 120;
        this->power_capacity_watts = 800;
        this->last_power_output_watts = 325;

        this->model = "";
        this->manufacturer = "";
        this->firmware_version = "";
        this->serial_number = "";
        this->part_number = "";
        this->spare_part_number = "";

        InputRange input;
        input.input_type = "AC";
        input.minimum_voltage = 100;
        input.maximum_voltage = 120;
        input.output_wattage = 800;
        this->input_ranges.push_back(input);

        input.input_type = "AC";
        input.minimum_voltage = 200;
        input.maximum_voltage = 240;
        input.output_wattage = 1300;
        this->input_ranges.push_back(input);

        g_record[_odata_id] = this;

    }
    PowerSupply(const string _odata_id, const string _member_id) : PowerSupply(_odata_id)
    {
        this->member_id = _member_id;
    };
    ~PowerSupply()
    {
        g_record.erase(this->odata.id);
    };

    json::value get_json(void);
    bool load_json(void);

};

class PowerControl : public Resource
{
    public:
    string member_id;
    double power_consumed_watts;
    double power_requested_watts;
    double power_available_watts;
    double power_capacity_watts;
    double power_allocated_watts;

    PowerLimit power_limit;
    PowerMetrics power_metrics;
    Status status;

    PowerControl(const string _odata_id) : Resource(POWER_CONTROL_TYPE, _odata_id, ODATA_POWER_TYPE)
    {
        this->member_id = "";
        this->status.state = STATUS_STATE_ENABLED;
        this->status.health = STATUS_HEALTH_OK;
        this->power_consumed_watts = 0;
        this->power_requested_watts = 0;
        this->power_available_watts = 0;
        this->power_capacity_watts = 0;
        this->power_allocated_watts = 0;

        this->power_limit.limit_in_watts = 0;
        this->power_limit.limit_exception = "LogEventOnly";
        this->power_limit.correction_in_ms = 0;

        this->power_metrics.interval_in_min = 0;
        this->power_metrics.min_consumed_watts = 0;
        this->power_metrics.max_consumed_watts = 0;
        this->power_metrics.avg_consumed_watts = 0;

        g_record[_odata_id] = this;
    }
    PowerControl(const string _odata_id, const string _member_id) : PowerControl(_odata_id)
    {
        this->member_id = _member_id;
    };
    ~PowerControl()
    {
        g_record.erase(this->odata.id);
    };

    json::value get_json(void);
    bool load_json(void);

};

class Power : public Resource
{
    public:
    string id;
    List *power_control;
    List *voltages;
    List *power_supplies;

    Power(const string _odata_id) : Resource(POWER_TYPE, _odata_id, ODATA_POWER_TYPE)
    {
        this->id = "Power";

        this->power_control = new List(this->odata.id + "/PowerControl", POWER_CONTROL_TYPE);
        this->power_control->name = "PowerControl";

        this->voltages = new List(this->odata.id + "/Voltages", VOLTAGE_TYPE);
        this->voltages->name = "Voltages";

        this->power_supplies = new List(this->odata.id + "/PowerSupplies", POWER_SUPPLY_TYPE);
        this->power_supplies->name = "PowerSupplies";

        g_record[_odata_id] = this;
    };
    ~Power()
    {
        g_record.erase(this->odata.id);
    };

    json::value get_json(void);
    bool load_json(void);

};


/**
 * @brief 시스템 리소스에 필요한 storage, bios, simplestorage, processors..  processorsummary,networkinterface는뺌
 * @authors 강
 * 
 */

class Bios : public Resource
{
    public:
    string id;
    string attribute_registry;
    Attribute attribute;
    //Action 있더라 근데 액션에 바이오스 패스워드 어딨지?

    Bios(const string _odata_id, const string _bios_id) : Resource(BIOS_TYPE, _odata_id, ODATA_BIOS_TYPE)
    {
        this->id = _bios_id;
        this->attribute_registry = "Attribute registry";
        this->attribute.boot_mode = "Uefi";
        this->attribute.embedded_sata = "Raid";
        this->attribute.nic_boot1 = "NetworkBoot";
        this->attribute.nic_boot2 = "Disabled";
        this->attribute.power_profile = "MaxPerf";
        this->attribute.proc_core_disable = 0;
        this->attribute.proc_hyper_threading = "Enabled";
        this->attribute.proc_turbo_mode = "Enabled";
        this->attribute.usb_control = "UsbEnabled";

        g_record[_odata_id] = this;
    };
    ~Bios()
    {
        g_record.erase(this->odata.id);
    };

    json::value get_json(void);
    bool load_json(void);

};

class SimpleStorage : public Resource
{
    public:
    string id;
    string description;
    string uefi_device_path;
    Status status;
    // string file_system; // Device_info 안으로 들어감

    Device_Info info;
    vector<Device_Info> devices;

    SimpleStorage(const string _odata_id) : Resource(SIMPLE_STORAGE_TYPE, _odata_id, ODATA_SIMPLE_STORAGE_TYPE)
    {
        // this->description = "System SATA";
        // this->uefi_device_path = "Acpi(PNP0A03, 0) / Pci(1F|1) / Ata(Primary,Master) / HD(Part3, Sig00110011)";
        // this->status.state = STATUS_STATE_ENABLED;
        // this->status.health = STATUS_HEALTH_OK;
        // // this->file_system = "";

        // this->info.capacity_KBytes = 8000000;
        // this->info.manufacturer = "Contoso";
        // this->info.model = "3000GT8";
        // this->info.name = "SATA Bay 1";
        // this->info.status.state = STATUS_STATE_ENABLED;
        // this->info.status.health = STATUS_HEALTH_OK;

        // this->devices.push_back(info);

        g_record[_odata_id] = this;
    }
    SimpleStorage(const string _odata_id, const string _simple_id) : SimpleStorage(_odata_id)
    {
        this->id = _simple_id;
    };
    ~SimpleStorage()
    {
        g_record.erase(this->odata.id);
    }

    json::value get_json(void);
    bool load_json(void);

};

class StorageControllers : public Resource
{
    public:
    string id;
    string manufacturer;
    string model;
    string serial_number;
    string part_number;
    double speed_gbps;
    string firmware_version;
    Identifier identifier;
    vector<string> support_controller_protocols;
    vector<string> support_device_protocols;
    Status status;

    StorageControllers(const string _odata_id, const string _controller_id) : Resource(STORAGE_CONTROLLER_TYPE, _odata_id, ODATA_STORAGE_CONTROLLER_TYPE)
    {
        this->id = _controller_id;
        this->manufacturer = "storage controller manufacturer";
        this->model = "storage controller model";
        this->serial_number = "storage controller serial";
        this->part_number = "storage controller part";
        this->speed_gbps = 0;
        this->firmware_version = "storage controller firmversion";
        this->identifier.durable_name = "iden durable name";
        this->identifier.durable_name_format = "iden durable format";

        this->support_controller_protocols.push_back("PCIe");
        this->support_device_protocols.push_back("SAS");
        this->support_device_protocols.push_back("SATA");

        this->status.state = STATUS_STATE_ENABLED;
        this->status.health = STATUS_HEALTH_OK;

        g_record[_odata_id] = this;

    };
    ~StorageControllers()
    {
        g_record.erase(this->odata.id);
    }

    json::value get_json(void);
    bool load_json(void);
};

class Storage : public Resource
{
    public:
    string id;
    string description;
    Status status;
    List *controller;

    Storage(const string _odata_id, const string _storage_id) : Resource(STORAGE_TYPE, _odata_id, ODATA_STORAGE_TYPE)
    {
        this->id = _storage_id;
        this->description = "";
        this->status.health = STATUS_HEALTH_OK;
        this->status.state = STATUS_STATE_ENABLED;

        this->controller = new List(this->odata.id + "/StorageControllers", STORAGE_CONTROLLER_TYPE);
        this->controller->name = "StorageControllers";

        g_record[_odata_id] = this;

    };
    ~Storage()
    {
        g_record.erase(this->odata.id);
    };

    json::value get_json(void); // 이거 할때 컨트롤러 리스트기때문에 Thermal-temperature 참고할것
    bool load_json(void);
};

// class NetworkInterfaces : public Resource
// {
//     public:
//     string id;
//     Collection *network_device_functions;
//     Collection *network_ports;
//     Collection *ports;
//     Status status;

//     NetworkInterfaces(const string _odata_id, const string _network_interface_id) : Resource(NETWORK_INTERFACE_TYPE, _odata_id, ODATA_NETWORK_INTERFACE_TYPE)
//     {
//         this->id = _network_interface_id;


//         g_record[_odata_id] = this;

//     };
//     ~NetworkInterfaces()
//     {
//         g_record.erase(this->odata.id);
//     };

//     bool load_json(void);
//     json::value get_json(void);

// };


// class ProcessorSummary : public Resource
// {
//     public:
//     string id;
//     int count;
//     int logical_processor_count;
//     string model;
//     Status status;
//     // Metrics *metric;

//     ProcessorSummary(const string _odata_id, const string _processor_summary_id) : Resource(PROCESSOR_SUMMARY_TYPE, _odata_id, ODATA_PROCESSOR_SUMMARY_TYPE)
//     {
//         this->id = _processor_summary_id;
//         this->name = "Processor Summary";
//         this->count = 0;
//         this->logical_processor_count = 0;
//         this->model = "";
//         this->status.health = STATUS_HEALTH_OK;
//         this->status.state = STATUS_STATE_ENABLED;

//         // this->metric = new Metrics(this->odata.id + "/ProcessorMetrics", "ProcessorMetrics");

//         g_record[_odata_id] = this;
//     };
//     ~ProcessorSummary()
//     {
//         // delete metric;
//         g_record.erase(this->odata.id);
//     };

//     json::value get_json(void); // 여기 가서 수정해줘야함
//     bool load_json(void);
// };

class Processors : public Resource
{
    public:
    string id;
    string socket;
    string processor_type;
    string processor_architecture;
    string instruction_set;
    string manufacturer;
    string model;
    int max_speed_mhz;
    int total_cores;
    int total_threads;
    Status status;
    //string UUID?
    ProcessorId p_id;

    Processors(const string _odata_id) : Resource(PROCESSOR_TYPE, _odata_id, ODATA_PROCESSOR_TYPE)
    {
        // this->id = "";
        // this->socket = "CPU 1";
        // this->processor_type = "CPU";
        // this->processor_architecture = "x86";
        // this->instruction_set = "x86-64";
        // this->manufacturer = "Intel(R) Corporation";
        // this->model = "Multi-Core Intel(R) Xeon(R) processor 7xxx Series";
        // this->max_speed_mhz = 3700;
        // this->total_cores = 8;
        // this->total_threads = 16;
        // this->status.state = STATUS_STATE_ENABLED;
        // this->status.health = STATUS_HEALTH_OK;

        // this->p_id.vendor_id = "GenuineIntel";
        // this->p_id.identification_registers = "0x34AC34DC8901274A";
        // this->p_id.effective_family = "0x42";
        // this->p_id.effective_model = "0x61";
        // this->p_id.step = "0x1";
        // this->p_id.microcode_info = "0x429943";

        g_record[_odata_id] = this;
    }
    Processors(const string _odata_id, const string _processor_id) : Processors(_odata_id)
    {
        this->id = _processor_id;
    };
    ~Processors()
    {
        g_record.erase(this->odata.id);
    };

    json::value get_json(void);
    bool load_json(void);

};

class Memory : public Resource
{
    public:
    string id;
    unsigned int rank_count;
    unsigned int capacity_kib;
    unsigned int data_width_bits;
    unsigned int bus_width_bits;
    string error_correction;
    MemoryLocation m_location;
    string memory_type;
    string memory_device_type;
    string base_module_type;
    vector<string> memory_media;
    vector<unsigned int> max_TDP_milliwatts;
    Status status;

    Memory(const string _odata_id) : Resource(MEMORY_TYPE, _odata_id, ODATA_MEMORY_TYPE)
    {
        // this->id = _memory_id;
        // this->rank_count = 2;
        // this->capacity_kib = 32768;
        // this->data_width_bits = 64;
        // this->bus_width_bits = 72;
        // this->error_correction = "MultiBitECC";

        // this->m_location.socket = 1;
        // this->m_location.memory_controller = 1;
        // this->m_location.channel = 1;
        // this->m_location.slot = 1;

        // this->memory_type = "DRAM";
        // this->memory_device_type = "DDR4";
        // this->base_module_type = "RDIMM";

        // this->memory_media.push_back("DRAM");
        // this->max_TDP_milliwatts.push_back(12000);
        // this->status.state = STATUS_STATE_ENABLED;
        // this->status.health = STATUS_HEALTH_OK;

        g_record[_odata_id] = this;
    }
    Memory(const string _odata_id, const string _memory_id) : Memory(_odata_id)
    {
        this->id = _memory_id;
    };
    ~Memory()
    {
        g_record.erase(this->odata.id);
    };

    json::value get_json(void);
    bool load_json(void);

};

/**
 * @brief Redfish resource of Systems
 * 
 */

class Systems : public Resource
{
    public :
    string id;
    // string sku;
    string system_type;
    string manufacturer;
    string model;
    string serial_number;
    string part_number;
    string description;
    string hostname;
    vector<string> hosting_roles;
    // string submodel;
    string asset_tag;
    string power_state;
    uint8_t indicator_led;
    string bios_version;

    Status status;
    // Location location;
    // Thermal *thermal;
    // Resource *power;
    string uuid;
    Boot boot;
    // Actions actions;
    // Ipmifru *fru_this;

    // ProcessorSummary *ps; // 구조체로 바꿔야할듯 현재리소슨데
    Collection *network; // resource NetworkInterfaces // 일단 없음
    Collection *storage; // resource Storages
    Collection *processor; // resource Processors
    Bios *bios; // resource Bios
    Collection *memory; // resource Memory
    // MemorySummary ms;
    Collection *ethernet; // resource EthernetInterfaces
    Collection *log_service; // resource LogService
    // Collection *actions;
    vector<Actions *> actions;
    Collection *simple_storage;

    Systems(const string _odata_id) : Resource(SYSTEM_TYPE, _odata_id, ODATA_SYSTEM_TYPE)
    {
        // this->id = _systems_id;
        // this->sku = "";
        this->system_type = "null";
        this->asset_tag = "null";
        this->manufacturer = "null";
        this->model = "null";
        this->serial_number = "null";
        this->part_number = "null";
        this->description = "null";
        this->uuid = "null";
        this->hostname = "null";       
        this->hosting_roles.push_back("null");
        // this->indicator_led = LED_OFF;
        this->power_state = "null";
        this->bios_version = "null";
        
        this->status.state = "null";
        this->status.health = "null";
        
        
        
        // this->actions
        this->boot.boot_source_override_enabled = "null";
        this->boot.boot_source_override_target = "null";
        this->boot.boot_source_override_mode = "null";
        this->boot.uefi_target_boot_source_override = "null";

        // this->bios = 0;
        // null로 넣는법





        // this->id = _systems_id;
        // // this->sku = "";
        // this->system_type = "Physical";
        // this->asset_tag = "Asset tag";
        // this->manufacturer = "Manufacturer";
        // this->model = "Model";
        // this->serial_number = "Serial Number";
        // this->part_number = "Part Number";
        // this->description = "Chassis Manager Module System";
        // this->uuid = "This is UUID";
        // this->hostname = "Keti";       
        // this->hosting_roles.push_back("ApplicationServer");
        // this->indicator_led = LED_OFF;
        // this->power_state = "On";
        // this->bios_version = "Bios Version";
        
        // this->status.state = STATUS_STATE_ENABLED;
        // this->status.health = STATUS_HEALTH_OK;
        
        
        // // this->actions
        // this->boot.boot_source_override_enabled = "Disabled";
        // this->boot.boot_source_override_target = "None";
        // this->boot.boot_source_override_mode = "UEFI";
        // this->boot.uefi_target_boot_source_override = "";

        // this->ms.total_system_memory_GiB = 96;
        // this->ms.total_

        // this->ps = new ProcessorSummary(_odata_id + "/ProcessorSummary", "ProcessorSummary");
        this->processor = new Collection(_odata_id + "/Processors", ODATA_PROCESSOR_COLLECTION_TYPE);
        this->processor->name = "Processor Collection";

        this->storage = new Collection(_odata_id + "/Storage", ODATA_STORAGE_COLLECTION_TYPE);
        this->storage->name = "Storage Collection";

        this->bios = new Bios(_odata_id + "/Bios", "BIOS");
        this->bios->name = "BIOS Configuration Current Settings";

        this->memory = new Collection(_odata_id + "/Memory", ODATA_MEMORY_COLLECTION_TYPE);
        this->memory->name = "Memory Collection";

        this->ethernet = new Collection(_odata_id + "/EthernetInterfaces", ODATA_ETHERNET_INTERFACE_COLLECTION_TYPE);
        this->ethernet->name = "Ethernet Interface Collection";

        this->log_service = new Collection(_odata_id + "/LogServices", ODATA_LOG_SERVICE_COLLECTION_TYPE);
        this->log_service->name = "Log Service Collection";


        Actions *act = new Actions(_odata_id + "/Actions/ComputerSystem.Reset", "Reset");
        actions.push_back(act);
        // this->actions = new Collection(_odata_id + "/Actions", ODATA_ACTIONS_COLLECTION_TYPE);
        // this->actions->name = "Systems Actions Collection";
        // Actions *act = new Actions(_odata_id + "/Actions/ComputerSystem.Reset", "Reset");
        // this->actions->add_member(act);

        this->simple_storage = new Collection(_odata_id + "/SimpleStorage", ODATA_SIMPLE_STORAGE_COLLECTION_TYPE);
        this->simple_storage->name = "Simple Storage Collection";
        SimpleStorage *sim = new SimpleStorage(this->simple_storage->odata.id + "/0", "0~");
        this->simple_storage->add_member(sim);

        ((Collection *)g_record[ODATA_SYSTEM_ID])->add_member(this);

        // cout << "!@#$ sys : " << _odata_id << endl;
        g_record[_odata_id] = this;
    }
    Systems(const string _odata_id, const string _systems_id) : Systems(_odata_id)
    {
        this->id = _systems_id;
    };
    ~Systems()
    {
        g_record.erase(this->odata.id);
    };

    bool load_json(void);
    json::value get_json(void);

};

/**
 * @brief Redfish resource of chassis
 */
class Chassis : public Resource
{
public:
    string id;
    string chassis_type;
    string manufacturer;
    string model;
    // string sku;
    string serial_number;
    string part_number;
    string asset_tag;
    string power_state;
    uint8_t indicator_led;

    // double height_mm;
    // double width_mm;
    // double depth_mm;
    // double weight_kg;
    
    Status status;
    Location location;

    // TODO 리소스 변경 필요
    Thermal *thermal;
    // Resource *power;
    Power *power;
    //sensor 들어가야됨
    Collection *sensors;

    // TODO Contains, ManagedBy 추가 필요
    Chassis(const string _odata_id) : Resource(CHASSIS_TYPE, _odata_id, ODATA_CHASSIS_TYPE)
    {
        this->name = "";
        // this->id = _chassis_id;
        this->chassis_type = "";
        this->manufacturer = "";
        this->model = "";
        // this->sku = "";
        this->serial_number = "";
        this->part_number = "";
        this->asset_tag = "";
        this->power_state = POWER_STATE_ON;
        this->indicator_led = LED_OFF;
        this->status.state = STATUS_STATE_ENABLED;
        this->status.health = STATUS_HEALTH_OK;
        this->location.postal_address.country = "";
        this->location.postal_address.territory = "";
        this->location.postal_address.city = "";
        this->location.postal_address.street = "";
        this->location.postal_address.house_number = "";
        this->location.postal_address.name = "";
        this->location.postal_address.postal_code = "";
        this->location.placement.row = "";
        this->location.placement.rack = "";
        this->location.placement.rack_offset_units = "";
        this->location.placement.rack_offset = 0;

        // this->height_mm = 0;
        // this->width_mm = 0;
        // this->depth_mm = 0;
        // this->weight_kg = 0;

        // Thermal configuration
        this->thermal = new Thermal(this->odata.id + "/Thermal");
        this->thermal->name = "EdgeServer Chassis Thermal";

        ostringstream os;
        os << this->thermal->fans->odata.id << "/" << "0";
        Fan *f = new Fan(os.str(), "0~~");
        this->thermal->fans->add_member(f);

        // Power configuration
        this->power = new Power(this->odata.id + "/Power");
        this->power->name = "EdgeServer Chassis Power";

        ostringstream s;
        s << this->power->power_control->odata.id << "/" << "0";
        // cout << "IN CHASSIS" << endl;
        // cout << "1st// " << s.str() << endl;
        PowerControl *pc = new PowerControl(s.str(), "0~~");
        this->power->power_control->add_member(pc);

        s.str("");
        s << this->power->voltages->odata.id << "/" << "0";
        Voltage *volt = new Voltage(s.str(), "0~~");
        this->power->voltages->add_member(volt);

        s.str("");
        s << this->power->power_supplies->odata.id << "/" << "0";
        PowerSupply *ps = new PowerSupply(s.str(), "0~~");
        this->power->power_supplies->add_member(ps);

        // Sensor configuration
        this->sensors = new Collection(this->odata.id + "/Sensors", ODATA_SENSOR_COLLECTION_TYPE);
        this->sensors->name = "Sensor Collection";
        s.str("");
        s << this->sensors->odata.id << "/" << "Sensor1";
        Sensor *sen = new Sensor(s.str(), "Sensor Number 1~~");
        this->sensors->add_member(sen);



        // cout << "2nd// " << s.str() << endl;
        // cout << "OUT CHASSIS" << endl;


        ((Collection *)g_record[ODATA_CHASSIS_ID])->add_member(this);

        g_record[_odata_id] = this;
    }
    Chassis(const string _odata_id, const string _chassis_id) : Chassis(_odata_id)
    {
        this->id = _chassis_id;
    };
    ~Chassis()
    {
        g_record.erase(this->odata.id);
    };

    json::value get_json(void);
    bool load_json(void);
    pplx::task<void> led_off(uint8_t _led_index);
    pplx::task<void> led_lit(uint8_t _led_index);
    pplx::task<void> led_blinking(uint8_t _led_index);
};

/**
 * @brief Root of redfish
 *        This resource create only once
 *        /redfish/v1
 */
class ServiceRoot : public Resource
{
public:
    string id;
    string redfish_version;
    string uuid;
    Collection *system_collection;
    Collection *chassis_collection;
    Collection *manager_collection;
    AccountService *account_service;
    SessionService *session_service;
    TaskService *task_service;
    // Collection *task_service;
    // Collection *event_service;

    EventService *event_service;
    Collection *update_service;

    // Class constructor, destructor oveloading
    ServiceRoot() : Resource(SERVICE_ROOT_TYPE, ODATA_SERVICE_ROOT_ID, ODATA_SERVICE_ROOT_TYPE)
    {
        string odata_id;

        this->id = "RootService";
        this->name = "Root Service";
        this->redfish_version = "1.0.0";
        this->uuid = "";

        // System configuration
        system_collection = new Collection(ODATA_SYSTEM_ID, ODATA_SYSTEM_COLLECTION_TYPE);
        system_collection->name = "Computer System Collection";



        module_id_table.insert({CMM_ID, CMM_ADDRESS});
        /**
         * @brief System init
         * @authors 강
         * @todo 나중에 첫 init은 systems 안으로 옮기고 함수만들어서 추가/수정..
         */
        odata_id = ODATA_SYSTEM_ID;
        odata_id = odata_id + "/1";
        // cout << "odata : " << odata_id << endl;
        // system
        Systems *system = new Systems(odata_id, "1~");
        system->name = "EdgeServer CMM System";


        //processor
        string res_id = odata_id + "/Processors";
        Processors *pro = new Processors(res_id + "/CPU1", "CPU1");
        system->processor->add_member(pro);


        //storage & storagecontrollers
        res_id = odata_id + "/Storage";
        res_id = res_id + "/1";
        Storage *sto = new Storage(res_id, "1~");
        system->storage->add_member(sto);
        res_id = res_id + "/StorageControllers";
        res_id = res_id + "/0";
        StorageControllers *stocon = new StorageControllers(res_id, "0~");
        sto->controller->add_member(stocon);

        //memory
        res_id = odata_id + "/Memory";
        res_id = res_id + "/Mem1";
        Memory *mem = new Memory(res_id, "DIMM1");
        system->memory->add_member(mem);
        
        // ethernetinterface
        res_id = odata_id + "/EthernetInterfaces";
        res_id = res_id + "/0";
        EthernetInterfaces *ether = new EthernetInterfaces(res_id, "0~");
        system->ethernet->add_member(ether);


        // logservice & logentry
        res_id = odata_id + "/LogServices";
        res_id = res_id + "/Log1";
        LogService *logservice = new LogService(res_id, "Log Service 1~");
        system->log_service->add_member(logservice);
        res_id = res_id + "/Entries";
        res_id = res_id + "/0";
        LogEntry *log = new LogEntry(res_id, "Log Entry 0~");
        logservice->entry->add_member(log);

        
        
        // Chassis configration
        chassis_collection = new Collection(ODATA_CHASSIS_ID, ODATA_CHASSIS_COLLECTION_TYPE);
        chassis_collection->name = "Chassis Collection";
        odata_id = ODATA_CHASSIS_ID;
        odata_id = odata_id + "/1";
        Chassis *chassis = new Chassis(odata_id, "1~");
        chassis->name = "EdgeServer CMM Chassis";
        chassis->chassis_type = "Enclosure";
        chassis->manufacturer = "KETI";
        chassis->indicator_led = LED_OFF;
        chassis->led_off(LED_YELLOW);
        chassis->led_off(LED_RED);
        chassis->led_blinking(LED_GREEN);

        double temp[2] = {0};
        if (get_intake_temperature_config(temp)) {
            log(info) << "Chassis temperature min value = " << temp[0];
            log(info) << "Chassis temperature max value = " << temp[1];
        }
        // 이거 순서 바뀐거같은데 temp[0]이 maxvalue인듯


        for (uint8_t i = 0; i < 4; i++)
        {
            ostringstream s;
            s << chassis->thermal->temperatures->odata.id << "/" << to_string(i);
            Temperature *intake_temperature = new Temperature(s.str(), to_string(i));
            intake_temperature->name = "Chassis Intake Temperature";
            intake_temperature->physical_context = "Intake";
            intake_temperature->min_reading_range_temp = temp[0];
            intake_temperature->max_reading_range_temp = temp[1];
            intake_temperature->upper_threshold_non_critical = round(temp[1] * 0.6);
            intake_temperature->upper_threshold_critical = round(temp[1] * 0.7);
            intake_temperature->upper_threshold_fatal = round(temp[1] * 0.85);
            intake_temperature->read(i, INTAKE_CONTEXT);
            chassis->thermal->temperatures->add_member(intake_temperature);
        }

        ostringstream s;
        s << chassis->thermal->temperatures->odata.id << "/" << to_string(chassis->thermal->temperatures->members.size());
        Temperature *cpu_temperature = new Temperature(s.str(), to_string(chassis->thermal->temperatures->members.size()));
        cpu_temperature->name = "Chassis Manager CPU Temperature";
        cpu_temperature->physical_context = "CPU";
        cpu_temperature->min_reading_range_temp = 0;
        cpu_temperature->max_reading_range_temp = 100;
        cpu_temperature->upper_threshold_non_critical = round(cpu_temperature->max_reading_range_temp * 0.7);
        cpu_temperature->upper_threshold_critical = round(cpu_temperature->max_reading_range_temp * 0.75);
        cpu_temperature->upper_threshold_fatal = round(cpu_temperature->max_reading_range_temp * 0.8);
        cpu_temperature->read(chassis->thermal->temperatures->members.size(), CPU_CONTEXT);
        chassis->thermal->temperatures->add_member(cpu_temperature);


        // Manager configration
        manager_collection = new Collection(ODATA_MANAGER_ID, ODATA_MANAGER_COLLECTION_TYPE);
        manager_collection->name = "Manager Collection";
        
        /**
         * @brief Manager init
         * @authors 강
         */
        odata_id = ODATA_MANAGER_ID;
        odata_id = odata_id + "/1";
        Manager *manager = new Manager(odata_id, "1~");
        manager->name = "EdgeServer CMM Manager";
        manager->manager_type = "Enclosure";
        manager->firmware_version = "v1";
        // cout << "ethernet->odata.id : " << manager->ethernet->odata.id << endl;
        for(uint8_t i =0; i<4; i++)
        {
            ostringstream s;
            s << manager->ethernet->odata.id << "/" << to_string(i);
            EthernetInterfaces *ether = new EthernetInterfaces(s.str(), to_string(i));
            ether->name = "EthernetInterface in Manager";
            manager->ethernet->add_member(ether);

        }

        /**
         * @authors 강
         */
        // TaskService configuration
        task_service = new TaskService();

        // EventService configuration
        event_service = new EventService();

        // UpdateService configuration
        update_service = new Collection(ODATA_UPDATE_SERVICE_ID, ODATA_UPDATE_SERVICE_COLLECTION_TYPE);
        update_service->name = "UpdateService Collection";
        string str_up = ODATA_UPDATE_SERVICE_ID;
        str_up = str_up + "/1";
        UpdateService *up = new UpdateService(str_up, "1~~");
        SoftwareInventory *sf1 = new SoftwareInventory(up->firmware_inventory->odata.id + "/CMM", "CMM Firmware");
        SoftwareInventory *sf2 = new SoftwareInventory(up->software_inventory->odata.id + "/CMM", "CMM Software");
        up->firmware_inventory->add_member(sf1);
        up->software_inventory->add_member(sf2);
        // update_service->add_member(up);

        
        // AccountService configuration
        account_service = new AccountService();

        // SessionService configuration
        session_service = new SessionService();

        g_record[ODATA_SERVICE_ROOT_ID] = this;
    };
    ~ServiceRoot()
    {
        g_record.erase(this->odata.id);
    };

    json::value get_json(void);
    bool load_json(void);
};

bool init_resource(void);
bool is_session_valid(const string _token_id);
void init_record_bmc(void);

#endif
