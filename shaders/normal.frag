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
    vec4 color_pixel = texture(color_map, fragTexCoord);
    
    // Discard transparent pixels - prevents lighting artifacts
    if (color_pixel.a < 0.1) discard;
    
    // Sample depth map and calculate height
    float height_pixel = 0.0; 
    if(pushConstants.useDepthMap){
        vec4 depth_pixel = texture(depth_map, fragTexCoord);
        height_pixel = depth_pixel.r * pushConstants.height;
        
        // Discard pixels with no height information (depth map alpha channel)
        // This prevents rendering normals for non-visible parts based on heightmap
        if (depth_pixel.r < 0.01) discard;
    }
    
    // Calculate normal direction
    vec3 direction = vec3(0.0, 0.0, 1.0);
    if(pushConstants.useNormalMap){
        direction = -1.0 + 2.0 * texture(normal_map, fragTexCoord).rgb;
    }
    
    // Calculate final depth for depth buffer
    float z_pixel = height_pixel + pushConstants.z_position;
    gl_FragDepth = 1.0 - color_pixel.a * (0.5 + z_pixel * 0.001);
    
    // Output normal in [0,1] range
    outColor.rgb = 0.5 + direction * 0.5;
    outColor.a = color_pixel.a;
}