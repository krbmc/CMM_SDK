#include "KETIhaNetService.hpp"

unique_ptr<Handler> g_listener;
src::severity_logger<severity_level> g_logger;
KETIhaNetService::KETIhaNetService():m_Quit(true)
{

}
KETIhaNetService::~KETIhaNetService()
{

}
 KETIhaError KETIhaNetService::Start(int Port, int TimeOut)
 {
     unique_lock<mutex> _(m_Lock);
     if (!m_Quit.load())
    {
        cout<<("Rest service is running.")<<endl;
        return KETIhaError::HA_ERROR_FAIL;
    }
    m_Quit.store(false);
    try
    {   cout<<("Rest service is start.")<<endl;
        m_thread=new std::thread([&](){RESTService(NULL);});
        cout<<("Rest service is start.")<<endl;
    }
    catch (std::system_error e)
    {
        cout<<("Fail to start Rest service: {}.", e.what())<<endl;;
        m_Quit.store(true);
        return KETIhaError::HA_ERROR_FAIL;
    }
    catch (std::bad_alloc e)
    {
         cout<<("Fail to start Rest service: {}.", e.what())<<endl;
        m_Quit.store(true);
        return KETIhaError::HA_ERROR_FAIL;
    }
    catch (...)
    {
        cout<<("Fail to start Rest service: Unknown exception.")<<endl;
        m_Quit.store(true);
        return KETIhaError::HA_ERROR_FAIL;
    }

   cout<<("Started Rest service.")<<endl;
    return KETIhaError::HA_ERROR_OK;

 }
KETIhaError KETIhaNetService::Stop()
{
    unique_lock<mutex> _(m_Lock);

    if (m_Quit.load())
    {
        cout<<("REST service is stopped.")<<endl;
        return KETIhaError::HA_ERROR_FAIL;
    }

    cout<<("Stopping REST service.")<<endl;

    m_Quit.store(true);

    m_thread->detach();
    cout<<("Stopped REST service.")<<endl;

    return KETIhaError::HA_ERROR_OK;
}

void KETIhaNetService::RESTService(void *data)
{
    http_listener_config listen_config;

    // Set SSL certification
    listen_config.set_ssl_context_callback([](boost::asio::ssl::context &_ctx) {
        _ctx.set_options(
            boost::asio::ssl::context::default_workarounds 
            | boost::asio::ssl::context::no_sslv2 // Not use SSL2
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

    // RESTful server start
    g_listener = unique_ptr<Handler>(new Handler(url, listen_config));

    g_listener->open().wait();
    log(info) << "CMM HA start";
    

    while (true) 
        pause();

    g_listener->close().wait();
    exit(0);
}