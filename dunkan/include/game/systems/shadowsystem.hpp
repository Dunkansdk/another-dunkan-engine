#pragma once

#include <SFML/OpenGL.hpp>

#include "ecs/utils/Mathematics.hpp"
#include "game/types.hpp"

using ShadowSystem_c = ADE::META_TYPES::Typelist<RenderComponent, PhysicsComponent, ShadowComponent>;
using ShadowSystem_t = ADE::META_TYPES::Typelist<>;

struct ShadowSystem {

    void calculate(EntityManager& entity_manager, LightComponent* light) {

        sf::IntRect max_shift_map(0,0,0,0);

        entity_manager.foreach<ShadowSystem_c, ShadowSystem_t>
        ([&](Entity&, RenderComponent& render, PhysicsComponent& physics, ShadowComponent& shadow)
        {
            if(light->require_shadow_computation()) {
                if(light->light_type == LightType::DIRECTIONAL && 
                (shadow.shadow_type == ShadowCastingType::DIRECTIONNAL || shadow.shadow_type == ShadowCastingType::ALL_SHADOWS)) 
                {
                    float height = render.height * render.getScale().y;
                    sf::Vector3f light_direction = light->direction;

                    sf::Vector2f shifting(
                        -light_direction.x * height / light_direction.z,
                        -light_direction.y * height / light_direction.z
                    );

                    sf::Vector2f max_shift = shifting;
                    max_shift.y -= height;
                    max_shift.x = (int)max_shift.x + ((max_shift.x > 0) ? 1 : -1);
                    max_shift.y = (int)max_shift.y + ((max_shift.y > 0) ? 1 : -1);

                    sf::IntRect shadow_bounds(
                        0,
                        0,
                        render.getGlobalBounds().width,
                        render.getGlobalBounds().height
                    );

                    if(max_shift.x < 0)
                        shadow_bounds.left = max_shift.x;
                    if(max_shift.y < 0)
                        shadow_bounds.top = max_shift.y;

                    sf::IntRect shift = sf::IntRect(shadow_bounds.left,shadow_bounds.top,
                                                  abs(max_shift.x), abs(max_shift.y));

                    if(shift.left < max_shift_map.left)
                        max_shift_map.left = shift.left;
                    if(shift.top < max_shift_map.top)
                        max_shift_map.top = shift.top;
                    if(shift.width > max_shift_map.width)
                        max_shift_map.width = shift.width;
                    if(shift.height > max_shift_map.height)
                        max_shift_map.height = shift.height;

                    m_shadow_max_shift = max_shift_map;

                    sf::Uint8 *shadow_map_array = new sf::Uint8[shadow_bounds.width * shadow_bounds.height * 4];

                    sf::Texture* depth_texture = render.m_depth;
                    sf::Image depth_img = depth_texture->copyToImage();
                    size_t depth_texture_width = depth_img.getSize().x;
                    const sf::Uint8* depth_array = depth_img.getPixelsPtr();

                    for(size_t t = 0 ; t < (size_t)(shadow_bounds.width * shadow_bounds.height) ; ++t)
                    {
                        shadow_map_array[t*4] = 0;
                        shadow_map_array[t*4+1] = 0;
                        shadow_map_array[t*4+2] = 0;
                        shadow_map_array[t*4+3] = 0;
                    }

                    float height_pixel = 0;
                    sf::Vector2f proj_pos(0, 0);

                    for(size_t x = 0 ; x < depth_texture_width ; ++x)
                    for(size_t y = 0 ; y < depth_img.getSize().y ; ++y)
                    if(depth_array[(x + y * depth_texture_width) * 4 + 3] == 255)
                    {
                        sf::Color color_pixel(depth_array[(x + y * depth_texture_width) * 4],
                                            depth_array[(x + y * depth_texture_width) * 4 + 1],
                                            depth_array[(x + y * depth_texture_width) * 4 + 2],
                                            depth_array[(x + y * depth_texture_width) * 4 + 3]);
                        height_pixel  = (color_pixel.r + color_pixel.g + color_pixel.b);
                        height_pixel *= height * 0.00130718954f;

                        sf::Vector2f position(x, y);
                        position.y -= height_pixel;
                        position -= (sf::Vector2f(light_direction.x / light_direction.z,
                                        light_direction.y/light_direction.z) * height_pixel);

                        position.x = (int)(position.x + 0.5f) - shadow_bounds.left;
                        position.y = (int)(position.y + 0.5f) - shadow_bounds.top;

                        for(int dx = -1 ; dx <= 1 ; ++dx)
                        for(int dy = -1 ; dy <= 1 ; ++dy)
                        {
                            int array_position = ((position.x + dx) + (position.y + dy) * shadow_bounds.width) * 4;

                            if(array_position >= 0 && array_position < shadow_bounds.width * shadow_bounds.height * 4)
                            if(color_pixel.a*(color_pixel.r + color_pixel.g + color_pixel.b) >
                            shadow_map_array[array_position + 3] * (shadow_map_array[array_position]
                                                            + shadow_map_array[array_position + 1]
                                                            + shadow_map_array[array_position + 2]))
                            {
                                shadow_map_array[array_position]        = color_pixel.r;
                                shadow_map_array[array_position + 1]    = color_pixel.g;
                                shadow_map_array[array_position + 2]    = color_pixel.b;
                                shadow_map_array[array_position + 3]    = color_pixel.a;
                            }
                        }
                    }

                    sf::Texture* shadow_texture = &shadow.m_shadow_map[light];
                    shadow_texture->create(shadow_bounds.width, shadow_bounds.height);
                    shadow_texture->update(shadow_map_array, shadow_bounds.width, shadow_bounds.height, 0, 0);
                    // Add blur
                    shadow.m_shadow_sprite[light].setTexture(*shadow_texture);
                    shadow.m_shadow_sprite[light].setOrigin(render.getOrigin()
                                                    -sf::Vector2f(shadow_bounds.left, shadow_bounds.top));

                    delete[] shadow_map_array;

                    light->require_shadow_computation(false);

                }
            } 
        });
    }

    void render(EntityManager& entity_manager, const sf::View& view, const sf::Vector2u& screen_size, LightComponent* light, sf::Shader* depth_shader) {
        if(m_shadow_map.getSize().x != screen_size.x + m_shadow_max_shift.width
        || m_shadow_map.getSize().y != screen_size.y + m_shadow_max_shift.height) {
            std::cout << "m_shadow_map: " << screen_size.x + m_shadow_max_shift.width << " " << screen_size.y + m_shadow_max_shift.height << " " <<
        m_shadow_map.getSize().x << " " << m_shadow_map.getSize().y << std::endl;
        m_shadow_map.create(screen_size.x + m_shadow_max_shift.width,
                                screen_size.y + m_shadow_max_shift.height, true);
        }
            
        // std::cout << "m_shadow_max_shift: " << m_shadow_max_shift.left << " " << m_shadow_max_shift.top << " " <<
        // m_shadow_max_shift.width << " " << m_shadow_max_shift.height << std::endl;

        sf::View shadow_view = view;
        shadow_view.move(m_shadow_max_shift.width * 0.5 + m_shadow_max_shift.left,
                        m_shadow_max_shift.height * 0.5 + m_shadow_max_shift.top);
        shadow_view.setSize(view.getSize().x + m_shadow_max_shift.width,
                            view.getSize().y + m_shadow_max_shift.height);

        sf::Vector2f view_shift = view.getCenter();
        view_shift -= sf::Vector2f(view.getSize().x / 2, view.getSize().y / 2);

        m_shadow_map.setActive(true);
            glClear(GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);
            m_shadow_map.clear(sf::Color::White);
            m_shadow_map.setView(shadow_view);

            entity_manager.foreach<ShadowSystem_c, ShadowSystem_t>
            ([&](Entity&, RenderComponent& render, PhysicsComponent& physics, ShadowComponent& shadow)
            {
                sf::Vector3f global_position{physics.x, physics.y, physics.z};
                depth_shader->setUniform("color_map", shadow.m_shadow_map[light]);
                depth_shader->setUniform("depth_map", shadow.m_shadow_map[light]);
                depth_shader->setUniform("useDepthMap", true);
                depth_shader->setUniform("height", render.height * render.getScale().y);
                depth_shader->setUniform("z_position", physics.z);

                sf::Vector3f light_direction = Normalize(light->direction);
                global_position.x -= physics.x * light_direction.x / light_direction.z;
                global_position.x -= physics.y * light_direction.y / light_direction.z;
                global_position.z = 0;

                sf::RenderStates state;
                sf::Vector3f t = global_position;
                state.transform.translate(t.x - view_shift.x, t.y - view_shift.y);
                state.shader = depth_shader;
                m_shadow_map.draw(shadow.m_shadow_sprite[light], state);
            });

            m_shadow_map.display();
        //m_shadow_map.getTexture().copyToImage().saveToFile("shadow.png");
        m_shadow_map.setActive(false);
    }

    sf::RenderTexture* get_shadow_map() {
        return &m_shadow_map;
    }

    sf::IntRect get_max_shadow_shift() {
        return m_shadow_max_shift;
    }

private:

    sf::RenderTexture m_shadow_map{};
    sf::IntRect m_shadow_max_shift{};

};