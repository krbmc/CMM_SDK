#include"KETI_Switch.hpp"

/**
 * @brief Active-Standby Switch Method 
 * @param origin Active Server == true, Standby Server == false
 * @param IsActived Active Server Status
 * @author Kicheol Park
 */
bool KETI_Switch::TryActiveStandbyMode(bool origin, bool IsActived)
{
    *this->IsChanged = true;
    
    json::value obj, primreturnobj, secreturnobj;
    
    // Server Active Status에 따라 IsSwitch Status 변경.
    IsActived ? *this->IsSwitch=KETIhaStatus::HA_STATUS_ACTIVE : *this->IsSwitch=KETIhaStatus::HA_STATUS_STANDBY; 
    IsActived ? obj[U("IsSwitch")] = json::value::string(U("Active")) : obj[U("IsSwitch")] = json::value::string(U("Standby"));

    string primuri = "http://"+PeerPrimaryAddress + ":" + to_string(this->PeerPort);
    string secmuri = "http://"+this->PeerSecondaryAddress + ":" + to_string(this->SecondaryPort);

    int count = 5;
    if (origin)
    {
        // count (5)번 요청
        for (int i = 0;; i++)
        {
            try
            {
                // StandbyServer로 HA Switch requset 전송
                primreturnobj = heart_request("/HAswitch", secmuri, obj);
                log(info) << "Result=" << primreturnobj.serialize() << endl;

                // HA Switch Complete
                if (primreturnobj["Result"].as_bool())
                {
                    string oldPrimary = this->PeerPrimaryAddress;
                    int oldPrimaryPort = this->PeerPort;

                    this->PeerPrimaryAddress = this->PeerSecondaryAddress;
                    this->PeerPort = this->SecondaryPort;
                    this->PeerSecondaryAddress = oldPrimary;
                    this->SecondaryPort = oldPrimaryPort;
                    
                    *this->IsSwitch == KETIhaStatus::HA_STATUS_ACTIVE ? *this->IsSwitch = KETIhaStatus::HA_STATUS_STANDBY : *this->IsSwitch = KETIhaStatus::HA_STATUS_ACTIVE;
                    *this->IsChanged = false;
                     
                    log(info) << "Active-Standby Switching Complete";
                    return true;
                }
                sleep(1);
                continue;
            }
            catch (...)
            {
                // count (5) 만큼의 시도를 했음에도 안되는 경우 return false
                if (i > count) {
                    *this->IsChanged = false;
                    return false;
                }
                log(error) << "Active-Standby Switching Fail " << i <<"/" << count;
            }
        }
    }
    //구상중
    else
    {
    }
}