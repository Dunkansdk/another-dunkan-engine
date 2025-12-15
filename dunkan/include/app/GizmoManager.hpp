#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <optional>

namespace dunkan {

class ApplicationConfig;
class LightingManager;
class Camera;

/**
 * @brief Manages interactive gizmos for manipulating entities and lights
 *
 * Provides visual handles for translation, rotation, and scaling of game objects.
 * Supports different manipulation modes and integrates with the entity/light systems.
 */
class GizmoManager {
public:
  enum class Mode {
    TRANSLATE,
    ROTATE,
    SCALE
  };

  enum class TargetType {
    NONE,
    ENTITY,
    LIGHT
  };

  struct GizmoState {
    Mode currentMode = Mode::TRANSLATE;
    TargetType targetType = TargetType::NONE;
    int selectedIndex = -1; // Index of selected entity or light
    bool isDragging = false;
    glm::vec2 dragStartPos{0.0f};
    glm::vec3 objectStartPos{0.0f};  // Physics position for delta calculation
    glm::vec3 objectRenderPos{0.0f}; // Render position for gizmo drawing
    int draggedAxis = -1; // 0=X, 1=Y, 2=Z, -1=all/center
  };

  GizmoManager(ApplicationConfig& config, LightingManager& lightingManager);
  ~GizmoManager() = default;

  /**
   * @brief Update gizmo logic (input handling, state updates)
   * @param mousePos Current mouse position in screen space
   * @param mousePressed Whether mouse button is currently pressed
   */
  void update(const glm::vec2& mousePos, bool mousePressed);

  /**
   * @brief Render gizmo UI and controls
   */
  void renderUI();

  /**
   * @brief Render gizmo panel in ImGui
   */
  void renderGizmoPanel();

  /**
   * @brief Select an entity for manipulation
   * @param entityIndex Index of the entity to select
   * @param physicsPos Physics position (for delta calculation)
   * @param renderPos Render position (for gizmo drawing, centered on sprite)
   */
  void selectEntity(int entityIndex, const glm::vec3& physicsPos, const glm::vec3& renderPos);

  /**
   * @brief Select a light for manipulation
   * @param lightIndex Index of the light to select
   */
  void selectLight(int lightIndex);

  /**
   * @brief Deselect current target
   */
  void clearSelection();

  /**
   * @brief Get current gizmo state
   */
  const GizmoState& getState() const { return state; }
  
  /**
   * @brief Update the render position of the gizmo (for entity sprite centering)
   */
  void updateRenderPosition(const glm::vec3& renderPos);

  /**
   * @brief Set gizmo manipulation mode
   */
  void setMode(Mode mode) { state.currentMode = mode; }

  /**
   * @brief Set camera reference for coordinate conversion
   */
  void setCamera(Camera* cam) { camera = cam; }

  /**
   * @brief Update the position of the currently selected entity (call during drag)
   */
  void updateEntityPosition(const glm::vec3& position);

  /**
   * @brief Get current translation delta (for real-time entity updates)
   */
  glm::vec3 getCurrentDelta() const;

  /**
   * @brief Render visual gizmo handles (arrows, arcs, boxes)
   */
  void renderGizmoVisuals();

private:
  ApplicationConfig& config;
  LightingManager& lightingManager;
  Camera* camera = nullptr;
  GizmoState state;
  
  // Current manipulation delta
  glm::vec3 currentDelta{0.0f};

  // Internal helper methods
  void handleDragStart(const glm::vec2& mousePos);
  void handleDragUpdate(const glm::vec2& mousePos);
  void handleDragEnd();
  
  glm::vec3 screenToWorld(const glm::vec2& screenPos);
  bool isMouseOverGizmo(const glm::vec2& mousePos);
  
  // Get position of currently selected object
  glm::vec3 getSelectedObjectPosition() const;
  
  // Visual rendering helpers
  void drawTranslationGizmo(const glm::vec3& position);
  void drawRotationGizmo(const glm::vec3& position);
  void drawScaleGizmo(const glm::vec3& position);
};

} // namespace dunkan
