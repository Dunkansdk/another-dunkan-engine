#version 450

layout (location = 0) in vec2 inUV;
layout (location = 0) out vec4 outColor;

// G-Buffer inputs
layout (binding = 0) uniform sampler2D samplerColor;    // Albedo
layout (binding = 1) uniform sampler2D samplerNormal;   // Normal
layout (binding = 2) uniform sampler2D samplerDepth;    // Depth
layout (binding = 3) uniform sampler2D samplerMaterial; // Material (R=roughness, G=metalness, B=AO)
layout (binding = 4) uniform sampler2D samplerSSAO;     // SSAO

// Light structure matching C++ UBO
struct Light {
    vec4 position;    // w = type (0=directional, 1=point, 2=spot)
    vec4 direction;   // w = radius
    vec4 color;       // w = intensity
    vec4 params;      // x=cutoffAngle, y=outerCutoff, z=attenuation, w=unused
};

// Lighting uniform buffer
layout (binding = 5) uniform LightingUBO {
    vec4 ambientLight;
    vec3 viewPos;
    int numLights;
    vec4 viewOffsetPadded;    // xy = viewOffset, zw = padding (for proper alignment before array)
    Light lights[10];
} lighting;

layout(push_constant) uniform PushConstants {
    int debugViewMode;  // 0=composite, 1=albedo, 2=normals, 3=depth, 4=material, 5=ssao
    int enableSSAO;     // 0=disabled, 1=enabled
    float gamma;        // Gamma correction value
} push;

const float PI = 3.14159265359;

// ACES Filmic Tone Mapping
// Approximation by Krzysztof Narkowicz
vec3 ACESFilmic(vec3 x) {
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((x*(a*x+b))/(x*(c*x+d)+e), 0.0, 1.0);
}

// PBR Functions
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    
    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    
    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    
    return ggx1 * ggx2;
}

vec3 calculateLight(Light light, vec3 fragPos, vec3 N, vec3 V, vec3 albedo, float roughness, float metallic, vec3 F0) {
    int lightType = int(light.position.w);
    vec3 L;
    float attenuation = 1.0;
    
    if (lightType == 0) {
        // Directional light
        L = normalize(-light.direction.xyz);
    } else if (lightType == 1) {
        // Point light
        vec3 lightPos = light.position.xyz;
        L = normalize(lightPos - fragPos);
        float distance = length(lightPos - fragPos);
        float radius = light.direction.w;
        attenuation = 1.0 / (1.0 + (distance / radius) * (distance / radius));
    } else {
        // Spot light
        vec3 lightPos = light.position.xyz;
        L = normalize(lightPos - fragPos);
        float distance = length(lightPos - fragPos);
        float radius = light.direction.w;
        attenuation = 1.0 / (1.0 + (distance / radius) * (distance / radius));
        
        // Spot cone
        vec3 spotDir = normalize(light.direction.xyz);
        float theta = dot(L, -spotDir);
        float cutoff = cos(radians(light.params.x));
        float outerCutoff = cos(radians(light.params.x + 5.0));
        float epsilon = cutoff - outerCutoff;
        float intensity = clamp((theta - outerCutoff) / epsilon, 0.0, 1.0);
        attenuation *= intensity;
    }
    
    vec3 H = normalize(V +  L);
    vec3 radiance = light.color.rgb * light.color.w * attenuation;
    
    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
    
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;
    
    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;
    
    float NdotL = max(dot(N, L), 0.0);
    return (kD * albedo / PI + specular) * radiance * NdotL;
}

void main() 
{
    // Debug views
    if (push.debugViewMode == 1) {
        outColor = texture(samplerColor, inUV);
        return;
    } else if (push.debugViewMode == 2) {
        outColor = texture(samplerNormal, inUV);
        return;
    } else if (push.debugViewMode == 3) {
        vec4 depthData = texture(samplerDepth, inUV);
        outColor = vec4(depthData.rgb, 1.0);
        return;
    } else if (push.debugViewMode == 4) {
        outColor = texture(samplerMaterial, inUV);
        return;
    } else if (push.debugViewMode == 5) {
        float ssao = texture(samplerSSAO, inUV).r;
        outColor = vec4(ssao, ssao, ssao, 1.0);
        return;
    }
    
    // PBR Lighting Composition
    vec4 albedoSample = texture(samplerColor, inUV);
    
    // Skip lighting for fully transparent pixels
    if (albedoSample.a < 0.01) {
        outColor = vec4(0.0, 0.0, 0.0, 0.0);
        return;
    }
    
    vec3 albedo = albedoSample.rgb;
    vec3 normal = normalize(texture(samplerNormal, inUV).rgb * 2.0 - 1.0);
    vec3 material = texture(samplerMaterial, inUV).rgb;
    
    // Apply SSAO only if enabled
    float ssao = push.enableSSAO != 0 ? texture(samplerSSAO, inUV).r : 1.0;
    
    float roughness = clamp(material.r, 0.04, 1.0);
    float metallic = clamp(material.g, 0.0, 1.0);
    float ao = material.b;
    
    // Fragment position for isometric 2.5D - use depth from heightmap
    vec4 depthData = texture(samplerDepth, inUV);
    float depth = depthData.a * 100.0; // Reconstruct depth from alpha channel
    
    // Convert screen-space UV to world-space position with view offset
    // This ensures point lights render circularly in isometric view
    vec3 fragPos = vec3(inUV.x * 1920.0 + lighting.viewOffsetPadded.x, 
                        inUV.y * 1080.0 + lighting.viewOffsetPadded.y, 
                        depth);
    
    // View position for isometric camera (positioned above and to the side)
    vec3 viewPos = vec3(960.0, 540.0, 500.0);  // Camera high above scene
    vec3 V = normalize(viewPos - fragPos);
    
    // Calculate F0 (base reflectivity)
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);
    
    // Lighting accumulation
    vec3 Lo = vec3(0.0);
    for (int i = 0; i < lighting.numLights && i < 10; i++) {
        Lo += calculateLight(lighting.lights[i], fragPos, normal, V, albedo, roughness, metallic, F0);
    }
    
    // Ambient + SSAO (only on opaque pixels)
    vec3 ambient = lighting.ambientLight.rgb * albedo * ao * ssao;
    vec3 color = ambient + Lo;
    
    // ACES Filmic Tone Mapping - superior to Reinhard for color reproduction
    color = ACESFilmic(color);
    
    // Gamma correction - configurable via ImGui
    color = pow(color, vec3(1.0 / push.gamma));
    
    outColor = vec4(color, albedoSample.a);
}
