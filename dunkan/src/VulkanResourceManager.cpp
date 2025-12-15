#include "vulkan/VulkanResourceManager.hpp"
#include <stdexcept>
#include <cassert>

VulkanResourceManager::VulkanResourceManager(VulkanContext& context)
    : m_context(context) {
}

VulkanResourceManager::~VulkanResourceManager() {
    cleanup();
}

void VulkanResourceManager::load(const std::string& id, const std::string& filename) {
    auto image = std::make_unique<VulkanImage>(m_context);
    image->loadFromFile(filename);
    m_textureStorage[id] = std::move(image);
}

VulkanImage* VulkanResourceManager::get(const std::string& id) {
    auto found = m_textureStorage.find(id);
    assert(found != m_textureStorage.end());
    return found->second.get();
}

VulkanImage* VulkanResourceManager::getPointer(const std::string& id) {
    return get(id);
}

void VulkanResourceManager::cleanup() {
    m_textureStorage.clear();
}
