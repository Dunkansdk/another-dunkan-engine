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

    void calculate_lights(EntityManager& entity_manager, sf::Vector2f view_shift, const sf::View &view,
                                 const sf::Vector2u &screen_size) {

        m_current_nbr_light = 0;

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
                position = physics.position(sf::Vector2f(0.0f, 0.0f));
            }

            gl_position[0] = position.x;
            gl_position[1] = position.y;
            gl_position[2] = position.z;

            glLightfv(GL_LIGHT0 + m_current_nbr_light, GL_POSITION, gl_position);
            // SfColorToGlColor(light.specular_color, glColor);
            // glLightfv(GL_LIGHT0 + m_current_nbr_light, GL_SPECULAR, glColor);
            SfColorToGlColor(light.diffuse_color, glColor);
            glColor[0] *= light.intensity;
            glColor[1] *= light.intensity;
            glColor[2] *= light.intensity;
            glLightfv(GL_LIGHT0 + m_current_nbr_light, GL_DIFFUSE, glColor);
            // glLightf(GL_LIGHT0 + m_current_nbr_light, GL_CONSTANT_ATTENUATION, light.constant_attenuation);
            glLightf(GL_LIGHT0 + m_current_nbr_light, GL_CONSTANT_ATTENUATION, light.radius);
            glLightf(GL_LIGHT0 + m_current_nbr_light, GL_LINEAR_ATTENUATION, light.linear_attenuation);
            glLightf(GL_LIGHT0 + m_current_nbr_light, GL_QUADRATIC_ATTENUATION, light.quadratic_attenuation);
            
            position = light.direction;
            gl_direction[0] = position.x;
            gl_direction[1] = position.y;
            gl_direction[2] = position.z;
            glLightfv(GL_LIGHT0 + m_current_nbr_light, GL_SPOT_DIRECTION, gl_direction);

            ++m_current_nbr_light;
        }); 

         m_lightingShader.setUniform("nbr_lights", (int)m_current_nbr_light);

    }
    sf::Shader* get_light_shader() {
        return &m_lightingShader;
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

