#pragma once

#include <glm/glm.hpp>
#include <vector>


// Forward declaration - VulkanBuffer is in global namespace
class VulkanBuffer;

namespace dunkan {

/**
 * @brief Configuration for a single light source
 *
 * For 2.5D isometric rendering:
 * - For directional lights: direction uses XZ for horizontal, Y for vertical
 * angle
 * - For point/spot lights: position uses XZ for horizontal plane, Y for
 * depth/height
 */
struct LightConfig {
  bool enabled = true;
  int type = 0; // 0=directional, 1=point, 2=spot

  glm::vec3 position = glm::vec3(0.0f); // XZ = horizontal, Y = depth/height
  glm::vec3 direction =
      glm::vec3(0.0f, -1.0f, 0.0f); // XZ = horiz dir, Y = vertical angle
  glm::vec3 color = glm::vec3(1.0f);

  float intensity = 1.0f;
  float radius = 100.0f;
  float cutoffAngle = 30.0f;
};

/**
 * @brief Light structure matching shader UBO layout
 */
struct Light {
  glm::vec4 position;  // w = type (0=directional, 1=point, 2=spot)
  glm::vec4 direction; // w = radius
  glm::vec4 color;     // w = intensity
  glm::vec4 params;    // x=cutoffAngle, y=outerCutoff, z=attenuation, w=unused
};

/**
 * @brief Lighting UBO matching shader layout
 */
struct LightingUBO {
  glm::vec4 ambientLight;
  glm::vec3 viewPos;
  int numLights;
  Light lights[10];
};

/**
 * @brief Manages all lighting for the application
 *
 * Handles light configuration, updates, and UBO synchronization.
 * Designed for 2.5D isometric view where:
 * - XZ plane is the ground (like topdown XY)
 * - Y axis is vertical depth/height
 */
class LightingManager {
public:
  LightingManager();
  ~LightingManager() = default;

  // Light management
  void addLight(const LightConfig &light);
  void removeLight(size_t index);
  LightConfig &getLight(size_t index);
  const std::vector<LightConfig> &getLights() const { return lights; }
  size_t getLightCount() const { return lights.size(); }

  // UBO updates
  void updateLightingUBO(VulkanBuffer *lightingUBO,
                         const glm::vec3 &ambientLight,
                         const glm::vec3 &viewPos);

  // Initialization
  void initializeDefaultLights();

private:
  std::vector<LightConfig> lights;
};

} // namespace dunkan
