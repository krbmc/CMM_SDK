#ifndef __ETHERNET_HPP__
#define __ETHERNET_HPP__

#include "stdafx.hpp"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <net/if.h>
#include <net/route.h>
#include <netinet/in.h>
#include <sys/ioctl.h>

/**
 * @brief 클라이언트 소켓 생성 함수
 * 본 함수는 BMC 네트워크 정보를 가져오거나 설정하기 위하여 생성하는 클라이언트 소켓 생성 함수입니다.\n
 * BMC 네트워크 정보를 가져오거나 변경 할 때 소켓을 생성하여 소켓 파일 디스크립터를 반환합니다.\n
 * 반환된 파일 디스크립터를 사용하여 MAC 주소, IP 주소, Netmask 주소 등 전반적인 네트워크 정보를 가져오거나 설정할 때 사용합니다.\n
 * 
 * @return int sockfd - 소켓 파일디스크립터
 */
int create_socket();
int get_ip_info(uint8_t index, uint8_t *ip_addr, uint8_t *netmask, uint8_t *gateway, uint8_t *mac_addr, int *mtu);
void get_gateway(unsigned char chan, unsigned char *gateway);

int set_ip(char *iface_name, char *ip_addr, char *net_mask, char *gateway_addr, char *mtu, char *mac);
int set_route(int sockfd, char *gateway_addr, struct sockaddr_in *addr, char *dev_name);

//////////// 소켓으로 변경해도 재부팅하면 다 날아가서 파일 수정으로 방식 변경

typedef struct _Add_Ethernet_Info
{
    string address = "";
    string netmask = "";
    string gateway = "";
    string mtu = "";
    string mac = "";

} Add_Ethernet_Info;

// 파일변경
// void read_file_line_by_line(string _filename, vector<string> &_lines);
// void write_file_line_by_line(string _filename, vector<string> _lines);
// void append_newline_and_push_back(string _str, vector<string> &_vec);
void change_hosts_file(string _target, string _new);
void change_hostname_file(string _new);
void change_interface_file(string _dev, string _keyword, string _value);
void change_web_app_file(string _origin, string _new);

void add_ethernet_to_interface_file(string _dev, Add_Ethernet_Info _info);




#endif