#include "handler.hpp"

// extern unordered_map<string, unordered_map<string, Task *> > task_map;
unsigned int g_count = 0;

/**
 * @brief Handler class constructor with method connection
 * 
 */
Handler::Handler(utility::string_t _url, http_listener_config _config) : m_listener(_url, _config)
{
    // Handler connection
    this->m_listener.support(methods::GET, bind(&Handler::handle_get, this, placeholders::_1));
    this->m_listener.support(methods::PUT, bind(&Handler::handle_put, this, placeholders::_1));
    this->m_listener.support(methods::POST, bind(&Handler::handle_post, this, placeholders::_1));
    this->m_listener.support(methods::DEL, bind(&Handler::handle_delete, this, placeholders::_1));
}

/**
 * @brief GET method request handler
 * 
 * @param _request Request object
 */
void Handler::handle_get(http_request _request)
{
}

/**
 * @brief DELETE request handler
 * 
 * @param _request Request object
 */
void Handler::handle_delete(http_request _request)
{
    _request.reply(status_codes::NotImplemented, U("DELETE Request Response"));
}

/**
 * @brief PUT request handler
 * 
 * @param _request Request object
 */
void Handler::handle_put(http_request _request)
{

    cout << "handle_put request" << endl;

    auto j = _request.extract_json().get();

    _request.reply(status_codes::NotImplemented, U("PUT Request Response"));
}

/**
 * @brief POST request handler
 * 
 * @param _request Request object
 */
void Handler::handle_post(http_request _request)
{

    log(info) << "Request method: POST";
    string uri = _request.request_uri().to_string();
    vector<string> uri_tokens = string_split(uri, '/');
    string filtered_uri = make_path(uri_tokens);
    log(info) << "Reqeust URL : " << filtered_uri;
    log(info) << "Request Body : " << _request.to_string();

    try
    {
        if(uri_tokens.size() == 1 && uri_tokens[0] == "Heartbeat")
        {
            json::value j;
            j[U("Status")] = json::value::string("OK");
            _request.reply(status_codes::OK, j);
            return ;
        }
    }  
    catch(json::json_exception& e)
    {
        _request.reply(status_codes::BadRequest);
        return ;
    }
}
/**
 * @brief : CMM모듈이 client가 되어 request요청하기
 * @authors 강
 * @todo : 호출될때 줄 파라미터에 따라서 수정필요, _uri 기능 추가될때마다 처리부 코드도 추가, try/catch
*/
// json::value/*pplx::task<http_response>*/ redfish_request_get(string _uri, string _host, int _port)// json::value j_data)
json::value redfish_request_get(string _path, string _url) // path가 /redfish/v1...부분  url이 http://10....부분
{
    cout << "<< GET >>" << endl;
    cout << "url : " << _url << endl;
    cout << "path : " << _path << endl;
    cout << "err1" << endl;
    string tmp = _url;
    http_client client(_url);
    cout << "err2" << endl;

    pplx::task<http_response> responseTask = client.request(methods::GET, _path);//b.to_string());
    cout << "err3" << endl;
    http_response response = responseTask.get();
    cout << "err4" << endl;
    json::value response_json = response.extract_json().get();
    cout << "####" << endl;
    cout << response_json << endl;

    return response_json;
    
}

// post를 따로 함수 만들기 AccountService/Accounts 로 똑같이 만들어지는거 같으니 파라미터로 json받아서
// 요청 넣는식으로 구현하기 반환값이 json이 따로 없는듯하여 status봐야할듯

void redfish_request_post(string _path, string _url)//json::value _jv)
{
}

void redfish_request_patch(string _path, string _url)
{
    cout << "!@#$ PATCH!!" << endl;
}

void redfish_request_delete(string _path, string _url)
{
    cout << "!@#$ DELETE!!" << endl;
}
