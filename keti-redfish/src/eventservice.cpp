#include "eventservice.hpp"



bool check_protocol(string _protocol)
{
    if(_protocol == "Redfish" || _protocol == "SMTP")
        return true;
    
    return false;
    // schema value : OEM, Redfish, SMTP, SNMPv1, SNMPv2c, SNMPv3, SyslogRELP, SyslogTCP, SyslogTLS, SyslogUDP
}

bool check_policy(string _policy)
{
    if(_policy == "RetryForever" || _policy == "SuspendRetries" || _policy == "TerminateAfterRetries")
        return true;

    return false;
}

bool check_subscription_type(string _type)
{
    if(_type == "RedfishEvent" || _type == "OEM") // OEM 을 SMTP로 지원
        return true;

    return false;
    // schema value : OEM , RedfishEvent, SNMPInform, SNMPTrap, SSE, Syslog
}

bool check_event_type(string _type)
{
    if(_type == "Alert" || _type == "ResourceAdded" || _type == "ResourceRemoved" 
    || _type == "ResourceUpdated" || _type == "StatusChange")
        return true;

    return false;
    // schema value : Alert, MetricReport, Other, ResourceAdded, ResourceRemoved, ResourceUpdated, StatusChange
}