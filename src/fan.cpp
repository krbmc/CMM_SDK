#include "fan.hpp"

#include "boost/bind/bind.hpp"
#include <boost/asio/post.hpp>
#include <boost/asio/thread_pool.hpp>
#include "logservice.hpp"

extern Value_About_HA ha_value;


bool checked_fan_status[CHASSIS_MAX_FAN];
using namespace std;
using namespace boost::asio;
using namespace this_thread;
ChassisFan::ChassisFan(int fanaddr, int idx, int rpm) {
  this->pwm_data = rpm;
  this->fanaddress = fanaddr;
  cout << "fan init = " << fanaddr << endl;
  this->idx = idx;
  this->mutex = false;
  ischange_pwm=false;
}
void ChassisFan::Run_Fan_RPM() {
  //   if (rpm != 0) {
  //     this->pwm_data = rpm;
  //     return true;
  //   }
  if (mutex) {
    cout << "Set_Fan_RPM: mutex .." << endl;
    return;
  }
  this->mutex = true;
    this->file = open(FAN_I2C, O_RDWR);
    if (file < 0) {
      log(info) << "keti-smbus erorr : not open " << endl;
      this->mutex = false;
    } else {
      if (ioctl(file, I2C_SLAVE, this->fanaddress) < 0) {

        log(info) << "keti-smbus erorr : not slave open " << endl;
        this->mutex = false;
        close(file);
      }
    }
    // if (this->ischange_pwm){
      // cout << "[" << std::hex << fanaddress << "]fan step 1" << endl;
    smbus_return = i2c_smbus_write_byte_data(file, reg_cr0, reg_cr0_data);
    i2c_smbus_write_byte_data(file, reg_cr0, reg_cr0_data);
    // cout << "[" << std::hex << fanaddress << "]fan step 2" << endl;
    i2c_smbus_write_byte_data(file, reg_cr1, reg_cr1_data);
    // cout << "[" << std::hex << fanaddress << "]fan step 3" << endl;
    i2c_smbus_write_byte_data(file, reg_pwm, pwm_data);

    cout << "[" << std::hex << fanaddress << "] [Run Fan RPM] [PWM] : ";
    printf("%d\n", pwm_data);
    // cout << "[" << std::hex << fanaddress << "]fan step 4" << endl;

    string fan_odata = "/redfish/v1/Chassis/CMM1/Thermal/Fans/CHASSIS_FAN_";
    fan_odata = fan_odata + to_string((this->idx + 1));
    Fan *fan = (Fan *)g_record[fan_odata];
    fan->reading = pwm_data;
    resource_save_json(fan);

    close(file);
    this->file = 0;
    this->mutex = false;
}
bool ChassisFan::Set_Fan_RPM(int pwm) {
  if (pwm < 0) {
    return false;
  }
  fan_init();
  this->pwm_data = pwm;
  ischange_pwm = true;
  Run_Fan_RPM();
}

ChassisFan *chassis_Fan[CHASSIS_MAX_FAN];
//?? i2c가 8개만 잡힘 확인필요 기철
int fanslaveaddr[CHASSIS_MAX_FAN] = {0x50, 0x51, 0x52, 0x53,
                                     0x54, 0x55, 0x56, 0x57};
int test = 50;

void *fan_measure_handler(void) {
//   chassis_Fan[0]->Set_Fan_RPM(100);
  log(info) << "fan_measure_handler start!!!!" << endl;
  // fan 생성자
  for (int i = 0; i < CHASSIS_MAX_FAN; i++) {
    chassis_Fan[i] = new ChassisFan(fanslaveaddr[i], i, 0);
  }


  int count = 0;
  while(1)
  {
      for(int j=0; j<CHASSIS_MAX_FAN; j++)
          chassis_Fan[j]->Run_Fan_RPM();
      
      if(count >= 60)
      {
          // DB insert
          for(int j=0; j<CHASSIS_MAX_FAN; j++)
              // insert_reading_table("CHASSIS_FAN_" + to_string(j+1), ha_value.myId, "thermal", "fan", chassis_Fan[j]->pwm_data, DB_currentDateTime());
              insert_reading_table("FAN_" + to_string(j+1), ha_value.myId, "thermal", "fan", chassis_Fan[j]->pwm_data, DB_currentDateTime());
          count = 0;
      }
      else
          count++;

      sleep(1);
  }
  
    
  //   chassis_Fan[0]->Set_Fan_RPM(0);
  //   chassis_Fan[0]->Run_Fan_RPM();
  //   sleep(20);
  //   chassis_Fan[0]->Set_Fan_RPM(150);
  //   chassis_Fan[0]->Run_Fan_RPM();
  //   sleep(20);
  //   chassis_Fan[0]->Set_Fan_RPM(0);
  //   chassis_Fan[0]->Run_Fan_RPM();
  //   sleep(20);

//   log(info) << "fan_measure_handler start!!!!" << endl;
//   //fan 생성자 
//   for (int i = 0; i < CHASSIS_MAX_FAN; i++) {
//     chassis_Fan[i] = new ChassisFan(fanslaveaddr[i], i, 50);
//   }
//   int count = 0;
//   for (;; count++) {
//     thread_pool *pool = new thread_pool(CHASSIS_MAX_FAN);

//     //
//     // chassis_Fan[j]->Run_Fan_RPM();
//     post(*pool, []() {
//       for (int j = 0; j <1;j++){ //CHASSIS_MAX_FAN; j++) {
//         chassis_Fan[j]->Run_Fan_RPM();
//       }

//     });
//     // pool->join();
//     //  delete(pool);
//     sleep(1);
//   }
}



// void *fan_measure_handler(void)
// {

//     // fan_init();
//     int file;
//     int adapter_nr = 2; /* CM/SM 아비터1 2=  */
//     char filename[20];
//     cout << "start" << endl;
//     snprintf(filename, 19, "/dev/i2c-%d", adapter_nr);
//     file = open(filename, O_RDWR);
//     if (file < 0) {
//         /* ERROR HANDLING; you can check errno to see what went wrong */
//         cout << "keti-smbus erorr : not open " << filename;
//     }
//     __s32 Fan1address = 0x52;
//     // int addr = 0x50; /* The I2C address */
//     if (ioctl(file, I2C_SLAVE, Fan1address) < 0) {
//         cout << "The I2C address  " << Fan1address << "error";
//     }
//     __u8 reg = 0x52; /* Device register to access */
//     __s32 res;

//     __s32 reg_config = 0x52;
//     __s32 reg_cr0 = 0x00;
//     __s32 reg_cr1 = 0x01;
//     __s32 reg_cr0_data = 0x18;
//     __s32 reg_cr1_data = 0x11;
//     __s32 reg_pwm = 0x50; // fan fault duty Cycle
//     __s32 pwm_data = 0x00;
//     __s32 smbus_return = 0x00;
//     __s32 pwmdir = 0x00;
//     smbus_return = i2c_smbus_write_byte_data(file, reg_cr0, reg_cr0_data);
//     cout << "return value = " << std::hex << smbus_return << endl;
//     sleep(2);
//     cout << "step 1" << endl;
//     i2c_smbus_write_byte_data(file, reg_cr0, reg_cr0_data);
//     sleep(2);
//     i2c_smbus_write_byte_data(file, reg_cr1, reg_cr1_data);
//     sleep(2);

//     pwm_data = 25;
//     i2c_smbus_write_byte_data(file, reg_pwm, pwm_data);

//     while(1)
//     {
//         __s32 my_result = i2c_smbus_read_byte_data(file, reg_pwm);
//         cout << "[MEASURE PWM] : " << my_result << endl;

//         sleep(5);
//     }
    

// }