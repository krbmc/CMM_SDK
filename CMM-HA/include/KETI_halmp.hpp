#pragma once
#include <chrono>
#include <mutex>

#include <iostream>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include "KETIhaNetService.hpp"
#include"KETIHA.hpp"
#include"KETIHA_Sync.hpp"
#include"KETI_Switch.hpp"
using namespace std;
using namespace KETIHA_NSP;
/**
 * @brief KETI HA
 * @param m_Status 모든 기준은 Primary기준으로 작성 Priamary 가 Active 일경우 Active  Standby일경우 Secondery가 Active
 * 
 */
class KETI_halmp :public KETIHA
{
private:
    
    KETI_halmp(const KETI_halmp&) = delete;
    KETI_halmp(const KETI_halmp&&) = delete;
    

    KETI_halmp& operator=(const KETI_halmp&) = delete;
    KETI_halmp& operator=(const KETI_halmp&&) = delete;
    KETIhaNetService m_RestService;
    KETIHA_Sync m_PrimarySyncer;
    KETIHA_Sync m_SecondarySyncer;
    KETI_Switch *keti_switch;
    bool IsChanged;
    
    //SinbohaSync m_SecondarySyncer;
    //SinbohaBrainSplit m_BrainsplitChecker;*/


public:
	static KETI_halmp* Instance();
    KETI_halmp();
	~KETI_halmp();
    KETIhaError Initialize(
        const string& PeerPrimaryAddress,
        const string& PeerSecondaryAddress,
        int PeerPort,
        int Port,
        std::chrono::milliseconds NetworkTimeout,
        std::chrono::seconds Heartbeat,
        std::chrono::milliseconds SwitchTimeout,
        bool Debug = false) override;

    KETIhaStatus IsSwtich;
    KETIhaError Release() override;
    void UnRegisterCallback() override;
    void RegisterCallback(std::shared_ptr<KETICallbackIf> Callback) override;
    virtual KETIhaError Switch() override;
    KETIhaStatus GetHaStatus() override;
    KETIhaError SyncData(const std::string& Data) override;

    void Query(chrono::system_clock::time_point& ChangeTime, KETIhaStatus& Status);

    KETIhaError RecvData(const string& Data);
    void TryActivate(bool PeerAllowActivate);
    chrono::system_clock::time_point PeerLiveTime();
    
};

