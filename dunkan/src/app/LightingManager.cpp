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
  
  // Set view offset (camera center) for world-space light calculations
  // This ensures circular point light falloff in isometric view
  ubo.viewOffsetPadded = glm::vec4(viewPos.x, viewPos.y, 0.0f, 0.0f);

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

  // Sun light (main 2.5D directional light)
  // Aligned with isometric view (from top-right-back)
  LightConfig sun;
  sun.type = 0; // Directional
  sun.position = glm::vec3(0.0f, 0.0f, 0.0f);
  sun.direction =
      glm::normalize(glm::vec3(-1.0f, -1.0f, -0.5f)); // XZ direction + Y depth
  sun.color = glm::vec3(1.0f, 0.95f, 0.9f);           // Warm sunlight
  sun.intensity = 0.6f; // Balanced intensity (was 4.0 with 0.15 scaling)
  sun.radius = 0.0f;
  sun.cutoffAngle = 0.0f;
  sun.enabled = true;
  addLight(sun);

  // Rim light (from opposite side for depth)
  // Positioned on the left-front side in XZ plane
  LightConfig rim;
  rim.type = 0; // Directional
  rim.position = glm::vec3(0.0f, 0.0f, 0.0f);
  rim.direction =
      glm::normalize(glm::vec3(1.0f, -0.5f, 0.3f)); // From left-front
  rim.color = glm::vec3(0.6f, 0.7f, 1.0f);          // Cool rim light
  rim.intensity = 0.3f; // Subtle intensity (was 2.0 with 0.15 scaling)
  rim.radius = 0.0f;
  rim.cutoffAngle = 0.0f;
  rim.enabled = true;
  addLight(rim);
  
  // Point light 1 (warm orange, center-left)
  // Tests circular falloff in isometric XZ plane
  LightConfig point1;
  point1.type = 1; // Point light
  point1.position = glm::vec3(600.0f, 400.0f, 15.0f); // XZ horizontal position, Y height
  point1.direction = glm::vec3(0.0f, -1.0f, 0.0f); // Not used for point lights
  point1.color = glm::vec3(1.0f, 0.6f, 0.3f);      // Warm orange
  point1.intensity = 1.5f;
  point1.radius = 300.0f; // Circular radius in world space
  point1.cutoffAngle = 0.0f;
  point1.enabled = true;
  addLight(point1);
  
  // Point light 2 (cool blue, center-right)
  // Tests circular falloff in isometric XZ plane
  LightConfig point2;
  point2.type = 1; // Point light
  point2.position = glm::vec3(1300.0f, 600.0f, 20.0f); // XZ horizontal position, Y height
  point2.direction = glm::vec3(0.0f, -1.0f, 0.0f); // Not used for point lights
  point2.color = glm::vec3(0.3f, 0.5f, 1.0f);      // Cool blue
  point2.intensity = 1.2f;
  point2.radius = 250.0f; // Circular radius in world space
  point2.cutoffAngle = 0.0f;
  point2.enabled = true;
  addLight(point2);
}

} // namespace dunkan
