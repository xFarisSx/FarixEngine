#pragma once

namespace farixEngine{
  
  struct Event {
    virtual ~Event() = default;
    bool handled = false; 
};
  
  
}
