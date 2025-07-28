#pragma once
#include "farixEngine/events/event.hpp"
#include "farixEngine/input/controller.hpp"

namespace farixEngine{
  class GameObject;
  struct CollisionEvent : public Event {
    GameObject* a;
    GameObject* b;
    CollisionEvent(GameObject* a, GameObject* b) : a(a), b(b) {}
  };

  struct KeyPressedEvent : public Event {
    Key key;
    KeyPressedEvent(Key k) : key(k) {}
  };

  struct KeyReleasedEvent : public Event {
    Key key;
    KeyReleasedEvent(Key k) : key(k) {}
};


}


