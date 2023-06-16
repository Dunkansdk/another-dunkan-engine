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
    "   if(color_pixel.a > 0.9) {"
    "   gl_FragDepth = 1.0 - color_pixel.a * (0.5 + z_pixel * 0.001);} else {gl_FragDepth = 1.0;}" \
    "   gl_FragColor = gl_Color * color_pixel; " \
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
    "   vec4 color_pixel = texture2D(color_map, gl_TexCoord[0].xy);" \
    "   float height_pixel = 0.0; "
    "   if(useDepthMap == true){"
    "       vec4 depth_pixel = texture2D(depth_map, gl_TexCoord[0].xy);" \
    "       height_pixel = (depth_pixel.r + depth_pixel.g + depth_pixel.b) *.33 * height;"
    "   }"
    "   float z_pixel = height_pixel + z_position;" \
    "   if(color_pixel.a > 0.9) {"
    "   gl_FragDepth = 1.0 - color_pixel.a * (0.5 + z_pixel * 0.001);} else {gl_FragDepth = 1.0;}" \
    "   gl_FragColor.r = gl_FragDepth;" \
    "   gl_FragColor.g = (gl_FragDepth - 256.0 * floor(gl_FragDepth / 256.0))*256.0;" \
    "   gl_FragColor.b = (gl_FragDepth - 65536.0 * floor(gl_FragDepth / (65536.0)))*65536.0;" \
    "   gl_FragColor.a = color_pixel.a;" \
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
    "   vec4 color_pixel = texture2D(color_map, gl_TexCoord[0].xy);" \
	"	vec3 direction = vec3(0.0, 0.0, -1.0);"
	"   if(useNormalMap){"
	"       direction = -1.0 + 2.0 * texture2D(normal_map, gl_TexCoord[0].xy).rgb;"
	"   }"
    "   float height_pixel = 0.0; "
    "   if(useDepthMap){"
    "       vec4 depth_pixel = texture2D(depth_map, gl_TexCoord[0].xy);" \
    "       height_pixel = (depth_pixel.r + depth_pixel.g + depth_pixel.b) *.33 * height;"
    "   }"
    "   float z_pixel = height_pixel + z_position;" \
    "   if(color_pixel.a > 0.9) {"
    "   gl_FragDepth = 1.0 - color_pixel.a * (0.5 + z_pixel * 0.001);} else {gl_FragDepth = 1.0;}" \
    "   gl_FragColor.rgb = 0.5 + direction * 0.5;" \
    "   gl_FragColor.a = color_alpha;" \
    "}";

const std::string lighting_fragShader = \
    "uniform sampler2D color_map;" \
    "uniform sampler2D normal_map;" \
    "uniform sampler2D depth_map;" \
    "uniform bool useSSAO;" \
    "uniform sampler2D SSAOMap;" \
    "uniform float z_position;" \
    "uniform vec4 ambient_light;" \
    "uniform int nbr_lights;" \
    "uniform vec2 screen_ratio;" \
    "uniform vec2 view_shift;" \
    "varying vec3 vertex; "\
    "uniform int shadow_caster[8];"
    "uniform sampler2D shadow_map_0;" \
    "uniform sampler2D shadow_map_1;" \
    "uniform sampler2D shadow_map_2;" \
    "uniform sampler2D shadow_map_3;" \
    "uniform sampler2D shadow_map_4;" \
    "uniform sampler2D shadow_map_5;" \
    "uniform sampler2D shadow_map_6;" \
    "uniform sampler2D shadow_map_7;" \
    "uniform vec2 shadow_shift[8];"
    "uniform vec2 shadow_ratio[8];"
    "uniform int debug_screen; "\
    ""
    "float GetShadowCastValue(int curShadowMap, float heightPixel, vec2 shadowPos) {"
    "   vec4 shadowPixel;"
    "   vec2 mapPos = (shadowPos - shadow_shift[curShadowMap]) * shadow_ratio[curShadowMap];"
    "   if(mapPos.x > 1.0) mapPos.x = 1.0;"
    "   if(mapPos.y > 1.0) mapPos.y = 1.0;"
    "   if(mapPos.x < 0.0) mapPos.x = 0.0;"
    "   if(mapPos.y < 0.0)  return 0.0;"
	"   if(curShadowMap == 0)"
	"       shadowPixel = texture2D(shadow_map_0, mapPos);"
	"   float shadowHeight = (0.5-(shadowPixel.r + shadowPixel.g / 256.0 + shadowPixel.b/65536.0)) * 1000.0;"
    "   return 1.0 - min(1.0, max(0.0, (shadowHeight - heightPixel - 5.0) * 0.05));"
    "}"
    ""
    "void main()" \
    "{" \
    "   vec4 color_pixel = texture2D(color_map, gl_TexCoord[0].xy);" \
    "   vec4 normal_pixel = texture2D(normal_map, gl_TexCoord[0].xy);" \
    "   vec4 depth_pixel = texture2D(depth_map, gl_TexCoord[0].xy);" \
    "   vec3 direction = -1.0 + 2.0 * texture2D(normal_map, gl_TexCoord[0].xy).rgb;"
    "   float height_pixel = (0.5 - (depth_pixel.r+depth_pixel.g / 256.0+depth_pixel.b / 65536.0)) * 1000.0;"
	"   vec3 frag_pos = vertex + vec3(view_shift.xy, 0);"
	"   frag_pos.y -= height_pixel;"
	"   frag_pos.z = height_pixel;"
    "   gl_FragColor = gl_Color * ambient_light * color_pixel; "
    "   int curShadowMap = 0;"
    "   for(int i = 0 ; i < nbr_lights ; ++i)" \
	"   {" \
	"	    float lighting = 0.0;" \
	"       vec3 light_direction = vec3(0,0,0);"
	"	    if(gl_LightSource[i].position.w == 0.0)" \
	"	    {		" \
	"	    	light_direction = -gl_LightSource[i].position.xyz;" \
    "           lighting = 1.0/( gl_LightSource[i].constantAttenuation);"
	"	    }" \
	"	    else" \
	"	    {" \
	"	    	light_direction = gl_LightSource[i].position.xyz - frag_pos.xyz;" \
    "	    	float dist = length(light_direction) / 100.0;" \
	"	    	lighting = 1.0 /( gl_LightSource[i].constantAttenuation +" \
	"	    							  dist*gl_LightSource[i].linearAttenuation +" \
	"	    							  dist*dist*gl_LightSource[i].quadraticAttenuation);" \
	"	    }" \
    "       light_direction = normalize(light_direction);"
	"       if(curShadowMap < 8 && shadow_caster[curShadowMap] == i) {"
	"           if(gl_LightSource[i].position.w == 0.0){"
	"               vec2 shadowPos = gl_FragCoord.xy;"
	"               shadowPos.y += height_pixel;"
	"               vec3 v = vec3((height_pixel * light_direction.xy / light_direction.z), 0.0);"
	"               shadowPos.x -= v.x;"
	"               shadowPos.y += v.y;"
	"               vec4 shadow_pixel= vec4(0.0, 0.0, 0.0, 0.0);"
	"               lighting *= (GetShadowCastValue(curShadowMap,height_pixel,shadowPos)*4.0"
    "                            +GetShadowCastValue(curShadowMap,height_pixel,shadowPos+vec2(3.0,0.0))*2.0"
    "                            +GetShadowCastValue(curShadowMap,height_pixel,shadowPos+vec2(-3.0,0.0))*2.0"
    "                            +GetShadowCastValue(curShadowMap,height_pixel,shadowPos+vec2(0.0,3.0))*2.0"
    "                            +GetShadowCastValue(curShadowMap,height_pixel,shadowPos+vec2(0.0,-3.0))*2.0"
    "                            +GetShadowCastValue(curShadowMap,height_pixel,shadowPos+vec2(2.0,2.0))"
    "                            +GetShadowCastValue(curShadowMap,height_pixel,shadowPos+vec2(-2.0,2.0))"
    "                            +GetShadowCastValue(curShadowMap,height_pixel,shadowPos+vec2(-2.0,-2.0))"
    "                            +GetShadowCastValue(curShadowMap,height_pixel,shadowPos+vec2(2.0,-2.0)))/16.0;"
	"           }"
	"           ++curShadowMap;"
	"       }"
	"       lighting *= max(0.0, dot(direction,light_direction));"
	"	    lighting *= gl_LightSource[i].diffuse.a;" \
	"	    gl_FragColor.rgb +=  gl_Color.rgb * color_pixel.rgb * gl_LightSource[i].diffuse.rgb * lighting;" \
	"   }"
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
	"       vec3 screen_pos = gl_FragCoord.xyz + decal;"
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

    bool m_enableSSAO{true};
    /**
     * Debug Screen:
     * 0 - Full
     * 1 - Albedo
     * 2 - Normal
     * 3 - Depth
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
        
        m_colorShader.loadFromMemory(color_fragShader,sf::Shader::Fragment);
        m_depthShader.loadFromMemory(depth_fragShader,sf::Shader::Fragment);
        m_normalShader.loadFromMemory(normal_fragShader,sf::Shader::Fragment);
        m_SSAOShader.loadFromMemory(vertexShader,SSAO_fragShader);
        light_system.m_lightingShader.loadFromMemory(vertexShader,lighting_fragShader);
        light_system.m_lightingShader.setUniform("ambient_light", sf::Glsl::Vec4(sf::Color{ 180, 180, 180, 255 }));
        
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

        m_renderer.setSize(sf::Vector2f(window_size.x, window_size.y));
        m_renderer.setTextureRect(sf::IntRect(0,0,window_size.x * m_superSampling, window_size.y * m_superSampling));
        m_renderer.setTexture(&m_colorScreen.getTexture());

        light_system.m_lightingShader.setUniform("color_map",m_colorScreen.getTexture());
        light_system.m_lightingShader.setUniform("normal_map",m_normalScreen.getTexture());
        light_system.m_lightingShader.setUniform("depth_map",m_depthScreen.getTexture());
        light_system.m_lightingShader.setUniform("screen_ratio",sf::Vector2f(1.0 / (float)m_colorScreen.getSize().x,
                                                                1.0 / (float)m_colorScreen.getSize().y));

        m_rendererStates.shader = &light_system.m_lightingShader;

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
            c.r = 1;
            m_SSAONoisePattern.setPixel(x,y,c);
        }

        m_SSAONoiseTexture.setRepeated(true);
        m_SSAONoiseTexture.loadFromImage(m_SSAONoisePattern);
        m_SSAOShader.setUniform("noise_map",m_SSAONoiseTexture);
    }

    sf::Shader* tmp_shader = nullptr;

    void update(EntityManager& entity_manager, sf::RenderWindow& window) {

        window.clear();

        light_system.m_lightingShader.setUniform("debug_screen", debug_screen);

        sf::View current_view = window.getView();
        sf::Vector2f view_shift = current_view.getCenter();
        view_shift -= sf::Vector2f(current_view.getSize().x / 2, current_view.getSize().y / 2);

        light_system.calculate_ligts(entity_manager, view_shift, current_view, m_colorScreen.getSize());

        m_colorScreen.setActive(true);
            glClear(GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);
            m_colorScreen.clear();
            m_colorScreen.setView(current_view);
        m_colorScreen.setActive(false);

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
                m_colorShader.setUniform("z_position",physics.z);
                render.prepare_shader(&m_colorShader);
                state.shader = &m_colorShader;
                m_colorScreen.draw(render, state);
            m_colorScreen.setActive(false);

            m_normalScreen.setActive(true);
                m_normalShader.setUniform("z_position",physics.z);
                render.prepare_shader(&m_normalShader);
                state.shader = &m_normalShader;
                m_normalScreen.draw(render, state);
            m_normalScreen.setActive(false);

            m_depthScreen.setActive(true);
                m_depthShader.setUniform("z_position",physics.z);
                render.prepare_shader(&m_depthShader);
                state.shader = &m_depthShader;
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
        m_depthScreen.display();
        m_normalScreen.display();

        if(m_enableSSAO)
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

    void SSAO_option(bool ssao_value) {
        m_enableSSAO = ssao_value;

        if(m_enableSSAO)
        {
            light_system.m_lightingShader.setUniform("useSSAO", true);
            light_system.m_lightingShader.setUniform("SSAOMap", m_SSAOScreen.getTexture());
            m_SSAOShader.setUniform("normal_map", m_normalScreen.getTexture());
            m_SSAOShader.setUniform("depth_map", m_depthScreen.getTexture());
            m_SSAOShader.setUniform("screen_ratio", 
                sf::Vector2f(1.0 / (float)m_depthScreen.getSize().x,
                             1.0 / (float)m_depthScreen.getSize().y));
            m_SSAOrenderer.setSize(sf::Vector2f(m_depthScreen.getSize().x,
                                                m_depthScreen.getSize().y));
            m_SSAOrenderer.setTexture(&m_colorScreen.getTexture());
        } else {
            light_system.m_lightingShader.setUniform("useSSAO", false);
        }
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

    sf::Shader m_colorShader;
    sf::Shader m_normalShader;
    sf::Shader m_depthShader;  

    int m_superSampling{1};
    sf::RenderTexture m_colorScreen;
    sf::RenderTexture m_normalScreen;
    sf::RenderTexture m_depthScreen;

    sf::RectangleShape m_renderer;
    sf::RenderStates m_rendererStates;

    sf::Shader m_SSAOShader;
    sf::RenderTexture m_SSAOScreen;
    sf::RectangleShape m_SSAOrenderer;
    sf::Texture m_SSAONoiseTexture;
    sf::Image m_SSAONoisePattern;
};


