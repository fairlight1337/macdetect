// Copyright 2016 Jan Winkler <jan.winkler.84@gmail.com>
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
// http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/** \author Jan Winkler */


#ifndef __DEVICE_H__
#define __DEVICE_H__


// System
#include <string>
#include <netinet/ip_icmp.h>
#include <sys/time.h>

// Detect
#include <macdetect/Wire.h>


namespace macdetect {
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
    std::string m_strMAC;
    std::string m_strIP;
    std::string m_strBroadcastIP;
    double m_dPingBroadcastLastSent;
    
  protected:
  public:
    Device(std::string strDeviceName, HardwareType hwtType);
    ~Device();
    
    std::string deviceName();
    HardwareType hardwareType();
    
    void setUp(bool bUp);
    void setRunning(bool bRunning);
    
    std::string mac();
    void setIP(std::string strIP);
    std::string ip();
    void setBroadcastIP(std::string strIP);
    std::string broadcastIP();
    
    bool up();
    bool running();
    
    unsigned char* read(int& nLengthRead);
    
    Wire* wire();
    
    bool sendPingBroadcast(double dTime, double dInterval);
    bool sendPing(std::string strDestinationIP, std::string strDestinationMAC);
    
    unsigned short icmpHeaderChecksum(unsigned short* usData, int nLength);
    uint16_t ipHeaderChecksum(void* vdData, int nLength);
    
    static bool systemDeviceExists(std::string strDeviceName);
  };
}


#endif /* __DEVICE_H__ */