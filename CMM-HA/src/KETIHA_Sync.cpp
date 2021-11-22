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
        log(info) << "origin not working " << endl;
    }
    return KETIhaError::HA_ERROR_OK;
}
void KETIHA_Sync::Thread_File_Sync()
{
    int count=0;
    //log(info) << "Thread_File_Sync 생성";
    for (;;)
    {
        //log(info) << "Thread_File_Sync "<<count<<"COUNT"<<max_count;
        count++;
        this_thread::sleep_for(this->m_Heartbeat);
        if(count>this->max_count){
            log(info) << "Thread_File_Sync "<<count<<"COUNT"<<max_count;
            SyncData();
            count=0;
        }
        
    }
}
void KETIHA_Sync::Thread_Origin_Heart()
{
    log(info) << "Thread_Origin_Heart 생성";
    string const_uri = "http://";
    string uri = const_uri + this->PeerPrimaryAddress + ":" + to_string(this->PeerPort);
    json::value obj, returnobj;
    int count=0;

    for (;;)
    {
        // hearbeat에 변화가 생길때까지 binding 
        this_thread::sleep_for(this->m_Heartbeat);
        while (*IsChanged)
        {
            this_thread::sleep_for(this->m_Heartbeat);
        }

        // heartbeat의 response가 오지 않을 경우, max_count 만큼 우선 기다림. 
        try
        {
            obj[U("Cycle")] = json::value::number(this->max_count);
            returnobj = heart_request("/Heartbeat", uri, obj);
            count = 0;
            this->OriginEnabled = true;
        }
        catch (...)
        {
            count += 1;
            log(warning) << "[Heartbeat Response] Active : " << uri << " is not working";
            if (count <= this->max_count)
                log(info) << "Waiting.. " << count << "/" << this->max_count;
        }
        
        // max_count 만큼 기다렸지만 서버가 재가동되지 않았고, Standby Server가 정상 작동 중일 때, Switch. Standby Server또한 작동중이지 않다면, Pending
        if (count > this->max_count)
        {
            this->OriginEnabled = false;
        }
        if (!this->OriginEnabled && this->SubEnabled)
        {
            // **DIRTY CODE**
            // KETI_Sync 객체는 KETI_Switch 객체를 포함하고 있음.
            // 또한 KETI_Switch의 모든 변수를 중복으로 가지고 있음.
            // => Switch 수행 시, 두 객체의 중복된 변수를 수정하는 상황 존재.
            // => 구조 최적화 필요할 듯
            if (this->keti_switch->TryActiveStandbyMode(true, false)){
                string oldPrimary = this->PeerPrimaryAddress;
                int oldPrimaryPort = this->PeerPort;

                this->PeerPrimaryAddress = this->PeerSecondaryAddress;
                this->PeerPort = this->SecondaryPort;
                this->PeerSecondaryAddress = oldPrimary;
                this->SecondaryPort = oldPrimaryPort;
                
                *this->IsSwitch == KETIhaStatus::HA_STATUS_ACTIVE ? *this->IsSwitch = KETIhaStatus::HA_STATUS_STANDBY : *this->IsSwitch = KETIhaStatus::HA_STATUS_ACTIVE;
                this->OriginEnabled = true;
                this->SubEnabled = false;
                *this->IsChanged = true;
                
                uri = const_uri + this->PeerPrimaryAddress + ":" + to_string(this->PeerPort);
            }
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
            log(info) << "[Heartbeat Response] Standby : " << uri << " is not working" << endl;
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
