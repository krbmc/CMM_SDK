#include "KETIHA_Sync.hpp"
#include "KETI_halmp.hpp"
#include "handler.hpp"
KETIHA_Sync::KETIHA_Sync() : m_Quit(true), m_SwitchTimeout(3), m_Heartbeat(chrono::seconds(50))
{
    this->cur_count = 0;
}

KETIHA_Sync::~KETIHA_Sync()
{
}

KETIhaError KETIHA_Sync::Start(const std::string &PeerAddress, int port,
                               std::chrono::milliseconds NetworkTimeout, std::chrono::seconds Heartbeat, bool IsActive,
                               KETIhaStatus *IsSwitch, bool *IsChanged, KETI_Switch *keti_switch)
{
    m_Heartbeat = Heartbeat;
    m_PeerAddress = PeerAddress;
    m_PeerPort = port;
    m_NetworkTimeout = NetworkTimeout;
    this->IsSwitch = IsSwitch;
    max_count = m_SwitchTimeout.count();
    this->IsActive = IsActive;
    IsOrigin = IsActive;
    this->IsChanged = IsChanged;
    this->keti_switch = keti_switch;
    if (PeerAddress.empty())
    {
        cout << "Peer address is not available:" << PeerAddress << endl;
        return KETIhaError::HA_ERROR_FAIL;
    }

    unique_lock<mutex> _(m_Lock);

    if (!m_Quit.load())
    {
        cout << "Heartbeat via {} is running." << PeerAddress << endl;
        return KETIhaError::HA_ERROR_FAIL;
    }
    cout << "Starting heartbeat via {}." << PeerAddress << endl;
    m_Quit.store(false);

    //m_Network = make_shared<SinbohaNetClient>();

    try
    {
        //m_Future = std::async(launch::async, &SinbohaSync::SyncHeartbeat, this);
    }
    catch (std::system_error e)
    {
        log(info) << "Fail to start heartbeat via {} : {}.", PeerAddress, e.what();
        m_Quit.store(true);
        return KETIhaError::HA_ERROR_FAIL;
    }
    catch (std::bad_alloc e)
    {
        log(info) << ("Fail to start heartbeat via {} : {}.", PeerAddress, e.what());
        m_Quit.store(true);
        return KETIhaError::HA_ERROR_FAIL;
    }
    catch (...)
    {
        log(info) << ("Fail to start heartbeat via {} : Unknown exception.", PeerAddress);
        m_Quit.store(true);
        return KETIhaError::HA_ERROR_FAIL;
    }
    Sync_Heartbit sy;
    sy.Heartbeat = m_Heartbeat;
    sy.NetworkTimeout = m_NetworkTimeout;
    t_SyncHeartbeat = new std::thread([&]()
                                      { SyncHeartbeat(&sy); });
    log(info) << "t_SyncHeartbeat 생성";
    return KETIhaError::HA_ERROR_OK;
}

KETIhaError KETIHA_Sync::Stop()
{
    {
        unique_lock<mutex> _(m_Lock);

        if (m_Quit.load())
        {
            log(info) << ("Heartbeat via {} is stopped.", m_PeerAddress);
            ;
            return KETIhaError::HA_ERROR_FAIL;
        }

        log(info) << ("Stopping heartbeat via {}.", m_PeerAddress);

        m_Quit.store(true);
        m_Cond.notify_one();
    }

    t_SyncHeartbeat->detach();

    //m_Network->Release();

    log(info) << ("Stopped heartbeat via {}.", m_PeerAddress);
    return KETIhaError::HA_ERROR_OK;
}

KETIhaError KETIHA_Sync::SyncData(const string &Data)
{
    unique_lock<mutex> lk(m_Lock);

    /*   if (m_Network)
    {
        return m_Network->SyncData(Data);
    }*/

    //spdlog::default_logger()->error("Network is not available.");
    return KETIhaError::HA_ERROR_FAIL;
}

void KETIHA_Sync::SyncHeartbeat(Sync_Heartbit *data)
{
    for (;;)
    {
        // while(*IsChanged)
        // {
        //     cout<<"change 대기"<<endl;
        //     this_thread::sleep_for(this->m_Heartbeat);

        // }
        string uri = "https://";
        uri = uri + this->m_PeerAddress + ":" + to_string(this->m_PeerPort);
        json::value obj, returnobj;
        this_thread::sleep_for(this->m_Heartbeat);
        // log(info) <<"cur_count ="<<cur_count;
        //max test 3
        //Active->Standby 전환

        if (cur_count > this->max_count || *this->IsSwitch == HA_STATUS_FORCE_ACTIVE || *this->IsSwitch == HA_STATUS_FORCE_STANDBY)
        {
            if (this->IsOrigin == true)
            {
                if (*this->IsSwitch == KETIhaStatus::HA_STATUS_ACTIVE && this->IsActive == true)
                {
                    this->IsActive = false;
                    cur_count = 0;
                    json::value swap;
                    swap["Switch"] = json::value::string("yes");
                    try
                    {
                        returnobj = heart_request("/HAswitch", uri, obj);
                        if (returnobj["IsSwitch"].as_string() == "yes")
                            break;
                    }
                    catch (...)
                    {
                        log(info) << "SyncHeartbeat : Error  json 요청 옲수행할 수 없음" << endl;
                    }
                    log(info) << "Origin Act=Active->standby 변환완료" << endl;
                    *IsChanged = true;
                    *this->IsSwitch = KETIhaStatus::HA_STATUS_STANDBY;
                }
                else if (*this->IsSwitch == KETIhaStatus::HA_STATUS_STANDBY && this->IsActive == false || *this->IsSwitch == HA_STATUS_FORCE_ACTIVE)
                {
                    this->keti_switch->TryActiveStandbyMode(this->IsOrigin, true);
                    this->IsActive = true;
                    cur_count = 0;
                    try
                    {
                        returnobj = heart_request("/HAswitch", uri, obj);
                    }
                    catch (...)
                    {
                        log(info) << "SyncHeartbeat : Error  json 요청 옲수행할 수 없음" << endl;
                    }

                    if (returnobj["IsSwitch"].as_string() == "yes")
                        break;
                    log(info) << "Origin Act=standby->active 변환완료" << endl;
                    *IsChanged = true;
                    *this->IsSwitch = KETIhaStatus::HA_STATUS_ACTIVE;
                }
            }
            else
            {
                if (*this->IsSwitch == KETIhaStatus::HA_STATUS_STANDBY && this->IsActive == false || *this->IsSwitch == HA_STATUS_FORCE_STANDBY)
                {
                    this->keti_switch->TryActiveStandbyMode(this->IsOrigin, false);
                    this->IsActive = true;
                    cur_count = 0;
                    log(info) << "sub Stan=standby->Active변환완료" << endl;
                    *IsChanged = true;
                    *this->IsSwitch = KETIhaStatus::HA_STATUS_STANDBY;
                }
                else if (*this->IsSwitch == KETIhaStatus::HA_STATUS_ACTIVE && this->IsActive == true)
                {
                    this->IsActive = false;
                    cur_count = 0;
                    log(info) << "sub Act=active->standby변환완료" << endl;
                    *IsChanged = true;
                    *this->IsSwitch = KETIhaStatus::HA_STATUS_ACTIVE;
                }
            }
        }
        try
        {
            obj[U("Cycle")] = json::value::string("asd");

            returnobj = heart_request("/Heartbeat", uri, obj);
            // log(info) << "Result="<<returnobj.serialize() << endl;
            cout << "cur_count ==0" << endl;
            cur_count = 0;
        }
        catch (...)
        {
            cur_count += 1;
            if (this->IsOrigin)
            {
                if (this->IsActive)
                    log(info) << "orgin Active" << endl;
                else
                    log(info) << "origin Standby" << endl;
            }
            else
            {
                if (this->IsActive)
                    log(info) << "sub Active" << endl;
                else
                    log(info) << "sub Standby" << endl;
            }
        }
        //cout<<returnobj.serialize().c_str()<<"=cstr"<<endl;
    }
}
