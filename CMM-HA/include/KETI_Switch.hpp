#pragma once
#include <future>
#include <atomic>
#include <memory>
#include <condition_variable>
#include"KETIHA.hpp"
#include"KETIhaNetService.hpp"
using namespace std;
using namespace KETIHA_NSP;

class KETI_Switch
{
	string PeerPrimaryAddress;
    string PeerSecondaryAddress;
    KETIhaStatus *IsSwitch;
    int PeerPort;
    int SecondaryPort;
    bool *IsChanged;
    
public:
	KETI_Switch(string PeerPrimaryAddress,string PeerSecondaryAddress,int PeerPort,int SecondaryPort,KETIhaStatus *IsSwitch,bool *IsChanged)
    {
        this->PeerPort=PeerPort;
        this->SecondaryPort=SecondaryPort;
        this->PeerSecondaryAddress=PeerSecondaryAddress;
        this->PeerPrimaryAddress=PeerPrimaryAddress;
        this->IsSwitch=IsSwitch;
        this->IsChanged=IsChanged;

        cout << "start REST service" << endl;
        string uri = "https://";
        try
        {
            uri = uri + PeerPrimaryAddress + ":" + to_string(PeerPort);
            json::value obj, returnobj;
            obj["PeerPrimaryAddress"] = json::value::string(PeerPrimaryAddress);
            obj["PrimaryPort"] = json::value::number(PeerPort);
            obj["PeerSecondaryAddress"] = json::value::string(PeerSecondaryAddress);
            obj["SecondPort"] = json::value::number(SecondaryPort);
            obj["Enabled"] = json::value::boolean(true);
            returnobj = heart_request("/CMM_HA", uri, obj);
        }
        catch (...)
        {
            log(info) << "PeerPrimary rest Init error";
        }

        uri = "https://";
        try
        {
            uri = uri + PeerSecondaryAddress + ":" + to_string(SecondaryPort);
            json::value obj, returnobj;
            obj["PeerPrimaryAddress"] = json::value::string(PeerPrimaryAddress);
            obj["PrimaryPort"] = json::value::number(PeerPort);
            obj["PeerSecondaryAddress"] = json::value::string(PeerSecondaryAddress);
            obj["SecondPort"] = json::value::number(SecondaryPort);
            obj["Enabled"] = json::value::boolean(false);
            returnobj = heart_request("/CMM_HA", uri, obj);
        }
        catch (...)
        {
            log(info) << "PeerSecondary rest Init error";
        }
    }

    void TryActiveStandbyMode(bool origin, bool IsActived);


	~KETI_Switch();
};

