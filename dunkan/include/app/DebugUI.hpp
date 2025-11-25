#pragma once

#include "app/ApplicationConfig.hpp"
#include "app/LightingManager.hpp"

#include <string>
#include <vector>


// Forward declarations - we only need pointers
struct RenderComponent;
struct PhysicsComponent;

namespace dunkan {

/**
 * @brief Entity data for editing in ImGui
 * Simple POD struct to avoid template complexity
 */
struct EntityEditData {
  RenderComponent *renderComp;
  PhysicsComponent *physicsComp;
  std::string textureName;
};

/**
 * @brief Handles all ImGui debug UI rendering
 *
 * Simplified design: takes data directly in render() instead of
 * storing EntityManager reference. This avoids template/include complexity.
 */
class DebugUI {
public:
  DebugUI(ApplicationConfig &config, LightingManager &lightingMgr);
  ~DebugUI() = default;

  /**
   * @brief Render all debug UI panels
   * @param fps Current FPS
   * @param entityCount Total number of entities
   * @param entityCache Entity data for editing (pass from main.cpp)
   */
  void render(int fps, int entityCount,
              const std::vector<EntityEditData> &entityCache);

private:
  ApplicationConfig &config;
  LightingManager &lightingMgr;

  // Panel rendering methods
  void renderGBufferViews();
  void renderSSAOPanel();
  void renderLightingPanel();
  void renderDepthDebug();
  void renderEntityEditor(const std::vector<EntityEditData> &entityCache);
  void renderRenderingSettings();
};

} // namespace dunkan
