#pragma once
#include "farixEngine/events/event.hpp"
#include <cstdint>
#include <functional>
#include <typeindex>
#include <unordered_map>
namespace farixEngine {

class EventDispatcher {
public:
  using ListenerID = uint64_t;

  template <typename EventType>
  ListenerID listen(std::function<void(EventType &event)> callback);

  template <typename EventType> void emit(EventType &event);

  template <typename EventType> void removeListener(ListenerID id);

private:
  ListenerID _nextListenerID = 1;
  std::unordered_map<
      std::type_index,
      std::unordered_map<ListenerID, std::function<void(Event &)>>>
      listeners;
};

template <typename EventType>
EventDispatcher::ListenerID
EventDispatcher::listen(std::function<void(EventType &event)> callback) {
  ListenerID id = _nextListenerID++;
  listeners[typeid(EventType)][id] = [cb = std::move(callback)](Event &e) {
    cb(static_cast<EventType &>(e));
  };
  ;
  return id;
}

template <typename EventType> void EventDispatcher::emit(EventType &event) {
  auto it = listeners.find(typeid(EventType));
  if (it != listeners.end()) {
    for (auto &[id, fn] : it->second) {
      if (!event.handled)
        fn(event);
    }
  }
}

template <typename EventType>
void EventDispatcher::removeListener(ListenerID id) {
  listeners[typeid(EventType)].erase(id);
}

}; // namespace farixEngine
