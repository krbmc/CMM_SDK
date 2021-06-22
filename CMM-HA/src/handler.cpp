#include "handler.hpp"
#include<fstream>
unsigned int g_count = 0;

/**
 * @brief Handler class constructor with method connection
 * 
 */
extern int redfish_power_ctrl;
Handler::Handler(utility::string_t _url, http_listener_config _config) : m_listener(_url, _config)
{
    log(info) << "_url=" << _url;

    // Handler connection
    this->m_listener.support(methods::GET, std::bind(&Handler::handle_get, this, std::placeholders::_1));
    this->m_listener.support(methods::PUT, std::bind(&Handler::handle_put, this, std::placeholders::_1));
    this->m_listener.support(methods::POST, std::bind(&Handler::handle_post, this, std::placeholders::_1));
    this->m_listener.support(methods::DEL, std::bind(&Handler::handle_delete, this, std::placeholders::_1));
    this->m_listener.support(methods::PATCH, std::bind(&Handler::handle_patch, this, std::placeholders::_1));
}
/**
 * @brief GET method request handler
 * 
 * @param _request Request object
 * @todo ipmi_redfish_handler 연동중
 */
void Handler::handle_get(http_request _request)
{

    log(info) << "Request method: GET";
    string uri = _request.request_uri().to_string();
    vector<string> uri_tokens = string_split(uri, '/');
    string filtered_uri = make_path(uri_tokens);
    string servicepath;
    json::value j = _request.extract_json().get();

    log(info) << "Reqeust URL : " << filtered_uri;
    log(info) << "Request Body : " << _request.to_string();

    try
    {
        _request.reply(status_codes::OK);
    }
    catch (json::json_exception &e)
    {
        log(info) << "badRequest" << uri_tokens[0];
        _request.reply(status_codes::BadRequest);
    }
    g_count++;
    log(info) << g_count;
}

/**
 * @brief DELETE request handler
 * 
 * @param _request Request object
 */
void Handler::handle_delete(http_request _request)
{

    cout << "handle_delete request" << endl;

    auto j = _request.extract_json().get();

    _request.reply(status_codes::NotImplemented, U("DELETE Request Response"));
}

/**
 * @brief PUT request handler
 * 
 * @param _request Request object
 */
void Handler::handle_put(http_request _request)
{
    log(info) << "Request method: PUT";
    cout << "handle_put request" << endl;

    auto j = _request.extract_json().get();

    _request.reply(status_codes::NotImplemented, U("PUT Request Response"));
}

/**
 * @brief PATCH request handler
 * 
 * @param _request Request object
 */
void Handler::handle_patch(http_request _request)
{
    log(info) << "Request method: PATCH1";
    string uri = _request.request_uri().to_string();
    vector<string> uri_tokens = string_split(uri, '/');
    string filtered_uri = make_path(uri_tokens);
    log(info) << "Reqeust uri : " << uri;
    json::value b = _request.extract_json(true).get();
    log(info) <<"request data "<<b.serialize();
    log(info) <<"filtered_uri = "<<filtered_uri;
    try
    {
        _request.reply(status_codes::BadRequest);
    }
    catch (json::json_exception &e)
    {
        log(info) << "json_Exeception error ";
        _request.reply(status_codes::BadRequest);
    }

    _request.reply(status_codes::NotImplemented, U("PATCH Request Response"));
}

/**
 * @brief POST request handler
 * 
 * @param _request Request object
 */
void Handler::handle_post(http_request _request)
{
    log(info) << "Request method: POST start";
    string uri = _request.request_uri().to_string();
    vector<string> uri_tokens = string_split(uri, '/');
    string filtered_uri = make_path(uri_tokens);
    log(info) << "Reqeust uri : " << uri;
    json::value b = _request.extract_json().get();
    
    log(info) << "Reqeust uri : " << filtered_uri;
    bool enabled = true;

    _request.reply(status_codes::BadRequest);
}
json::value heart_request(string _uri, string _path, json::value _jv)
{
    cout << "<< POST >>" << endl;
    cout << "path : " << _path << endl;
    cout << "uri : " << _uri << endl;
    // 파라미터로 json::value를 그대로 받으면 그냥 쓰면 되고
    // 그 안에 내용물을 받았으면 이렇게 만들어주어야함
    http_client client(_path);

    pplx::task<http_response> responseTask = client.request(methods::POST, _uri, _jv.serialize(), U("application/json"));
    http_response response = responseTask.get();
    json::value response_json = response.extract_json().get();
    // cout << "status code : " << response.status_code() << endl;
    // cout << response_json << endl;
    // cout << "!@#$ Good POST" << endl;
    return response_json;
}