#include "handler.hpp"
#include "resource.hpp"
#include "hwcontrol.hpp"
#include "task.hpp"

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

/**
 * @brief 
 * 
 * @param _url 
 * @param _config 
 */
void start_server(utility::string_t &_url, http_listener_config _config)
{
    g_listener = unique_ptr<Handler>(new Handler(_url, _config));
    g_listener->open().wait();
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
    // Initialization
    if(init_gpio())
        log(info) << "GPIO initialization complete";

    if(init_i2c())
        log(info) << "I2C initialization complete";

    if (init_resource())
        log(info) << "Redfish resource initialization complete";

    uuid_str = generate_uuid();
    log(info) << "global uuid : " << uuid_str;
    // @@@@@ cmm이 요청보내는 것 테스트용
    // json::value ddd;
    // ddd = redfish_request_get("/redfish", "http://10.0.6.104:443");
    // cout << ddd << endl;
    // cout << "!@#$ Good GET" << endl;

    // redfish_request_post("/redfish/v1/AccountService/Accounts", "http://10.0.6.104:443");
    // redfish_request_post("/redfish/v1/SessionService/Sessions", "http://10.0.6.104:443");
    // redfish_request_patch("/redfish/v1/SessionService/Sessions", "http://10.0.6.104:443");
    // redfish_request_delete("/redfish/v1/SessionService/Sessions", "http://10.0.6.104:443");
    // @@@@@

<<<<<<< HEAD
    // ssdp discover (not working yet)
    std::thread t_ssdp(ssdp_handler);
    log(info) << "ssdp discover start";
    t_ssdp.join();
=======
    // // ssdp discover (not working yet)
    // std::thread t_ssdp(ssdp_handler);
    // log(info) << "ssdp discover start";
    // t_ssdp.join();
>>>>>>> e363a34bb856e6f86ae0963c762ae6a32375c3f1

    pplx::create_task([]{
        sleep(3);
        string cmm_chassis = ODATA_CHASSIS_ID;
        cmm_chassis = cmm_chassis + "/" + CMM_ID;
        ((Chassis *)g_record[cmm_chassis])->led_lit(LED_GREEN);
        // ((Chassis *)g_service_root->chassis_collection->members[0])->led_lit(LED_GREEN);
    });

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
        _ctx.use_certificate_chain_file(SERVER_CERTIFICATE_CHAIN_PATH);
        log(info) << "Server key file path: " << SERVER_PRIVATE_KEY_PATH;
        _ctx.use_private_key_file(SERVER_PRIVATE_KEY_PATH, boost::asio::ssl::context::pem);
        log(info) << "Server pem file path: " << SERVER_TMP_DH_PATH;
        _ctx.use_tmp_dh_file(SERVER_TMP_DH_PATH);
    });

    // Set request timeout
    log(info) << "Server request timeout: " << SERVER_REQUEST_TIMEOUT << " sec";
    listen_config.set_timeout(utility::seconds(SERVER_REQUEST_TIMEOUT));

    // Set server entry point
    log(info) << "Server entry point: " << SERVER_ENTRY_POINT;
    utility::string_t url = U(SERVER_ENTRY_POINT);
    // cout << "!@#$URL?? : " << url << endl;

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
    utility::string_t HAurl = U("http://0.0.0.0:80");

    HA_listener = unique_ptr<Handler>(new Handler(HAurl, HAlisten_config));
    HA_listener->open().wait();
    log(info) << "HA Heartbeat server start";

    while (true) 
        pause();

    g_listener->close().wait();
    HA_listener->close().wait();

    exit(0);
}