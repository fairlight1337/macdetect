// macdetect, a daemon and clients for detecting MAC addresses
// Copyright (C) 2015 Jan Winkler <jan.winkler.84@gmail.com>
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

/** \author Jan Winkler */


#include <macdetectd/Device.h>


namespace macdetect {
  Device::Device(std::string strDeviceName, HardwareType hwtType) : m_strDeviceName(strDeviceName), m_hwtType(hwtType), m_bUp(false), m_bRunning(false), m_wrWire(strDeviceName, ETH_FRAME_LEN), m_strMAC(""), m_strIP(""), m_strBroadcastIP(""), m_dPingBroadcastLastSent(0) {
  }
  
  Device::~Device() {
  }
  
  std::string Device::deviceName() {
    return m_strDeviceName;
  }
  
  Device::HardwareType Device::hardwareType() {
    return m_hwtType;
  }
  
  std::string Device::mac() {
    if(m_strMAC == "") {
      struct ifreq ifrTemp;
      
      memset(&ifrTemp, 0, sizeof(ifrTemp));
      strcpy(ifrTemp.ifr_name, this->deviceName().c_str());
      ioctl(m_wrWire.socket(), SIOCGIFHWADDR, &ifrTemp);
      
      std::string strDeviceMAC = "";
      char carrBuffer[17];
      int nOffset = 0;
      for(int nI = 0; nI < 6; ++nI) {
	sprintf(&(carrBuffer[nI + nOffset]), "%.2x", (unsigned char)ifrTemp.ifr_hwaddr.sa_data[nI]);
	nOffset++;
	
	if(nI < 5) {
	  nOffset++;
	  carrBuffer[nI + nOffset] = ':';
	}
      }
      
      strDeviceMAC = std::string(carrBuffer, 17);
      
      m_strMAC = strDeviceMAC;
    }
    
    return m_strMAC;
  }
  
  void Device::setIP(std::string strIP) {
    m_strIP = strIP;
  }
  
  std::string Device::ip() {
    return m_strIP;
  }
  
  void Device::setBroadcastIP(std::string strIP) {
    m_strBroadcastIP = strIP;
  }
  
  std::string Device::broadcastIP() {
    return m_strBroadcastIP;
  }
  
  void Device::setUp(bool bUp) {
    m_bUp = bUp;
  }
  
  void Device::setRunning(bool bRunning) {
    m_bRunning = bRunning;
  }
  
  bool Device::up() {
    return m_bUp;
  }
  
  bool Device::running() {
    return m_bRunning;
  }
  
  unsigned char* Device::read(int& nLengthRead) {
    unsigned char* ucBuffer;
    nLengthRead = m_wrWire.defaultReadingLength();
    nLengthRead = m_wrWire.bufferedRead(&ucBuffer, nLengthRead);
    
    return ucBuffer;
  }
  
  bool Device::sendPingBroadcast(double dTime, double dInterval) {
    if(dTime >= m_dPingBroadcastLastSent + dInterval) {
      m_dPingBroadcastLastSent = dTime;
      
      return this->sendPing("255.255.255.255"/*this->broadcastIP()*/, "ff:ff:ff:ff:ff:ff");
    }
    
    return false;
  }
  
  bool Device::sendPing(std::string strDestinationIP, std::string strDestinationMAC) {
    if(this->ip() != "" && this->broadcastIP() != "") {
      // IP
      struct iphdr iphHeader;
      memset(&iphHeader, 0, sizeof(struct iphdr));
      
      // TODO(winkler): version and ihl both only have 4 bits; ihl =
      // 21 sets version = 4 and ihl = 20 correctly, but this should
      // be cleaned out.
      iphHeader.version = 4;
      iphHeader.ihl = 21;
      iphHeader.tot_len = htons(84);
      iphHeader.frag_off = 0x0040;
      iphHeader.ttl = 64;
      iphHeader.protocol = 1; // ICMP
      
      struct in_addr iaAddr;
      inet_aton(this->ip().c_str(), &iaAddr); // The device's own IP
      iphHeader.saddr = iaAddr.s_addr;
      inet_aton(strDestinationIP.c_str(), &iaAddr);
      iphHeader.daddr = iaAddr.s_addr;
      
      iphHeader.check = this->ipHeaderChecksum(&iphHeader, sizeof(struct iphdr));
      
      // ICMP
      struct icmp icmpICMP;
      memset(&icmpICMP, 0, sizeof(struct icmp));
      
      icmpICMP.icmp_type = ICMP_ECHO;
      icmpICMP.icmp_code = 0;
      icmpICMP.icmp_cksum = 0;
      icmpICMP.icmp_seq = 12345;
      icmpICMP.icmp_id = getpid();
      
      struct timeval tvorig;
      long tsorig;
      gettimeofday(&tvorig, (struct timezone *)NULL);
      tsorig = (tvorig.tv_sec % (24*60*60)) * 1000 + tvorig.tv_usec / 1000;
      
      icmpICMP.icmp_otime = htonl(tsorig);
      icmpICMP.icmp_rtime = 0;
      icmpICMP.icmp_ttime = 0;
      
      icmpICMP.icmp_cksum = this->icmpHeaderChecksum((unsigned short*)&icmpICMP, 12 + 8);
      
      int nLen = sizeof(struct iphdr) + 12 + 8;
      unsigned char carrBufferPre[nLen];
      memcpy(carrBufferPre, &iphHeader, sizeof(struct iphdr));
      memcpy(&(carrBufferPre[sizeof(struct iphdr)]), &icmpICMP, sizeof(struct icmp));
      
      int nLength = nLen + sizeof(struct ethhdr);
      unsigned char carrBuffer[nLength];
      
      int nL = m_wrWire.wrapInEthernetFrame(this->mac(), strDestinationMAC, ETH_P_IP, carrBufferPre, 12 + 8 + sizeof(struct iphdr), carrBuffer);
      
      return m_wrWire.write(carrBuffer, nL);
    }
    
    return false;
  }
  
  Wire* Device::wire() {
    return &m_wrWire;
  }
  
  unsigned short Device::icmpHeaderChecksum(unsigned short* usData, int nLength) {
    int nleft = nLength;
    unsigned short* w = usData;
    int sum = 0;
    u_short answer = 0;
    
    while(nleft > 1)  {
      sum += *w++;
      nleft -= 2;
    }
    
    if(nleft == 1) {
      *(unsigned char*)(&answer) = *(unsigned char*)w ;
      sum += answer;
    }
    
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = ~sum;
    
    return answer;
  }
  
  uint16_t Device::ipHeaderChecksum(void* vdData, int nLength) {
    char* carrData = (char*)vdData;
    
    // Initialise the accumulator.
    uint32_t acc = 0xffff;
    
    // Handle complete 16-bit blocks.
    for(size_t i = 0; i + 1 < nLength; i += 2) {
      uint16_t word;
      memcpy(&word, carrData + i, 2);
      acc += ntohs(word);
      
      if(acc > 0xffff) {
	acc -= 0xffff;
      }
    }
    
    // Handle any partial block at the end of the data.
    if(nLength & 1) {
      uint16_t word = 0;
      memcpy(&word, carrData + nLength - 1, 1);
      acc += ntohs(word);
      
      if(acc > 0xffff) {
	acc -= 0xffff;
      }
    }
    
    // Return the checksum in network byte order.
    return htons(~acc);
  }
}
