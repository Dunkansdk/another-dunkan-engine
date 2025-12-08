#pragma once

#include <glm/glm.hpp>

namespace dunkan {

/**
 * @brief Application-wide configuration state
 *
 * Centralizes all configuration parameters that were previously scattered
 * throughout VulkanApplication. Simple POD struct for easy access.
 */
struct ApplicationConfig {
  // Debug View Settings
  enum class DebugView {
    COMPOSITE = 0,
    ALBEDO = 1,
    NORMALS = 2,
    DEPTH = 3,
    MATERIAL = 4,
    SSAO = 5
  };

  DebugView currentDebugView = DebugView::COMPOSITE;
  bool showDebugWindow = true;

  // SSAO Configuration
  bool enableSSAO = false; // Disabled by default
  float ssaoRadius = 0.5f;
  float ssaoBias = 0.025f;
  float ssaoPower = 1.0f;

  // Rendering Configuration
  float gammaCorrection = 0.8f; // Gamma for final output (stylized)

  // Depth Configuration
  float globalDepthMultiplier = 0.01f;

  // Ambient Lighting
  glm::vec3 ambientLight = glm::vec3(0.03f, 0.03f, 0.04f);

  // Camera Configuration
  glm::vec3 cameraPosition = glm::vec3(960.0f, 540.0f, 1000.0f); // Screen center
  float cameraZoom = 1.0f;
  bool cameraFollowEnabled = false;
  int cameraFollowEntityIndex = 0;
  float cameraFollowSmoothness = 5.0f;

  // Gizmo Configuration
  bool enableGizmos = true; // Gizmos for object/light manipulation
};

} // namespace dunkan
