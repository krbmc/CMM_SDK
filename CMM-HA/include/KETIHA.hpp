#pragma once
#include <chrono>
#include <string>
#include <memory>
#include<iostream>
#include<list>
#define ENTRY_PORT 808
using namespace std;

namespace KETIHA_NSP
{
    enum KETIhaError : unsigned short
    {
        HA_ERROR_OK = 0,  
        HA_ERROR_FAIL,     
    };

    enum KETIhaStatus : unsigned short
    {
        HA_STATUS_PENDING = 0,    
        HA_STATUS_ACTIVE,        
        HA_STATUS_STANDBY,
        HA_STATUS_FORCE_ACTIVE,
        HA_STATUS_FORCE_STANDBY
    };
    class KETICallbackIf
    { 
    public:
        virtual void OnStatusChange(KETIhaStatus Status) = 0;
        virtual void OnReceiveData(const std::string& Data) = 0;
    };

    /**
    * @param ActiveIP 
    * @param StandbyIP 
    * @param Port 
    * @NetworkTimeout
    * @param          Heartbeat
    * @param          SwitchTimeout
    * @param          Debug
    * @return         KETIhaError
       * @author 기철
    **/
	class KETIHA
	{  
        public:
        virtual KETIhaError Initialize(
            const string& PeerPrimaryAddress,
            const string& PeerSecondaryAddress,
            int PeerPort,
            int Port,
            std::chrono::milliseconds NetworkTimeout,
            std::chrono::seconds Heartbeat,
            std::chrono::milliseconds SwitchTimeout,
            bool Debug = false) = 0;

            virtual KETIhaError Release() = 0;

            virtual void RegisterCallback(std::shared_ptr<KETICallbackIf> Callback) = 0;

            virtual void UnRegisterCallback()=0;

            virtual KETIhaError SyncData(const std::string& Data) = 0;

            virtual KETIhaError Switch() = 0;

            virtual KETIhaStatus GetHaStatus() = 0;

        
        
            
	};

}


KETIHA_NSP::KETIHA* GetKetihaIf();