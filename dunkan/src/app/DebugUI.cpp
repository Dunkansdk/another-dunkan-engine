#include "app/DebugUI.hpp"
#include "game/components/physicscomponent.hpp"
#include "game/components/rendercomponent.hpp"
#include <imgui.h>


namespace dunkan {

DebugUI::DebugUI(ApplicationConfig &config, LightingManager &lightingMgr)
    : config(config), lightingMgr(lightingMgr) {}

void DebugUI::render(int fps, int entityCount,
                     const std::vector<EntityEditData> &entityCache) {
  if (!config.showDebugWindow) {
    return;
  }

  ImGui::Begin("Debug Controls", &config.showDebugWindow);

  ImGui::Text("FPS: %d | Entities: %d", fps, entityCount);
  ImGui::Separator();

  renderGBufferViews();
  renderSSAOPanel();
  renderLightingPanel();
  renderDepthDebug();
  renderEntityEditor(entityCache);
  renderRenderingSettings();

  ImGui::End();
}

void DebugUI::renderGBufferViews() {
  if (ImGui::CollapsingHeader("G-Buffer Views",
                              ImGuiTreeNodeFlags_DefaultOpen)) {
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
}

void DebugUI::renderSSAOPanel() {
  if (ImGui::CollapsingHeader("SSAO Configuration")) {
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
}

void DebugUI::renderLightingPanel() {
  if (ImGui::CollapsingHeader("Lighting System",
                              ImGuiTreeNodeFlags_DefaultOpen)) {
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

    // Individual light controls
    for (size_t i = 0; i < lightingMgr.getLightCount(); i++) {
      auto &light = lightingMgr.getLight(i);
      bool lightOpen =
          ImGui::TreeNode((void *)(intptr_t)i, "Light %d - %s", (int)i,
                          light.type == 0   ? "Directional"
                          : light.type == 1 ? "Point"
                                            : "Spot");

      if (lightOpen) {
        ImGui::Checkbox("Enabled##light", &light.enabled);
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
          lightingMgr.removeLight(i);
          ImGui::TreePop();
          break;
        }

        ImGui::TreePop();
        ImGui::Spacing();
      }
    }
  }
}

void DebugUI::renderDepthDebug() {
  if (ImGui::CollapsingHeader("Depth Debug")) {
    ImGui::SliderFloat("Depth Multiplier", &config.globalDepthMultiplier,
                       0.001f, 0.1f, "%.4f");
    ImGui::Text("Current: %.4f", config.globalDepthMultiplier);
    ImGui::TextDisabled("Note: Not yet applied to shader");
  }
}

void DebugUI::renderEntityEditor(
    const std::vector<EntityEditData> &entityCache) {
  if (ImGui::CollapsingHeader("Entity Editor")) {
    for (size_t i = 0; i < entityCache.size(); i++) {
      const auto &data = entityCache[i];

      if (ImGui::TreeNode((void *)(intptr_t)i, "Entity %d - %s", (int)i,
                          data.textureName.c_str())) {
        ImGui::Separator();

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

        // Texture info (read-only)
        ImGui::Separator();
        ImGui::Text("Texture Size: %.0fx%.0f", data.renderComp->textureRect.z,
                    data.renderComp->textureRect.w);
        ImGui::Text("Normal: %s", data.renderComp->normalTextureName.c_str());
        if (!data.renderComp->depthTextureName.empty()) {
          ImGui::Text("Depth Map: %s",
                      data.renderComp->depthTextureName.c_str());
        }

        ImGui::TreePop();
        ImGui::Spacing();
      }
    }
  }
}

void DebugUI::renderRenderingSettings() {
  if (ImGui::CollapsingHeader("Rendering Settings")) {
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
}

} // namespace dunkan
