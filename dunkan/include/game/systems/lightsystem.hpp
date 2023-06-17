#pragma once

#include "game/types.hpp"
#include "game/systems/shadowsystem.hpp"
#include <cwchar>
#include <ratio>
#include <sstream>

#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>

using LightSystem_c = ADE::META_TYPES::Typelist<LightComponent, PhysicsComponent>;
using LightSystem_t = ADE::META_TYPES::Typelist<>;

struct LightSystem {

    ShadowSystem shadow_system{};
    sf::Shader m_lightingShader;
    sf::Shader m_depthShader;

    void calculate_ligts(EntityManager& entity_manager, sf::Vector2f view_shift, const sf::View &view,
                                 const sf::Vector2u &screen_size) {

        m_current_nbr_light = 0;
        m_current_nbr_shadows = 0;

        float shadow_casting_lights[8] = {-1,-1,-1,-1,-1,-1,-1,-1};
        sf::Vector2f shadow_shift[8];
        sf::Vector2f shadow_ratio[8];

        entity_manager.foreach<LightSystem_c, LightSystem_t>
        ([&](Entity&, LightComponent& light, PhysicsComponent& physics)
        {
            sf::Vector3f position(0, 0, 0);
            GLfloat gl_position[] = {0, 0, 0, 1.0};
            GLfloat gl_direction[] = {0, 0, 1};
            GLfloat glColor[] = {1, 1, 1, 1};

            if(light.radius < 0.f) light.radius = 0.f;
            if(light.intensity < 0.f) light.intensity = 0.f;

            if(light.light_type == LightType::DIRECTIONAL) {
                position = light.direction;
                gl_position[3] = 0;
            } else {
                position = physics.position(view_shift);
            }

            gl_position[0] = position.x;
            gl_position[1] = position.y;
            gl_position[2] = position.z;

            glLightfv(GL_LIGHT0 + m_current_nbr_light, GL_POSITION, gl_position);
            SfColorToGlColor(light.diffuse_color, glColor);
            // glColor[0] *= light.intensity;
            // glColor[1] *= light.intensity;
            // glColor[2] *= light.intensity;

            glLightfv(GL_LIGHT0 + m_current_nbr_light, GL_DIFFUSE, glColor);
            SfColorToGlColor(light.specular_color, glColor);
            glLightfv(GL_LIGHT0 + m_current_nbr_light, GL_SPECULAR, glColor);
            glLightf(GL_LIGHT0 + m_current_nbr_light, GL_CONSTANT_ATTENUATION, light.contant_attenuation);
            // glLightf(GL_LIGHT0 + m_current_nbr_light, GL_CONSTANT_ATTENUATION, light.radius);
            glLightf(GL_LIGHT0 + m_current_nbr_light, GL_LINEAR_ATTENUATION, light.linear_attenuation);
            glLightf(GL_LIGHT0 + m_current_nbr_light, GL_QUADRATIC_ATTENUATION, light.quadratic_attenuation);
            
            position = light.direction;
            gl_direction[0] = position.x;
            gl_direction[1] = position.y;
            gl_direction[2] = position.z;
            glLightfv(GL_LIGHT0 + m_current_nbr_light, GL_SPOT_DIRECTION, gl_direction);

            if(light.cast_shadow) {
                if(light.require_shadow_computation())
                    shadow_system.calculate(entity_manager, &light); 
                shadow_system.render(entity_manager, view, screen_size, &light, &m_depthShader);
            }


            // TODO: Refactor this render.
            std::ostringstream buffer;

            if(light.cast_shadow)
            {
                buffer << "shadow_map_" << m_current_nbr_shadows;
                shadow_casting_lights[m_current_nbr_shadows] = (float)m_current_nbr_light;
                sf::IntRect cur_shift = shadow_system.get_max_shadow_shift();
                shadow_shift[m_current_nbr_shadows] = sf::Vector2f(cur_shift.left,
                                                        -cur_shift.height - cur_shift.top ); /*GLSL Reverse y-coord*/
                shadow_ratio[m_current_nbr_shadows] = sf::Vector2f(1.0/(float) shadow_system.get_shadow_map()->getSize().x,
                                                        1.0/(float)shadow_system.get_shadow_map()->getSize().y);
                m_lightingShader.setUniform(buffer.str(), shadow_system.get_shadow_map());

                ++m_current_nbr_shadows;
            }
            
            ++m_current_nbr_light;
        }); 

        m_lightingShader.setUniformArray("shadow_casters",shadow_casting_lights, 8);
        m_lightingShader.setUniformArray("shadow_shift",shadow_shift, 8);
        m_lightingShader.setUniformArray("shadow_ratio",shadow_ratio, 8);
        m_lightingShader.setUniform("view_shift",view_shift);
        m_lightingShader.setUniform("nbr_lights",(int)m_current_nbr_light);

    }

private:
    int m_current_nbr_light = 0;
    int m_current_nbr_shadows = 0;

    void SfColorToGlColor(const sf::Color& sfColor, float glColor[4])
    {
        glColor[0] = (float)sfColor.r * 0.00392156862;
        glColor[1] = (float)sfColor.g * 0.00392156862;
        glColor[2] = (float)sfColor.b * 0.00392156862;
        glColor[3] = (float)sfColor.a * 0.00392156862;
    }

};

