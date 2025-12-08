#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace dunkan {

/**
 * @brief Camera system for orthographic (isometric) projection
 *
 * Manages view and projection matrices for the isometric game view.
 * Provides screen-to-world and world-to-screen coordinate conversion.
 */
class Camera {
public:
  Camera(float viewportWidth, float viewportHeight);
  ~Camera() = default;

  /**
   * @brief Update camera matrices (call after changing position/zoom)
   */
  void update();

  /**
   * @brief Set viewport dimensions (call on window resize)
   */
  void setViewportSize(float width, float height);

  /**
   * @brief Convert screen coordinates to world coordinates
   * @param screenPos Screen position (pixels, origin top-left)
   * @return World position (origin center, Y-up)
   */
  glm::vec3 screenToWorld(const glm::vec2& screenPos) const;

  /**
   * @brief Convert world coordinates to screen coordinates
   * @param worldPos World position
   * @return Screen position (pixels, origin top-left)
   */
  glm::vec2 worldToScreen(const glm::vec3& worldPos) const;

  /**
   * @brief Get view matrix
   */
  const glm::mat4& getViewMatrix() const { return viewMatrix; }

  /**
   * @brief Get projection matrix
   */
  const glm::mat4& getProjectionMatrix() const { return projectionMatrix; }

  /**
   * @brief Get view-projection matrix
   */
  const glm::mat4& getViewProjectionMatrix() const { return viewProjectionMatrix; }

  /**
   * @brief Get camera position (world space)
   */
  const glm::vec3& getPosition() const { return position; }

  /**
   * @brief Set camera position (world space)
   */
  void setPosition(const glm::vec3& pos) { position = pos; update(); }

  /**
   * @brief Move camera by offset
   */
  void move(const glm::vec3& offset) { position += offset; update(); }

  /**
   * @brief Get zoom level (1.0 = default, higher = zoomed in)
   */
  float getZoom() const { return zoom; }

  /**
   * @brief Set zoom level
   */
  void setZoom(float z) { zoom = glm::clamp(z, minZoom, maxZoom); update(); }

  /**
   * @brief Get orthographic size (half-height of visible area)
   */
  float getOrthoSize() const { return orthoSize / zoom; }

  /**
   * @brief Follow a target position (smoothly move camera towards it)
   * @param targetPos Target position in world space
   * @param deltaTime Time since last frame
   * @param smoothness How smooth the follow is (higher = smoother/slower)
   */
  void followTarget(const glm::vec3& targetPos, float deltaTime, float smoothness = 5.0f);

  /**
   * @brief Set camera to follow target immediately (no smoothing)
   */
  void setFollowTarget(const glm::vec3& targetPos) { 
    position.x = targetPos.x;
    position.y = targetPos.y;
    update(); 
  }

private:
  // Camera state
  glm::vec3 position{960.0f, 540.0f, 1000.0f}; // Camera position at screen center
  float zoom{1.0f};                            // Zoom level
  float orthoSize{540.0f};                     // Half-height of orthographic view (1080/2)
  
  // Zoom constraints
  float minZoom{0.1f};
  float maxZoom{5.0f};

  // Viewport
  float viewportWidth{1920.0f};
  float viewportHeight{1080.0f};

  // Matrices
  glm::mat4 viewMatrix{1.0f};
  glm::mat4 projectionMatrix{1.0f};
  glm::mat4 viewProjectionMatrix{1.0f};

  void updateProjectionMatrix();
  void updateViewMatrix();
};

} // namespace dunkan
