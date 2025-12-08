#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 vertex;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D color_map;
layout(binding = 2) uniform sampler2D depth_map;
layout(binding = 3) uniform sampler2D normal_map;

layout(push_constant) uniform PushConstants {
    mat4 model;
    float z_position;
    float height;
    bool useDepthMap;
    bool useNormalMap;
} pushConstants;

void main()
{
    float color_alpha = texture(color_map, fragTexCoord).a;
    vec3 direction = vec3(0.0, 0.0, 1.0);
    if(pushConstants.useNormalMap){
        direction = -1.0 + 2.0 * texture(normal_map, fragTexCoord).rgb;
    }
    float height_pixel = 0.0; 
    if(pushConstants.useDepthMap){
        vec4 depth_pixel = texture(depth_map, fragTexCoord);
        height_pixel = (depth_pixel.r + depth_pixel.g + depth_pixel.b) * 0.33 * pushConstants.height;
    }
    float z_pixel = height_pixel + pushConstants.z_position;
    gl_FragDepth = 1.0 - color_alpha * (0.5 + z_pixel * 0.001);
    outColor.rgb = 0.5 + direction * 0.5;
    outColor.a = color_alpha;
}