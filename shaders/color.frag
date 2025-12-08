#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 vertex;
layout(location = 3) in vec3 fragNormal;

layout(location = 0) out vec4 outColor;      // Albedo
layout(location = 1) out vec4 outNormal;     // Normal + Roughness
layout(location = 2) out vec4 outDepth;      // Heightmap RGB + calculated depth
layout(location = 3) out vec4 outMaterial;   // Material (Roughness, Metalness, AO)

layout(binding = 1) uniform sampler2D color_map;
layout(binding = 2) uniform sampler2D depth_map;
layout(binding = 3) uniform sampler2D normal_map;
layout(binding = 4) uniform sampler2D material_map;

layout(push_constant) uniform PushConstants {
    mat4 model;
    float z_position;
    float height;
    bool useDepthMap;
} pushConstants;

void main()
{
    vec4 color_pixel = texture(color_map, fragTexCoord);
    
    // Discard transparent pixels
    if (color_pixel.a < 0.1) discard;
    
    // Output 0: Albedo color
    outColor = color_pixel;
    
    // Heightmap sampling
    vec4 heightmap_pixel = vec4(0.0);
    float height_pixel = 0.0;
    if(pushConstants.useDepthMap){
        heightmap_pixel = texture(depth_map, fragTexCoord);
        height_pixel = heightmap_pixel.r * pushConstants.height;
    }
    float z_pixel = height_pixel + pushConstants.z_position;
    
    // Set fragment depth for proper sprite ordering
    // Higher red channel in depth map + higher z_position = closer to camera (lower depth value)
    // Depth range [0, 1] where 0 = near, 1 = far
    gl_FragDepth = 1.0 - color_pixel.a * (0.5 + z_pixel * 0.001);
    
    // Sample normal map
    vec3 normalMapSample = texture(normal_map, fragTexCoord).rgb;
    vec3 normal = fragNormal;
    
    // Check if normal map has meaningful data (not default flat normal)
    // Default/flat normal map would be (0.5, 0.5, 1.0) in [0,1] or (0, 0, 1) in [-1,1]
    bool hasNormalMap = length(normalMapSample - vec3(0.5, 0.5, 1.0)) > 0.01;
    
    if (hasNormalMap) {
        // Transform normal from [0,1] to [-1,1]
        vec3 tangentNormal = normalize(normalMapSample * 2.0 - 1.0);
        
        // Build TBN matrix (approximation for 2.5D sprite rendering)
        vec3 N = normal;
        vec3 T = normalize(cross(vec3(0.0, 1.0, 0.0), N)); // Tangent (perpendicular to N and up)
        if (length(T) < 0.01) { // If N is parallel to up, use different vector
            T = normalize(cross(vec3(1.0, 0.0, 0.0), N));
        }
        vec3 B = normalize(cross(N, T)); // Bitangent
        
        // TBN matrix transforms from tangent space to world space
        mat3 TBN = mat3(T, B, N);
        normal = normalize(TBN * tangentNormal);
    }
    
    // Output 1: Store normal in [0,1] range
    outNormal = vec4(normal * 0.5 + 0.5, 0.5);
    
    // Output 2: Store ACTUAL heightmap RGB in rgb, linear depth in alpha
    outDepth = vec4(heightmap_pixel.rgb, z_pixel / 100.0);
    
    // Output 3: Material Properties (R: Roughness, G: Metalness, B: AO)
    // Sample from material map, or use default values
    vec4 materialSample = texture(material_map, fragTexCoord);
    
    // Check if material map has meaningful data (not default white/gray)
    bool hasMaterialMap = length(materialSample.rgb - vec3(1.0)) > 0.01 || 
                          length(materialSample.rgb - vec3(0.5)) > 0.01;
    
    if (hasMaterialMap) {
        outMaterial = vec4(materialSample.rgb, 1.0);
    } else {
        // Default: moderate roughness, no metalness, full AO
        outMaterial = vec4(0.5, 0.0, 1.0, 1.0);
    }
}