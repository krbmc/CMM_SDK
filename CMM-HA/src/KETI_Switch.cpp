#include"KETI_Switch.hpp"

bool KETI_Switch::TryActiveStandbyMode(bool origin, bool IsActived)
{
    *this->IsChanged = true;
    json::value obj, primreturnobj, secreturnobj;
    IsActived ? *this->IsSwitch=KETIhaStatus::HA_STATUS_ACTIVE :*this->IsSwitch=KETIhaStatus::HA_STATUS_STANDBY; 
    IsActived ? obj[U("IsSwitch")] = json::value::string(U("Active")) : obj[U("IsSwitch")] = json::value::string(U("Standby"));
    string primuri = "http://"+PeerPrimaryAddress + ":" + to_string(this->PeerPort);
    string secmuri = "http://"+this->PeerSecondaryAddress + ":" + to_string(this->SecondaryPort);
    int count = 5;
    if (origin)
    {
        for (int i = 0;; i++)
        {
            try
            {
                primreturnobj = heart_request("/HAswitch", primuri, obj);
                log(info) << "Result=" << primreturnobj.serialize() << endl;
                if (primreturnobj["Result"].as_bool())
                {
                    *this->IsChanged = false;
                    return true;
                }
                if (i > count)
                {
                    log(info) << "카운트 초과";
                    *this->IsChanged = false;
                    return false;
                }
                sleep(1);
                continue;
            }
            catch (...)
            {
                log(info) << "error : TryActiveStandbyMode json  " << endl;
                *this->IsChanged = false;
                return false;
            }
        }
    }
    //구상중
    else
    {
    }
}