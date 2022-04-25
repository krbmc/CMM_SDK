// #pragma once
#include "handler.hpp"
#include "resource.hpp"
#include "hwcontrol.hpp"
#include "task.hpp"
#include "chassis_controller.hpp"
#include "logservice.hpp"
#include "ethernetinterface.hpp"
// #include <glib-2.0/glib.h>
// #include <gssdp-1.2/libgssdp/gssdp.h>

unique_ptr<Handler> g_listener, HA_listener;
unordered_map<string, Resource *> g_record;
src::severity_logger<severity_level> g_logger;
ServiceRoot *g_service_root;
Value_About_HA ha_value;
int heart_beat_cycle;
map<string, string> module_id_table;
// unordered_map<string, unordered_map<string, Task *> > task_map;
unordered_map<uint8_t, Task_Manager *> task_map;
string uuid_str;
map<string, int> last_command_index_map;
vector<LCI> last_command_list;

/**
 * @brief 
 * 
 * @param _url 
 * @param _config 
 */
void start_server(utility::string_t &_url, http_listener_config _config)
{
    g_listener = unique_ptr<Handler>(new Handler(_url, _config));
    g_listener->open()
    .wait();
    log(info) << "Chassis Manager server start";
}

/**
 * @brief KETI Redfish main entry point
 * 
 * @param argc Argument vector count
 * @param argv Argument vector array
 * @return int 
 */
int main(int _argc, char *_argv[])
{
    // 논문을 위한 BMC 등록
    // module_id_table.insert(make_pair("2", "http://10.0.6.104:443"));

    uuid_str = generate_uuid();
    log(info) << "global uuid : " << uuid_str;

    // Initialization
    if(init_gpio())
        log(info) << "GPIO initialization complete";

    if(init_i2c())
        log(info) << "I2C initialization complete";

    if (init_resource())
        log(info) << "Redfish resource initialization complete";

    // add_system("/redfish/v1/Systems/CM1", "http://10.0.6.104:443");
    // add_manager("/redfish/v1/Managers/CM1", "http://10.0.6.104:443");
    // add_chassis("/redfish/v1/Chassis/CM1", "http://10.0.6.104:443");
    // add test

    // cout << " DB TEST ss" << endl;
    // make_test_db();
    // cout << " DB TEST ee" << endl;
    //
    // time_t my_time = time(NULL);
    // struct tm my_tm = *localtime(&my_time);
    // insert_reading_table("Sensor number 002", "Temperature", 20.3, my_tm, "cpu");
    // my_tm = *localtime(&my_time);
    // insert_reading_table("Sensor number 004", "Fan", 5000, my_tm, "chassis");
    // select_all_reading("Temperature");
    // select_hour_reading("Temperature");

    // uint16_t f1 = 0, f2 = 0;
    // f1 |= 0x1;
    // f1 |= 0x2;
    // f1 |= 0x4000;
    // SensorMake s1, s2;
    // s1.id = "CabinetTemp";
    // s1.reading_type = "Rotational";
    // s1.reading_time = "2021-11-05 10:00:00";
    // s1.reading = 5000;
    // s1.reading_units = "UNIT";
    // s1.reading_range_max = 10000;
    // s1.status.health = STATUS_HEALTH_WARNING;

    // s2.id = "NewTemp";
    // s2.reading_type = "Temperature";
    // s2.reading_time = "2021-11-05 20:00:00";
    // s2.reading = 50.5;
    
    // make_sensor(s1, f1);
    // make_sensor(s2, f2);

    // generate_log test
    // Event_Info ev;
    // SEL sel;
    // vector<string> arg;
    // ev = generate_event_info("event id", "Info", "CHANGE MESSAGE", arg);
    // // ev = generate_event_info("event id", "Error", "Do It");
    // sel = generate_sel(7, "Limit Exceeded", "Temperature", "Temperature is over the threshold", "Critical");
    // // sel = generate_sel(150, "Limit Exceeded", "Temperature", "Temp is Temp Temp", "Why So Serious?");
    // // string console_tt = "/redfish/v1/Chassis/CMM/LogServices/Console";
    // // LogService *lll = (LogService *)g_record[console_tt];
    // // lll->record_count = 1000;
    // generate_log("Chassis", "Console", ev);
    // generate_log("Chassis", "Console", sel);

    // mkdir("/conf/database", 0755);
    // insert_reading_table("SID 1", "CMM1", "thermal", "fan", 5000, "2021-11-10 17:20:00");
    // insert_reading_table("SID 2", "CMM1", "thermal", "temperature", 36.5, "2021-11-10 17:20:00");

    // 문서작업 결과 넣기용
   
    // generate_test();

    // shoot event/log test ev, sel...
    // vector<string> args;
    // Event_Info ev;
    // ev = generate_event_info("Event ID", "ResourceAdded", "ResourceCreated", args);
    // send_event_to_subscriber(ev);
    // generate_log("Manager", "Log1", ev);
    // // generate_log("Chassis", "Facility 1", ev);

    // args.push_back("/redfish/v1/Managers/CMM1");
    // // args.push_back("/redfish/v1/Chassis/CMM1");
    // ev = generate_event_info("Event ID 2", "StatusChange", "StatusChangeCritical", args);
    // send_event_to_subscriber(ev);
    // generate_log("Manager", "Log1", ev);
    // // generate_log("System", "Facility 5", ev);

    // SEL sel;
    // sel = generate_sel(10, "Lower Critical - going high", "temperature",
    // "The reading crossed the Lower Critical threshold while going high", "Criticial",
    // "2021-11-26 15:00:00", "Alert");
    // send_event_to_subscriber(sel);
    // generate_log("Manager", "Log1", sel);
    // // generate_log("Chassis", "Facility 1", sel);

    // Log Entry 생성
    // Event_Info t_ev;
    // vector<string> args;
    // t_ev = generate_event_info("TEST Event 1", "Alert", "ResourceUpdated", args);
    // generate_log("Manager", "TestLogService", t_ev);


    // cm1 샤시 스토리지 만들기
    // string chassis_sto_odata = "/redfish/v1/Chassis/CM1";
    // Chassis *cha = (Chassis *)g_record[chassis_sto_odata];
    // // cha->storage = new Collection(cha->odata.id + "/Storage", ODATA_STORAGE_COLLECTION_TYPE);
    // // cha->storage->name = "Chassis Storage Collection";

    // Storage *sto = new Storage(cha->storage->odata.id + "/1", "1");
    // cha->storage->add_member(sto);

    // 샘플 fan 생성
    // Thermal *therm = (Thermal *)g_record["/redfish/v1/Chassis/CMM1/Thermal"];
    // Fan *fan_1 = new Fan(therm->fans->odata.id + "/1");
    // fan_1->status.state = STATUS_STATE_ENABLED;
    // fan_1->status.health = STATUS_HEALTH_OK;
    // fan_1->reading = 1000;
    // therm->fans->add_member(fan_1);
    // resource_save_json(fan_1);
    
    
    // Fan *fan_2 = new Fan(therm->fans->odata.id + "/4");
    // fan_2->status.state = STATUS_STATE_ENABLED;
    // fan_2->status.health = STATUS_HEALTH_OK;
    // fan_2->reading = 800;
    // therm->fans->add_member(fan_2);
    // resource_save_json(fan_2);
    
    // Fan *fan_3 = new Fan(therm->fans->odata.id + "/6");
    // fan_3->status.state = STATUS_STATE_ENABLED;
    // fan_3->status.health = STATUS_HEALTH_WARNING;
    // fan_3->reading = 1500;
    // therm->fans->add_member(fan_3);
    // resource_save_json(fan_3);
    // resource_save_json(therm->fans);

    // mac 주소 유효 테스트
    // if(validateMACAddress("11:22:33:44:55:66"))
    //     cout << "11:22:33:44:55:66 : VALID" << endl;
    // else
    //     cout << "11:22:33:44:55:66 : NOP" << endl;

    // if(validateMACAddress("1b:c2:a3:f4:5d:e6"))
    //     cout << "1b:c2:a3:f4:5d:e6 : VALID" << endl;
    // else
    //     cout << "1b:c2:a3:f4:5d:e6 : NOP" << endl;

    // if(validateMACAddress("1b:c2:a3"))
    //     cout << "1b:c2:a3 : VALID" << endl;
    // else
    //     cout << "1b:c2:a3 : NOP" << endl;

    // if(validateMACAddress("1b:c2:a355:f4:5d:e6"))
    //     cout << "1b:c2:a355:f4:5d:e6 : VALID" << endl;
    // else
    //     cout << "1b:c2:a355:f4:5d:e6 : NOP" << endl;

    // if(validateMACAddress("1b:c2:a355:f4:5d:e6:77"))
    //     cout << "1b:c2:a355:f4:5d:e6:77 : VALID" << endl;
    // else
    //     cout << "1b:c2:a355:f4:5d:e6:77 : NOP" << endl;

    // if(validateMACAddress("tb:c2:a3:f4:5d:e6"))
    //     cout << "tb:c2:a3:f4:5d:e6 : VALID" << endl;
    // else
    //     cout << "tb:c2:a3:f4:5d:e6 : NOP" << endl;

    // network 테스트
    // get
    // uint8_t idx=1;
    // uint8_t *ip_addr = new uint8_t[4], *netmask = new uint8_t[4],
    //         *gateway = new uint8_t[4], *mac_addr = new uint8_t[6];
    // int mtu;
    // get_ip_info(idx, ip_addr, netmask, gateway, mac_addr, &mtu);
    // cout << "----------------- BEFORE eth1 -------------------" << endl;
    // printf("IP : %d.%d.%d.%d\n", ip_addr[0], ip_addr[1], ip_addr[2], ip_addr[3]);
    // printf("MASK : %d.%d.%d.%d\n", netmask[0], netmask[1], netmask[2], netmask[3]);
    // printf("GATE : %d.%d.%d.%d\n", gateway[0], gateway[1], gateway[2], gateway[3]);
    // printf("MAC : %02x:%02x:%02x:%02x:%02x:%02x\n", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3]
    // , mac_addr[4], mac_addr[5]);
    // printf("MTU : %d\n", mtu);
    
    // // set
    // char dev[5] = {0};
    // char str_ip[20] = {0};
    // char str_net[20] = {0};
    // char str_gw[20] = {0};
    // char str_mtu[20] = {0};
    // char str_mac[20] = {0};
    // sprintf(dev, "eth1");
    // sprintf(str_ip, "10.0.6.100");
    // sprintf(str_net, "255.255.0.0");
    // sprintf(str_gw, "10.0.0.5");
    // sprintf(str_mtu, "1300");
    // sprintf(str_mac, "00:00:00:00:00:01");
    // set_ip(dev, str_ip, str_net, str_gw, str_mtu, str_mac);

    // get_ip_info(idx, ip_addr, netmask, gateway, mac_addr, &mtu);
    // cout << "----------------- BEFORE eth1 -------------------" << endl;
    // printf("IP : %d.%d.%d.%d\n", ip_addr[0], ip_addr[1], ip_addr[2], ip_addr[3]);
    // printf("MASK : %d.%d.%d.%d\n", netmask[0], netmask[1], netmask[2], netmask[3]);
    // printf("GATE : %d.%d.%d.%d\n", gateway[0], gateway[1], gateway[2], gateway[3]);
    // printf("MAC : %02x:%02x:%02x:%02x:%02x:%02x\n", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3]
    // , mac_addr[4], mac_addr[5]);
    // printf("MTU : %d\n", mtu);

    // 파일수정 테스트
    // change_hosts_file("keti", "mymy");
    // change_hostname_file("newhostname");
    // change_interface_file("eth1", "address", "10.0.6.100");
    // change_interface_file("eth2", "address", "10.0.6.10");
    // change_interface_file("eth1", "netmask", "255.255.0.0");
    // change_interface_file("eth1", "gateway", "this is gateway");
    // change_interface_file("eth1", "gateway", "10.0.0.1");
    // change_interface_file("eth1", "mtu", "5000");
    // change_interface_file("eth1", "hwaddress ether", "this is MAC");
    // change_interface_file("eth2", "netmask", "mask is kf94");

    // Add_Ethernet_Info eth_info;
    // eth_info.address = "10.0.6.100";
    // eth_info.netmask = "255.255.0.0";
    // eth_info.gateway = "10.0.0.1";
    // eth_info.mtu = "2000";

    // add_ethernet_to_interface_file("eth2", eth_info);

    // change_interface_file("eth1", "address", "10.0.6.106");
    // change_web_app_file("10.0.6.100", "10.0.6.106");

    // system("/etc/init.d/S40network restart");
    

    // ssdp discover (not working yet)
    // std::thread t_ssdp(ssdp_handler);
    // log(info) << "ssdp discover start";
    // t_ssdp.join();

    // pplx::create_task([]{
    //     sleep(3);
    //     string cmm_chassis = ODATA_CHASSIS_ID;
    //     cmm_chassis = cmm_chassis + "/" + CMM_ID;
    //     ((Chassis *)g_record[cmm_chassis])->led_lit(LED_GREEN);
    //     // ((Temperature *)(((Chassis *)g_record[cmm_chassis])->thermal->temperatures->members[0]))->read(0, INTAKE_CONTEXT);
    //     // cout << "ASDFASDFASDF : " << ((Temperature *)(((Chassis *)g_record[cmm_chassis])->thermal->temperatures->members[0]))->reading_celsius << endl;
    //     // ((Chassis *)g_record[cmm_chassis])->led_lit(LED_YELLOW);
    //     // ((Chassis *)g_record[cmm_chassis])->led_lit(LED_RED);
    //     sleep(3);
    //     // ((Chassis *)g_record[cmm_chassis])->led_off(LED_GREEN);
    //     // ((Chassis *)g_record[cmm_chassis])->led_blinking(LED_RED);
    //     // ((Chassis *)g_service_root->chassis_collection->members[0])->led_lit(LED_GREEN);
    // });

    http_listener_config listen_config;

    // Set SSL certification
    listen_config.set_ssl_context_callback([](boost::asio::ssl::context &_ctx) {
        _ctx.set_options(
            boost::asio::ssl::context::default_workarounds | boost::asio::ssl::context::no_sslv2 // Not use SSL2
            | boost::asio::ssl::context::no_tlsv1                                                // NOT use TLS1
            | boost::asio::ssl::context::no_tlsv1_1                                              // NOT use TLS1.1
            | boost::asio::ssl::context::single_dh_use);

        // Certificate Password Provider
        // _ctx.set_password_callback([](size_t max_length,
        //                               boost::asio::ssl::context::password_purpose purpose) {
        //     return "ketilinux";
        // });

        log(info) << "Server crt file path: " << SERVER_CERTIFICATE_CHAIN_PATH;
        // _ctx.use_certificate_chain_file("/conf/ssl/test/my_rootca.crt");
        _ctx.use_certificate_chain_file(SERVER_CERTIFICATE_CHAIN_PATH);
        log(info) << "Server key file path: " << SERVER_PRIVATE_KEY_PATH;
        // _ctx.use_private_key_file("/conf/ssl/test/my_rootca.key", boost::asio::ssl::context::pem);
        _ctx.use_private_key_file(SERVER_PRIVATE_KEY_PATH, boost::asio::ssl::context::pem);
        log(info) << "Server pem file path: " << SERVER_TMP_DH_PATH;
        // _ctx.use_tmp_dh_file("/conf/ssl/test/my_dh2048.pem");
        _ctx.use_tmp_dh_file(SERVER_TMP_DH_PATH);
    });

    // Set request timeout
    log(info) << "Server request timeout: " << SERVER_REQUEST_TIMEOUT << " sec";
    listen_config.set_timeout(utility::seconds(SERVER_REQUEST_TIMEOUT));

    // Set server entry point
    log(info) << "Server entry point: " << SERVER_ENTRY_POINT;
    utility::string_t url = U(SERVER_ENTRY_POINT);

    // RESTful server start
    start_server(url, listen_config);
    


    http_listener_config HAlisten_config;

    // Set SSL certification
    HAlisten_config.set_ssl_context_callback([](boost::asio::ssl::context &_ctx) {
        _ctx.set_options(
            boost::asio::ssl::context::default_workarounds | boost::asio::ssl::context::no_sslv2 // Not use SSL2
            | boost::asio::ssl::context::no_tlsv1                                                // NOT use TLS1
            | boost::asio::ssl::context::no_tlsv1_1                                              // NOT use TLS1.1
            | boost::asio::ssl::context::single_dh_use);});
    HAlisten_config.set_timeout(utility::seconds(SERVER_REQUEST_TIMEOUT));
    utility::string_t HAurl = U("http://0.0.0.0:8000");
    // ha용.. http용..

    HA_listener = unique_ptr<Handler>(new Handler(HAurl, HAlisten_config));
    HA_listener->open().wait();
    log(info) << "HA Heartbeat server start";

    // cout << "This is Original File!!!" << endl;

    // std::thread t_ssdp(ssdp_handler);
    // log(info) << "ssdp discover start";
    // t_ssdp.join();

    while (true) 
        pause();

    g_listener->close().wait();
    HA_listener->close().wait();

    exit(0);
}