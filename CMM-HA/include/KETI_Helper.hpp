#pragma once
#include <iomanip>
#include <sstream>
#include <string>
#include <chrono>
#include <ctime>

using namespace std;

namespace KETI_HELPER 
{
    string ToString(const chrono::time_point<chrono::system_clock>& haTime);
}