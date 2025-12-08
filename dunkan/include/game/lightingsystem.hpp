#pragma once
#include <glm/glm.hpp>

// Light structure for shader UBO
struct Light {
    alignas(16) glm::vec4 position;    // w = type (0=directional, 1=point, 2=spot)
    alignas(16) glm::vec4 direction;   // w = radius
    alignas(16) glm::vec4 color;       // w = intensity
    alignas(16) glm::vec4 params;      // x=cutoffAngle, y=outerCutoff, z=attenuation, w=unused
};

// Lighting uniform buffer object
struct LightingUBO {
    alignas(16) glm::vec4 ambientLight;
    alignas(16) glm::vec3 viewPos;
    alignas(4)  int numLights;
    Light lights[10];  // Max 10 lights
};

// SSAO uniform buffer (for ImGui controls)
struct SSAOParams {
    float radius = 0.5f;
    float bias = 0.025f;
    float power = 1.0f;
    int kernelSize = 64;
};
