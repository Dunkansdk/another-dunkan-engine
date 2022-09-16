#include "game/systems/rendersystem.hpp"
#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/RenderStates.hpp"
#include "SFML/Graphics/Shader.hpp"
#include "SFML/System/Vector2.hpp"

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>

using RenderSystem_c = ADE::META_TYPES::Typelist<RenderComponent, PhysicsComponent>;
using RenderSystem_t = ADE::META_TYPES::Typelist<>;

const std::string depth_fragShader = \
    "uniform sampler2D color;" \
    "uniform sampler2D depth;" \
    "uniform float height;" \
    "uniform float z_position;" \
    "void main()" \
    "{" \
    "   vec4 pixel = texture2D(color, gl_TexCoord[0].xy);" \
    "   gl_FragDepth = 1.0 - texture2D(depth, gl_TexCoord[0].xy).a*(texture2D(depth, gl_TexCoord[0].xy).r * height + z_position);" \
    "   gl_FragColor = gl_Color * pixel; " \
    "}";

void RenderSystem::update(EntityManager& entity_manager, /*test*/sf::RenderWindow& window) {

    window.clear();
    glClear(GL_DEPTH_BUFFER_BIT);
    
    entity_manager.foreach<RenderSystem_c, RenderSystem_t>
    ([&](auto& entity, RenderComponent& render, PhysicsComponent& physics)
    {
        render.setOrigin(sf::Vector2f(physics.x, physics.y));

        window.pushGLStates();

        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);

        sf::Shader depth;
        depth.loadFromMemory(depth_fragShader, sf::Shader::Fragment);
        depth.setUniform("color", render.getTexture());
        depth.setUniform("depth", render.depth_texture());
        depth.setUniform("height", render.getTextureRect().height * render.getScale().y * 0.001f);
        depth.setUniform("z_position", physics.z * 0.001f);

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
