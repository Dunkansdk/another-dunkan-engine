#pragma once

#include <map>
#include <string>
#include <memory>
#include <stdexcept>
#include <cassert>

template<typename ASSET>
struct AssetManager {

    explicit AssetManager() = default;
    AssetManager(AssetManager&) = delete;
    AssetManager(AssetManager&&) = delete;
    AssetManager& operator=(AssetManager&) = delete;
    AssetManager& operator=(AssetManager&&) = delete;

    void load(std::string id, std::string filename) {
        ASSET* asset = new ASSET();
        if (!asset->loadFromFile(filename))
            throw std::runtime_error("AssetManager load failed. " + filename);
        insert(id, std::move(asset));
    }

    [[nodiscard]] ASSET& get(std::string id)
    {
        auto found = m_asset_storage.find(id);  
        assert(found != m_asset_storage.end());
            return *found->second;
    }

    [[nodiscard]] ASSET* get_pointer(std::string id)
    {
        auto found = m_asset_storage.find(id);  
        assert(found != m_asset_storage.end());
            return found->second;
    }

    // [[nodiscard]] const std::map<std::string, ASSET*> get_all(std::string id) const
    // {
    //     auto found = m_asset_storage.find(id);  
    //     assert(found != m_asset_storage.end());
    //     return *found->second;
    // }

private:
    std::map<std::string, ASSET*> m_asset_storage;

    void insert(std::string id, ASSET* resource) 
    {
        auto inserted = m_asset_storage.emplace(id, std::move(resource));
        assert(inserted.second);
    }

};