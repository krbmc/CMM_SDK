#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <string>
#include<fstream>

#define _CRT_SECURE_NO_WARNINGS
#include <boost/serialization/export.hpp>
#include <unordered_map>
#include <boost/serialization/version.hpp>
#include<boost/serialization/boost_array.hpp>
#include <boost/serialization/library_version_type.hpp>
#include<boost/serialization/boost_unordered_map.hpp>
#include <map>
#include <sstream>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/unordered_set.hpp>


#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/valarray.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>

#include <chrono>

using namespace std;
using boost::asio::ip::tcp;
using boost::serialization::access;
using boost::archive::detail::oserializer;

struct MyData

{

    vector<string> vec;

    int num;

    double pi;

private: //여기가 시리얼라이즈 처리의 실장
    friend class boost::serialization::access;

    template <class Archive>

    void serialize(Archive &ar, unsigned int ver)

    {

        ar &vec;

        ar &num;

        ar &pi;
    }
};

class TaskServer
{
private:
    int port;
    unordered_map<string, int> g_record;

public:
    TaskServer(int port)
    {

        MyData data;

        data.num = 100;

        data.pi = 3.14;

        data.vec.push_back("Hello");

        data.vec.push_back("World");
        ofstream file("save.dat");

        boost::archive::text_oarchive oa(file);

        oa << (MyData &)data;

        // this->port = port;
        // g_record["1"] = 4;
        // g_record["2"] = 5;
        // std::stringstream ss;

        // std::unordered_map<int, std::unordered_set<int>> s, out;
        // s.emplace(0, std::unordered_set<int>{9, 19});
        // boost::archive::text_oarchive oarch(ss);
        // cout<<oarch.get_library_version()<<endl;
        // //oarch.operator<<(s);
        // oarch << s;
        // // boost::archive::text_iarchive iarch(ss);
        // // iarch >> out;
    }
    ~TaskServer();
    void start();
};
