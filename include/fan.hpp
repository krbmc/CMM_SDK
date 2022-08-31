#ifndef __FAN_HPP__
#define __FAN_HPP__

#include "smbus.hpp"
#include "stdafx.hpp"
#include "resource.hpp"

#include <bitset>
#include <fcntl.h>

#define CHASSIS_MAX_FAN 8
#define FAN_I2C "/dev/i2c-2"

enum FAN_SPEED
{
    // 팬 속도 제어 select box로 일단 프로토타입
    FAN_SPEED_MIN, 
    FAN_SPEED_LOW,
    FAN_SPEED_HALF,
    FAN_SPEED_HIGH,
    FAN_SPEED_MAX
};

class ChassisFan {
private:

  int idx;

  double tacho;
  bool check_fan;
  bool isopen;
  bool mutex;
  int file;
  int fanaddress;
  __s32 reg_config;
  __s32 reg_cr0;
  __s32 reg_cr1;
  __s32 reg_cr0_data;
  __s32 reg_cr1_data;
  __s32 reg_pwm; // fan fault duty Cycle
  
  __s32 smbus_return;
  __s32 pwmdir;
  __u8 reg;
  void fan_init()
  {
    this->reg = 0x52; /* Device register to access */
    this->reg_config = 0x52;
    this->reg_cr0 = 0x00;
    this->reg_cr1 = 0x01;
    this->reg_cr0_data = 0x18;
    this->reg_cr1_data = 0x11;
    this->reg_pwm = 0x50; // fan fault duty Cycle
    this->smbus_return = 0x00;
    this->pwmdir = 0x00;
  }

public:
  __s32 pwm_data;
  bool ischange_pwm;
  ChassisFan(){};
  ChassisFan(int fanaddr, int idx, int rpm = 50);
  ~ChassisFan();
  void Run_Fan_RPM();
  bool Set_Fan_RPM(int pwm);
};

// cmm fan 9 measure
void *fan_measure_handler(void);

// void control_fan_speed(int fan_num, int fan_speed);

// fan init 할때 9개 resource 만들어야 됨
// 



#endif