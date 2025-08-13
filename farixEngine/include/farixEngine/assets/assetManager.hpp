#pragma once
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <typeindex>
#include <unordered_map>

namespace farixEngine {

class Asset {
public:
  std::string id = "";
  std::string name = "";

  virtual ~Asset() = default;
};

class AssetManager {
  template <typename T>
  using AssetMap = std::unordered_map<std::string, std::shared_ptr<T>>;
  using UUID = std::string;
  using AssetID = std::string;

public:
  template <typename T>
  AssetID add(std::shared_ptr<T> asset, const std::string &name = "") {
    UUID id = asset->id;
    getAssetMap<T>()[id] = asset;
    allAssetsRaw[id] = asset;
    if (!name.empty()) {
      nameToUUIDMap[name] = id;
      uuidToNameMap[id] = name;
    }
    return id;
  }

  template <typename T, typename... Args>
  AssetID load(const std::string &name, Args &&...args) {

    if (has<T>(name)) {
      auto idOpt = findIdByName(name);
      if (idOpt)
        return *idOpt;
    }

    auto asset = T::load(std::forward<Args>(args)...);
    if (!asset) {
      throw std::runtime_error("Failed to load asset: " + name);
    }

    return add<T>(asset, name);
  }

  template <typename T> std::shared_ptr<T> get(const UUID &idOrName) {
    auto &assets = getAssetMap<T>();

    auto it = assets.find(idOrName);
    if (it != assets.end()) {
      return it->second;
    }

    auto idOpt = findIdByName(idOrName);
    if (idOpt) {
      auto it2 = assets.find(*idOpt);
      if (it2 != assets.end()) {
        return it2->second;
      }
    }

    return nullptr;
  }
  std::shared_ptr<Asset> getRaw(const UUID &idOrName) {
    UUID id = idOrName;
    auto itName = nameToUUIDMap.find(idOrName);
    if (itName != nameToUUIDMap.end()) {
      id = itName->second;
    }
    auto it = allAssetsRaw.find(id);
    if (it != allAssetsRaw.end())
      return it->second;
    return nullptr;
  }

  template <typename T> bool has(const std::string &idOrName) const {
    const auto &assets = getAssetMap<T>();
    if (assets.find(idOrName) != assets.end()) {
      return true;
    }
    if (nameToUUIDMap.find(idOrName) != nameToUUIDMap.end()) {
      auto id = nameToUUIDMap.at(idOrName);
      return assets.find(id) != assets.end();
    }
    return false;
  }

  bool has(const std::string &idOrName) const {
    const auto &assets = allAssetsRaw;
    if (assets.find(idOrName) != assets.end()) {
      return true;
    }
    if (nameToUUIDMap.find(idOrName) != nameToUUIDMap.end()) {
      auto id = nameToUUIDMap.at(idOrName);
      return assets.find(id) != assets.end();
    }
    return false;
  }
  std::optional<UUID> findIdByName(const std::string &name) {
    auto it = nameToUUIDMap.find(name);
    if (it != nameToUUIDMap.end())
      return it->second;
    return std::nullopt;
  }
  std::optional<std::string> findNameById(const UUID &id) const {
    auto it = uuidToNameMap.find(id);
    if (it != uuidToNameMap.end())
      return it->second;
    return std::nullopt;
  }

  template <typename T, typename Func> void forEachAsset(Func func) {
    auto &assets = getAssetMap<T>();
    for (auto &[uuid, asset] : assets) {
      func(uuid, asset);
    }
  }
  template <typename Func> void forEachRawAsset(Func func) {
    for (auto &[uuid, asset] : allAssetsRaw) {
      func(uuid, asset);
    }
  }

private:
  template <typename T> AssetMap<T> &getAssetMap() {
    static AssetMap<T> assetMap;
    return assetMap;
  }
  template <typename T> const AssetMap<T> &getAssetMap() const {
    static AssetMap<T> assetMap;
    return assetMap;
  }

  std::unordered_map<std::string, std::string> nameToUUIDMap;
  std::unordered_map<std::string, std::string> uuidToNameMap;
  std::unordered_map<AssetID, std::shared_ptr<Asset>> allAssetsRaw;
};

} // namespace farixEngine
