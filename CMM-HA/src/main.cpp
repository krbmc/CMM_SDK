#include <iostream>
#include <thread>
#include "KETI_halmp.hpp"
#include "KETIHA.hpp"

using namespace std;
using namespace KETIHA_NSP;
using namespace std::chrono;


int main(void)
{
    
    auto ha = GetKetihaIf();
    auto rc = ha->Initialize("10.0.6.117", "10.0.6.106",HTTPPORT , HTTPPORT,chrono::milliseconds(50), chrono::seconds(5), chrono::seconds(10), false);
    // for (int i = 0; i < 3; i++)
    // {
    //     this_thread::sleep_for(chrono::seconds(1));
    // }
    //ha->Switch();
    
    //mainserver();
    while(true)
     pause();
    return 0;
}