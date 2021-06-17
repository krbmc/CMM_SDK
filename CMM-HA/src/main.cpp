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
    auto rc = ha->Initialize("10.0.6.107", "10.0.6.1", 443, 5678,chrono::milliseconds(50), chrono::seconds(5), chrono::seconds(10), false);
    // for (int i = 0; i < 10; i++)
    // {
    //     this_thread::sleep_for(chrono::seconds(1));
    //     ha->SyncData("hello, I am " + std::to_string((int)ha->GetHaStatus()));
    // }
    while(true)
     pause();
    return 0;
}