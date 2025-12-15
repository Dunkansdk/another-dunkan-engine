#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 vertex;

layout(location = 0) out vec4 outColor;

// Textures
layout(binding = 1) uniform sampler2D color_map;
layout(binding = 2) uniform sampler2D normal_map;
layout(binding = 3) uniform sampler2D depth_map;
layout(binding = 4) uniform sampler2D material_map;
layout(binding = 5) uniform sampler2D map_brdflut;

// Uniforms (simplified version - full lighting will be added later)
layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
    vec4 ambient_light;
    vec2 screen_ratio;
    vec2 view_shift;
    vec3 view_pos;
    float p_exposure;
    int debug_screen;
    bool enable_sRGB;
    int nbr_lights;
} ubo;

const float PI = 3.14159265359;

void main()
{
    vec4 color_pixel = texture(color_map, fragTexCoord);
    if(color_pixel.a > 0.1) {
        vec4 normal_pixel = texture(normal_map, fragTexCoord);
        vec4 depth_pixel = texture(depth_map, fragTexCoord);
        vec4 material_pixel = texture(material_map, fragTexCoord);
        
        vec3 direction = -1.0 + 2.0 * normal_pixel.rgb;
        float height_pixel = (0.5 - (depth_pixel.r + depth_pixel.g / 256.0 + depth_pixel.b / 65536.0)) * 1000.0;
        
        outColor.a = color_pixel.a;
        if(ubo.enable_sRGB)
            color_pixel.rgb = pow(color_pixel.rgb, vec3(2.2));
        
        // Simplified lighting - just ambient for now
        vec3 irradianceAmbient = ubo.ambient_light.rgb * ubo.ambient_light.a;
        outColor.rgb = color_pixel.rgb * irradianceAmbient;
        
        // TODO: Add full PBR lighting calculations here
        // This is a simplified version to get the build working
        
        // Exposure and gamma correction
        outColor.rgb = vec3(1.0) - exp(-outColor.rgb * ubo.p_exposure);
        if(ubo.enable_sRGB)
            outColor.rgb = pow(outColor.rgb, vec3(1.0 / 2.2));
        
        // Debug screens
        if(ubo.debug_screen == 1) { outColor.rgb = color_pixel.rgb; }
        if(ubo.debug_screen == 2) { outColor.rgb = normal_pixel.rgb; }
        if(ubo.debug_screen == 3) {
            outColor.rgb = vec3(height_pixel * 0.001, height_pixel * 0.001, height_pixel * 0.001);
        }
        if(ubo.debug_screen == 4) { outColor.rgb = material_pixel.rgb; }
    }
}