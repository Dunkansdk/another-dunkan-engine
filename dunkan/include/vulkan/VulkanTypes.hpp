#pragma once

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <array>
#include <cstdint>

// Color structure to replace sf::Color
struct Color4 {
    uint8_t r, g, b, a;
    
    Color4() : r(255), g(255), b(255), a(255) {}
    Color4(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) 
        : r(r), g(g), b(b), a(a) {}
    
    glm::vec4 normalized() const {
        return glm::vec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
    }
    
    static const Color4 White;
    static const Color4 Black;
    static const Color4 Red;
    static const Color4 Green;
    static const Color4 Blue;
};

inline const Color4 Color4::White = Color4(255, 255, 255, 255);
inline const Color4 Color4::Black = Color4(0, 0, 0, 255);
inline const Color4 Color4::Red = Color4(255, 0, 0, 255);
inline const Color4 Color4::Green = Color4(0, 255, 0, 255);
inline const Color4 Color4::Blue = Color4(0, 0, 255, 255);

// Vertex structure for sprite rendering
struct Vertex {
    glm::vec2 pos;
    glm::vec2 texCoord;
    glm::vec3 color;
    glm::vec3 normal;
    
    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }
    
    static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions{};
        
        // Position
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);
        
        // Texture coordinates
        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, texCoord);
        
        // Color
        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, color);
        
        // Normal
        attributeDescriptions[3].binding = 0;
        attributeDescriptions[3].location = 3;
        attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[3].offset = offsetof(Vertex, normal);
        
        return attributeDescriptions;
    }
};

// Matrix 3x3 structure
struct Mat3x3 {
    float values[9];
    
    Mat3x3() {
        for(int i = 0; i < 9; i++) values[i] = 0.0f;
    }
    
    Mat3x3(float m00, float m01, float m02,
           float m10, float m11, float m12,
           float m20, float m21, float m22) {
        values[0] = m00; values[1] = m01; values[2] = m02;
        values[3] = m10; values[4] = m11; values[5] = m12;
        values[6] = m20; values[7] = m21; values[8] = m22;
    }
};
