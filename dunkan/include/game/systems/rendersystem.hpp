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

#define PI 3.14159265

using RenderSystem_c = ADE::META_TYPES::Typelist<RenderComponent, PhysicsComponent>;
using RenderSystem_t = ADE::META_TYPES::Typelist<>;

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

    void init_renderer(sf::RenderWindow& window, TextureManager& texture_manager) {
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
        
        m_colorShader.loadFromFile("shaders/color.frag", sf::Shader::Fragment);
        light_system.m_depthShader.loadFromFile("shaders/depth.frag", sf::Shader::Fragment);
        m_normalShader.loadFromFile("shaders/normal.frag",sf::Shader::Fragment);
        m_SSAOShader.loadFromFile("shaders/default.vert", "shaders/ssao.frag");
        m_pbrShader.loadFromFile("shaders/pbr.frag", sf::Shader::Fragment);
        light_system.get_light_shader()->loadFromFile("shaders/default.vert", "shaders/light.frag");
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
        light_system.get_light_shader()->setUniform("map_brdflut", texture_manager.get("brd-flut"));
        
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
            c.r = (int)(static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/255))) / 2.0;
            c.g = (int)(static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/255))) / 2.0;
            c.r = 0;
            m_SSAONoisePattern.setPixel(x,y,c);
        }

        m_SSAONoiseTexture.setRepeated(true);
        m_SSAONoiseTexture.loadFromImage(m_SSAONoisePattern);
        m_SSAONoiseTexture.setRepeated(true);
        m_SSAOShader.setUniform("noise_map",m_SSAONoiseTexture);


        compute_trigonometry();
    }

    void update(EntityManager& entity_manager, sf::RenderWindow& window) {

        window.clear();

        light_system.get_light_shader()->setUniform("debug_screen", debug_screen);

        sf::View current_view = window.getView();
        sf::Vector2f view_shift = current_view.getCenter();
        view_shift -= sf::Vector2f(current_view.getSize().x / 2, current_view.getSize().y / 2);

        light_system.calculate_lights(entity_manager, view_shift, current_view, m_colorScreen.getSize());
        light_system.get_light_shader()->setUniform("view_shift", view_shift);
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
        // m_pbrScreen.display();
        // m_depthScreen.display();
        // m_normalScreen.display();

        if(Configuration::get()->enable_SSAO)
        {
            m_SSAOScreen.draw(m_SSAOrenderer,&m_SSAOShader);
            m_SSAOScreen.display();
        }

        light_system.get_light_shader()->setUniform("view_pos", sf::Vector3f(
            current_view.getCenter().x, 
            current_view.getCenter().y, 
            750.0f
        ));
        
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

    Mat3x3 m_isoToCartMat;

    void compute_trigonometry() {
        float cosXY = cos(90*PI/180.0);
        float sinXY = sin(90*PI/180.0);
        float cosZ = cos(30*PI/180);
        float sinZ = sin(30*PI/180);

        m_isoToCartMat = Mat3x3(cosXY        , -sinXY       , 0    ,
                             sinXY * sinZ , cosXY * sinZ , -cosZ,
                             0            , 0            , 0);

        light_system.get_light_shader()->setUniform("p_isoToCartMat",sf::Glsl::Mat3(m_isoToCartMat.values));
        light_system.get_light_shader()->setUniform("p_isoToCartZFactor",m_isoToCartMat.values[5]);
        m_SSAOShader.setUniform("p_isoToCartMat",sf::Glsl::Mat3(m_isoToCartMat.values));
        m_SSAOShader.setUniform("p_isoToCartZFactor",m_isoToCartMat.values[5]);

    }

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


