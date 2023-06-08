#pragma once

#include "game/types.hpp"
#include <cwchar>
#include <ratio>

#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>

using LightSystem_c = ADE::META_TYPES::Typelist<LightComponent, PhysicsComponent>;
using LightSystem_t = ADE::META_TYPES::Typelist<>;

struct LightSystem {

    int update(EntityManager& entity_manager) {

        m_current_nbr_light = 0;

        entity_manager.foreach<LightSystem_c, LightSystem_t>
        ([&](Entity& entity, LightComponent& light, PhysicsComponent& physics)
        {
            sf::Vector3f position(0, 0, 0);
            GLfloat gl_position[] = {0, 0, 0, 1.0};
            GLfloat gl_direction[] = {0, 0, 1};
            GLfloat glColor[] = {1, 1, 1, 1};

            position = sf::Vector3f(physics.x, physics.y, physics.z);
            
            if(light.global_light) {
                gl_position[3] = 0;
            }

            gl_position[0] = position.x;
            gl_position[1] = position.y;
            gl_position[2] = position.z;

            // TODO: DIRECTIONAL_LIGHT
            //gl_position[3] = 0;

            glLightfv(GL_LIGHT0 + m_current_nbr_light, GL_POSITION, gl_position);
            SfColorToGlColor(light.diffuse_color, glColor);
            glLightfv(GL_LIGHT0 + m_current_nbr_light, GL_DIFFUSE, glColor);
            SfColorToGlColor(light.specular_color, glColor);
            glLightfv(GL_LIGHT0 + m_current_nbr_light, GL_SPECULAR, glColor);
            glLightf(GL_LIGHT0 + m_current_nbr_light, GL_CONSTANT_ATTENUATION, light.contant_attenuation);
            glLightf(GL_LIGHT0 + m_current_nbr_light, GL_LINEAR_ATTENUATION, light.linear_attenuation);
            glLightf(GL_LIGHT0 + m_current_nbr_light, GL_QUADRATIC_ATTENUATION, light.quadratic_attenuation);

            position = light.direction;
            gl_direction[0] = position.x;
            gl_direction[1] = position.y;
            gl_direction[2] = position.z;
            glLightfv(GL_LIGHT0 + m_current_nbr_light, GL_SPOT_DIRECTION, gl_direction);
            
            ++m_current_nbr_light;
        });

        return m_current_nbr_light;

    }

private:
    int m_current_nbr_light = 0;

    void SfColorToGlColor(const sf::Color& sfColor, float glColor[4])
    {
        glColor[0] = (float)sfColor.r * 0.00392156862;
        glColor[1] = (float)sfColor.g * 0.00392156862;
        glColor[2] = (float)sfColor.b * 0.00392156862;
        glColor[3] = (float)sfColor.a * 0.00392156862;
    }

};

