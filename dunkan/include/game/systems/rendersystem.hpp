#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/RenderStates.hpp"
#include "SFML/Graphics/Shader.hpp"
#include "imgui.h"
#include "imgui-SFML.h"
#include "SFML/System/Vector2.hpp"

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

const std::string depth_fragShader = \
"uniform sampler2D color;" \
"uniform sampler2D depth;" \
"uniform float height;" \
"uniform float z_pos;" \
"uniform bool debug_heightmap;" \
"void main()" \
"{" \
"   vec4 depth_pixel = texture2D(depth, gl_TexCoord[0].xy);" \
"   vec4 color_pixel = texture2D(color, gl_TexCoord[0].xy);" \
"   if(depth_pixel.a == 0.0) discard;" \
"   float z_pixel = height + z_pos;"
"   gl_FragDepth = (((depth_pixel.r + depth_pixel.g + depth_pixel.b) / 3.0) * z_pixel );" \
"   if(debug_heightmap) {" \
"   gl_FragColor = vec4(gl_FragDepth, gl_FragDepth, gl_FragDepth, color_pixel.a);" \
"   } else { " \
"   gl_FragColor = color_pixel;" \
"   }" \
"}";

struct RenderSystem {

    bool debug_heightmap{true};

    void update(EntityManager& entity_manager, sf::RenderWindow& window) {
        window.clear();

        glClear(GL_DEPTH_BUFFER_BIT);
        glClearDepth(0.0);

        entity_manager.foreach<RenderSystem_c, RenderSystem_t>
        ([&](Entity& entity, RenderComponent& render, PhysicsComponent& physics)
        {
            render.setPosition(sf::Vector2f(
                        physics.x - (render.get_texture().getSize().x * render.scale * 0.5),
                        physics.y - physics.z - (render.get_texture().getSize().y * render.scale * 0.5)));

            render.setScale(sf::Vector2f(render.scale, render.scale));
            window.pushGLStates();

            glDepthFunc(GL_GREATER);
            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);

            sf::Shader depth;
            depth.loadFromMemory(depth_fragShader, sf::Shader::Fragment);
            depth.setUniform("color", render.get_texture());
            depth.setUniform("depth", render.depth_texture());
            depth.setUniform("height", render.height * render.getScale().y * 0.001f);
            depth.setUniform("z_pos", physics.z * 0.01f);
            depth.setUniform("debug_heightmap", debug_heightmap);

            sf::Transform totalTransform;
            totalTransform = sf::Transform::Identity;

            sf::RenderStates state;
            state.transform = totalTransform;
            state.shader = &depth;
            window.draw(render, state);

            window.popGLStates();
        });

#ifdef DEBUG_IMGUI
        ImGui::SFML::Render(window);
#endif

        window.display();
    }

};


