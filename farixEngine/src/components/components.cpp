#include "farixEngine/components/components.hpp"
#include "farixEngine/core/engineServices.hpp"
#include <farixEngine/utils/uuid.hpp>

namespace farixEngine {

MaterialComponent::MaterialComponent(UUID id) {
  auto &am = EngineServices::get().getAssetManager();

  if (id.empty()) {

    static UUID defaultMatID = "default-material";
    if (!am.has<Material>(defaultMatID)) {
      am.add<Material>(std::make_shared<Material>(Material{defaultMatID}));
    }
    material = defaultMatID;
  } else {
    material = id;
  }
}

} // namespace farixEngine
