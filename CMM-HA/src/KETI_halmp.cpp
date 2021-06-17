#include "KETI_halmp.hpp"
#include "KETI_Helper.hpp"


KETI_halmp::KETI_halmp()
{
   
}
KETI_halmp::~KETI_halmp()
{
   
}
KETIhaError KETI_halmp::SyncData(const string& Data)
{

    // if (KETIhaStatus::HA_STATUS_ACTIVE == m_Status.Query())
    // {
    //     cout << "I am  active, ignore data synchronization." << endl;
    //     return KETIhaError::HA_ERROR_FAIL;
    // }

    // return m_PrimarySyncer.SyncData(Data);
}
KETIhaError KETI_halmp::Release()
{
    //m_BrainsplitChecker.Stop();
    //m_SecondarySyncer.Stop();
    m_PrimarySyncer.Stop();
    //m_RPCService.Stop();
    return KETIhaError::HA_ERROR_OK;
}
void KETI_halmp::UnRegisterCallback()
{
    //m_Status.UnRegisterCallback();
}
void KETI_halmp::RegisterCallback(std::shared_ptr<KETICallbackIf> Callback)
{
    //m_Status.RegisterCallback(Callback);
}
KETIhaError KETI_halmp::Switch()
{
    //return m_Status.Switch();
}
KETIhaStatus KETI_halmp::GetHaStatus()
{
    //return m_Status.Query();
}
KETI_halmp * KETI_halmp::Instance()
{
    static KETI_halmp ins;
    return &ins;
}
/**
 * @brief Query -> Rep->쿼리변경 = KETIhaStatus 변경 및 변경 시간 추가
 * 
 * @param ChangeTime 
 * @param Status 
 */
void KETI_halmp::Query(chrono::system_clock::time_point& ChangeTime, KETIhaStatus& Status)
{
    //return m_Status.Query(ChangeTime, Status);  
}

KETIhaError KETI_halmp::RecvData(const string& Data)
{
    //return m_Status.RecvData(Data);
}
void KETI_halmp::TryActivate(bool PeerAllowActivate)
{
    //MUTEX 할것
    while(!this->IsChanged)
    {
            string uri = "https://";
    try
    {
        if(PeerAllowActivate)
            keti_switch->TryActiveStandbyMode(true,true);
        else
            keti_switch->TryActiveStandbyMode(false,true);
    }
    catch (...)
    {
        log(info) << "PeerPrimary rest Init error";
    }
        this->IsChanged=false;
    }
    
    
    return;
   // return m_Status.TryActivate(PeerAllowActivate);
}
chrono::system_clock::time_point KETI_halmp::PeerLiveTime()
{
    //return m_Status.PeerLiveTime();
}

/**
 * @brief 실제 HA 를 수행하는 부분
 * 
 * @param PeerPrimaryAddress primary ip
 * @param PeerSecondaryAddress seceondery ip
 * @param PeerPort primary address
 * @param Port  secondery port
 * @param NetworkTimeout 
 * @param Heartbeat 
 * @param SwitchTimeout 
 * @param Debug true일경우 debug 수행하게 함 warning 아하만 출력함
 * @return KETIhaError 
 */
KETIhaError KETI_halmp::Initialize(
        const string& PeerPrimaryAddress,
        const string& PeerSecondaryAddress,
        int PeerPort,
        int SecondPort,
        std::chrono::milliseconds NetworkTimeout,
        std::chrono::seconds Heartbeat,
        std::chrono::milliseconds SwitchTimeout,
        bool Debug)
{
    if (Debug)
    {
        cout<<"Debug flag Setting"<<endl;
        logging::core::get()->set_filter(
        logging::trivial::severity >= logging::trivial::warning);
    }
    IsSwtich=KETIhaStatus::HA_STATUS_ACTIVE;

    keti_switch= new KETI_Switch(PeerPrimaryAddress,PeerSecondaryAddress,PeerPort,SecondPort,&IsSwtich,&IsChanged);
    if (KETIhaError::HA_ERROR_OK != m_RestService.Start(PeerPort, NetworkTimeout.count()))
    {
        cout <<("Could not start REST service.")<<endl;
        return KETIhaError::HA_ERROR_FAIL;
    }
    

    if (KETIhaError::HA_ERROR_OK != m_SecondarySyncer.Start(
        PeerSecondaryAddress,
        SecondPort,
        NetworkTimeout,
        Heartbeat,
        false,&IsSwtich,&this->IsChanged,keti_switch))
    {
        cout <<("Could not start primary heartbeat.")<<endl;
        return KETIhaError::HA_ERROR_FAIL;
    }

    if (KETIhaError::HA_ERROR_OK != m_PrimarySyncer.Start(
        PeerPrimaryAddress,
        PeerPort,
        NetworkTimeout,
        Heartbeat,
        true,&IsSwtich,&this->IsChanged,keti_switch))
    {
        
        return KETIhaError::HA_ERROR_FAIL;
    }
    cout <<("start Active heartbeat.")<<endl;
    
    cout <<("start Standby heartbeat.")<<endl;
    //TryActivate(true);

    return KETIhaError::HA_ERROR_OK;
}
        