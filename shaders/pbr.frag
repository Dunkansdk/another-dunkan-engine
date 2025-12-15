#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 vertex;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D color_map;
layout(binding = 2) uniform sampler2D depth_map;
layout(binding = 4) uniform sampler2D material_map;

layout(push_constant) uniform PushConstants {
    mat4 model;
    float z_position;
    float height;
    bool useDepthMap;
    bool enable_materialMap;
    float p_roughness;
    float p_metalness;
    float p_translucency;
} pushConstants;

void main()
{
    vec4 color_pixel = texture(color_map, fragTexCoord);
    vec4 material_pixel = vec4(pushConstants.p_roughness, pushConstants.p_metalness, pushConstants.p_translucency, color_pixel.a);
    if(pushConstants.enable_materialMap){
        material_pixel = texture(material_map, fragTexCoord);
    }
    float height_pixel = 0.0; 
    if(pushConstants.useDepthMap){
        vec4 depth_pixel = texture(depth_map, fragTexCoord);
        height_pixel = (depth_pixel.r + depth_pixel.g + depth_pixel.b) * 0.33 * pushConstants.height;
    }
    float z_pixel = height_pixel + pushConstants.z_position;
    if(color_pixel.a < 0.9)
        color_pixel.a = 0.0;
    gl_FragDepth = 1.0 - color_pixel.a * (0.5 + z_pixel * 0.001);
    outColor = vec4(fragColor, 1.0) * material_pixel; 
}