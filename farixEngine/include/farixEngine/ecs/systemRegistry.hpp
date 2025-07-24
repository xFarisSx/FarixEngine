#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <functional>

//#include "farixEngine/script/script.hpp" 

namespace farixEngine {
class System;
using SystemPtr = std::shared_ptr<System>;
class SystemRegistry {
public:
    using FactoryFn = std::function<SystemPtr()>;

    SystemRegistry() = default;

    template<typename T>
    void registerSystem(const std::string& name) {
        static_assert(std::is_base_of<System, T>::value, "T must inherit from System");
        factories[name] = [] { return std::make_shared<T>(); };
    }

    SystemPtr create(const std::string& name) const;

    bool exists(const std::string& name) const;

private:
    std::unordered_map<std::string, FactoryFn> factories;
};
}
