#pragma once
#include <future>
#include <atomic>

#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include "KETIHA.hpp"
#include "handler.hpp"
using namespace std;
using namespace KETIHA_NSP;

class KETIhaNetService
{
public:
    KETIhaNetService();
    ~KETIhaNetService();

    KETIhaError Start(int Port, int TimeOut);
    KETIhaError Stop();
    void RESTService(void *data);
private:
    
    thread  *m_thread;
    //boost::shared_ptr<TServer> m_Server;
    atomic<bool> m_Quit;
    mutex m_Lock;
};
