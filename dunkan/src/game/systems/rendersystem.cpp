#include "game/systems/rendersystem.hpp"
#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/RenderStates.hpp"
#include "SFML/Graphics/Shader.hpp"
#include "SFML/System/Vector2.hpp"
#include <GL/gl.h>

using RenderSystem_c = ADE::META_TYPES::Typelist<RenderComponent, PhysicsComponent>;
using RenderSystem_t = ADE::META_TYPES::Typelist<>;

const std::string depth_fragShader = \
    "uniform sampler2D colorMap;" \
    "uniform sampler2D depthMap;" \
    "uniform float height;" \
    "uniform float zPos;" \
    "void main()" \
    "{" \
    "   vec4 pixel = texture2D(colorMap, gl_TexCoord[0].xy);" \
    "   gl_FragDepth = 1.0 - texture2D(depthMap, gl_TexCoord[0].xy).a*(texture2D(depthMap, gl_TexCoord[0].xy).r*height + zPos);" \
    "   gl_FragColor = gl_Color * pixel; " \
    "}";

void RenderSystem::update(EntityManager& entity_manager, /*test*/sf::RenderWindow& window) {
    window.clear();
    entity_manager.foreach<RenderSystem_c, RenderSystem_t>
    ([&](auto& entity, RenderComponent& render, PhysicsComponent& physics)
    {
        glClear(GL_DEPTH_BUFFER_BIT);
        render.setOrigin(sf::Vector2f(physics.x, physics.y));

        window.pushGLStates();

        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);

        sf::Shader depth;
        depth.loadFromMemory(depth_fragShader, sf::Shader::Fragment);
        depth.setUniform("colorMap", render.get_texture());
        depth.setUniform("depthMap", render.depth_texture());
        depth.setUniform("height", render.getTextureRect().height * render.getScale().y*0.001f);
        depth.setUniform("zPos", 0.1f * 0.001f);

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
