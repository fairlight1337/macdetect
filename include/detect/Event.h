#ifndef __EVENT_H__
#define __EVENT_H__


#include <detect/Device.h>


namespace detect {
  class Event {
  public:
    typedef enum {
      DeviceAdded = 0,
      DeviceRemoved = 1,
      DeviceStateChanged = 2
    } EventType;
    
  private:
    EventType m_tpType;
    
  protected:
  public:
    Event(EventType tpType);
    ~Event();
    
    EventType type();
    void setEventType(EventType tpSet);
  };
  
  class DeviceEvent : public Event {
  private:
    std::string m_strDeviceName;
    bool m_bStateChangeUp;
    bool m_bStateChangeRunning;
    
  protected:
  public:
    DeviceEvent(EventType tpType, std::string strDeviceName);
    ~DeviceEvent();
    
    std::string deviceName();
    
    void setStateChangeUp(bool bStateChangeUp);
    void setStateChangeRunning(bool bStateChangeUp);
    
    bool stateChangeUp();
    bool stateChangeRunning();
  };
}


#endif /* __EVENT_H__ */
