#include "KETIHA_Sync.hpp"
#include "KETI_halmp.hpp"
#include "handler.hpp"

KETIHA_Sync::KETIHA_Sync(string PeerPrimaryAddress, string PeerSecondaryAddress, int PeerPort, int SecondaryPort, KETIhaStatus *IsSwitch, bool *IsChanged) : m_Quit(true), m_SwitchTimeout(2), m_Heartbeat(chrono::seconds(50))
{
    this->cur_count = 0;
    this->OriginEnabled = true;
    this->SubEnabled = false;
    (this->IsSwitch)=IsSwitch;
    this->PeerPort = PeerPort;
    this->SecondaryPort = SecondaryPort;
    this->PeerSecondaryAddress = PeerSecondaryAddress;
    this->PeerPrimaryAddress = PeerPrimaryAddress;
    this->IsSwitch = IsSwitch;
    this->IsChanged = IsChanged;
}

KETIHA_Sync::~KETIHA_Sync()
{
}
KETIhaError KETIHA_Sync::Start(std::chrono::milliseconds NetworkTimeout, std::chrono::seconds Heartbeat, bool IsActive,
                               KETIhaStatus *IsSwitch, bool *IsChanged, KETI_Switch *keti_switch)
{
    m_Heartbeat = Heartbeat;
    m_NetworkTimeout = NetworkTimeout;
    this->IsSwitch = IsSwitch;
    max_count = m_SwitchTimeout.count();
    this->IsActive = IsActive;
    IsOrigin = IsActive;
    this->IsChanged = IsChanged;
    this->keti_switch = keti_switch;
    if (this->PeerPrimaryAddress.empty())
    {
        cout << "Peer address is not available:" << this->PeerPrimaryAddress << endl;
        return KETIhaError::HA_ERROR_FAIL;
    }

    unique_lock<mutex> _(m_Lock);

    if (!m_Quit.load())
    {
        cout << "Heartbeat via {} is running." << this->PeerPrimaryAddress << endl;
        return KETIhaError::HA_ERROR_FAIL;
    }
    cout << "Starting heartbeat via {}." << this->PeerPrimaryAddress << endl;
    m_Quit.store(false);

    //m_Network = make_shared<SinbohaNetClient>();

    try
    {
        //m_Future = std::async(launch::async, &SinbohaSync::SyncHeartbeat, this);
    }
    catch (std::system_error e)
    {
        log(info) << "Fail to start heartbeat via {} : {}.", this->PeerPrimaryAddress, e.what();
        m_Quit.store(true);
        return KETIhaError::HA_ERROR_FAIL;
    }
    catch (std::bad_alloc e)
    {
        log(info) << ("Fail to start heartbeat via {} : {}.", this->PeerPrimaryAddress, e.what());
        m_Quit.store(true);
        return KETIhaError::HA_ERROR_FAIL;
    }
    catch (...)
    {
        log(info) << ("Fail to start heartbeat via {} : Unknown exception.", this->PeerPrimaryAddress);
        m_Quit.store(true);
        return KETIhaError::HA_ERROR_FAIL;
    }
    Sync_Heartbit sy;
    sy.Heartbeat = m_Heartbeat;
    sy.NetworkTimeout = m_NetworkTimeout;
    t_SyncHeartbeat[0] = new std::thread([&]()
                                         { SyncHeartbeat(&sy); });

                                        
    log(info) << "t_SyncHeartbeat 생성";
    t_SyncHeartbeat[3] = new std::thread([&]()
                                         { Thread_File_Sync(); });

                                         
    return KETIhaError::HA_ERROR_OK;
}

KETIhaError KETIHA_Sync::Stop()
{
    {
        unique_lock<mutex> _(m_Lock);

        if (m_Quit.load())
        {
            log(info) << ("Heartbeat via {} is stopped.", this->PeerPrimaryAddress);
            return KETIhaError::HA_ERROR_FAIL;
        }

        log(info) << ("Stopping heartbeat via {}.", this->PeerPrimaryAddress);

        m_Quit.store(true);
        m_Cond.notify_one();
    }
    for(std::thread *th:t_SyncHeartbeat)
        th->detach();
    //m_Network->Release();

    log(info) << ("Stopped heartbeat via {}.", this->PeerPrimaryAddress);
    return KETIhaError::HA_ERROR_OK;
}

KETIhaError KETIHA_Sync::SyncData(const KETIhaStatus sw)
{
    unique_lock<mutex> lk(m_Lock);
    string command = "FileSync";
    string uri = "http://";
    log(info) << "FileSync";
    json::value obj, returnobj;
    obj["Cmd"]=json::value::string(command);
    string sendcmd ="sshpass -p "+ passwd+" scp -r /redfish/ "+id+"@";
    if (*this->IsSwitch == KETIhaStatus::HA_STATUS_ACTIVE)
    {
        uri = uri + this->PeerPrimaryAddress + ":" + to_string(this->PeerPort);
        sendcmd += this->PeerSecondaryAddress+ ":/";
    }
    else if (*this->IsSwitch==KETIhaStatus::HA_STATUS_STANDBY)
    {
        uri = uri + this->PeerSecondaryAddress + ":" + to_string(this->SecondaryPort);
        sendcmd+=this->PeerPrimaryAddress + ":/";
    }
    else if (*this->IsSwitch==KETIhaStatus::HA_STATUS_PENDING)
    {
        log(info) << "KETIHA Pendding" << endl;
        return KETIhaError::HA_ERROR_FAIL;
    }
    log(info) << "sendcmd=" << sendcmd << endl;
    try
    {
        obj["Path"]=json::value::string(sendcmd);
        returnobj = heart_request("/Command", uri, obj);
        log(info) << "Status=" << returnobj.serialize() << endl;

        this->OriginEnabled = true;
    }
    catch (...)
    {

        log(info) << "orgin not working " << endl;
    }
    return KETIhaError::HA_ERROR_OK;
}
void KETIHA_Sync::Thread_File_Sync()
{
    int count=0;
    //log(info) << "Thread_File_Sync 생성";
    for (;;)
    {
        log(info) << "Thread_File_Sync "<<count<<"COUNT"<<max_count;
        count++;
        this_thread::sleep_for(this->m_Heartbeat);
        if(count>this->max_count){
            SyncData();
            count=0;
        }
        
    }
}
void KETIHA_Sync::Thread_Origin_Heart()
{
    log(info) << "Thread_Origin_Heart 생성";
    string uri = "http://";
    uri = uri + this->PeerPrimaryAddress + ":" + to_string(this->PeerPort);
    json::value obj, returnobj;
    int count=0;

    for (;;)
    {
        this_thread::sleep_for(this->m_Heartbeat);
        while (*IsChanged)
        {
            this_thread::sleep_for(this->m_Heartbeat);
        }

        try
        {
            obj[U("Cycle")] = json::value::number(this->max_count);
           // log(info) << "uri ==" << uri;
            returnobj = heart_request("/Heartbeat", uri, obj);
            //log(info) << "Status=" << returnobj.serialize() << endl;
            count = 0;
            this->OriginEnabled = true;
        }
        catch (...)
        {
            count += 1;
            //log(info) << "orgin not working " << endl;
        }
        //log(info) <<"count = "<<count<<" max_count="<<max_count;
        if (count > this->max_count)
        {
            this->OriginEnabled = false;
        }
        if (!this->OriginEnabled && this->SubEnabled)
        {
            //log(info) << "origin switching " << endl;
            this->keti_switch->TryActiveStandbyMode(true, false);
        }
        else if(!this->OriginEnabled && !this->SubEnabled)
            *this->IsSwitch=HA_STATUS_PENDING;
    }
}

void KETIHA_Sync::Thread_Sub_Heart()
{
    log(info) << "Thread_Sub_Heart 생성";
    string uri = "http://";
    uri = uri + this->PeerSecondaryAddress + ":" + to_string(this->SecondaryPort);
    json::value obj, returnobj;
    int count =0;
    for (;;)
    {
        // if (*this->IsSwitch == HA_STATUS_ACTIVE)
        //     //log(info) << "Origin Activate";
        // else
        //     log(info) << "sub Activate";
        this_thread::sleep_for(this->m_Heartbeat);
        while (*IsChanged)
        {
            //cout << "change 대기" << endl;
            this_thread::sleep_for(this->m_Heartbeat);
        }

        try
        {
            obj[U("Cycle")] = json::value::number(this->max_count);
            //log(info) << "uri ==" << uri;
            returnobj = heart_request("/Heartbeat", uri, obj);
            //log(info) << "status=" << returnobj.serialize() << endl;
            count = 0;
            this->SubEnabled = true;
        }
        catch (...)
        {
            count += 1;
            log(info) << "sub not working " << endl;
        }
        if (count > this->max_count)
        {
            this->SubEnabled = false;
        }
        if (this->OriginEnabled && !this->SubEnabled)
        {
            //log(info) << "sub switching " << endl;
            this->keti_switch->TryActiveStandbyMode(true, true);
        }
        else if(!this->OriginEnabled && !this->SubEnabled)
            *this->IsSwitch=HA_STATUS_PENDING;
        
    }
}
void KETIHA_Sync::SyncHeartbeat(Sync_Heartbit *data)
{
    t_SyncHeartbeat[1] = new std::thread([&]()
                                         { this->Thread_Origin_Heart(); });
    t_SyncHeartbeat[2] = new std::thread([&]()
                                         { this->Thread_Sub_Heart(); });
}
