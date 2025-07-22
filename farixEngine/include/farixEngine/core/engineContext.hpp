#pragma once


namespace farixEngine{
struct Controller;
struct Renderer;

struct EngineContext {
  Controller *controller = nullptr;
  Renderer *renderer = nullptr;
};
}
