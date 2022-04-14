#ifndef _STDAFX_HPP_
#define _STDAFX_HPP_

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <random>
#include <functional>
#include <set>
#include <thread>
#include <sstream>

#include <unistd.h>
#include <dirent.h>
#include <time.h> 
#include <sys/types.h> 
#include <sys/stat.h> 
#include <limits.h>

#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

// default port define
#define DEFAULT_SNMP_PORT 161
#define DEFAULT_IPMI_PORT 623
#define DEFAULT_NTP_PORT 123
#define DEFAULT_KVMIP_PORT 5288
#define DEFAULT_HTTPS_PORT 443
#define DEFAULT_HTTP_PORT 80
#define DEFAULT_VIRTUAL_MEDIA_PORT 3900
#define DEFAULT_SSH_PORT 22
#define DEFAULT_LDAP_PORT 636
#define DEFAULT_AD_PORT 389
#define DEFAULT_SYSLOG_PORT 514
#define DEFAULT_RADIUS_PORT 1812

// default conf file path define
#define DHCPV4_CONF "/etc/dhcp/dhcpd.conf"
#define DHCPV6_CONF "/etc/dhcp/dhcpd6.conf"
#define DNS_CONF "/etc/resolv.conf"
#define VLAN_CONF "/proc/net/vlan/config"

using namespace std;
using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;
using namespace web::http::client; 
using namespace utility;
// using namespace concurrency::streams;

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;
using namespace logging::trivial;

#ifndef __has_include
  static_assert(false, "__has_include not supported");
#else
#  if __cplusplus >= 201703L && __has_include(<filesystem>)
#    include <filesystem>
     namespace fs = std::filesystem;
#  elif __has_include(<experimental/filesystem>)
#    include <experimental/filesystem>
     namespace fs = std::experimental::filesystem;
#  elif __has_include(<boost/filesystem.hpp>)
#    include <boost/filesystem.hpp>
     namespace fs = boost::filesystem;
#  endif
#endif

extern src::severity_logger<severity_level> g_logger;

/**
 * @brief Logging macro
 */
#define log(level) BOOST_LOG_SEV(g_logger, level)

/**
 * @brief Function of utilities
 */
vector<string> string_split(const string _string, char _delimiter);
string make_path(vector<string> tokens);
bool comp(const string &s1, const string &s2);
void timer(boost::asio::deadline_timer* _timer, unsigned int *_remain_expires_time);
string generate_token(const int len);

/**
 * @brief allocate number 
 * @authors 강
 * */

enum ALLOCATE_NUM
{
     ALLOCATE_TASK_NUM,
     ALLOCATE_ACCOUNT_NUM,
     ALLOCATE_SESSION_NUM,
     ALLOCATE_VM_CD_NUM,
     ALLOCATE_VM_USB_NUM,
     ALLOCATE_SUBSCRIPTION_NUM,


     ALLOCATE_NUM_COUNT
};

// map<int, set<unsigned int> > numset;
// map<int, unsigned int> numset_num;

void init_numset(void);
unsigned int allocate_numset_num(int _index);
void insert_numset_num(int _index, unsigned int num);
void delete_numset_num(int _index, unsigned int num);

// unsigned int allocate_task_num(void);
// void insert_task_num(unsigned int num);
// void delete_task_num(unsigned int num);
// unsigned int allocate_account_num(void);
// void insert_account_num(unsigned int num);
// void delete_account_num(unsigned int num);
// unsigned int allocate_session_num(void);
// void insert_session_num(unsigned int num);
// void delete_session_num(unsigned int num);

string get_current_object_name(string _uri, string delimiter);
string get_parent_object_uri(string _uri, string delimiter);
bool isNumber(const string str);
bool validateIPv4(const string str);
bool validateDatetimeLocalOffset(const string str);
bool validateSeverity(const string str);
bool validateLogFacility(const string str);
bool validateDHCPv6OperatingMode(const string str);

bool validateModuleID(const string str);

string ltrim(string str);

char *get_popen_string(char *command);
string get_popen_string(string command);
string get_extracted_bmc_id_uri(string _uri);
bool check_role_privileges(string _pri);
string get_value_from_cmd_str(string cmd_str, string key);

void generate_logservice(string _res_odata, string _service_id);
void generate_logentry(string _res_odata, string _entry_id);

string generate_uuid(void);
void make_time_with_tm(tm _tm, char *_output);
// void time_function(uint8_t t_year, uint8_t t_mon, uint8_t t_days, uint8_t t_hour, uint8_t t_min, uint8_t t_sec, char *output);

/**
 * @brief Function of record
 */
bool record_is_exist(const string _uri);
bool event_is_exist(const string _uri);
json::value record_get_json(const string _uri);
bool record_load_json(void);
bool record_save_json(void);
void record_print(void);
void record_init_load(string _path);
void delete_resource(string odata_id);
void synchronize_dir_list();

/**
 * @brief Function of module_id
 */
void save_module_id(void);
void load_module_id(void);
string make_module_address(string _id); // module_id 에 대한 주소생성함수
void remove_if_exists(fs::path file);


/**
 * @brief Get the value from json key object
 * 
 * @param body json내용이 있는 곳... ~에서
 * @param key 읽어오는 Key값 ... ~을 읽어서
 * @param value 읽어온 값을 담을 변수 ... ~에 저장
 */
bool get_value_from_json_key(json::value body, string key, int& value);
bool get_value_from_json_key(json::value body, string key, unsigned int& value);
bool get_value_from_json_key(json::value body, string key, string& value);
bool get_value_from_json_key(json::value body, string key, json::value& value);
bool get_value_from_json_key(json::value body, string key, double& value);
bool get_value_from_json_key(json::value body, string key, bool& value);
#endif