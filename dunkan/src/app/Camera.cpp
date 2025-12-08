#include "app/Camera.hpp"

namespace dunkan {

Camera::Camera(float viewportWidth, float viewportHeight)
  : viewportWidth(viewportWidth), viewportHeight(viewportHeight) {
  update();
}

void Camera::update() {
  updateProjectionMatrix();
  updateViewMatrix();
  viewProjectionMatrix = projectionMatrix * viewMatrix;
}

void Camera::setViewportSize(float width, float height) {
  viewportWidth = width;
  viewportHeight = height;
  update();
}

glm::vec3 Camera::screenToWorld(const glm::vec2& screenPos) const {
  // Convert from screen space (origin top-left, Y-down) to NDC (origin center, Y-up)
  // Screen space: (0, 0) is top-left, (viewportWidth, viewportHeight) is bottom-right
  // NDC: (-1, -1) is bottom-left, (1, 1) is top-right
  
  glm::vec2 ndc;
  ndc.x = (2.0f * screenPos.x / viewportWidth) - 1.0f;
  ndc.y = 1.0f - (2.0f * screenPos.y / viewportHeight); // Flip Y
  
  // For orthographic projection, we can directly convert from NDC to world space
  // using the inverse view-projection matrix
  glm::vec4 worldPos4 = glm::inverse(viewProjectionMatrix) * glm::vec4(ndc, 0.0f, 1.0f);
  
  return glm::vec3(worldPos4);
}

glm::vec2 Camera::worldToScreen(const glm::vec3& worldPos) const {
  // Convert from world space to clip space
  glm::vec4 clipPos = viewProjectionMatrix * glm::vec4(worldPos, 1.0f);
  
  // Convert from clip space to NDC (perspective divide)
  glm::vec3 ndc = glm::vec3(clipPos) / clipPos.w;
  
  // Convert from NDC to screen space
  glm::vec2 screenPos;
  screenPos.x = (ndc.x + 1.0f) * 0.5f * viewportWidth;
  screenPos.y = (1.0f - ndc.y) * 0.5f * viewportHeight; // Flip Y
  
  return screenPos;
}

void Camera::updateProjectionMatrix() {
  // Orthographic projection for isometric view
  // The visible area depends on zoom level
  float aspect = viewportWidth / viewportHeight;
  float height = orthoSize / zoom;
  float width = height * aspect;
  
  // Create orthographic projection
  // For isometric view, we want to see everything from near to far plane
  float nearPlane = -1000.0f;
  float farPlane = 1000.0f;
  
  projectionMatrix = glm::ortho(-width, width, -height, height, nearPlane, farPlane);
}

void Camera::updateViewMatrix() {
  // Simple view matrix - looking down the Z-axis
  // For isometric, we might want to rotate the view, but for now keep it simple
  glm::vec3 target = position - glm::vec3(0.0f, 0.0f, 1.0f); // Look down Z
  glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f); // Y is up
  
  viewMatrix = glm::lookAt(position, target, up);
}

void Camera::followTarget(const glm::vec3& targetPos, float deltaTime, float smoothness) {
  // Smoothly interpolate camera position towards target (only XY, keep Z constant)
  float lerpFactor = 1.0f - std::exp(-smoothness * deltaTime);
  
  position.x += (targetPos.x - position.x) * lerpFactor;
  position.y += (targetPos.y - position.y) * lerpFactor;
  // Keep Z position constant (camera distance)
  
  update();
}

} // namespace dunkan
