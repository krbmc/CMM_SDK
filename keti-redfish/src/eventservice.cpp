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


Event_Info generate_event_info(string _event_id, string _event_type, string _msg_id, vector<string> _args)
{
  Event_Info ev;
  ev.event_id = _event_id;
  ev.event_timestamp = currentDateTime();
  ev.event_type = _event_type;
  ev.message.id = _msg_id;

  string registry_odata = ODATA_MESSAGE_REGISTRY_ID;
  MessageRegistry *registry = (MessageRegistry *)g_record[registry_odata];
  int flag=0;

  for(int i=0; i<registry->messages.v_msg.size(); i++)
  {
    if(registry->messages.v_msg[i].pattern == _msg_id)
    {
      ev.message.message = registry->messages.v_msg[i].message;
      ev.message.severity = registry->messages.v_msg[i].severity;
      // ev.message.message_args = registry->messages.v_msg[i].message_args;
      ev.message.resolution = registry->messages.v_msg[i].resolution;

      if(registry->messages.v_msg[i].number_of_args != _args.size())
      {
        flag = 1;
        break;
      }
      else
        ev.message.message_args = _args;
      flag = 2;
      break;
    }
  }

  if(flag == 0)
  {
    log(warning) << "In MessageRegistry, No Information about Message ID : " << _msg_id;
  }
  else if(flag == 1)
  {
    log(warning) << "Message Args Count Incorrect";
  }
  // 이렇게 하고 message에서 message 내용 severity, message_args, resolution정도 있는데
  // 이게 messageregistry에 지정된거 가지고 다 사용하는거면 id로 레지스트리 검색해서 message, severity, args
  // resolution 가져오면 될듯?

  return ev;
}

SEL generate_sel(unsigned int _sensor_num, string _code, string _sensor_type, string _msg, string _severity)
{
  SEL sel;
  sel.sensor_number = _sensor_num;
  sel.entry_code = _code;
  sel.sensor_type = _sensor_type;
  // sel.message.id = _msg_id;
  sel.message.message = _msg;
  sel.message.severity = _severity;

  // sel event 정보를 따로받아야할거같은데? 메세지도 들어있고 severity라든가

  return sel;
}


void send_event_to_subscriber(Event_Info _ev)
{
    string col_odata = ODATA_EVENT_DESTINATION_ID;
    Collection *col = (Collection *)g_record[col_odata];

    // 구독자 목록 순회
    for(int i=0; i<col->members.size(); i++)
    {
        EventDestination *ed = (EventDestination *)(col->members[i]);

        // #1 구독 state 유효한지 검사
        if(ed->status.state != "Enabled")
            continue;
        
        // #2 구독자의 이벤트 타입목록에 발생한 이벤트 타입이 있는지 검사
        if(!event_type_exist(ed->event_types, _ev.event_type))
            continue;

        if(ed->protocol == "Redfish")
        {
            // HTTP로 전송
            // 폼을 만들고 전송할 json폼 그다음에 해당 uri에 보내보내
            Event event;
            event.events.push_back(_ev);
            shoot_redfish(ed->destination, event.get_json());
        }
        else if(ed->protocol == "SMTP")
        {
            // SMTP로 전송
        }

    }

}

bool event_type_exist(vector<string> _vector, string _type)
{
    for(int i=0; i<_vector.size(); i++)
    {
        if(_type == _vector[i])
            return true;
    }

    return false;
}

void shoot_redfish(string _destination, json::value _form)
{
    http_client client(_destination);
    http_request req(methods::POST);
    req.set_body(_form);
    http_response res;

    try{
        pplx::task<http_response> responseTask = client.request(req);
        res = responseTask.get();
        log(info) << "[Response Redfish Event From " + _destination + "] : " << res.status_code();
    }
    catch(const std::exception& e){
        std::cerr << e.what() << '\n';
    }
}
