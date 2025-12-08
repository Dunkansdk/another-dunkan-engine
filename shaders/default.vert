#version 450

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec3 inNormal;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 vertex;
layout(location = 3) out vec3 fragNormal;

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} ubo;

layout(push_constant) uniform PushConstants {
    mat4 model;
    float z_position;
} pushConstants;

void main() {
    vec4 worldPos = pushConstants.model * vec4(inPosition, 0.0, 1.0);
    worldPos.y -= pushConstants.z_position;  // SUBTRACT - higher Z = lower Y = farther back  
    gl_Position = ubo.proj * ubo.view * worldPos;
    vertex = worldPos.xyz;
    fragColor = inColor;
    fragTexCoord = inTexCoord;
    
    // Transform normal to world space (assuming uniform scaling)
    mat3 normalMatrix = mat3(pushConstants.model);
    fragNormal = normalize(normalMatrix * inNormal);
}