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
#include <cpprest/http_listener.h>
#include <cpprest/json.h>
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
#include <boost/date_time/posix_time/posix_time.hpp>
#include <unistd.h>
#include <dirent.h>
#include <cpprest/http_client.h> // @@@@
#include <time.h> // @@@@
#include <sys/types.h> // @@@@
#include <sys/stat.h> // @@@@
#include <limits.h> // @@@@
#include <set> // @@@@

#define DEFAULT_SNMP_PORT 161
#define DEFAULT_IPMI_PORT 623
#define DEFAULT_NTP_PORT 123
#define DEFAULT_KVMIP_PORT 5288
#define DEFAULT_HTTPS_PORT 443
#define DEFAULT_HTTP_PORT 80
#define DEFAULT_VIRTUAL_MEDIA_PORT 3900
#define DEFAULT_SSH_PORT 22

using namespace web::http::client; // @@@@
using namespace std;
using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;
using namespace utility;

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

unsigned int allocate_task_num(void);
void insert_task_num(unsigned int num);
void delete_task_num(unsigned int num);
unsigned int allocate_account_num(void);
void insert_account_num(unsigned int num);
void delete_account_num(unsigned int num);
unsigned int allocate_session_num(void);
void insert_session_num(unsigned int num);
void delete_session_num(unsigned int num);

string get_current_object_name(string _uri, string delimiter);
string get_parent_object_uri(string _uri, string delimiter);
bool isNumber(const string str);


char *get_popen_string(char *command);
string get_popen_string(string command);
string get_extracted_bmc_id_uri(string _uri);
bool check_role_privileges(string _pri);

/**
 * @brief Function of record
 */
bool record_is_exist(const string _uri);
bool event_is_exist(const string _uri);
json::value record_get_json(const string _uri);
bool record_load_json(void);
bool record_save_json(void);
void record_print(void);
void record_init_load(string _path); // @@@@@

/**
 * @brief Function of module_id
 */
void save_module_id(void);
void load_module_id(void);
void remove_if_exists(fs::path file);

bool get_value_from_json_key(json::value body, string key, int& value);
bool get_value_from_json_key(json::value body, string key, string& value);
bool get_value_from_json_key(json::value body, string key, json::value& value);
bool get_value_from_json_key(json::value body, string key, double& value);
bool get_value_from_json_key(json::value body, string key, bool& value);
#endif