uniform sampler2D color_map;
uniform sampler2D normal_map;
uniform sampler2D depth_map;
uniform sampler2D material_map;
uniform sampler2D map_brdflut;
uniform bool useSSAO;
uniform sampler2D SSAOMap;
uniform float z_position;
uniform vec4 ambient_light;
uniform int nbr_lights;
uniform vec2 screen_ratio;
uniform vec2 view_shift;
uniform vec3 view_pos;
uniform float p_exposure;
varying vec3 vertex;
uniform int debug_screen;
uniform bool enable_sRGB;
uniform mat3 p_isoToCartMat;
uniform float p_isoToCartZFactor;

const float PI = 3.14159265359;

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1 - cosTheta, 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}   

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    return nom / denom;
}
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;
    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    return nom / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

void main()
{
    const vec3 constant_list = vec3(1.0, 0.0, -2.0);
    vec4 color_pixel = texture2D(color_map, gl_TexCoord[0].xy);
    if(color_pixel.a > 0.1) {
        vec4 normal_pixel = texture2D(normal_map, gl_TexCoord[0].xy);
        vec4 depth_pixel = texture2D(depth_map, gl_TexCoord[0].xy);
        vec4 material_pixel = texture2D(material_map, gl_TexCoord[0].xy);
        vec3 direction = -1.0 + 2.0 * normal_pixel.rgb;
        float height_pixel = (0.5 - (depth_pixel.r+depth_pixel.g / 256.0 + depth_pixel.b / 65536.0)) * 1000.0;
        gl_FragColor.a = color_pixel.a;
        if(enable_sRGB == true)
            color_pixel.rgb = pow(color_pixel.rgb, vec3(2.2));

        vec3 frag_pos;
        frag_pos.xy = vertex.xy + view_shift.xy;
        frag_pos.y -= height_pixel;
        frag_pos.z = height_pixel;
        
        vec3 view_direction = normalize(view_pos - frag_pos);
        vec3 surfaceReflection0 = vec3(0.04);
        surfaceReflection0 = mix(surfaceReflection0, color_pixel.rgb, material_pixel.g);
        float FAmbient = fresnelSchlickRoughness(max(dot(direction, view_direction), 0.0), surfaceReflection0, material_pixel.r);
        vec3 kSAmbient = FAmbient;
        vec3 kDAmbient = (1.0 - kSAmbient) * (1.0 - material_pixel.g);
        vec3 irradianceAmbient = ambient_light.rgb * ambient_light.a;
        vec3 reflection_view = reflect(-view_direction, direction);
        vec3 reflection_color = ambient_light.rgb;
        if(reflection_view.z < 0) {
            vec2 ground_position = gl_FragCoord.xy;
            vec3 v = vec3((height_pixel / reflection_view.z) * reflection_view.xy, 0.0);
            ground_position.xy += height_pixel * constant_list.yx 
                                    + (v).xy * constant_list.zx;
            vec3 groundColor = texture2DLod(color_map, ground_position * screen_ratio, material_pixel.r * 10.0).rgb 
                                            * 150.0 / (1.0 + length(v) + height_pixel);
            reflection_color = mix(groundColor, ambient_light.rgb, reflection_view.z + 1.0); 
        }
        vec2 envBRDF = texture2D(map_brdflut, vec2(max(dot(direction, view_direction), 0.0), material_pixel.r)).rg;
        vec3 specular_ambient = reflection_color * (FAmbient * envBRDF.x + envBRDF.y);

        gl_FragColor.rgb = (color_pixel.rgb * kDAmbient + kSAmbient)* irradianceAmbient * specular_ambient;
        gl_FragColor.a   = color_pixel.a; 

        for(int i = 0 ; i < nbr_lights ; ++i)
        {
            float attenuation = 0.0;
            vec3 light_direction = vec3(0,0,0);
            if(gl_LightSource[i].position.w == 0.0)
            {		
                light_direction = -gl_LightSource[i].position.xyz;
                attenuation = 1.0;
            }
            else
            {
                light_direction = gl_LightSource[i].position.xyz - frag_pos.xyz;
                float dist = length(light_direction) * 0.01;
                float dr = dist / gl_LightSource[i].constantAttenuation;
                float sqrtnom = 1.0 - dr * dr * dr * dr;
                if(sqrtnom >= 0.0)
                        attenuation = saturate(sqrtnom * sqrtnom / (dist * dist + 1.0));
            }

            light_direction     = normalize(light_direction);
            vec3 halfwayVector  = normalize(view_direction + light_direction);
            vec3 radiance       = gl_LightSource[i].diffuse * attenuation; 
            float NDF           = DistributionGGX(direction, halfwayVector, material_pixel.r); 
            float G             = GeometrySmith(direction, view_direction, light_direction, material_pixel.r);
            vec3 F              = fresnelSchlick(max(dot(halfwayVector, view_direction), 0.0), surfaceReflection0); 
            vec3 kS             = F;
            vec3 kD             = vec3(1.0) - kS;
            kD                  *= 1.0 - material_pixel.g;
            vec3 nominator      = NDF * G * F;
            float denominator   = 4.0 * max(dot(direction, view_direction), 0.0) * max(dot(direction, light_direction), 0.0);
            vec3 specular       = nominator / max(denominator, 0.01);
            float NdotL         = max(dot(direction, light_direction), 0.0);
            gl_FragColor.rgb    += (kD * color_pixel.rgb * (1.0 / PI) + specular) * radiance * NdotL;
            gl_FragColor.rgb    -= (color_pixel.rgb * (1.0 / PI)) * radiance * min(dot(direction, light_direction), 0.0) * material_pixel.b;
        }
    
        gl_FragColor.rgb = vec3(1.0) - exp(-gl_FragColor.rgb * p_exposure);

        if(enable_sRGB == true)
                gl_FragColor.rgb = pow(gl_FragColor.rgb, vec3(1.0 / 2.2));

    //    if(useSSAO == true) {
    // 	       float occlusion  = (texture2D(SSAOMap, gl_TexCoord[0].xy + vec2(0,0) * screen_ratio).b * 4.0
    //  	                      + texture2D(SSAOMap, gl_TexCoord[0].xy + (vec2(1,0)) * screen_ratio).b * 2.0
    //  	                      + texture2D(SSAOMap, gl_TexCoord[0].xy + (vec2(-1,0)) * screen_ratio).b * 2.0
    //  	                      + texture2D(SSAOMap, gl_TexCoord[0].xy + (vec2(0,2)) * screen_ratio).b * 2.0
    //  	                      + texture2D(SSAOMap, gl_TexCoord[0].xy + (vec2(0,-2)) * screen_ratio).b * 2.0
    //  	                      + texture2D(SSAOMap, gl_TexCoord[0].xy + (vec2(1,2)) * screen_ratio).b * 1.0
    //  	                      + texture2D(SSAOMap, gl_TexCoord[0].xy + (vec2(1,-2)) * screen_ratio).b * 1.0
    //  	                      + texture2D(SSAOMap, gl_TexCoord[0].xy + (vec2(-1,-2)) * screen_ratio).b * 1.0
    //  	                      + texture2D(SSAOMap, gl_TexCoord[0].xy + (vec2(1,-2)) * screen_ratio).b * 1.0
    // 	                    ) / 16.0;
    //        gl_FragColor.rgb *= occlusion;
    //     }
        
        if(useSSAO == true) {
            float occlusion  = (texture2D(SSAOMap, gl_TexCoord[0].xy+vec2(0,0)*screen_ratio).b*2
                        +texture2D(SSAOMap, gl_TexCoord[0].xy + (vec2(1,0))*screen_ratio).b
                        +texture2D(SSAOMap, gl_TexCoord[0].xy + (vec2(-1,0))*screen_ratio).b
                        +texture2D(SSAOMap, gl_TexCoord[0].xy + (vec2(0,1))*screen_ratio).b
                        +texture2D(SSAOMap, gl_TexCoord[0].xy + (vec2(0,-1))*screen_ratio).b) / 6.0;
            if(occlusion > .2 && occlusion < .8) {
                occlusion = occlusion * 0.75
                        +(texture2D(SSAOMap, gl_TexCoord[0].xy + (vec2(1,1))*screen_ratio).b
                        +texture2D(SSAOMap, gl_TexCoord[0].xy + (vec2(1,-1))*screen_ratio).b
                        +texture2D(SSAOMap, gl_TexCoord[0].xy + (vec2(-1,-1))*screen_ratio).b
                        +texture2D(SSAOMap, gl_TexCoord[0].xy + (vec2(1,-1))*screen_ratio).b) / 16.0;
            }
            gl_FragColor.rgb *= occlusion;
        }

        if(debug_screen == 1) { gl_FragColor.rgb = color_pixel.rgb; }
        if(debug_screen == 2) { gl_FragColor.rgb = normal_pixel.rgb; }
        if(debug_screen == 3) {
            gl_FragColor.rgb = vec3(height_pixel * 0.001, height_pixel * 0.001, height_pixel * 0.001);
        }
        if(debug_screen == 4) { gl_FragColor.rgb = material_pixel.rgb; }

    }

}