#ifndef __DEVICE_H__
#define __DEVICE_H__


#include <string>

#include <detect/Wire.h>


namespace detect {
  class Device {
  public:
    typedef enum {
      Loopback = 0,
      Wired = 1,
      Wireless = 2
    } HardwareType;
    
  private:
    int m_nSocketFD;
    bool m_bUp;
    bool m_bRunning;
    std::string m_strDeviceName;
    HardwareType m_hwtType;
    Wire m_wrWire;
    
  protected:
  public:
    Device(std::string strDeviceName, HardwareType hwtType);
    ~Device();
    
    std::string deviceName();
    HardwareType hardwareType();
    
    void setUp(bool bUp);
    void setRunning(bool bRunning);
    
    bool up();
    bool running();
    
    unsigned char* read(int& nLengthRead);
    
    static bool systemDeviceExists(std::string strDeviceName);
  };
}


#endif /* __DEVICE_H__ */
