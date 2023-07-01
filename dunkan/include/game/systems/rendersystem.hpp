#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/RenderStates.hpp"
#include "SFML/Graphics/Shader.hpp"
#include "imgui.h"
#include "imgui-SFML.h"
#include "SFML/System/Vector2.hpp"
#include "game/systems/lightsystem.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

#if defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "game/types.hpp"
#include "utils/Configuration.hpp"

using RenderSystem_c = ADE::META_TYPES::Typelist<RenderComponent, PhysicsComponent>;
using RenderSystem_t = ADE::META_TYPES::Typelist<>;

const std::string vertexShader = \
    "varying vec3 vertex; "\
    "void main() "\
    "{ "\
    "    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; "\
    "    vertex = (gl_ModelViewMatrix*gl_Vertex).xyz; "\
    "    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0; "\
    "    gl_FrontColor = gl_Color; "\
    "}";

const std::string color_fragShader = \
    "uniform sampler2D color_map;" \
    "uniform bool useDepthMap;" \
    "uniform sampler2D depth_map;" \
    "uniform float height;" \
    "uniform float z_position;" \
    "void main()" \
    "{" \
    "   vec4 color_pixel = texture2D(color_map, gl_TexCoord[0].xy);" \
    "   float height_pixel = 0.0; "
    "   if(useDepthMap == true){"
    "        vec4 depth_pixel = texture2D(depth_map, gl_TexCoord[0].xy);" \
    "       height_pixel = (depth_pixel.r + depth_pixel.g + depth_pixel.b) *.33 * height;"
    "   }"
    "   float z_pixel = height_pixel + z_position;" \
    "   gl_FragDepth = 1.0 - color_pixel.a * (0.5 + z_pixel * 0.001);" \
    "   gl_FragColor = gl_Color * color_pixel; " \
    "}";

const std::string pbr_fragShader = \
    "uniform sampler2D color_map;" \
    "uniform bool useDepthMap;" \
    "uniform sampler2D depth_map;" \
    "uniform sampler2D material_map;" \
    "uniform float height;" \
    "uniform bool enable_materialMap;" \
    "uniform float z_position;" \
    "uniform float p_roughness;" \
    "uniform float p_metalness;" \
    "uniform float p_translucency;" \
    "void main()" \
    "{" \
    "   vec4 color_pixel = texture2D(color_map, gl_TexCoord[0].xy);" \
    "   vec4 material_pixel = vec4(p_roughness,p_metalness,p_translucency,color_pixel.a);"
    "   if(enable_materialMap){"
    "       material_pixel = texture2D(material_map, gl_TexCoord[0].xy);"
    "   }"
    "   float height_pixel = 0.0; "
    "   if(useDepthMap == true){"
    "        vec4 depth_pixel = texture2D(depth_map, gl_TexCoord[0].xy);" \
    "       height_pixel = (depth_pixel.r + depth_pixel.g + depth_pixel.b) *.33 * height;"
    "   }"
    "   float z_pixel = height_pixel + z_position;" \
    "   if(color_pixel.a < .9)"
    "       color_pixel.a = 0;"
    "   gl_FragDepth = 1.0 - color_pixel.a * (0.5 + z_pixel * 0.001);" \
    "   gl_FragColor = gl_Color * material_pixel; " \
    "}";

const std::string depth_fragShader = \
    "uniform sampler2D color_map;" \
    "uniform bool useDepthMap;" \
    "uniform sampler2D depth_map;" \
    "uniform float height;" \
    "uniform float z_position;" \
    "void main()" \
    "{" \
    "   float color_alpha = texture2D(color_map, gl_TexCoord[0].xy).a;" \
    "   float height_pixel = 0.0; "
    "   if(useDepthMap == true){"
    "       vec4 depth_pixel = texture2D(depth_map, gl_TexCoord[0].xy);" \
    "       height_pixel = (depth_pixel.r + depth_pixel.g + depth_pixel.b) *.33 * height;"
    "   }"
    "   float z_pixel = height_pixel + z_position;" \
    "   gl_FragDepth = 1.0 - color_alpha * (0.5 + z_pixel * 0.001);" \
    "   gl_FragColor.r = gl_FragDepth;" \
    "   gl_FragColor.g = (gl_FragDepth - floor(gl_FragDepth * 256.0) / 256.0) * 256.0;" \
    "   gl_FragColor.b = (gl_FragDepth - floor(gl_FragDepth * (65536.0)) / 65536.0) * 65536.0;" \
    "   gl_FragColor.a = color_alpha;" \
    "}";

const std::string normal_fragShader = \
    "uniform sampler2D color_map;" \
    "uniform bool useDepthMap;" \
    "uniform sampler2D depth_map;" \
    "uniform bool useNormalMap;" \
    "uniform sampler2D normal_map;" \
    "uniform float height;" \
    "uniform float z_position;" \
    "void main()" \
    "{" \
    "   float color_alpha = texture2D(color_map, gl_TexCoord[0].xy).a;" \
	"	vec3 direction = vec3(0.0, 0.0, 1.0);"
	"   if(useNormalMap){"
	"       direction = -1.0 + 2.0 * texture2D(normal_map, gl_TexCoord[0].xy).rgb;"
	"   }"
    "   float height_pixel = 0.0; "
    "   if(useDepthMap){"
    "       vec4 depth_pixel = texture2D(depth_map, gl_TexCoord[0].xy);" \
    "       height_pixel = (depth_pixel.r + depth_pixel.g + depth_pixel.b) *.33 * height;"
    "   }"
    "   float z_pixel = height_pixel + z_position;" \
    "   gl_FragDepth = 1.0 - color_alpha * (0.5 + z_pixel * 0.001);" \
    "   gl_FragColor.rgb = 0.5 + direction * 0.5;" \
    "   gl_FragColor.a = color_alpha;" \
    "}";

const std::string lighting_fragShader = \
    "uniform sampler2D color_map;" \
    "uniform sampler2D normal_map;" \
    "uniform sampler2D depth_map;" \
    "uniform sampler2D material_map;" \
    "uniform bool useSSAO;" \
    "uniform sampler2D SSAOMap;" \
    "uniform float z_position;" \
    "uniform vec4 ambient_light;" \
    "uniform int nbr_lights;" \
    "uniform vec2 screen_ratio;" \
    "uniform vec2 view_shift;" \
    "uniform vec3 view_pos;" \
    "uniform float p_exposure;" \
    "varying vec3 vertex; "\
    "uniform int debug_screen; "\
    "uniform bool enable_sRGB;" \
        ""
    " const float PI = 3.14159265359;"
    ""
    "vec3 fresnelSchlick(float cosTheta, vec3 F0)"
    "{"
    "    return F0 + (1.0 - F0) * pow(1 - cosTheta, 5.0);"
    "}"
    ""
    "float DistributionGGX(vec3 N, vec3 H, float roughness)"
    "{"
    "    float a      = roughness*roughness;"
    "    float a2     = a*a;"
    "    float NdotH  = max(dot(N, H), 0.0);"
    "    float NdotH2 = NdotH*NdotH;"
    "    float nom   = a2;"
    "    float denom = (NdotH2 * (a2 - 1.0) + 1.0);"
    "    denom = PI * denom * denom;"
    "    return nom / denom;"
    "}"
    "float GeometrySchlickGGX(float NdotV, float roughness)"
    "{"
    "    float r = (roughness + 1.0);"
    "    float k = (r*r) / 8.0;"
    "    float nom   = NdotV;"
    "    float denom = NdotV * (1.0 - k) + k;"
    "    return nom / denom;"
    "}"
    "float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)"
    "{"
    "    float NdotV = max(dot(N, V), 0.0);"
    "    float NdotL = max(dot(N, L), 0.0);"
    "    float ggx2  = GeometrySchlickGGX(NdotV, roughness);"
    "    float ggx1  = GeometrySchlickGGX(NdotL, roughness);"
    "    return ggx1 * ggx2;"
    "}"
    ""
    "void main()" \
    "{" \
    "   vec4 color_pixel = texture2D(color_map, gl_TexCoord[0].xy);" \
    "   vec4 normal_pixel = texture2D(normal_map, gl_TexCoord[0].xy);" \
    "   vec4 depth_pixel = texture2D(depth_map, gl_TexCoord[0].xy);" \
    "   vec4 material_pixel = texture2D(material_map, gl_TexCoord[0].xy);"
    "   vec3 direction = -1.0 + 2.0 * normal_pixel.rgb;"
    "   if(enable_sRGB == true)"
	"       color_pixel.rgb = pow(color_pixel.rgb, vec3(2.2));"
    "   float height_pixel = (0.5 - (depth_pixel.r+depth_pixel.g / 256.0+depth_pixel.b / 65536.0)) * 1000.0;"
	"   vec3 frag_pos = vertex + vec3(view_shift.xy, 0);"
	"   frag_pos.y -= height_pixel;"
	"   frag_pos.z = height_pixel;"
    "   vec3 view_direction = normalize(view_pos - frag_pos);"
    "   vec3 surfaceReflection0 = vec3(0.04);"
    "   surfaceReflection0 = mix(surfaceReflection0, color_pixel.rgb, material_pixel.g);"
    "   gl_FragColor = gl_Color * ambient_light * color_pixel; "
    "   for(int i = 0 ; i < nbr_lights ; ++i)" \
	"   {" \
    "       float attenuation = 0.0;" \
	"       vec3 light_direction = vec3(0,0,0);"
	"	    if(gl_LightSource[i].position.w == 0.0)" \
	"	    {		" \
	"	    	light_direction = -gl_LightSource[i].position.xyz;" \
    "           attenuation = 1.0;"
	"	    }" \
	"	    else" \
	"	    {" \
	"	    	light_direction = gl_LightSource[i].position.xyz - frag_pos.xyz;" \
    "	    	float dist = length(light_direction) / 100.0;" \
	"           float dr = dist / gl_LightSource[i].constantAttenuation;"
	"           float sqrtnom = 1.0 - dr*dr*dr*dr;"
    "           if(sqrtnom >= 0.0)"
	"           attenuation = saturate(sqrtnom*sqrtnom/(dist*dist+1));"
	"	    }" \
    "	    light_direction = normalize(light_direction);" \
    "       vec3 halfwayVector = normalize(view_direction + light_direction);"
    "       vec3 radiance = gl_LightSource[i].diffuse * attenuation; "
    "       float NDF = DistributionGGX(direction, halfwayVector, material_pixel.r); "
    "       float G   = GeometrySmith(direction, view_direction, light_direction, material_pixel.r);"
    "       vec3 F    = fresnelSchlick(max(dot(halfwayVector, view_direction), 0.0), surfaceReflection0); "
    "       vec3 kS = F;"
    "       vec3 kD = vec3(1.0) - kS;"
    "       kD *= 1.0 - material_pixel.g;"
    "       vec3 nominator    = NDF * G * F;"
    "       float denominator = 4.0 * max(dot(direction, view_direction), 0.0) * max(dot(direction, light_direction), 0.0);"
    "       vec3 specular     = nominator / max(denominator, 0.01);"
    "       float NdotL       = max(dot(direction, light_direction), 0.0);"
	"	    gl_FragColor.rgb += (kD * color_pixel.rgb / PI + specular) * radiance * NdotL;"
	"       float t = material_pixel.b;"
	"	    gl_FragColor.rgb -= (color_pixel.rgb/ PI) * radiance * min(dot(direction, light_direction), 0.0) * t;"
	"   }"
    "   gl_FragColor.rgb = vec3(1.0) - exp(-gl_FragColor.rgb * p_exposure);"
    "   if(enable_sRGB == true)"
	"       gl_FragColor.rgb = pow(gl_FragColor.rgb, vec3(1.0/2.2));"
    "   if(useSSAO == true) {"
	"       float occlusion  = (texture2D(SSAOMap, gl_TexCoord[0].xy + vec2(0,0) * screen_ratio).b * 4.0"
 	"                      + texture2D(SSAOMap, gl_TexCoord[0].xy + (vec2(1,0)) * screen_ratio).b * 2.0"
 	"                      + texture2D(SSAOMap, gl_TexCoord[0].xy + (vec2(-1,0)) * screen_ratio).b * 2.0"
 	"                      + texture2D(SSAOMap, gl_TexCoord[0].xy + (vec2(0,2)) * screen_ratio).b * 2.0"
 	"                      + texture2D(SSAOMap, gl_TexCoord[0].xy + (vec2(0,-2)) * screen_ratio).b * 2.0"
 	"                      + texture2D(SSAOMap, gl_TexCoord[0].xy + (vec2(1,2)) * screen_ratio).b * 1.0"
 	"                      + texture2D(SSAOMap, gl_TexCoord[0].xy + (vec2(1,-2)) * screen_ratio).b * 1.0"
 	"                      + texture2D(SSAOMap, gl_TexCoord[0].xy + (vec2(-1,-2)) * screen_ratio).b * 1.0"
 	"                      + texture2D(SSAOMap, gl_TexCoord[0].xy + (vec2(1,-2)) * screen_ratio).b * 1.0"
	"                       ) / 16.0;"
    "       gl_FragColor.rgb *= occlusion;"
	"   };"
    "   if(debug_screen == 1) { gl_FragColor.rgb = color_pixel.rgb; }"
    "   if(debug_screen == 2) { gl_FragColor.rgb = normal_pixel.rgb; }"
    "   if(debug_screen == 3) {"
    "       gl_FragColor.rgb = vec3(height_pixel * 0.001, height_pixel * 0.001, height_pixel * 0.001);" \
    "   }"
    "   if(debug_screen == 4) { gl_FragColor.rgb = material_pixel.rgb; }"
    "}";

const std::string SSAO_fragShader = \
    "uniform sampler2D normal_map;" \
    "uniform sampler2D depth_map;" \
    "uniform sampler2D noise_map;" \
    "uniform float z_position;" \
    "uniform vec2 screen_ratio;" \
    "uniform float zoom;"
    "uniform vec3 samples_hemisphere[16];"
    "varying vec3 vertex; "\
    "void main()" \
    "{" \
    "   vec4 normal_pixel = texture2D(normal_map, gl_TexCoord[0].xy);" \
    "   vec4 depth_pixel = texture2D(depth_map, gl_TexCoord[0].xy);" \
    "   vec3 direction = -1.0 + 2.0 * texture2D(normal_map, gl_TexCoord[0].xy).rgb;" \
    "   float height_pixel = (0.5 - (depth_pixel.r + depth_pixel.g / 256.0+depth_pixel.b / 65536.0)) * 1000.0;" \
	"   vec3 frag_pos = vertex;" \
	"   frag_pos.y -= height_pixel;" \
	"   frag_pos.z = height_pixel;" \
    "   float occlusion = 0.0;" \
    "   vec3 rVec = -1.0 + 2.0 * texture2D(noise_map, gl_TexCoord[0].xy).rgb;" \
	"   vec3 t = normalize(rVec - direction * dot(rVec, direction));" 
	"   mat3 rot = mat3(t, cross(direction,t), direction);"
	"   for(int i =0 ; i < 16 ; ++i){"
	"       vec3 decal = rot * samples_hemisphere[i] * 20.0;"
	"       vec3 screen_decal = decal;"
	"       screen_decal.y *= -1.0;"
	"       vec3 screen_pos = gl_FragCoord.xyz  + screen_decal;"
	"       vec3 occl_depth_pixel = texture2D(depth_map, (screen_pos.xy) * screen_ratio).rgb;"
	"       float occl_height = (0.5 - (occl_depth_pixel.r + occl_depth_pixel.g / 256.0 + occl_depth_pixel.b / 65536.0)) * 1000.0;"
    "       if(occl_height > (frag_pos.z+decal.z) + 1.0"
    "        && occl_height - (frag_pos.z+decal.z) < 20.0)"
    "           occlusion += 1.0;"
	"   } "
    "   float color_rgb = 1.0 - occlusion / 12.0;" \
    "   gl_FragColor.rgb = vec3(color_rgb, color_rgb, color_rgb);" \
    "   gl_FragColor.a = 1.0;" \
    "}";

struct RenderSystem {

    /**
     * Debug Screen:
     * 0 - Full
     * 1 - Albedo
     * 2 - Normal
     * 3 - Depth
     * 4 - Material
     **/
    int debug_screen{0};

    void init_renderer(sf::RenderWindow& window) {
        bool r = true;

        sf::Vector2u window_size = window.getSize();

        if(!m_colorScreen.create(window_size.x * m_superSampling, window_size.y * m_superSampling, true))
         r = false;
        if(!m_depthScreen.create(window_size.x * m_superSampling, window_size.y * m_superSampling, true))
            r = false;
        if(!m_normalScreen.create(window_size.x * m_superSampling, window_size.y * m_superSampling, true))
            r = false;
        if(!m_SSAOScreen.create(window_size.x * m_superSampling, window_size.y * m_superSampling, true))
            r = false;
        if(!m_pbrScreen.create(window_size.x * m_superSampling, window_size.y * m_superSampling, true))
            r = false;
        
        m_colorShader.loadFromMemory(color_fragShader,sf::Shader::Fragment);
        light_system.m_depthShader.loadFromMemory(depth_fragShader,sf::Shader::Fragment);
        m_normalShader.loadFromMemory(normal_fragShader,sf::Shader::Fragment);
        m_SSAOShader.loadFromMemory(vertexShader,SSAO_fragShader);
        m_pbrShader.loadFromMemory(pbr_fragShader, sf::Shader::Fragment);
        light_system.get_light_shader()->loadFromMemory(vertexShader, lighting_fragShader);
        light_system.get_light_shader()->setUniform("ambient_light", sf::Glsl::Vec4(Configuration::get()->ambient_light));
        light_system.get_light_shader()->setUniform("enable_sRGB", Configuration::get()->enable_SRGB);
        
        ImGui::SFML::Init(window, m_colorScreen, true);

        m_colorScreen.setActive(true);
            m_colorScreen.setSmooth(true);
        m_colorScreen.setActive(false);

        m_depthScreen.setActive(true);
            m_depthScreen.setSmooth(true);
        m_depthScreen.setActive(false);

        m_normalScreen.setActive(true);
            m_normalScreen.setSmooth(true);
        m_normalScreen.setActive(false);

        m_pbrScreen.setActive(true);
            m_pbrScreen.setSmooth(true);
        m_pbrScreen.setActive(false);

        m_renderer.setSize(sf::Vector2f(window_size.x, window_size.y));
        m_renderer.setTextureRect(sf::IntRect(0,0,window_size.x * m_superSampling, window_size.y * m_superSampling));
        m_renderer.setTexture(&m_colorScreen.getTexture());

        light_system.get_light_shader()->setUniform("color_map",m_colorScreen.getTexture());
        light_system.get_light_shader()->setUniform("normal_map",m_normalScreen.getTexture());
        light_system.get_light_shader()->setUniform("depth_map",m_depthScreen.getTexture());
        light_system.get_light_shader()->setUniform("material_map",m_pbrScreen.getTexture());
        light_system.get_light_shader()->setUniform("screen_ratio",sf::Vector2f(1.0 / (float)m_colorScreen.getSize().x,
                                                                1.0 / (float)m_colorScreen.getSize().y));

        m_rendererStates.shader = light_system.get_light_shader();

        SSAO_option(true);

        sf::Glsl::Vec3 samples_hemisphere[16];
        samples_hemisphere[0 ] = sf::Glsl::Vec3(.4,0,.8);
        samples_hemisphere[1] = sf::Glsl::Vec3(0,.2,.4);
        samples_hemisphere[2] = sf::Glsl::Vec3(.1,0,.2);
        samples_hemisphere[3] = sf::Glsl::Vec3(0,0,.1);

        samples_hemisphere[4] = sf::Glsl::Vec3(1,0,.4);
        samples_hemisphere[5] = sf::Glsl::Vec3(-1,0,.4);
        samples_hemisphere[6] = sf::Glsl::Vec3(0,1,.4);
        samples_hemisphere[7] = sf::Glsl::Vec3(0,-1,.4);

        samples_hemisphere[8] = sf::Glsl::Vec3(.5,.5,.5);
        samples_hemisphere[9] = sf::Glsl::Vec3(.5,-.5,.5);
        samples_hemisphere[10] = sf::Glsl::Vec3(-.5,.5,.5);
        samples_hemisphere[11] = sf::Glsl::Vec3(-.5,-.5,.5);

        samples_hemisphere[12] = sf::Glsl::Vec3(.5,0,.5);
        samples_hemisphere[13] = sf::Glsl::Vec3(-.5,0,.5);
        samples_hemisphere[14] = sf::Glsl::Vec3(0,.5,.5);
        samples_hemisphere[15] = sf::Glsl::Vec3(0,-.5,.5);

        m_SSAOShader.setUniformArray("samples_hemisphere",samples_hemisphere,16);

        m_SSAONoisePattern.create(4,4);

        for(int x = 0 ; x < 4 ; ++x)
        for(int y = 0 ; y < 4 ; ++y)
        {
            sf::Color c = sf::Color::White;
            c.r = (int)(static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/255)));
            c.g = (int)(static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/255)));
            c.b = (int)(static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/255)));;
            m_SSAONoisePattern.setPixel(x,y,c);
        }

        m_SSAONoiseTexture.setRepeated(true);
        m_SSAONoiseTexture.loadFromImage(m_SSAONoisePattern);
        m_SSAOShader.setUniform("noise_map",m_SSAONoiseTexture);
    }

    sf::Shader* tmp_shader = nullptr;

    void update(EntityManager& entity_manager, sf::RenderWindow& window) {

        window.clear();

        light_system.get_light_shader()->setUniform("debug_screen", debug_screen);

        sf::View current_view = window.getView();
        sf::Vector2f view_shift = current_view.getCenter();
        view_shift -= sf::Vector2f(current_view.getSize().x / 2, current_view.getSize().y / 2);

        light_system.calculate_lights(entity_manager, view_shift, current_view, m_colorScreen.getSize());
        light_system.get_light_shader()->setUniform("view_shift",view_shift);
        light_system.get_light_shader()->setUniform("view_pos", sf::Vector3f(current_view.getCenter().x, current_view.getCenter().y, 750.0f));
        light_system.get_light_shader()->setUniform("p_exposure", Configuration::get()->exposure);

        m_colorScreen.setActive(true);
            glClear(GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);
            m_colorScreen.clear(sf::Color::White);
            m_colorScreen.setView(current_view);
        m_colorScreen.setActive(false);

        m_pbrScreen.setActive(true);
            glClear(GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);
            m_pbrScreen.clear();
            m_pbrScreen.setView(current_view);
        m_pbrScreen.setActive(false);

        m_depthScreen.setActive(true);
            glClear(GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);
            m_depthScreen.clear(sf::Color::White);
            m_depthScreen.setView(current_view);
        m_depthScreen.setActive(false);

        m_normalScreen.setActive(true);
            glClear(GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);
            m_normalScreen.clear();
            m_normalScreen.setView(current_view);
        m_normalScreen.setActive(false);

        entity_manager.foreach<RenderSystem_c, RenderSystem_t>
        ([&](Entity& entity, RenderComponent& render, PhysicsComponent& physics)
        {
            render.setPosition(physics.position(view_shift).x, physics.position(view_shift).y);
            render.setScale(sf::Vector2f(render.scale, render.scale));

            sf::RenderStates state;
            state.transform = sf::Transform::Identity;

            m_colorScreen.setActive(true);
                m_colorShader.setUniform("z_position", physics.z);
                render.prepare_shader(&m_colorShader);
                state.shader = &m_colorShader;
                m_colorScreen.draw(render, state);
            m_colorScreen.setActive(false);

            m_pbrScreen.setActive(true);
                m_pbrShader.setUniform("z_position", physics.z);
                render.prepare_shader(&m_pbrShader);
                state.shader = &m_pbrShader;
                m_pbrScreen.draw(render, state);
            m_pbrScreen.setActive(false);

            m_normalScreen.setActive(true);
                m_normalShader.setUniform("z_position", physics.z);
                render.prepare_shader(&m_normalShader);
                state.shader = &m_normalShader;
                m_normalScreen.draw(render, state);
            m_normalScreen.setActive(false);

            m_depthScreen.setActive(true);
                light_system.m_depthShader.setUniform("z_position", physics.z);
                render.prepare_shader(&light_system.m_depthShader);
                state.shader = &light_system.m_depthShader;
                m_depthScreen.draw(render, state);
            m_depthScreen.setActive(false);

            if(render.is_selected) {
                _debug_render_lines(render, m_colorScreen);
            }
        
        }); 

        #ifdef DEBUG_IMGUI
            ImGui::SFML::Render(m_colorScreen);
        #endif

        m_colorScreen.display();
        m_pbrScreen.display();
        m_depthScreen.display();
        m_normalScreen.display();

        if(Configuration::get()->enable_SSAO)
        {
            m_SSAOScreen.draw(m_SSAOrenderer,&m_SSAOShader);
            m_SSAOScreen.display();
        }

        m_rendererStates.transform = sf::Transform::Identity;
        m_rendererStates.transform.translate(view_shift.x, view_shift.y);

        window.draw(m_renderer, m_rendererStates);

    }

    void destroy() {
        m_normalScreen.clear();
        m_colorScreen.clear();
        m_depthScreen.clear();
        m_SSAOScreen.clear();
    }

    void SSAO_option(bool value) {
        Configuration::get()->enable_SSAO = value;

        if(Configuration::get()->enable_SSAO)
        {
            light_system.get_light_shader()->setUniform("useSSAO", true);
            light_system.get_light_shader()->setUniform("SSAOMap", m_SSAOScreen.getTexture());
            m_SSAOShader.setUniform("normal_map", m_normalScreen.getTexture());
            m_SSAOShader.setUniform("depth_map", m_depthScreen.getTexture());
            m_SSAOShader.setUniform("screen_ratio", 
                sf::Vector2f(1.0 / (float)m_depthScreen.getSize().x,
                             1.0 / (float)m_depthScreen.getSize().y));
            m_SSAOrenderer.setSize(sf::Vector2f(m_depthScreen.getSize().x,
                                                m_depthScreen.getSize().y));
            m_SSAOrenderer.setTexture(&m_colorScreen.getTexture());
        } else {
            light_system.get_light_shader()->setUniform("useSSAO", false);
        }
    }

    void enable_gamma_correction(bool value) {
        Configuration::get()->enable_SRGB = value;
        light_system.get_light_shader()->setUniform("enable_sRGB", value);
    }

private:

    void _debug_render_lines(RenderComponent& render, sf::RenderTarget& window) {
        float initial_x = render.getGlobalBounds().left;
        float initial_y = render.getGlobalBounds().top;
        float height = render.getGlobalBounds().height;
        float width = render.getGlobalBounds().width;

        sf::Vertex line[] =
        {
            sf::Vertex(sf::Vector2f(initial_x, initial_y)),
            sf::Vertex(sf::Vector2f(initial_x + width, initial_y)),
            sf::Vertex(sf::Vector2f(initial_x + width, initial_y + height)),
            sf::Vertex(sf::Vector2f(initial_x, initial_y + height)),
            sf::Vertex(sf::Vector2f(initial_x, initial_y)),
        };

        sf::RenderStates debug_state = sf::RenderStates::Default;     

        window.draw(line, 5, sf::LineStrip, debug_state);
    }

    LightSystem light_system {};

    sf::RenderTexture m_geometry_screen[2];
    sf::Shader m_default_shader;
    sf::Shader m_geometry_shader;

    // TODO: Refactor shaders with a shader manager
    sf::Shader m_colorShader;
    sf::Shader m_normalShader;
    sf::Shader m_pbrShader;

    int m_superSampling{1};
    sf::RenderTexture m_colorScreen;
    sf::RenderTexture m_normalScreen;
    sf::RenderTexture m_depthScreen;
    sf::RenderTexture m_pbrScreen;

    sf::RectangleShape m_renderer;
    sf::RenderStates m_rendererStates;

    sf::Shader m_SSAOShader;
    sf::RenderTexture m_SSAOScreen;
    sf::RectangleShape m_SSAOrenderer;
    sf::Texture m_SSAONoiseTexture;
    sf::Image m_SSAONoisePattern;
};


