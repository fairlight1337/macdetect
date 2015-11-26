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


#include <macdetect-client/MDClient.h>


namespace macdetect_client {
  MDClient::MDClient() {
  }
  
  MDClient::~MDClient() {
  }
  
  macdetect::Packet* MDClient::getPacket(std::string strKey, std::string strValue, bool bBlock) {
    macdetect::Packet* pktReturn = NULL;
    bool bReceived = false;
    
    while(!bReceived) {
      while(m_lstReceivedPackets.size() == 0) {
	macdetect::Packet* pktReceived = m_cliClient.receivePacket();
	
	if(pktReceived) {
	  m_lstReceivedPackets.push_back(pktReceived);
	}
      }
      
      for(std::list<macdetect::Packet*>::iterator itPacket = m_lstReceivedPackets.begin(); itPacket != m_lstReceivedPackets.end(); itPacket++) {
	macdetect::Packet* pktPacket = *itPacket;
	
	if((strKey == "" || pktPacket->key() == strKey) &&
	   (strValue == "" || pktPacket->value() == strValue)) {
	  pktReturn = pktPacket;
	  bReceived = true;
	  m_lstReceivedPackets.erase(itPacket);
	  
	  break;
	}
      }
      
      if(pktReturn == NULL) {
	if(!bBlock) {
	  break;
	}
      }
    }
    
    return pktReturn;
  }
  
  bool MDClient::connect(std::string strIP) {
    return m_cliClient.connect(strIP);
  }
  
  macdetect::Packet* MDClient::requestResponse(std::string strRequest) {
    macdetect::Packet* pktRequest = new macdetect::Packet("request", strRequest);
    macdetect::Packet* pktResponse = this->requestResponse(pktRequest);
    delete pktRequest;
    
    return pktResponse;
  }
  
  macdetect::Packet* MDClient::requestResponse(macdetect::Packet* pktRequest, std::string strKey) {
    m_cliClient.sendPacket(pktRequest);
    
    return this->getPacket(strKey, pktRequest->value());
  }
  
  std::list<std::string> MDClient::deviceNames() {
    std::list<std::string> lstDeviceNames;
    macdetect::Packet* pktDevices = this->requestResponse("devices-list");
    
    for(macdetect::Packet* pktSub : pktDevices->subPackets()) {
      lstDeviceNames.push_back(pktSub->value());
    }
    
    delete pktDevices;
    
    return lstDeviceNames;
  }
  
  std::list<std::string> MDClient::knownMACAddresses() {
    std::list<std::string> lstMACAddresses;
    macdetect::Packet* pktMACAddresses = this->requestResponse("known-mac-addresses");
    
    for(macdetect::Packet* pktSub : pktMACAddresses->subPackets()) {
      lstMACAddresses.push_back(pktSub->value());
    }
    
    delete pktMACAddresses;
    
    return lstMACAddresses;
  }
  
  bool MDClient::enableStream(std::string strDeviceName) {
    bool bSuccess = false;
    
    macdetect::Packet* pktRequest = new macdetect::Packet("request", "enable-stream");
    pktRequest->add(new macdetect::Packet("device-name", strDeviceName));
    
    macdetect::Packet* pktResult = this->requestResponse(pktRequest);
    delete pktRequest;
    
    if(pktResult->sub("result")) {
      if(pktResult->sub("result")->value() == "success") {
	bSuccess = true;
      }
    }
    
    delete pktResult;
    
    return bSuccess;
  }
  
  bool MDClient::disableStream(std::string strDeviceName) {
    bool bSuccess = false;
    
    macdetect::Packet* pktRequest = new macdetect::Packet("request", "disable-stream");
    pktRequest->add(new macdetect::Packet("device-name", strDeviceName));
    
    macdetect::Packet* pktResult = this->requestResponse(pktRequest);
    delete pktRequest;
    
    if(pktResult->sub("result")) {
      if(pktResult->sub("result")->value() == "success") {
	bSuccess = true;
      }
    }
    
    delete pktResult;
    
    return bSuccess;
  }
  
  macdetect::Packet* MDClient::info() {
    return this->getPacket(""/*info*/, "", false);
  }
}