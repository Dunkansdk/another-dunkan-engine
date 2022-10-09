#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/RenderStates.hpp"
#include "SFML/Graphics/Shader.hpp"
#include "imgui.h"
#include "imgui-SFML.h"
#include "SFML/System/Vector2.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include "game/types.hpp"

using RenderSystem_c = ADE::META_TYPES::Typelist<RenderComponent, PhysicsComponent>;
using RenderSystem_t = ADE::META_TYPES::Typelist<>;

const std::string depth_fragShader = \
"uniform sampler2D color;" \
"uniform sampler2D depth;" \
"uniform float height;" \
"uniform float z_pos;" \
"void main()" \
"{" \
"   vec4 depth_pixel = texture2D(depth, gl_TexCoord[0].xy);" \
"   vec4 color_pixel = texture2D(color, gl_TexCoord[0].xy);" \
"   if(depth_pixel.a == 0.0) discard;" \
"   float z_pixel = height + z_pos;"
"   gl_FragDepth = (((depth_pixel.r + depth_pixel.g + depth_pixel.b) / 3.0) * z_pixel );" \
"   gl_FragColor = color_pixel * vec4(gl_FragDepth, gl_FragDepth, gl_FragDepth, color_pixel.a);" \
"}";

struct RenderSystem {

    void update(EntityManager& entity_manager, sf::RenderWindow& window) {
        window.clear();

        glClear(GL_DEPTH_BUFFER_BIT);
        glClearDepth(0.0);

        entity_manager.foreach<RenderSystem_c, RenderSystem_t>
        ([&](Entity& entity, RenderComponent& render, PhysicsComponent& physics)
        {
            render.setOrigin(
                    sf::Vector2f((window.getView().getViewport().height) + (physics.x + (render.get_texture().getSize().x * 0.5)),
                    (window.getView().getViewport().width) + (physics.y + (render.get_texture().getSize().y * 0.5))));

            window.pushGLStates();

            glDepthFunc(GL_GREATER);
            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);

            sf::Shader depth;
            depth.loadFromMemory(depth_fragShader, sf::Shader::Fragment);
            depth.setUniform("color", render.get_texture());
            depth.setUniform("depth", render.depth_texture());
            depth.setUniform("height", render.height * render.getScale().y * 0.0003f);
            depth.setUniform("z_pos", physics.z * 0.01f);

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


