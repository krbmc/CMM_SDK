#include "stdafx.hpp"

/**
 * @brief Split string to delimiter
 * 
 * @param _string Target string
 * @param _delimiter Delimiter
 * @return vector<string> Vector of string token
 */
vector<string> string_split(const string _string, char _delimiter)
{
    vector<string> tokens;
    string sub_string;
    size_t start = 0;
    size_t end = 0;
    while ((end = _string.find(_delimiter, start)) != string::npos)
    {
        sub_string = _string.substr(start, end - start);
        if (sub_string != "" || sub_string != "\0")
            tokens.push_back(sub_string);
        start = end + 1;
    }
    if (_string.substr(start) != "")
        tokens.push_back(_string.substr(start));

    return tokens;
}

/**
 * @brief Make full path from tokens
 * 
 * @param _tokens String token vector
 * @return string Path string
 */
string make_path(vector<string> _tokens)
{
    string path = "/";
    for (unsigned int i = 0; i < _tokens.size(); i++)
        if (_tokens[i] != "")
        {
            if (i == 0)
                path = path + _tokens[i];
            else
                path = path + '/' + _tokens[i];
        }
    return path;
}

/**
 * @brief Sort function of string compare
 * 
 * @param _firt_string First string
 * @param _second_string Second string
 * @return true First string faster
 * @return false Second string faster
 */
bool comp(const string &_firt_string, const string &_second_string)
{
    if (_firt_string.size() == _second_string.size())
        return _firt_string < _second_string;
    return _firt_string.size() < _second_string.size();
}

void timer(boost::asio::deadline_timer *_timer, unsigned int *_remain_expires_time)
{
    if (*_remain_expires_time > 0)
    {
        log(debug) << "Session expires remain: " << *_remain_expires_time;
        --(*_remain_expires_time);
        _timer->expires_at(_timer->expires_at() + boost::posix_time::seconds(1));
        _timer->async_wait(boost::bind(timer, _timer, _remain_expires_time));
    }else if (*_remain_expires_time == 0){
        log(info) << "session close call in timer~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
        _timer->expires_from_now();
    }
}

/**
 * @brief Generate random token string
 * 
 * @param _length Token length
 * @return string Token string
 */
string generate_token(const int _length)
{
    const vector<char> character = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                                    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
                                    'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
                                    'U', 'V', 'W', 'X', 'Y', 'Z',
                                    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
                                    'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
                                    'u', 'v', 'w', 'x', 'y', 'z'};

    default_random_engine random_engine(random_device{}());
    uniform_int_distribution<> distribution(0, character.size() - 1);
    auto random_char = [character, &distribution, &random_engine]() { return character[distribution(random_engine)]; };

    string token(_length, 0);
    generate_n(token.begin(), _length, random_char);

    return token;
}

/**
 * @brief generate task, account number
 * @author 강
 * @return unsigned int , unsigned int(to string변환필요)
 */

map<int, set<unsigned int> > numset;
map<int, unsigned int> numset_num;



// set<unsigned int> task_numset;
// set<unsigned int> account_numset;
// set<unsigned int> session_numset;
// set<unsigned int> vm_cd_numset;
// set<unsigned int> vm_usb_numset;
// unsigned int account_numset_num = 1;
// unsigned int session_numset_num = 1;
// unsigned int vm_cd_numset_num = 1;
// unsigned int vm_usb_numset_num = 1;

unsigned int allocate_numset_num(int _index)
{
    set<unsigned int> *set_ptr = &(numset[_index]);
    unsigned int *num_ptr = &(numset_num[_index]);

    unsigned int i = *num_ptr;
    for( ; i <= UINT_MAX; i++)
    {
        if(i == UINT_MAX)
        {
            i = 0;
            continue;
        }

        if(set_ptr->find(i) == set_ptr->end())
        {
            set_ptr->insert(i);
            *num_ptr = i;
            return i;
        }
    }
}

void insert_numset_num(int _index, unsigned int num)
{
    set<unsigned int> *set_ptr = &(numset[_index]);
    set_ptr->insert(num);
}

void delete_numset_num(int _index, unsigned int num)
{
    set<unsigned int> *set_ptr = &(numset[_index]);
    set_ptr->erase(num);
}

// // task id_num
// unsigned int allocate_task_num(void)
// {
//     unsigned int num = 1;
//     for(num; num < UINT_MAX; num++)
//     {
//         if(task_numset.find(num) == task_numset.end())
//         {
//             task_numset.insert(num);
//             return num;
//         }
//     }
// }

// void insert_task_num(unsigned int num)
// {
//     task_numset.insert(num);
//     return;
// }

// void delete_task_num(unsigned int num)
// {
//     task_numset.erase(num);
//     return ;
// }

// // account id_num
// unsigned int allocate_account_num(void)
// {
//     for(account_numset_num; account_numset_num <= UINT_MAX; account_numset_num++)
//     {
//         if(account_numset_num == UINT_MAX)
//         {
//             account_numset_num = 0;
//             continue;
//         }

//         if(account_numset.find(account_numset_num) == account_numset.end())
//         {
//             account_numset.insert(account_numset_num);
//             return account_numset_num;
//         }
//     }

// }

// void insert_account_num(unsigned int num)
// {
//     account_numset.insert(num);
//     return;
// }

// void delete_account_num(unsigned int num)
// {
//     account_numset.erase(num);
//     return ;
// }

// // session id_num
// unsigned int allocate_session_num(void)
// {
//     for(session_numset_num; session_numset_num <= UINT_MAX; session_numset_num++)
//     {
//         if(session_numset_num == UINT_MAX)
//         {
//             session_numset_num = 0;
//             continue;
//         }

//         if(session_numset.find(session_numset_num) == session_numset.end())
//         {
//             session_numset.insert(session_numset_num);
//             return session_numset_num;
//         }
//     }
// }

// void insert_session_num(unsigned int num)
// {
//     session_numset.insert(num);
//     return ;
// }

// void delete_session_num(unsigned int num)
// {
//     session_numset.erase(num);
//     return ;
// }

// 꽉차면?

/**
 * @brief get uri, return current resource name
 * @author dyk
 * @return resource type string 
 */
string get_current_object_name(string _uri, string delimiter)
{
    // return _uri.substr(_uri.rfind(delimiter) + 1);
    return _uri.substr(_uri.rfind(delimiter) + delimiter.length());
}
// 문자열 뒤부터 시작해서 delimiter찾고 뒷부분 get

/**
 * @brief get uri, return parent resource uri
 * @author dyk
 * @return resource uri string 
 */
string get_parent_object_uri(string _uri, string delimiter)
{
    return _uri.substr(0, _uri.rfind(delimiter));
}
// 문자열 뒤부터 시작해서 delimiter찾고 앞부분 get

/**
 * @brief get string, check if it is number
 * @author dyk
 * @return if string is num, retrun 1. else return 0. 
 */
bool isNumber(const string str)
{
    return !str.empty() && (str.find_first_not_of("0123456789") == string::npos);
}

/**
 * @brief get string, check if it is validate ipv4 address
 * @author dyk
 * @return if string is ipv4 addr, retrun 1. else return 0. 
 */
bool validateIPv4(const string str)
{
    vector<string> ip_nums = string_split(str, '.');
    if (ip_nums.size() != 4)
        return false;
    
    for (string num : ip_nums){
        if (!isNumber(num) || stoi(num) > 255 || stoi(str) < 0)
            return false;
    }
    return true;
}

/**
 * @brief get string, check if it is validate Datetime Local Offset (ex. +09:00)
 * @author dyk
 * @return if string is available datetime local offset, retrun 1. else return 0. 
 */
bool validateDatetimeLocalOffset(const string str)
{
    vector<string> available{
        "-12:00",
        "-11:00",
        "-10:00",
        "-09:00",
        "-08:00",
        "-07:00",
        "-06:00",
        "-05:00",
        "-04:00",
        "-03:00",
        "-02:00",
        "-01:00",
        "00:00",
        "+01:00",
        "+02:00",
        "+03:00",
        "+04:00",
        "+05:00",
        "+06:00",
        "+07:00",
        "+08:00",
        "+09:00",
        "+10:00",
        "+11:00",
        "+12:00",
    };

    if (find(available.begin(), available.end(), str) == available.end()) {
        return false;
    }
    return true;
}

/**
 * @brief get string, check if it is validate Severity
 * @author dyk
 * @return if string is available severity, retrun 1. else return 0. 
 */
bool validateSeverity(const string str)
{
    vector<string> available{
        "Alert",
        "All",
        "Critical",
        "Debug",
        "Emergency",
        "Error",
        "Informational",
        "Notice",
        "Warning",
    };

    if (find(available.begin(), available.end(), str) == available.end()) {
        return false;
    }
    return true;
}

/**
 * @brief get string, check if it is validate log facility
 * @author dyk
 * @return if string is available log facility, retrun 1. else return 0. 
 */
bool validateLogFacility(const string str)
{
    vector<string> available{
        "Auth",
        "Authpriv",
        "Console",
        "Cron",
        "Daemon",
        "FTP",
        "Kern",
        "Local0",
        "Local1",
        "Local2",
        "Local3",
        "Local4",
        "Local5",
        "Local6",
        "Local7",
        "LPR",
        "Mail",
        "News",
        "NTP",
        "Security",
        "SolarisCron",
        "Syslog",
        "User",
        "UUCP",
    };

    if (find(available.begin(), available.end(), str) == available.end()) {
        return false;
    }
    return true;
}

/**
 * @brief get string, check if it is validate DHCPv6 Operating Mode
 * @author dyk
 * @return if string is available DHCPv6 Operating Mode, retrun 1. else return 0. 
 */
bool validateDHCPv6OperatingMode(const string str)
{
    vector<string> available{
        "Disabled",
        "Stateful",
        "Stateless",
    };

    if (find(available.begin(), available.end(), str) == available.end()) {
        return false;
    }
    return true;
}

char *get_popen_string(char *command)
{
    FILE *fp = popen(command, "r");
    char *temp = (char *)malloc(sizeof(char)*256);
    if(fp != NULL)
    {
        while(fgets(temp, 256, fp) != NULL)
        {
        }
        pclose(fp);
    }
    if (temp[strlen(temp) -1] == '\n')
        temp[strlen(temp) -1] = '\0';
    return temp;
}

/**
 * @brief get popen string c++ style
 * @author dyk
 * @return string which command returns 
 */
string get_popen_string(string command)
{
    // log(warning) << command;
    FILE *fp = popen(command.c_str(), "r");
    char *temp = (char *)malloc(sizeof(char)*256);
    string ret;
    if (fp != NULL){
        while(fgets(temp, 256, fp) != NULL)
        {
            string str(temp);
            ret += temp;
        }
        pclose(fp);
    }
    if (ret.back() == '\n')
        ret.pop_back();
        
    // log(warning) << ret;
    return ret;
}

string get_extracted_bmc_id_uri(string _uri)
{
    vector<string> uri_tokens = string_split(_uri, '/');
    string new_uri;

    for(int i=0; i<uri_tokens.size(); i++)
    {
        if(i == 3)
            continue;
        new_uri += "/";
        new_uri += uri_tokens[i];
    }

    return new_uri;
}

bool check_role_privileges(string _pri)
{
    if(_pri == "ConfigureComponents" || _pri == "ConfigureManager" || _pri == "ConfigureSelf"
    || _pri == "ConfigureUsers" || _pri == "Login" || _pri == "NoAuth")
        return true;

    return false;
}
/**
 * @brief in body, find value with key and return value. if value doesn't exist... return init value
 * @author dyk
 * @return value that matches request type
 */
bool get_value_from_json_key(json::value body, string key, int& value)
{
    try
    {
        if (body.has_field(key)){
            value = body.at(key).as_integer();
        }
        else{
            value = 0;
            log(warning) << "error with parsing " << key << " to int";
            return false;
        }
    }
    catch(const std::exception& e)
    {
        value = 0;
        log(warning) << "error with parsing " << key << " to int";
        return false;
    }
    
    return true;
}

bool get_value_from_json_key(json::value body, string key, unsigned int& value)
{
    try
    {
        if (body.has_field(key)){
            value = body.at(key).as_integer();
        }
        else{
            value = 0;
            log(warning) << "error with parsing " << key << " to unsigned int";
            return false;
        }
    }
    catch(const std::exception& e)
    {
        value = 0;
        log(warning) << "error with parsing " << key << " to unsigned int";
        return false;
    }
    
    return true;
}

bool get_value_from_json_key(json::value body, string key, string& value)
{
    try
    {
        if (body.has_field(key)){
            value = body.at(key).as_string();
        }
        else{
            value = "";
            log(warning) << "error with parsing " << key << " to string";
            return false;
        }
    }
    catch(const std::exception& e)
    {
        value = "";
        log(warning) << "error with parsing " << key << " to string";
        return false;
    }
    
    return true;
}
bool get_value_from_json_key(json::value body, string key, json::value& value)
{
    try
    {
        if (body.has_field(key)){
            value = body.at(key);
        }
        else{
            value = json::value::null();
            log(warning) << "error with parsing " << key << " to json::value";
            return false;
        }
    }
    catch(const std::exception& e)
    {
        value = json::value::null();
        log(warning) << "error with parsing " << key << " to json::value";
        return false;
    }
    
    return true;
}
bool get_value_from_json_key(json::value body, string key, double& value)
{
    try
    {
        if (body.has_field(key)){
            value = body.at(key).as_double();
        }
        else{
            value = 0.0;
            log(warning) << "error with parsing " << key << " to double";
            return false;
        }
    }
    catch(const std::exception& e)
    {
        value = 0.0;
        log(warning) << "error with parsing " << key << " to double";
        return false;
    }
    
    return true;
}
bool get_value_from_json_key(json::value body, string key, bool& value)
{
    try
    {
        if (body.has_field(key)){
            value = body.at(key).as_bool();
        }
        else{
            value = false;
            log(warning) << "error with parsing " << key << " to bool";
            return false;
        }
    }
    catch(const std::exception& e)
    {
        value = false;
        log(warning) << "error with parsing " << key << " to bool";
        return false;
    }
    
    return true;
}

/**
 * @brief if file is exists, remove. 
 * @author dyk
 */
void remove_if_exists(fs::path file)
{
    if (fs::exists(file))
    {
        fs::remove(file);
        log(info) << "[###] remove old file : " << file.string();
    }
    return;
}

/**
 * @brief read value(string) with key from string that cmd returns
 * @author dyk
 */
string get_value_from_cmd_str(string cmd_str, string key)
{
    // log(warning) << cmd_str;
    string cmd_ret = get_popen_string(cmd_str);
    // log(warning) << cmd_ret;
    if (cmd_ret.rfind(key) == string::npos)
        return "";
    string ret = cmd_ret.substr(cmd_ret.rfind(key) + key.size());
    return string_split(ltrim(ret), ' ')[0];
}

/**
 * @brief string ltrim
 * @author dyk
 */
string ltrim(string str)
{
    return str.erase(0, str.find_first_not_of(" :\n\t`-|"));
}

/**
 * @brief uuid string generate
 * @author dyk
 */
string generate_uuid(void)
{
    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    return boost::uuids::to_string(uuid);
}


/**
 * @brief time format generate using tm struct
 */
void make_time_with_tm(tm _tm, char *_output)
// void time_function(uint8_t t_year, uint8_t t_mon, uint8_t t_days, uint8_t t_hour, uint8_t t_min, uint8_t t_sec,char *output)
{
	char month[3], days[3], hour[3], min[3], sec[3] = {0};
    uint8_t t_year, t_mon, t_days, t_hour, t_min, t_sec;
    t_year = _tm.tm_year;
    t_mon = _tm.tm_mon;
    t_days = _tm.tm_mday;
    t_hour = _tm.tm_hour;
    t_min = _tm.tm_min;
    t_sec = _tm.tm_sec;
    
	if((t_mon+1) < 10)
		sprintf(month, "0%d", t_mon+1);
	else
		sprintf(month, "%d", t_mon+1);

	if(t_days < 10)
		sprintf(days, "0%d", t_days);
	else
		sprintf(days, "%d", t_days);

	if(t_hour < 10)
		sprintf(hour, "0%d", t_hour);
	else
		sprintf(hour, "%d", t_hour);
	
	if(t_min < 10)
		sprintf(min, "0%d", t_min);
	else
		sprintf(min, "%d", t_min);

	if(t_sec < 10)
		sprintf(sec, "0%d", t_sec);
	else
		sprintf(sec, "%d", t_sec);

	sprintf(_output, "%d-%s-%s %s:%s:%s", t_year+1900, month, days, hour, min, sec);
}
