#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/RenderStates.hpp"
#include "SFML/Graphics/Shader.hpp"
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
"   vec4 color_pixel = texture2D(color, gl_TexCoord[0].xy);" \
"   vec4 depth_pixel = texture2D(depth, gl_TexCoord[0].xy);" \
"   float z_pixel = (depth_pixel.r + depth_pixel.g + depth_pixel.b) * .33 * height + z_pos;" \
"   gl_FragDepth = 1.0 - depth_pixel.a * (0.5 + z_pixel * 0.0001);" \
"   gl_FragColor = gl_Color * color_pixel; " \
"}";

struct RenderSystem {

    void update(EntityManager& entity_manager, sf::RenderWindow& window) {
        window.clear();
        glClear(GL_DEPTH_BUFFER_BIT);

        entity_manager.foreach<RenderSystem_c, RenderSystem_t>
        ([&](auto& entity, RenderComponent& render, PhysicsComponent& physics)
        {
            render.setOrigin(
                    sf::Vector2f((window.getView().getViewport().height) + (physics.x + (render.get_texture().getSize().x * 0.5)),
                    (window.getView().getViewport().width) + (physics.y + (render.get_texture().getSize().y * 0.5))));

            window.pushGLStates();

            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);

            sf::Shader depth;
            depth.loadFromMemory(depth_fragShader, sf::Shader::Fragment);
            depth.setUniform("color", render.getTexture());
            depth.setUniform("depth", render.depth_texture());
            depth.setUniform("height", render.getTextureRect().height * render.getScale().y * 0.001f);
            depth.setUniform("z_pos", physics.z * 0.001f);

            sf::Transform totalTransform;
            totalTransform = sf::Transform::Identity;

            sf::RenderStates state;
            state.transform = totalTransform;
            state.shader = &depth;
            window.draw(render, state);

            window.popGLStates();
        });

        window.display();
    }

};


