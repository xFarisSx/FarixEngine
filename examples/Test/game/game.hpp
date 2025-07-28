
#pragma once
#include <farixEngine/farixEngine.hpp>
#include <iostream>


using namespace farixEngine;

class TestCollisionScript : public Script {
public:
  TestCollisionScript() : Script("TestCollisionScript") {}

  void onStart() override { std::cout << "[Script] " << name << " started\n"; }

  // void onCollision(CollisionEvent &event) override {
  //
  //   GameObject *other = (event.a->getEntity() == entity) ? event.b : event.a;
  //   if (!other) {
  //     std::cout << "[Error] CollisionEvent had null 'other'\n";
  //     return;
  //   }
  //   std::cout << "[Collision] " << getGameObject()->getName()
  //             << " collided with " << other->getName() << "\n";
  // }


  void onKeyPressed(KeyPressedEvent& e) override{
    std::cout << "Key pressed: " << utils::keyToString(e.key) << "\n";
  }
  void onKeyReleased(KeyReleasedEvent& e) override{
    std::cout << "Key released: " << utils::keyToString(e.key) << "\n";
  }
}; 

class Game : public Application { 
public:
  void onStart() override;
  void onUpdate(float dt) override;
};
