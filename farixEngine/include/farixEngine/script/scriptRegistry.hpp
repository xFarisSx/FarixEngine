#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <functional>

//#include "farixEngine/script/script.hpp" 

namespace farixEngine {
class Script;
class ScriptRegistry {
public:
    using ScriptPtr = std::shared_ptr<Script>;
    using FactoryFn = std::function<ScriptPtr()>;

    ScriptRegistry() = default;

    template <typename T>
    void registerScript(const std::string& name) {
        static_assert(std::is_base_of<Script, T>::value, "T must inherit from Script");
        factories[name] = []() -> ScriptPtr {
            return std::make_shared<T>();
        };
    }

    ScriptPtr create(const std::string& name) const;

    bool exists(const std::string& name) const;

private:
    std::unordered_map<std::string, FactoryFn> factories;
};
}
