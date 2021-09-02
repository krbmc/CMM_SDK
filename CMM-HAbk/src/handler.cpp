#include "handler.hpp"
#include<fstream>
#include "KETI_halmp.hpp"
#include "KETIHA.hpp"
#include "xml2json.hpp"
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
    this->m_listener.support(std::bind(&Handler::handle_get, this, std::placeholders::_1));
    //this->m_listener.support(methods::PUT, std::bind(&Handler::handle_put, this, std::placeholders::_1));
    //this->m_listener.support(methods::POST, std::bind(&Handler::handle_post, this, std::placeholders::_1));
    //this->m_listener.support(methods::DEL, std::bind(&Handler::handle_delete, this, std::placeholders::_1));
    //this->m_listener.support(methods::PATCH, std::bind(&Handler::handle_patch, this, std::placeholders::_1));
}
json::value string2json(http_request &_request,json::value &obj)
{
    try{
        string wstr_utf8 = _request.extract_string().get();
        string converter =xml2json(wstr_utf8.c_str());
        utility::stringstream_t s;
        s<<converter;
        obj=json::value::parse(s);

        //cout <<obj.serialize().c_str()<<endl;
    }
    catch(...)
    {
        obj["Error"]=json::value::string("Not XML,JSON FORMAT");
    }
    return obj;
}
/**
 * @brief GET method request handler
 * 
 * @param _request Request object
 * @todo ipmi_redfish_handler 연동중
 */
void Handler::handle_get(http_request _request)
{
    auto ha = KETI_halmp::Instance();
    string senduri="http://";
json::value obj ;
    if (ha->IsSwtich==KETIhaStatus::HA_STATUS_ACTIVE)
        senduri+=ha->PeerPrimaryAddress+":"+to_string(80);
    else if(ha->IsSwtich==KETIhaStatus::HA_STATUS_STANDBY)
        senduri+=ha->PeerSecondaryAddress+":"+to_string(80);
    else
    {
        log(info) << "Not response" ;
        _request.reply(status_codes::BadRequest);
    }
    log(info) << "Request method: GET";
    string uri = _request.request_uri().to_string();
    vector<string> uri_tokens = string_split(uri, '/');
    string filtered_uri = make_path(uri_tokens);
    string servicepath;
    try
    {
        json::value j = _request.extract_json().get();
    }
    catch (...)
    {
        log(info) << "NOT JSON : ";
        
        string2json(_request,obj);
// printf("root 노드 이름: %s\n", root->name()); 
        
        // std::cout << wstr_utf8 << std::endl;

    }
    log(info) << "Reqeust URL : " << filtered_uri;
    log(info) << "send URL : " << senduri;
    log(info) << "Request Body : " << _request.to_string();
    http_response response;
    response=All_request(senduri,filtered_uri,_request.method(),obj);
    json::value response_json = response.extract_json().get();
    try
    {
        _request.reply(response.status_code(),response_json);
    }
    catch (json::json_exception &e)
    {
        log(info) << "badRequest" << uri_tokens[0];
        _request.reply(status_codes::BadRequest);
    }
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
/**
 * @brief All request command 
 * @param _uri sending uri
 * @param _path sending path
 * @param _jv sending json format data
 * @param mth http protocal method GET PUT DELETE POST only GET method don't have _jv
 * 
 */
http_response All_request(string _uri, string _path,const web::http::method &mth,json::value _jv)
{

    http_client client(_uri);
    pplx::task<http_response> responseTask;
    responseTask = client.request(mth, _path,_jv.serialize(), U("application/json"));
    http_response response = responseTask.get();
    return response;
}
json::value heart_request(string _uri, string _path, json::value _jv)
{
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
