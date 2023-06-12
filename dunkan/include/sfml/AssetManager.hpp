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
        
        std::cout << "Texture loaded: " << id << "\n";
        insert(id, std::move(asset));
        std::cout << "Texture loaded: " << m_asset_storage.size() << "\n";
    }

    [[nodiscard]] ASSET& get(std::string id)
    {
        std::cout << "Getting texture: " << id << "\n";
        auto found = m_asset_storage.find(id);
        
        assert(found != m_asset_storage.end());

        return *found->second;
    }

    // [[nodiscard]] const ASSET& get(std::string id) const
    // {
    //     std::cout << "Loading texture: " << id << "\n";
    //     auto found = m_asset_storage.find(id);
    //     std::get
    //     //assert(found != m_asset_storage.end());

    //     return *found->second;
    // }

private:
    std::map<std::string, std::unique_ptr<ASSET>> m_asset_storage;

    void insert(std::string id, std::unique_ptr<ASSET> resource) 
    {
        // Insert and check success
        auto inserted = m_asset_storage.emplace(id, std::move(resource));
        std::cout << "Texture inserted: " << id << "\n";
        assert(inserted.second);
    }

};