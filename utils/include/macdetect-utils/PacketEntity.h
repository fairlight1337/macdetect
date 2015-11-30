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


#ifndef __PACKETENTITY_H__
#define __PACKETENTITY_H__


// System
#include <unistd.h>
#include <sys/socket.h>
#include <mutex>

// MAC detect
#include <macdetect-utils/Value.h>


namespace macdetect {
  class PacketEntity {
  private:
  protected:
    int m_nSocketFD;
    bool m_bFailureState;
    std::mutex m_mtxSocketAccess;
    
  public:
    PacketEntity(int nSocketFD);
    ~PacketEntity();
    
    bool send(Value* valSend);
    Value* receive();
    
    int socket();
    bool failureState();
  };
}


#endif /* __PACKETENTITY_H__ */
