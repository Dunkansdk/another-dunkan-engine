#pragma once

#include "app/ApplicationConfig.hpp"
#include "app/LightingManager.hpp"
#include "app/GizmoManager.hpp"
#include "app/Camera.hpp"

#include <string>
#include <vector>
#include <memory>


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
 * Modular panel system with main menu bar for toggling individual panels.
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

  /**
   * @brief Get reference to gizmo manager
   */
  GizmoManager& getGizmoManager() { return gizmoManager; }

  /**
   * @brief Get reference to camera
   */
  Camera& getCamera() { return camera; }

private:
  ApplicationConfig &config;
  LightingManager &lightingMgr;
  Camera camera;
  GizmoManager gizmoManager;
  
  // Panel visibility flags
  bool showGBufferPanel = true;
  bool showSSAOPanel = false;
  bool showLightingPanel = true;
  bool showDepthDebugPanel = false;
  bool showEntityEditorPanel = true;
  bool showRenderingPanel = false;
  bool showGizmoPanel = false;
  bool showCameraPanel = false;
  bool showStatsPanel = true;
  
  // Main UI methods
  void renderMainMenuBar(int fps, int entityCount);

  // Main panel rendering methods
  void renderGBufferViews();
  void renderSSAOPanel();
  void renderLightingPanel();
  void renderDepthDebug();
  void renderEntityEditor(const std::vector<EntityEditData> &entityCache);
  void renderRenderingSettings();
  void renderGizmoPanel();
  void renderCameraPanel();
  
  // Sub-panel rendering methods (modular)
  void renderLightControl(size_t index, LightConfig &light);
  void renderEntityTransform(EntityEditData &data, int index);
  void renderEntityMaterial(EntityEditData &data);
  void renderEntityTextures(const EntityEditData &data);
};

} // namespace dunkan
