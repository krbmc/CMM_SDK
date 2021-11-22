#ifndef EVENTSERVICE_H
#define EVENTSERVICE_H

#include "stdafx.hpp"
#include "resource.hpp"


// subscription value check
bool check_protocol(string _protocol);
bool check_policy(string _policy);
bool check_subscription_type(string _type);
bool check_event_type(string _type);


#endif