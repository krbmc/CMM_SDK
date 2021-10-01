#pragma once
#include <future>
#include <atomic>
#include <memory>
#include <condition_variable>
#include"KETIHA.hpp"
#include "KETI_Switch.hpp"
using namespace std;
using namespace KETIHA_NSP;
static string id="root";
static string passwd="ketilinux";
struct Sync_Heartbit{
	std::chrono::seconds Heartbeat;
	std::chrono::milliseconds NetworkTimeout;
};
class KETIHA_Sync
{
	
public:
	KETI_Switch *keti_switch;
	KETIHA_Sync(string PeerPrimaryAddress,string PeerSecondaryAddress,int PeerPort,int SecondaryPort,KETIhaStatus *IsSwitch,bool *IsChanged);
	~KETIHA_Sync();
	
	KETIhaError Start(std::chrono::milliseconds NetworkTimeout,std::chrono::seconds Heartbeat,bool IsActive, KETIhaStatus *IsSwitch,bool *IsChanged,KETI_Switch *keti_switch);
	bool SwitchedAS(bool Actived)
	{
		// this->IsActive=Actived;
		// this->cur_count=0;
		// *this->IsChanged=true;
		// if(this->IsOrigin&& IsActive)
		// 	*this->IsSwitch = KETIhaStatus::HA_STATUS_ACTIVE;
		// else if(this->IsOrigin&& !IsActive)
		// 	*this->IsSwitch = KETIhaStatus::HA_STATUS_STANDBY;
		// else if(!this->IsOrigin&& IsActive)
		// 	*this->IsSwitch = KETIhaStatus::HA_STATUS_STANDBY;	
		// else if(this->IsOrigin&& !IsActive)
		// 	*this->IsSwitch = KETIhaStatus::HA_STATUS_ACTIVE;	
	}
	KETIhaError Stop();	
	
	void Thread_Origin_Heart();
	void Thread_Sub_Heart();
	KETIhaError SyncData(const KETIhaStatus sw=KETIhaStatus::HA_STATUS_PENDING);

private:
	void SyncHeartbeat(Sync_Heartbit *data);
	void Thread_File_Sync();
	future<void> m_Future;
	condition_variable m_Cond;
	atomic<bool> m_Quit;
	bool IsActive;
	mutex m_Lock;
	thread *t_SyncHeartbeat[4];
	int max_count;
	int cur_count;
	KETIhaStatus *IsSwitch;
	bool IsOrigin;
	bool OriginEnabled;
	bool SubEnabled;
	chrono::milliseconds m_NetworkTimeout;
	chrono::seconds m_Heartbeat;
	chrono::milliseconds m_SwitchTimeout;
	string PeerPrimaryAddress;
    string PeerSecondaryAddress;
    int PeerPort;
    int SecondaryPort;
    bool *IsChanged;
};

