#include "ethernetinterface.hpp"



int create_socket() {
  int sockfd = 0;
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd == -1) {
    return -1;
  }
  return sockfd;
}

int get_ip_info(uint8_t index, uint8_t *ip_addr, uint8_t *netmask,
                uint8_t *gateway, uint8_t *mac_addr, int *mtu) {
  struct ifreq ifr;
  uint8_t mac_address[6];
  struct sockaddr_in *sin;
  int sockfd = create_socket();
  int i = 0;
  uint8_t dev[5] = {
      0,
  };

  sprintf(reinterpret_cast<char *>(dev), "eth%d", index);
  strncpy(ifr.ifr_name, reinterpret_cast<char *>(dev), IFNAMSIZ);

  if (ioctl(sockfd, SIOCGIFFLAGS, &ifr) < 0) {
  }
  if (ioctl(sockfd, SIOCGIFHWADDR, &ifr) < 0) {
  }

    // cout << "INNER >>>>>>>>>>>>>>>>>>>>>>>" << endl;
  for (i = 0; i < 6; i++) {
    mac_addr[i] = ifr.ifr_addr.sa_data[i];
    // printf("in mac : %02x\n", mac_addr[i]);
    // cout << "in mac : " << mac_addr[i] << endl;
  }

  if (ioctl(sockfd, SIOCGIFADDR, &ifr) >= 0) {
    sin = (struct sockaddr_in *)&ifr.ifr_addr;

    for (i = 0; i < 4; i++){
      ip_addr[i] = ifr.ifr_addr.sa_data[i + 2];
      // printf("in addr : %d\n", ip_addr[i]);
      // cout << "in addr : " << ip_addr[i] << endl;
    }
  } else {
    for (i = 0; i < 4; i++)
      ip_addr[i] = 0;
  }

  if (ioctl(sockfd, SIOCGIFNETMASK, &ifr) >= 0) {
    sin = (struct sockaddr_in *)&ifr.ifr_addr;

    for (i = 0; i < 4; i++){
      netmask[i] = ifr.ifr_addr.sa_data[i + 2];
      // printf("in netmask : %d\n", netmask[i]);
      // cout << "in netmask : " << netmask[i] << endl;
    }
  } else {
    for (i = 0; i < 4; i++)
      netmask[i] = 0;
  }

  // uint8_t *mtu_size = new uint8_t[1];
  if (ioctl(sockfd, SIOCGIFMTU, &ifr) >= 0) {
    // sin = (struct sockaddr_in *)&ifr.ifr_addr;

    *mtu = ifr.ifr_mtu;
    // printf("in mtu first : %d\n", *mtu);
    // for (i = 0; i < 4; i++){
    //   netmask[i] = ifr.ifr_addr.sa_data[i + 2];
    //   printf("in netmask : %d\n", netmask[i]);
    //   // cout << "in netmask : " << netmask[i] << endl;
    // }
  } else {
    *mtu = 0;
    // for (i = 0; i < 4; i++)
      // netmask[i] = 0;
  }
  

  get_gateway(index, gateway);
  close(sockfd);
  return 0;
}

void get_gateway(uint8_t chan, uint8_t *gateway) {
  char cmd[1000] = {0x0};
  uint8_t dev[5] = {0};
  int i;
  sprintf(reinterpret_cast<char *>(dev), "eth%d", chan);
  sprintf(cmd, "route -n | grep %s  | grep \'UG[ \t]\' | awk \'{print $2}\'",
          dev);
  FILE *fp = popen(cmd, "r");
  // cout << "in gateway : " << cmd << endl;
  char line[256] = {0x0};

  if (fgets(line, sizeof(line), fp) != NULL) {
    for (i = 0; i < 1000; i++) {
      if ((line[i] != '.') && (line[i] < '0' || line[i] > '9'))
        break;
    }
    line[i] = 0;
    inet_pton(AF_INET, line, gateway);

    // cout << "in gateway2 : " << gateway[0] << endl;
    // printf("in gateway2 : %d.%d.%d.%d\n", gateway[0], gateway[1], gateway[2], gateway[3]);
  }
  pclose(fp);
}

int set_ip(char *iface_name, char *ip_addr, char *net_mask,
           char *gateway_addr, char *mtu, char *mac) {
  struct ifreq ifr;
  struct sockaddr_in sin;
  int sockfd = create_socket();

  sin.sin_family = AF_INET;
  // Convert IP from numbers and dots to binary notation

  /* get interface name */
  strncpy(ifr.ifr_name, iface_name, IFNAMSIZ);

  /* Read interface flags */
  if (ioctl(sockfd, SIOCGIFFLAGS, &ifr) < 0) {
  }

  // If interface is down, bring it up
  if (ifr.ifr_flags | ~(IFF_UP)) {
    ifr.ifr_flags |= IFF_UP;
    if (ioctl(sockfd, SIOCSIFFLAGS, &ifr) < 0) {
    }
  }
  // Set route
  inet_pton(AF_INET, ip_addr, &sin.sin_addr);
  memcpy(&ifr.ifr_addr, &sin, sizeof(struct sockaddr));
  // Set interface address
  if (ioctl(sockfd, SIOCSIFADDR, &ifr) < 0) {
    {
    }
    perror(ifr.ifr_name);
    return -1;
  }

  inet_pton(AF_INET, net_mask, &sin.sin_addr);
  memcpy(&ifr.ifr_netmask, &sin, sizeof(struct sockaddr));
  // memcpy(&ifr.ifr_addr, &sin, sizeof(struct sockaddr));

  if (ioctl(sockfd, SIOCSIFNETMASK, &ifr) < 0) {
    {
    }
    perror(ifr.ifr_name);
    return -1;
  }

  inet_pton(AF_INET, mtu, &sin.sin_addr);
  memcpy(&ifr.ifr_mtu, &sin, sizeof(struct sockaddr));
  // memcpy(&ifr.ifr_addr, &sin, sizeof(struct sockaddr));

  if (ioctl(sockfd, SIOCGIFMTU, &ifr) < 0) {
    {
    }
    perror(ifr.ifr_name);
    return -1;
  }

  inet_pton(AF_INET, mac, &sin.sin_addr);
  memcpy(&ifr.ifr_hwaddr, &sin, sizeof(struct sockaddr));
  // memcpy(&ifr.ifr_addr, &sin, sizeof(struct sockaddr));

  if (ioctl(sockfd, SIOCGIFHWADDR, &ifr) < 0) {
    {
    }
    perror(ifr.ifr_name);
    return -1;
  }

#undef IRFFLAGS

  set_route(sockfd, gateway_addr, &sin, iface_name);

  return 0;
}

int set_route(int sockfd, char *gateway_addr, struct sockaddr_in *addr,
              char *dev_name) {

  struct rtentry route;
  struct ifreq ifr;
  int err = 0;
  memset(&route, 0, sizeof(route));
  strncpy(ifr.ifr_name, dev_name, IFNAMSIZ - 1);
  route.rt_dev = ifr.ifr_name;
  addr = (struct sockaddr_in *)&route.rt_gateway;
  addr->sin_family = AF_INET;
  addr->sin_addr.s_addr = inet_addr(gateway_addr);
  addr = (struct sockaddr_in *)&route.rt_dst;
  addr->sin_family = AF_INET;
  addr->sin_addr.s_addr = inet_addr("0.0.0.0");
  addr = (struct sockaddr_in *)&route.rt_genmask;
  addr->sin_family = AF_INET;
  addr->sin_addr.s_addr = inet_addr("0.0.0.0");
  route.rt_flags = RTF_UP | RTF_GATEWAY;
  route.rt_metric = 100;

  if ((err = ioctl(sockfd, SIOCADDRT, &route)) < 0) {
    printf("route error\n");
    return -1;
  }
  return 1;
}

// /**
//  * @brief file을 한 줄씩 읽어서 lines에 반환
//  * 
//  * @param _filename 읽을 파일 이름
//  * @param _lines 한 줄씩 읽은 결과 벡터
//  */
// void read_file_line_by_line(string _filename, vector<string> &_lines)
// {
//   ifstream read_file(_filename);
//   if(read_file.is_open())
//   {
//     while(!read_file.eof())
//     {
//         string tmp;
//         getline(read_file, tmp);
//         // if(tmp[0] == '\0')
//         //   cout << "null" << endl;
//         // if(tmp[0] == ' ')
//         //   cout << "space" << endl;
//         // if(tmp[0] == '\n')
//         //   cout << "enter" << endl;

//         // if(tmp[0] == '\0' && tmp.length() == 0)
//         //   continue;
//         // cout << tmp << endl;
//         _lines.push_back(tmp);
//     }

//     _lines.pop_back(); // 마지막 빈줄추가되는거 방지
    
//   }
//   else
//   {
//     cout << "ERROR : file is not open" << endl;
//     return ;
//   }

//   read_file.close();
// }

// /**
//  * @brief 벡터의 내용을 파일에 한줄씩 쓰기
//  * 
//  * @param _filename write할 파일이름
//  * @param _lines 쓸 내용 담긴 벡터
//  */
// void write_file_line_by_line(string _filename, vector<string> _lines)
// {
//   ofstream write_file(_filename);
//   if(write_file.is_open())
//   {
//     for(string w_line : _lines)
//     {
//       write_file << w_line;
//     }
//   }
//   else
//   {
//       cout << "ERROR : file is not open" << endl;
//       return ;
//   }
//   write_file.close();
// }

// /**
//  * @brief _str의 끝에 개행문자'\n'를 추가하고 _vec에 push_back하는 함수
//  */
// void append_newline_and_push_back(string _str, vector<string> &_vec)
// {
//   _str.append("\n");
//   _vec.push_back(_str);

//   return ;
// }


/**
 * @brief hostname변경을 위한 /etc/hosts 파일 내용 수정
 * @details /etc/hosts에서 _target을 찾아서 _new로 바꿈
 * @param _target 찾을 hostname
 * @param _new 바꿀 새로운 hostname
 */
void change_hosts_file(string _target, string _new)
{
    vector<string> lines_work, lines_write;
    lines_work.clear();
    lines_write.clear();
    read_file_line_by_line("/etc/hosts", lines_work);
    // 라인 by 라인으로 읽기 함수...

    // lines_work가지고 작업해 지지고볶고
    for(int i=0; i<lines_work.size(); i++)
    {
      string current_line = lines_work[i];
      // cout << "Current line : " << current_line << endl;
      if(current_line.find(_target) == string::npos)
      {
        append_newline_and_push_back(current_line, lines_write);
        continue;
      }

      int pos = current_line.find(_target);
      // cout << "POS : " << pos << endl;
      
      if(pos-1 < 0)
      {
        append_newline_and_push_back(current_line, lines_write);
        continue;
      }
    

      if((current_line[pos-1] == ' ' || current_line[pos-1] == '\t')
      && ((pos+_target.length()) == current_line.length()))
      {
        // cout << "ORIGIN : " << current_line << endl;
        string new_line = current_line.substr(0, pos);
        new_line.append(_new);
        append_newline_and_push_back(new_line, lines_write);
      }
      else
      {
        append_newline_and_push_back(current_line, lines_write);
        continue;
      }
    }

    // 지지고 볶은걸 다시 lines_write로 저장할거임
    // 그걸 이제 파일에 새로이 쓰면됨
    // fs::path target_file("/etc/hosts");
    // fs::remove(target_file);

    write_file_line_by_line("/etc/hosts", lines_write);
}

void change_hostname_file(string _new)
{
  vector<string> lines_work, lines_write;
  lines_work.clear();
  lines_write.clear();
  read_file_line_by_line("/etc/hostname", lines_work);

  int line_count = lines_work.size();
  if(line_count == 0)
  {
    cout << "[Error] : Hostname File is empty" << endl;
    return ;
  }
  else
  {
    // hostname 에 그냥 1줄로 write하면 2줄이상일경우 뒤에 내용이 남아있는지 테스트해야함
    // 그냥 1줄로 바뀜 굿
    string tmp = _new;
    append_newline_and_push_back(tmp, lines_write);
  }

  // fs::path target_file("/etc/hostname");
  // fs::remove(target_file);

  write_file_line_by_line("/etc/hostname", lines_write);
  // 라인 by 라인으로 쓰기 함수...
}

void change_interface_file(string _dev, string _keyword, string _value)
{
  vector<string> lines_work, lines_write;
  lines_work.clear();
  lines_write.clear();
  read_file_line_by_line("/etc/network/interfaces", lines_work);

  // #1 #으로 시작하면 주석이니깐 무시, changed_keyword_value 플래그가 on이면 처리완료한거라 지나감
  // #2 _dev에 맞는 eth블록을 찾음(iface _dev inet static 이라는 문자열로) 찾으면#3, 못찾으면#5
  // #3 키워드를 찾고 키워드를 발견하였으면 수정작업 진행하고 Done처리
  // #4 키워드 찾지 못한 채로, interface파일의 뒷부분에 다른eth블록이 발견되거나 파일이 끝나면
  // 키워드에 맞는 정보 부분추가
  // #5 eth블록을 찾지 못한채로 파일이 끝나면 _dev블록을 새로 생성후 키워드정보 추가

  bool found_dev_block = false; // 해당 dev를 찾았는지 플래그
  bool found_another_block = false; // dev블록 찾은 이후에 다른 블록을 찾았는지 플래그
  bool changed_keyword_value = false; // 해당 dev의 keyword를 value로 변경했는지 플래그 == done
  for(int i=0; i<lines_work.size(); i++)
  {
    string current_line = lines_work[i];

    // #으로 시작하는 주석이거나 이미 keyword를 수정하였을 때(Done일때)
    if(current_line[0] == '#' || changed_keyword_value)
    {
      append_newline_and_push_back(current_line, lines_write);
      continue;
    }

    // eth dev Block 찾기
    if(found_dev_block == false) // 아직 dev block을 찾지 못한 상태라면
    {
      if(find_ethernet_dev_block(current_line, _dev)) // 검사하고
        found_dev_block = true;
      
      append_newline_and_push_back(current_line, lines_write);
      continue;
    }
    else
    {
      // eth dev 블록을 찾은 이후
      int pos;
      // 다른 블록이 아직 나오지 않음
      if(found_another_block == false)
      {
         // 키워드 검사해서 키워드 수정이면 키워드 수정하고
         if(find_ethernet_keyword(current_line, _keyword, pos))
         {
           string new_line = current_line.substr(0, pos+_keyword.length()+1);
           new_line.append(_value);
           append_newline_and_push_back(new_line, lines_write);
           changed_keyword_value = true;
           continue;
         }
      }

      // 다른블록 검사하고 다른블록이 나오는순간 바로 부분 추가 -> Done
      if(find_ethernet_dev_block(current_line, ""))
      {
        add_keyword_part_to_interface_file(_keyword, _value, lines_write);
        found_another_block = true;
        changed_keyword_value = true;
      }

      append_newline_and_push_back(current_line, lines_write);
      continue;
    }

  }

  // 마지막 까지 다 검사했는데 Done이 아닌경우는
  // 블록을 찾았는데 다음블록 없이 키워드수정없이 왔을경우 부분추가
  // 블록도 못찾고 왔을경우 완전추가 
  if(changed_keyword_value == false)
  {
    // 블록도 못찾았다 완전추가
    if(found_dev_block == false)
    {
      // 하나의 dev블록 완전추가
      add_keyword_whole_to_interface_file(_dev, _keyword, _value, lines_write);
    }
    else
    {
      if(found_another_block == false)
      {
        // 부분추가
        add_keyword_part_to_interface_file(_keyword, _value, lines_write);
      }
    }

  }

  write_file_line_by_line("/etc/network/interfaces", lines_write);

}

/**
 * @brief _line에서 iface _dev inet static이 있는지 검사
 *  _dev가 "" 일 때는 다른 iface가 있는지 검사
 * 
 * @param _line line string
 * @param _dev eth id
 */
bool find_ethernet_dev_block(string _line, string _dev)
{
  string searching_str;
  // dev block 찾기 이전
  if(_dev != "")
  {
    searching_str = "iface " + _dev + " inet static";
    // _dev 블록 판별
  }
  else
    searching_str = "auto ";
    // 다른 블록 판별

  if(_line.find(searching_str) == string::npos)
    return false;
  else
    return true;
}

/**
 * @brief _line에서 _keyword가 있는지 검사 찾았을 경우 pos에 반환
 * 
 * @param _line line string
 * @param _keyword keyword
 * @param pos search position
 */
bool find_ethernet_keyword(string _line, string _keyword, int &pos)
{
  // cout << "## [KEYWORD SEARCH LINE] : " << _line << " / " << _keyword << endl;
  // keyword 가 없으면 false
  if((pos = _line.find(_keyword)) == string::npos)
  {
    pos = -1;
    return false;
  }

  // keyword 는 있으나 line의 첫 시작으로 오거나 맨 끝이라 올바른 형태가 아닐 때
  if(pos-1 < 0 || pos+_keyword.length() >= _line.length())
  {
    pos = -1;
    return false;
  }
  
  // keyword 는 있으나 앞 뒤가 공백이 아닐때
  if(_line[pos-1] != ' ' || _line[pos+_keyword.length()] != ' ')
  {
    pos = -1;
    return false;
  }
  // *TODO : 키워드가 맨앞에 나오는게 적용이 되는지 안 되는지 확인필요
  // 앞 뒤가 공백이 왔으나 뒤에 value값이 없는경우.... 는 그냥 값 바꾸면 되니까 상관없을듯
  return true;
}

void change_web_app_file(string _origin, string _new)
{
  vector<string> lines_work, lines_write;
  lines_work.clear();
  lines_write.clear();
  read_file_line_by_line("/web/www/html/js/app.js", lines_work);

  string old_str = "var host = \'http://" + _origin + ":8000\';";
  string new_str = "var host = \'http://" + _new + ":8000\';";
  for(int i=0; i<lines_work.size(); i++)
  {
    string current_line = lines_work[i];
    if(current_line.find(old_str) == string::npos)
    {
      append_newline_and_push_back(current_line, lines_write);
      continue;
    }

    append_newline_and_push_back(new_str, lines_write);
  }

  write_file_line_by_line("/web/www/html/js/app.js", lines_write);
  
}


/**
 * @brief /etc/network/interfaces 파일에 쓸 _write 벡터에 _keyword _value 라인을 추가하는 함수
 * @details interface 파일에 eth dev블록이 존재하는데 해당 키워드의 대한 내용이 없을 때 추가하는 용도
 * @param _keyword 키워드
 * @param _value 값
 * @param _write write할 벡터
 */
void add_keyword_part_to_interface_file(string _keyword, string _value, vector<string> &_write)
{
  string add_string = "  " + _keyword + " " + _value;
  append_newline_and_push_back(add_string, _write);
  return ;
}

/**
 * @brief /etc/network/interfaces 파일에 쓸 _write 벡터에 dev블록을 추가하고
 * _keyword _value 라인을 추가하는 함수
 * @details interface 파일에 eth dev블록이 존재하지 않고 해당 키워드에 대한 내용을 추가해야할 때
 * 사용함
 * @param _dev eth id
 * @param _keyword 키워드
 * @param _value 값
 * @param _write write할 벡터
 */
void add_keyword_whole_to_interface_file(string _dev, string _keyword, string _value, vector<string> &_write)
{
  string enter = "\n";
  string first = "auto " + _dev + "\n";
  string second = "iface " + _dev + " inet static\n";
  _write.push_back(enter);
  _write.push_back(first);
  _write.push_back(second);

  add_keyword_part_to_interface_file(_keyword, _value, _write);

  return ;
}


/**
 * @brief /etc/network/interfaces 파일에 ethernet 정보를 추가하는 함수
 * @details eth는 존재하는데 interfaces파일에 해당 eth의 내용이 없을 때 추가할 수 있는 함수로 구현해놓음
 * @param _dev ethernet이름
 * @param _info 추가할정보
 */
void add_ethernet_to_interface_file(string _dev, Add_Ethernet_Info _info)
{
  vector<string> lines_write;

  string enter = "\n";
  string first = "auto " + _dev + "\n";
  string second = "iface " + _dev + " inet static\n";
  lines_write.push_back(enter);
  lines_write.push_back(first);
  lines_write.push_back(second);

  if(_info.address != "")
  {
    string tmp = "  address " + _info.address + "\n";
    lines_write.push_back(tmp);
  }

  if(_info.netmask != "")
  {
    string tmp = "  netmask " + _info.netmask + "\n";
    lines_write.push_back(tmp);
  }

  if(_info.gateway != "")
  {
    string tmp = "  gateway " + _info.gateway + "\n";
    lines_write.push_back(tmp);
  }

  if(_info.mac != "")
  {
    string tmp = "  hwaddress ether " + _info.mac + "\n";
    lines_write.push_back(tmp);
  }

  if(_info.mtu != "")
  {
    string tmp = "  mtu " + _info.mtu + "\n";
    lines_write.push_back(tmp);
  }

  if(lines_write.size() < 4)
  {
    cout << "No Ethernet Info" << endl;
    return ;
  }

  ofstream write_file("/etc/network/interfaces", std::ios_base::app);
  if(write_file.is_open())
  {
    for(string w_line : lines_write)
    {
      write_file << w_line;
    }
  }

  write_file.close();

}