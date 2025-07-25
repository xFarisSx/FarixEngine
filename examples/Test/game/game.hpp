
#pragma once
#include <farixEngine/farixEngine.hpp>

using namespace farixEngine;

class Game : public Application {
public:
  void onStart() override;
  void onUpdate(float dt) override;
};
