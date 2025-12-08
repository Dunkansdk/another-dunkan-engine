#version 450

layout (location = 0) in vec2 inUV;

layout (location = 0) out float outFragColor;

layout (binding = 0) uniform sampler2D samplerPositionDepth;
layout (binding = 1) uniform sampler2D samplerNormal;
layout (binding = 2) uniform sampler2D ssaoNoise;

layout (binding = 3) uniform UBO {
    mat4 projection;
    vec4 samples[64];
    float radius;
    float bias;
    float power;
    float _padding;
} uboSSAOKernel;

const int kernelSize = 64;

void main() 
{
    // Get G-Buffer values
    vec3 fragPos = texture(samplerPositionDepth, inUV).rgb;
    vec3 normal = normalize(texture(samplerNormal, inUV).rgb * 2.0 - 1.0);

    // Get Random Vector
    ivec2 texDim = textureSize(samplerPositionDepth, 0); 
    ivec2 noiseDim = textureSize(ssaoNoise, 0);
    const vec2 noiseScale = vec2(float(texDim.x) / float(noiseDim.x), float(texDim.y) / float(noiseDim.y));
    vec3 randomVec = texture(ssaoNoise, inUV * noiseScale).xyz;

    // Create TBN Change-of-Basis Matrix: from Tangent-Space to View-Space
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; ++i)
    {
        // sample position
        vec3 samplePos = TBN * uboSSAOKernel.samples[i].xyz; 
        samplePos = fragPos + samplePos * uboSSAOKernel.radius; 
        
        // project sample position (to sample texture) (to get position on screen/texture)
        vec4 offset = vec4(samplePos, 1.0);
        offset = uboSSAOKernel.projection * offset; 
        offset.xyz /= offset.w; 
        offset.xyz = offset.xyz * 0.5 + 0.5; 
        
        // get sample depth
        float sampleDepth = texture(samplerPositionDepth, offset.xy).z; 
        
        // range check & accumulate
        float rangeCheck = smoothstep(0.0, 1.0, uboSSAOKernel.radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + uboSSAOKernel.bias ? 1.0 : 0.0) * rangeCheck;           
    }
    
    occlusion = 1.0 - (occlusion / float(kernelSize));
    outFragColor = pow(occlusion, uboSSAOKernel.power);
}