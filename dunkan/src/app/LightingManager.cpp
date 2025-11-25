#include "app/LightingManager.hpp"
#include "vulkan/VulkanBuffer.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace dunkan {

LightingManager::LightingManager() {
  // Reserve space for typical light count
  lights.reserve(10);
}

void LightingManager::addLight(const LightConfig &light) {
  if (lights.size() < 10) {
    lights.push_back(light);
  }
}

void LightingManager::removeLight(size_t index) {
  if (index < lights.size()) {
    lights.erase(lights.begin() + index);
  }
}

LightConfig &LightingManager::getLight(size_t index) { return lights[index]; }

void LightingManager::updateLightingUBO(VulkanBuffer *lightingUBO,
                                        const glm::vec3 &ambientLight,
                                        const glm::vec3 &viewPos) {
  LightingUBO ubo{};
  ubo.ambientLight = glm::vec4(ambientLight, 1.0f);
  ubo.viewPos = viewPos;
  ubo.numLights = 0;

  for (size_t i = 0; i < lights.size() && i < 10; i++) {
    if (!lights[i].enabled)
      continue;

    Light &light = ubo.lights[ubo.numLights++];
    light.position =
        glm::vec4(lights[i].position, static_cast<float>(lights[i].type));
    light.direction =
        glm::vec4(glm::normalize(lights[i].direction), lights[i].radius);
    light.color = glm::vec4(lights[i].color, lights[i].intensity);
    light.params = glm::vec4(lights[i].cutoffAngle, 0.0f, 0.0f, 0.0f);
  }

  lightingUBO->copyFrom(&ubo, sizeof(LightingUBO));
}

void LightingManager::initializeDefaultLights() {
  lights.clear();

  // Sun (directional light) - adjusted for 2.5D isometric (XZ ground plane, Y =
  // depth)
  LightConfig sun;
  sun.type = 0;                                   // Directional
  sun.direction = glm::vec3(-0.3f, -0.5f, -0.8f); // Angled down toward XZ plane
  sun.color = glm::vec3(1.0f, 0.98f, 0.95f);
  sun.intensity = 0.4f;
  addLight(sun);

  // Rim light (opposite direction for depth perception)
  LightConfig rim;
  rim.type = 0;                                // Directional
  rim.direction = glm::vec3(0.5f, 0.3f, 0.8f); // From opposite angle
  rim.color = glm::vec3(0.8f, 0.85f, 1.0f);    // Slightly cool tint
  rim.intensity = 0.15f;
  addLight(rim);
}

} // namespace dunkan
