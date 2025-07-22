#include "farixEngine/script/scriptRegistry.hpp"
#include <stdexcept>

namespace farixEngine{

ScriptRegistry::ScriptPtr ScriptRegistry::create(const std::string& name) const {
    auto it = factories.find(name);
    if (it != factories.end()) {
        return it->second(); 
    }
    throw std::runtime_error("Script type '" + name + "' not registered");
}

bool ScriptRegistry::exists(const std::string& name) const {
    return factories.find(name) != factories.end();
}
}
