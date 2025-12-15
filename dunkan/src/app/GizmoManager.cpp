#include "app/GizmoManager.hpp"
#include "app/ApplicationConfig.hpp"
#include "app/LightingManager.hpp"
#include "app/Camera.hpp"
#include <imgui.h>

namespace dunkan {

GizmoManager::GizmoManager(ApplicationConfig& config, LightingManager& lightingManager)
  : config(config), lightingManager(lightingManager) {
}

void GizmoManager::update(const glm::vec2& mousePos, bool mousePressed) {
  if (!config.enableGizmos) {
    return;
  }

  // Handle drag state machine
  if (mousePressed && !state.isDragging) {
    if (isMouseOverGizmo(mousePos)) {
      handleDragStart(mousePos);
    }
  } else if (mousePressed && state.isDragging) {
    handleDragUpdate(mousePos);
  } else if (!mousePressed && state.isDragging) {
    handleDragEnd();
  }
}

void GizmoManager::renderUI() {
  if (!config.enableGizmos || !camera) {
    return;
  }

  renderGizmoVisuals();
}

void GizmoManager::renderGizmoPanel() {
  ImGui::Checkbox("Enable Gizmos", &config.enableGizmos);

  ImGui::Separator();
  ImGui::Text("Gizmo Mode");
  
  if (ImGui::RadioButton("Translate", state.currentMode == Mode::TRANSLATE)) {
    setMode(Mode::TRANSLATE);
  }
  if (ImGui::RadioButton("Rotate", state.currentMode == Mode::ROTATE)) {
    setMode(Mode::ROTATE);
  }
  if (ImGui::RadioButton("Scale", state.currentMode == Mode::SCALE)) {
    setMode(Mode::SCALE);
  }

  ImGui::Separator();
  ImGui::Text("Selection");

  if (state.targetType == TargetType::NONE) {
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "No selection");
  } else if (state.targetType == TargetType::ENTITY) {
    ImGui::Text("Entity: %d", state.selectedIndex);
    if (ImGui::Button("Clear Selection")) {
      clearSelection();
    }
  } else if (state.targetType == TargetType::LIGHT) {
    ImGui::Text("Light: %d", state.selectedIndex);
    if (ImGui::Button("Clear Selection")) {
      clearSelection();
    }
  }

  ImGui::Separator();
  ImGui::Text("Quick Select");
  
  // TODO: Add buttons to quickly select entities/lights from lists
  ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), 
                     "Click entities/lights in other\npanels to select them");

  if (state.isDragging) {
    ImGui::Separator();
    ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f), "DRAGGING");
  }
}

void GizmoManager::selectEntity(int entityIndex, const glm::vec3& physicsPos, const glm::vec3& renderPos) {
  state.targetType = TargetType::ENTITY;
  state.selectedIndex = entityIndex;
  state.objectStartPos = physicsPos;   // Store physics position for delta calculation
  state.objectRenderPos = renderPos;   // Store render position for gizmo drawing
  state.isDragging = false;
  currentDelta = glm::vec3(0.0f);
}

void GizmoManager::selectLight(int lightIndex) {
  state.targetType = TargetType::LIGHT;
  state.selectedIndex = lightIndex;
  state.isDragging = false;
  currentDelta = glm::vec3(0.0f);
  
  // Get light position
  if (lightIndex >= 0 && lightIndex < static_cast<int>(lightingManager.getLightCount())) {
    state.objectStartPos = lightingManager.getLight(lightIndex).position;
  }
}

void GizmoManager::clearSelection() {
  state.targetType = TargetType::NONE;
  state.selectedIndex = -1;
  state.isDragging = false;
  currentDelta = glm::vec3(0.0f);
}

void GizmoManager::handleDragStart(const glm::vec2& mousePos) {
  state.isDragging = true;
  state.dragStartPos = mousePos;
  currentDelta = glm::vec3(0.0f);
  
  // Detect which axis is being clicked
  if (camera && state.targetType != TargetType::NONE) {
    // For entities, use render position (centered on sprite), for lights use world position
    glm::vec3 objectPos = (state.targetType == TargetType::ENTITY) ? state.objectRenderPos : getSelectedObjectPosition();
    glm::vec2 center = camera->worldToScreen(objectPos);
    glm::vec2 mouseVec = mousePos - center;
    
    // Check click proximity to each axis handle
    float arrowLength = 60.0f;
    float threshold = 20.0f;
    
    // X-axis (horizontal right)
    glm::vec2 xEnd(center.x + arrowLength, center.y);
    if (glm::length(mousePos - xEnd) < threshold) {
      state.draggedAxis = 0; // X-axis
    }
    // Y-axis (vertical up)
    else if (glm::length(mousePos - glm::vec2(center.x, center.y - arrowLength)) < threshold) {
      state.draggedAxis = 1; // Y-axis
    }
    // Z-axis (diagonal up-right for height)
    else if (glm::length(mousePos - glm::vec2(center.x + arrowLength * 0.5f, center.y - arrowLength * 0.5f)) < threshold) {
      state.draggedAxis = 2; // Z-axis
    }
    else {
      state.draggedAxis = -1; // Center/free movement
    }
  }
  
  // objectStartPos is already set by selectEntity/selectLight
}

void GizmoManager::handleDragUpdate(const glm::vec2& mousePos) {
  if (!state.isDragging) {
    return;
  }

  // Calculate screen space delta (in pixels)
  glm::vec2 screenDelta = mousePos - state.dragStartPos;
  
  // For 2D/isometric games, use direct screen-to-world mapping
  // Apply a scale factor to make movement feel more responsive (0.5 = half speed)
  float moveScale = 0.25f; // Adjust this value: lower = slower, higher = faster
  glm::vec3 worldDelta(screenDelta.x * moveScale, -screenDelta.y * moveScale, 0.0f); // Flip Y (screen Y down, world Y up)
  
  // Constrain movement based on dragged axis
  if (state.draggedAxis == 0) {
    // X-axis only
    worldDelta.y = 0.0f;
    worldDelta.z = 0.0f;
  } else if (state.draggedAxis == 1) {
    // Y-axis only
    worldDelta.x = 0.0f;
    worldDelta.z = 0.0f;
  } else if (state.draggedAxis == 2) {
    // Z-axis (height) - use screen Y delta for Z movement
    // Moving mouse up = increasing height (negative screen Y = positive Z)
    worldDelta.x = 0.0f;
    worldDelta.y = 0.0f;
    worldDelta.z = -screenDelta.y * 0.25f; // Adjusted scale for Z-axis
  }
  // else: draggedAxis == -1, free movement (all axes)
  
  switch (state.currentMode) {
    case Mode::TRANSLATE:
      currentDelta = worldDelta;
      
      // Apply to light if selected
      if (state.targetType == TargetType::LIGHT && 
          state.selectedIndex >= 0 && 
          state.selectedIndex < static_cast<int>(lightingManager.getLightCount())) {
        lightingManager.getLight(state.selectedIndex).position = state.objectStartPos + worldDelta;
      }
      break;
      
    case Mode::ROTATE:
      // TODO: Implement rotation
      break;
      
    case Mode::SCALE:
      // TODO: Implement scaling
      break;
  }
}

void GizmoManager::handleDragEnd() {
  state.isDragging = false;
  
  // For entities, objectStartPos is updated from DebugUI when dragging ends
  // For lights, update position from current light data
  if (state.targetType == TargetType::LIGHT && 
      state.selectedIndex >= 0 && 
      state.selectedIndex < static_cast<int>(lightingManager.getLightCount())) {
    state.objectStartPos = lightingManager.getLights()[state.selectedIndex].position;
  }
}

glm::vec3 GizmoManager::screenToWorld(const glm::vec2& screenPos) {
  if (!camera) {
    // Fallback if camera not set
    return glm::vec3(screenPos.x, screenPos.y, 0.0f);
  }
  
  return camera->screenToWorld(screenPos);
}

bool GizmoManager::isMouseOverGizmo(const glm::vec2& mousePos) {
  if (!camera || state.targetType == TargetType::NONE) {
    return false;
  }

  // Get the current position of the selected object
  // For entities, use render position (centered on sprite), for lights use world position
  glm::vec3 objectPos = (state.targetType == TargetType::ENTITY) ? state.objectRenderPos : getSelectedObjectPosition();
  
  // Convert to screen space
  glm::vec2 screenPos = camera->worldToScreen(objectPos);
  
  // Check if mouse is within gizmo bounds (expanded for better click detection)
  float distance = glm::length(mousePos - screenPos);
  float threshold = 80.0f; // pixels - larger for easier clicking
  
  return distance < threshold;
}

void GizmoManager::renderGizmoVisuals() {
  if (state.targetType == TargetType::NONE) {
    return;
  }

  // Get the current position of the selected object
  // For entities, use render position (centered on sprite), for lights use world position
  glm::vec3 objectPos = (state.targetType == TargetType::ENTITY) ? state.objectRenderPos : getSelectedObjectPosition();
  
  switch (state.currentMode) {
    case Mode::TRANSLATE:
      drawTranslationGizmo(objectPos);
      break;
    case Mode::ROTATE:
      drawRotationGizmo(objectPos);
      break;
    case Mode::SCALE:
      drawScaleGizmo(objectPos);
      break;
  }
}

void GizmoManager::drawTranslationGizmo(const glm::vec3& position) {
  if (!camera) return;
  
  // Convert world position to screen space
  glm::vec2 center = camera->worldToScreen(position);
  
  // Use ImGui's draw list for overlay rendering
  ImDrawList* drawList = ImGui::GetBackgroundDrawList();
  
  // Gizmo size in pixels
  float arrowLength = 60.0f;
  float arrowThickness = 4.0f;
  float arrowheadSize = 12.0f;
  
  // Draw X-axis (Red)
  ImVec2 xStart(center.x, center.y);
  ImVec2 xEnd(center.x + arrowLength, center.y);
  drawList->AddLine(xStart, xEnd, IM_COL32(255, 0, 0, 255), arrowThickness);
  drawList->AddTriangleFilled(
    ImVec2(xEnd.x, xEnd.y),
    ImVec2(xEnd.x - arrowheadSize, xEnd.y - arrowheadSize/2),
    ImVec2(xEnd.x - arrowheadSize, xEnd.y + arrowheadSize/2),
    IM_COL32(255, 0, 0, 255)
  );
  
  // Draw Y-axis (Green)
  ImVec2 yStart(center.x, center.y);
  ImVec2 yEnd(center.x, center.y - arrowLength); // Y-up in world space
  drawList->AddLine(yStart, yEnd, IM_COL32(0, 255, 0, 255), arrowThickness);
  drawList->AddTriangleFilled(
    ImVec2(yEnd.x, yEnd.y),
    ImVec2(yEnd.x - arrowheadSize/2, yEnd.y + arrowheadSize),
    ImVec2(yEnd.x + arrowheadSize/2, yEnd.y + arrowheadSize),
    IM_COL32(0, 255, 0, 255)
  );
  
  // Draw Z-axis (Blue) - vertical height control
  // Diagonal up-right for isometric height
  float zOffsetX = arrowLength * 0.5f;
  float zOffsetY = -arrowLength * 0.5f;
  ImVec2 zStart(center.x, center.y);
  ImVec2 zEnd(center.x + zOffsetX, center.y + zOffsetY);
  drawList->AddLine(zStart, zEnd, IM_COL32(0, 100, 255, 255), arrowThickness);
  drawList->AddTriangleFilled(
    ImVec2(zEnd.x, zEnd.y),
    ImVec2(zEnd.x - arrowheadSize * 0.7f, zEnd.y + arrowheadSize * 0.3f),
    ImVec2(zEnd.x - arrowheadSize * 0.3f, zEnd.y + arrowheadSize * 0.7f),
    IM_COL32(0, 100, 255, 255)
  );
  
  // Draw center circle
  drawList->AddCircleFilled(ImVec2(center.x, center.y), 6.0f, IM_COL32(255, 255, 255, 255));
  drawList->AddCircle(ImVec2(center.x, center.y), 6.0f, IM_COL32(0, 0, 0, 255), 12, 2.0f);
}

void GizmoManager::drawRotationGizmo(const glm::vec3& position) {
  if (!camera) return;
  
  glm::vec2 center = camera->worldToScreen(position);
  ImDrawList* drawList = ImGui::GetBackgroundDrawList();
  
  // Draw rotation circles
  float radius = 60.0f;
  
  // Outer circle (Z-axis rotation)
  drawList->AddCircle(ImVec2(center.x, center.y), radius, IM_COL32(100, 100, 255, 255), 64, 3.0f);
  
  // Inner circle
  drawList->AddCircle(ImVec2(center.x, center.y), radius * 0.8f, IM_COL32(200, 200, 200, 128), 64, 2.0f);
  
  // Center dot
  drawList->AddCircleFilled(ImVec2(center.x, center.y), 4.0f, IM_COL32(255, 255, 255, 255));
}

void GizmoManager::drawScaleGizmo(const glm::vec3& position) {
  if (!camera) return;
  
  glm::vec2 center = camera->worldToScreen(position);
  ImDrawList* drawList = ImGui::GetBackgroundDrawList();
  
  // Gizmo size
  float lineLength = 50.0f;
  float boxSize = 10.0f;
  float lineThickness = 3.0f;
  
  // Draw X-axis (Red) with box
  ImVec2 xEnd(center.x + lineLength, center.y);
  drawList->AddLine(ImVec2(center.x, center.y), xEnd, IM_COL32(255, 0, 0, 255), lineThickness);
  drawList->AddRectFilled(
    ImVec2(xEnd.x - boxSize/2, xEnd.y - boxSize/2),
    ImVec2(xEnd.x + boxSize/2, xEnd.y + boxSize/2),
    IM_COL32(255, 0, 0, 255)
  );
  
  // Draw Y-axis (Green) with box
  ImVec2 yEnd(center.x, center.y - lineLength);
  drawList->AddLine(ImVec2(center.x, center.y), yEnd, IM_COL32(0, 255, 0, 255), lineThickness);
  drawList->AddRectFilled(
    ImVec2(yEnd.x - boxSize/2, yEnd.y - boxSize/2),
    ImVec2(yEnd.x + boxSize/2, yEnd.y + boxSize/2),
    IM_COL32(0, 255, 0, 255)
  );
  
  // Center box (uniform scale)
  drawList->AddRectFilled(
    ImVec2(center.x - boxSize/2, center.y - boxSize/2),
    ImVec2(center.x + boxSize/2, center.y + boxSize/2),
    IM_COL32(255, 255, 255, 255)
  );
  drawList->AddRect(
    ImVec2(center.x - boxSize/2, center.y - boxSize/2),
    ImVec2(center.x + boxSize/2, center.y + boxSize/2),
    IM_COL32(0, 0, 0, 255),
    0.0f, 0, 2.0f
  );
}

glm::vec3 GizmoManager::getCurrentDelta() const {
  return currentDelta;
}

void GizmoManager::updateEntityPosition(const glm::vec3& position) {
  if (state.targetType == TargetType::ENTITY) {
    state.objectStartPos = position;
  }
}

void GizmoManager::updateRenderPosition(const glm::vec3& renderPos) {
  if (state.targetType == TargetType::ENTITY) {
    state.objectRenderPos = renderPos;
  }
}

glm::vec3 GizmoManager::getSelectedObjectPosition() const {
  if (state.targetType == TargetType::LIGHT && 
      state.selectedIndex >= 0 && 
      state.selectedIndex < static_cast<int>(lightingManager.getLightCount())) {
    return lightingManager.getLights()[state.selectedIndex].position;
  }
  
  // For entities, return the stored position (will be updated from outside)
  return state.objectStartPos;
}

} // namespace dunkan
