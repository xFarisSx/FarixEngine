# Events

FarixEngine includes a lightweight event system that allows global communication between components and systems without tight coupling.

## Built-in Events
```cpp
struct KeyPressedEvent : public Event {
  Key key;
  KeyPressedEvent(Key k) : key(k) {}
};

struct KeyReleasedEvent : public Event {
  Key key;
  KeyReleasedEvent(Key k) : key(k) {}
};

struct CollisionEvent : public Event {
  GameObject* a;
  GameObject* b;
  CollisionEvent(GameObject* a, GameObject* b) : a(a), b(b) {}
};
```
Used by overriding the handlers inside scripts:
```cpp
virtual void onKeyPressed(KeyPressedEvent& event) {}
virtual void onKeyReleased(KeyReleasedEvent& event) {}
virtual void onCollision(CollisionEvent& event) {}
```

## Subscribing
Inside your script, you can subscribe to events in onCreate():
```cpp
keyListener = scene->getEventDispatcher().listen<KeyPressedEvent>(
  [this](KeyPressedEvent& e) { onKeyPressed(e); });
```
Unsubscribe in onDestroy():
```cpp
scene->getEventDispatcher().removeListener<KeyPressedEvent>(keyListener);
```

## Emitting
The engine automatically emits input and collision events:
```cpp
if (event.type == SDL_KEYDOWN && event.key.repeat == 0)
  dispatcher.emit(KeyPressedEvent(*mappedKey));

else if (event.type == SDL_KEYUP)
  dispatcher.emit(KeyReleasedEvent(*mappedKey));
```
You can also emit custom events:
```cpp
MyCustomEvent e(...);
dispatcher.emit(e);
```

## Custom Events
```cpp
struct HealthChangedEvent : public Event {
  int newHealth;
  HealthChangedEvent(int h) : newHealth(h) {}
};
```
Then use `.listen<HealthChangedEvent>(...)` and `.emit(HealthChangedEvent(...))` just like any other event.



