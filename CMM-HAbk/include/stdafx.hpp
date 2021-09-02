
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
#include <cpprest/http_client.h> // @@@@@@
#include <time.h> // @@@@@@@

using namespace web::http::client; // @@@@@@
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