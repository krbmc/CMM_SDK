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
        string uri = "http://";
        try
        {
            uri = uri + PeerPrimaryAddress + ":" + to_string(PeerPort);
            json::value obj, returnobj;
            obj[U("PeerPrimaryAddress")] = json::value::string(U(PeerPrimaryAddress));
            obj[U("PrimaryPort")] = json::value::number(PeerPort);
            obj[U("PeerSecondaryAddress")] = json::value::string(U(PeerSecondaryAddress));
            obj[U("SecondPort")] = json::value::number(SecondaryPort);
            obj[U("Enabled")] = json::value::boolean(true);
            obj[U("Origin")]= json::value::boolean(true);
            returnobj = heart_request("/CMM_HA", uri, obj);
            log(info)<<"Primary CMM_SDK 전송";
        }
        catch (...)
        {
            log(info) << "PeerPrimary rest Init error";
        }

        uri = "http://";
        try
        {
            uri = uri + PeerSecondaryAddress + ":" + to_string(SecondaryPort);
            json::value obj, returnobj;
            obj[U("PeerPrimaryAddress")] = json::value::string(U(PeerPrimaryAddress));
            obj[U("PrimaryPort")] = json::value::number(PeerPort);
            obj[U("PeerSecondaryAddress")] = json::value::string(U(PeerSecondaryAddress));
            obj[U("SecondPort")] = json::value::number(SecondaryPort);
            obj[U("Enabled")] = json::value::boolean(false);
            obj[U("Origin")]= json::value::boolean(false);
            returnobj = heart_request("/CMM_HA", uri, obj);
            log(info)<<"Second CMM_SDK 전송";
        }
        catch (...)
        {
            log(info) << "PeerSecondary rest Init error";
        }
    }

    bool TryActiveStandbyMode(bool origin, bool IsActived);


	~KETI_Switch();
};

