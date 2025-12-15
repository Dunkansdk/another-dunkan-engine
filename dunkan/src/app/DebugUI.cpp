#include "app/DebugUI.hpp"
#include "game/components/physicscomponent.hpp"
#include "game/components/rendercomponent.hpp"
#include <imgui.h>


namespace dunkan {

DebugUI::DebugUI(ApplicationConfig &config, LightingManager &lightingMgr)
    : config(config), lightingMgr(lightingMgr), 
      camera(1920.0f, 1080.0f), gizmoManager(config, lightingMgr) {
  // Connect camera to gizmo manager
  gizmoManager.setCamera(&camera);
  
  // Sync camera with config
  camera.setPosition(config.cameraPosition);
  camera.setZoom(config.cameraZoom);
}

void DebugUI::render(int fps, int entityCount,
                     const std::vector<EntityEditData> &entityCache) {
  if (!config.showDebugWindow) {
    return;
  }
  
  // Track dragging state to detect when drag ends
  static bool wasDragging = false;
  bool isDragging = gizmoManager.getState().isDragging;
  
  // Camera follow logic
  if (config.cameraFollowEnabled && 
      config.cameraFollowEntityIndex >= 0 && 
      config.cameraFollowEntityIndex < static_cast<int>(entityCache.size())) {
    auto& followEntity = entityCache[config.cameraFollowEntityIndex];
    if (followEntity.physicsComp) {
      // Get entity's render position
      glm::vec3 targetPos(followEntity.physicsComp->x, 
                         followEntity.physicsComp->y - followEntity.physicsComp->z,
                         camera.getPosition().z); // Keep camera Z
      
      // Smooth follow (assuming ~60 FPS, deltaTime ≈ 0.016)
      camera.followTarget(targetPos, 0.016f, config.cameraFollowSmoothness);
      config.cameraPosition = camera.getPosition();
    }
  }
  
  // Apply gizmo delta to selected entity if dragging
  if (gizmoManager.getState().targetType == GizmoManager::TargetType::ENTITY &&
      gizmoManager.getState().isDragging &&
      gizmoManager.getState().selectedIndex >= 0 &&
      gizmoManager.getState().selectedIndex < static_cast<int>(entityCache.size())) {
    glm::vec3 delta = gizmoManager.getCurrentDelta();
    auto& entity = entityCache[gizmoManager.getState().selectedIndex];
    if (entity.physicsComp) {
      // Apply delta to entity position
      glm::vec3 startPos = gizmoManager.getState().objectStartPos;
      entity.physicsComp->x = startPos.x + delta.x;
      // For Y, we need to account for the y-z rendering coordinate
      entity.physicsComp->y = startPos.y + delta.y + entity.physicsComp->z;
      // Z-axis for height (from world delta Z to physics Z)
      entity.physicsComp->z = startPos.z + delta.z;
      
      // Update gizmo's render position to keep it centered on sprite
      float centerOffsetX = entity.renderComp ? entity.renderComp->textureRect.z * 0.5f : 0.0f;
      float centerOffsetY = entity.renderComp ? entity.renderComp->textureRect.w * 0.5f : 0.0f;
      
      glm::vec3 currentRenderPos(entity.physicsComp->x + centerOffsetX, 
                                  entity.physicsComp->y - entity.physicsComp->z + centerOffsetY, 
                                  entity.physicsComp->z);
      gizmoManager.updateRenderPosition(currentRenderPos);
    }
  }
  
  // Detect when dragging stops and update physics start position
  if (wasDragging && !isDragging && 
      gizmoManager.getState().targetType == GizmoManager::TargetType::ENTITY &&
      gizmoManager.getState().selectedIndex >= 0 &&
      gizmoManager.getState().selectedIndex < static_cast<int>(entityCache.size())) {
    auto& entity = entityCache[gizmoManager.getState().selectedIndex];
    if (entity.physicsComp) {
      // Update the physics start position to the new position after drag
      glm::vec3 newPhysicsPos(entity.physicsComp->x, entity.physicsComp->y, entity.physicsComp->z);
      gizmoManager.updateEntityPosition(newPhysicsPos);
    }
  }
  
  // Update tracking variable for next frame
  wasDragging = isDragging;
  
  // Update entity position even when not dragging (for gizmo centering)
  if (gizmoManager.getState().targetType == GizmoManager::TargetType::ENTITY &&
      !gizmoManager.getState().isDragging &&
      gizmoManager.getState().selectedIndex >= 0 &&
      gizmoManager.getState().selectedIndex < static_cast<int>(entityCache.size())) {
    auto& entity = entityCache[gizmoManager.getState().selectedIndex];
    if (entity.physicsComp && entity.renderComp) {
      // Keep gizmo centered on sprite by updating render position
      float centerOffsetX = entity.renderComp->textureRect.z * 0.5f;
      float centerOffsetY = entity.renderComp->textureRect.w * 0.5f;
      
      glm::vec3 currentRenderPos(entity.physicsComp->x + centerOffsetX, 
                                  entity.physicsComp->y - entity.physicsComp->z + centerOffsetY, 
                                  entity.physicsComp->z);
      gizmoManager.updateRenderPosition(currentRenderPos);
    }
  }

  // Main menu bar for panel toggles
  renderMainMenuBar(fps, entityCount);
  
  // Render individual panels based on visibility flags
  if (showGBufferPanel) {
    ImGui::Begin("G-Buffer Views", &showGBufferPanel);
    renderGBufferViews();
    ImGui::End();
  }
  
  if (showSSAOPanel) {
    ImGui::Begin("SSAO Configuration", &showSSAOPanel);
    renderSSAOPanel();
    ImGui::End();
  }
  
  if (showLightingPanel) {
    ImGui::Begin("Lighting System", &showLightingPanel);
    renderLightingPanel();
    ImGui::End();
  }
  
  if (showDepthDebugPanel) {
    ImGui::Begin("Depth Debug", &showDepthDebugPanel);
    renderDepthDebug();
    ImGui::End();
  }
  
  if (showEntityEditorPanel) {
    ImGui::Begin("Entity Editor", &showEntityEditorPanel);
    renderEntityEditor(entityCache);
    ImGui::End();
  }
  
  if (showRenderingPanel) {
    ImGui::Begin("Rendering Settings", &showRenderingPanel);
    renderRenderingSettings();
    ImGui::End();
  }
  
  if (showGizmoPanel) {
    ImGui::Begin("Gizmo Controls", &showGizmoPanel);
    renderGizmoPanel();
    ImGui::End();
  }
  
  if (showCameraPanel) {
    ImGui::Begin("Camera Controls", &showCameraPanel);
    renderCameraPanel();
    ImGui::End();
  }
  
  // Update gizmo with mouse input
  ImVec2 mousePos = ImGui::GetMousePos();
  bool mousePressed = ImGui::IsMouseDown(ImGuiMouseButton_Left) && !ImGui::GetIO().WantCaptureMouse;
  gizmoManager.update(glm::vec2(mousePos.x, mousePos.y), mousePressed);
  
  // Render gizmo visuals on top of everything
  gizmoManager.renderUI();
}

void DebugUI::renderMainMenuBar(int fps, int entityCount) {
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("Panels")) {
      ImGui::MenuItem("Stats", nullptr, &showStatsPanel);
      ImGui::Separator();
      ImGui::MenuItem("G-Buffer Views", nullptr, &showGBufferPanel);
      ImGui::MenuItem("SSAO", nullptr, &showSSAOPanel);
      ImGui::MenuItem("Lighting", nullptr, &showLightingPanel);
      ImGui::MenuItem("Depth Debug", nullptr, &showDepthDebugPanel);
      ImGui::MenuItem("Entity Editor", nullptr, &showEntityEditorPanel);
      ImGui::MenuItem("Rendering", nullptr, &showRenderingPanel);
      ImGui::Separator();
      ImGui::MenuItem("Gizmos", nullptr, &showGizmoPanel);
      ImGui::MenuItem("Camera", nullptr, &showCameraPanel);
      ImGui::EndMenu();
    }
    
    if (showStatsPanel) {
      ImGui::SameLine(ImGui::GetWindowWidth() - 200);
      ImGui::Text("FPS: %d | Entities: %d", fps, entityCount);
    }
    
    ImGui::EndMainMenuBar();
  }
}

void DebugUI::renderGBufferViews() {
  if (ImGui::RadioButton("Composite",
                         config.currentDebugView ==
                             ApplicationConfig::DebugView::COMPOSITE))
    config.currentDebugView = ApplicationConfig::DebugView::COMPOSITE;
  if (ImGui::RadioButton("Albedo Only",
                         config.currentDebugView ==
                             ApplicationConfig::DebugView::ALBEDO))
    config.currentDebugView = ApplicationConfig::DebugView::ALBEDO;
  if (ImGui::RadioButton("Normals Only",
                         config.currentDebugView ==
                             ApplicationConfig::DebugView::NORMALS))
    config.currentDebugView = ApplicationConfig::DebugView::NORMALS;
  if (ImGui::RadioButton("Depth Only",
                         config.currentDebugView ==
                             ApplicationConfig::DebugView::DEPTH))
    config.currentDebugView = ApplicationConfig::DebugView::DEPTH;
  if (ImGui::RadioButton("Material Only",
                         config.currentDebugView ==
                             ApplicationConfig::DebugView::MATERIAL))
    config.currentDebugView = ApplicationConfig::DebugView::MATERIAL;
  if (ImGui::RadioButton("SSAO Only", config.currentDebugView ==
                                          ApplicationConfig::DebugView::SSAO))
    config.currentDebugView = ApplicationConfig::DebugView::SSAO;
}

void DebugUI::renderSSAOPanel() {
  ImGui::Checkbox("Enable SSAO", &config.enableSSAO);

  if (config.enableSSAO) {
    ImGui::SliderFloat("Radius", &config.ssaoRadius, 0.1f, 2.0f, "%.2f");
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
      ImGui::SetTooltip("Sampling radius for occlusion");
    }

    ImGui::SliderFloat("Bias", &config.ssaoBias, 0.001f, 0.1f, "%.4f");
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
      ImGui::SetTooltip("Prevents surface acne");
    }

    ImGui::SliderFloat("Power", &config.ssaoPower, 0.5f, 4.0f, "%.2f");
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
      ImGui::SetTooltip("Intensity multiplier for AO effect");
    }
  } else {
    ImGui::TextDisabled("SSAO is disabled");
  }
}

void DebugUI::renderLightingPanel() {
  // Ambient light
  ImGui::ColorEdit3("Ambient Light", &config.ambientLight.x);
  ImGui::Separator();

  // Light list
  ImGui::Text("Lights: %d / 10", (int)lightingMgr.getLightCount());
  if (ImGui::Button("Add Light")) {
    LightConfig newLight;
    lightingMgr.addLight(newLight);
  }
  ImGui::Separator();

  // Individual light controls - now using modular method
  for (size_t i = 0; i < lightingMgr.getLightCount(); i++) {
    auto &light = lightingMgr.getLight(i);
    bool lightOpen =
        ImGui::TreeNode((void *)(intptr_t)i, "Light %d - %s", (int)i,
                        light.type == 0   ? "Directional"
                        : light.type == 1 ? "Point"
                                          : "Spot");

    if (lightOpen) {
      renderLightControl(i, light);
      ImGui::TreePop();
      ImGui::Spacing();
    }
  }
}

void DebugUI::renderLightControl(size_t index, LightConfig &light) {
  ImGui::Checkbox("Enabled##light", &light.enabled);
  
  // Gizmo selection button
  bool isSelected = (gizmoManager.getState().targetType == GizmoManager::TargetType::LIGHT && 
                    gizmoManager.getState().selectedIndex == static_cast<int>(index));
  if (isSelected) {
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.4f, 0.2f, 1.0f));
  }
  
  if (ImGui::Button(isSelected ? "Selected for Gizmo" : "Select for Gizmo", ImVec2(-1, 0))) {
    gizmoManager.selectLight(static_cast<int>(index));
  }
  
  if (isSelected) {
    ImGui::PopStyleColor();
  }
  
  ImGui::Separator();

  // Light type
  const char *types[] = {"Directional", "Point", "Spot"};
  ImGui::Combo("Type", &light.type, types, 3);

  // Color and intensity
  ImGui::ColorEdit3("Color", &light.color.x);
  ImGui::SliderFloat("Intensity", &light.intensity, 0.0f, 10.0f, "%.2f");

  // Type-specific parameters
  if (light.type == 0) {
    // Directional - XZ horizontal direction + Y vertical angle
    ImGui::Text("Direction (XZ plane + Y angle):");
    ImGui::DragFloat3("##dir", &light.direction.x, 0.01f, -1.0f, 1.0f,
                      "X:%.2f Z:%.2f Y:%.2f");
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
      ImGui::SetTooltip(
          "X,Z = horizontal direction\nY = vertical angle (down/up)");
    }
  } else if (light.type == 1) {
    // Point light - XZ position + Y depth
    ImGui::Text("Position (XZ + Depth):");
    float pos[2] = {light.position.x, light.position.z};
    ImGui::DragFloat2("XZ##pos", pos, 1.0f, 0.0f, 1920.0f,
                      "X:%.0f Z:%.0f");
    light.position.x = pos[0];
    light.position.z = pos[1];

    ImGui::DragFloat("Depth (Y)##depth", &light.position.y, 1.0f, -100.0f,
                     500.0f, "%.0f");
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
      ImGui::SetTooltip("Higher Y = light is elevated above ground");
    }

    ImGui::SliderFloat("Radius", &light.radius, 1.0f, 1000.0f, "%.1f");
  } else {
    // Spot light - XZ position + Y depth + direction
    ImGui::Text("Position (XZ + Depth):");
    float pos[2] = {light.position.x, light.position.z};
    ImGui::DragFloat2("XZ##pos", pos, 1.0f, 0.0f, 1920.0f,
                      "X:%.0f Z:%.0f");
    light.position.x = pos[0];
    light.position.z = pos[1];

    ImGui::DragFloat("Depth (Y)##depth", &light.position.y, 1.0f, -100.0f,
                     500.0f, "%.0f");

    ImGui::Text("Direction:");
    ImGui::DragFloat3("##dir", &light.direction.x, 0.01f, -1.0f, 1.0f,
                      "X:%.2f Z:%.2f Y:%.2f");
    ImGui::SliderFloat("Radius", &light.radius, 1.0f, 1000.0f, "%.1f");
    ImGui::SliderFloat("Cutoff Angle", &light.cutoffAngle, 5.0f, 90.0f,
                       "%.1f");
  }

  // Remove button
  if (ImGui::Button("Remove Light")) {
    lightingMgr.removeLight(index);
  }
}

void DebugUI::renderDepthDebug() {
  ImGui::SliderFloat("Depth Multiplier", &config.globalDepthMultiplier,
                     0.001f, 0.1f, "%.4f");
  ImGui::Text("Current: %.4f", config.globalDepthMultiplier);
  ImGui::TextDisabled("Note: Not yet applied to shader");
}

void DebugUI::renderEntityEditor(
    const std::vector<EntityEditData> &entityCache) {
  for (size_t i = 0; i < entityCache.size(); i++) {
    auto data = entityCache[i]; // Non-const copy for editing

    if (ImGui::TreeNode((void *)(intptr_t)i, "Entity %d - %s", (int)i,
                        data.textureName.c_str())) {
        ImGui::Separator();
        
        // Gizmo selection button
        bool isSelected = (gizmoManager.getState().targetType == GizmoManager::TargetType::ENTITY && 
                          gizmoManager.getState().selectedIndex == i);
        if (isSelected) {
          ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.8f, 0.2f, 1.0f));
        }
        
        if (ImGui::Button(isSelected ? "Selected for Gizmo" : "Select for Gizmo", ImVec2(-1, 0))) {
          // Physics position (for delta calculation)
          glm::vec3 physicsPos(data.physicsComp->x, data.physicsComp->y, data.physicsComp->z);
          
          // Render position (for gizmo drawing, centered on sprite)
          float centerOffsetX = data.renderComp ? data.renderComp->textureRect.z * 0.5f : 0.0f;
          float centerOffsetY = data.renderComp ? data.renderComp->textureRect.w * 0.5f : 0.0f;
          glm::vec3 renderPos(data.physicsComp->x + centerOffsetX, 
                             data.physicsComp->y - data.physicsComp->z + centerOffsetY, 
                             data.physicsComp->z);
          
          gizmoManager.selectEntity(i, physicsPos, renderPos);
        }
        
        if (isSelected) {
          ImGui::PopStyleColor();
        }
        
        ImGui::Separator();
        
        // Modular rendering
        renderEntityTransform(data, i);
        ImGui::Separator();
        renderEntityMaterial(data);
        ImGui::Separator();
        renderEntityTextures(data);

      ImGui::TreePop();
      ImGui::Spacing();
    }
  }
}

void DebugUI::renderEntityTransform(EntityEditData &data, int) {
  ImGui::SetNextItemOpen(true, ImGuiCond_Once);
  if (ImGui::CollapsingHeader("Transform")) {
    // Position
    ImGui::Text("Position:");
    ImGui::DragFloat2("##pos", &data.physicsComp->x, 1.0f, -2000.0f,
                      2000.0f, "%.1f");

    // Z-Position (depth)
    ImGui::Text("Z-Position (Depth):");
    ImGui::SliderFloat("##z", &data.physicsComp->z, -10.0f, 10.0f, "%.2f");
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
      ImGui::SetTooltip("Lower = farther back, Higher = closer");
    }

    // Scale
    ImGui::Text("Scale:");
    ImGui::SliderFloat("##scale", &data.renderComp->scale, 0.1f, 5.0f,
                       "%.2f");

    // Height multiplier
    ImGui::Text("Height Multiplier (Parallax):");
    ImGui::SliderFloat("##height", &data.renderComp->height, 0.0f, 50.0f,
                       "%.1f");
  }
}

void DebugUI::renderEntityMaterial(EntityEditData &data) {
  if (ImGui::CollapsingHeader("PBR Material")) {
    ImGui::Text("Material Properties:");
    
    // Roughness
    ImGui::SliderFloat("Roughness", &data.renderComp->roughness, 0.0f, 1.0f, "%.2f");
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
      ImGui::SetTooltip("0 = Smooth/Glossy, 1 = Rough/Matte\nUsed if no material map");
    }
    
    // Metalness
    ImGui::SliderFloat("Metalness", &data.renderComp->metalness, 0.0f, 1.0f, "%.2f");
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
      ImGui::SetTooltip("0 = Dielectric, 1 = Metal\nUsed if no material map");
    }
    
    // Translucency
    ImGui::SliderFloat("Translucency", &data.renderComp->translucency, 0.0f, 1.0f, "%.2f");
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
      ImGui::SetTooltip("Subsurface scattering effect (experimental)");
    }
    
    // Material map status
    ImGui::Separator();
    if (!data.renderComp->materialTextureName.empty()) {
      ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Material Map: Active");
      ImGui::Text("%s", data.renderComp->materialTextureName.c_str());
      ImGui::TextDisabled("(Texture overrides manual values)");
    } else {
      ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.0f, 1.0f), "Using Manual Values");
      ImGui::TextDisabled("(No material map loaded)");
    }
  }
}

void DebugUI::renderEntityTextures(const EntityEditData &data) {
  if (ImGui::CollapsingHeader("Textures")) {
    ImGui::Text("Texture Size: %.0fx%.0f", data.renderComp->textureRect.z,
                data.renderComp->textureRect.w);
    
    ImGui::Separator();
    ImGui::Text("Albedo: %s", data.textureName.c_str());
    
    if (!data.renderComp->normalTextureName.empty()) {
      ImGui::Text("Normal: %s", data.renderComp->normalTextureName.c_str());
    } else {
      ImGui::TextDisabled("Normal: (none)");
    }
    
    if (!data.renderComp->depthTextureName.empty()) {
      ImGui::Text("Depth: %s", data.renderComp->depthTextureName.c_str());
    } else {
      ImGui::TextDisabled("Depth: (none)");
    }
    
    if (!data.renderComp->materialTextureName.empty()) {
      ImGui::Text("Material: %s", data.renderComp->materialTextureName.c_str());
    } else {
      ImGui::TextDisabled("Material: (none)");
    }
  }
}

void DebugUI::renderRenderingSettings() {
  ImGui::SliderFloat("Gamma Correction", &config.gammaCorrection, 0.5f, 3.0f,
                     "%.2f");
  ImGui::SameLine();
  ImGui::TextDisabled("(?)");
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("Lower = brighter, Higher = darker\nStandard: 2.2, "
                      "Stylized: 0.8-1.5");
  }
  if (ImGui::Button("Reset Gamma to 0.8")) {
    config.gammaCorrection = 0.8f;
  }
}

void DebugUI::renderGizmoPanel() {
  gizmoManager.renderGizmoPanel();
}

void DebugUI::renderCameraPanel() {
  ImGui::Text("Camera Controls");
  ImGui::Separator();
  
  // Position controls
  glm::vec3 pos = camera.getPosition();
  if (ImGui::DragFloat3("Position", &pos.x, 10.0f)) {
    camera.setPosition(pos);
    config.cameraPosition = pos;
  }
  
  // Zoom controls
  float zoom = camera.getZoom();
  if (ImGui::SliderFloat("Zoom", &zoom, 0.1f, 5.0f, "%.2f")) {
    camera.setZoom(zoom);
    config.cameraZoom = zoom;
  }
  
  // Camera info
  ImGui::Separator();
  ImGui::Text("Camera Info");
  ImGui::Text("Ortho Size: %.1f", camera.getOrthoSize());
  
  // Quick presets
  ImGui::Separator();
  ImGui::Text("Quick Presets");
  
  if (ImGui::Button("Reset Position")) {
    camera.setPosition(glm::vec3(960.0f, 540.0f, 1000.0f));
    config.cameraPosition = camera.getPosition();
    config.cameraFollowEnabled = false;
  }
  
  ImGui::SameLine();
  if (ImGui::Button("Reset Zoom")) {
    camera.setZoom(1.0f);
    config.cameraZoom = 1.0f;
  }
  
  // Camera follow
  ImGui::Separator();
  ImGui::Text("Camera Follow");
  
  if (ImGui::Checkbox("Follow Entity", &config.cameraFollowEnabled)) {
    if (!config.cameraFollowEnabled) {
      // Store current camera position when disabling follow
      config.cameraPosition = camera.getPosition();
    }
  }
  
  if (config.cameraFollowEnabled) {
    ImGui::SliderInt("Entity Index", &config.cameraFollowEntityIndex, 0, 10, "%d");
    ImGui::SliderFloat("Smoothness", &config.cameraFollowSmoothness, 1.0f, 20.0f, "%.1f");
    ImGui::TextDisabled("Higher = smoother/slower");
  }
  
  // Movement controls
  ImGui::Separator();
  ImGui::Text("Manual Movement");
  ImGui::TextDisabled("Use WASD or arrow keys to move camera");
  ImGui::TextDisabled("Mouse wheel to zoom in/out");
  if (config.cameraFollowEnabled) {
    ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f), "Camera follow is active");
  }
}

} // namespace dunkan
