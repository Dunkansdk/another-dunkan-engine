#pragma once

#include <map>
#include <string>
#include <memory>
#include <stdexcept>
#include <cassert>

template<typename ASSET>
struct AssetManager {

    AssetManager() {
        // blank constructor
    }

    void load(std::string id, std::string filename) {
        std::unique_ptr<ASSET> asset(new ASSET());
        if (!asset->loadFromFile(filename))
            throw std::runtime_error("AssetManager load failed. " + filename);
        insert(id, std::move(asset));
        std::cout << "Texture loaded: " << m_asset_storage.size() <<
        " ID: " << id << "\n";
    }

    [[nodiscard]] ASSET& get(std::string id)
    {
        auto found = m_asset_storage.find(id);  
        assert(found != m_asset_storage.end());
            return *found->second;
    }

    // [[nodiscard]] const ASSET& get(std::string id) const
    // {
    //     auto found = m_asset_storage.find(id);  
    //     assert(found != m_asset_storage.end());
    //         return *found->second;
    // }

    [[nodiscard]] const std::map<std::string, std::unique_ptr<ASSET>> get_all(std::string id) const
    {
        auto found = m_asset_storage.find(id);  
        assert(found != m_asset_storage.end());
            return *found->second;
    }

private:
    std::map<std::string, std::unique_ptr<ASSET>> m_asset_storage;

    void insert(std::string id, std::unique_ptr<ASSET> resource) 
    {
        auto inserted = m_asset_storage.emplace(id, std::move(resource));
        assert(inserted.second);
    }

};