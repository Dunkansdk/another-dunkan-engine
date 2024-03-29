#pragma once

#include <cstddef>
#include <iostream>
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/Texture.hpp"
#include "SFML/System/Vector2.hpp"
#include <SFML/Graphics.hpp>

#define GL_SILENCE_DEPRECATION

struct RenderComponent : public sf::Sprite {

    RenderComponent() : sf::Sprite() {}

    RenderComponent(sf::Texture& albedo, const sf::IntRect &rectangle, float height, float scale) 
        : sf::Sprite(albedo, rectangle)
    {
        m_texture = &albedo;
        std::cout << "Asset Manager Reference:\nAlbedoID: " << &albedo << "\nTextureID: " << &(*m_texture) << "\n";
        m_texture->setSmooth(true);
        m_texture->setRepeated(true);
        this->height = height;
        this->scale = scale;
    }

    RenderComponent(sf::Texture& albedo, const sf::IntRect &rectangle, float height, float scale, sf::Texture& normal, sf::Texture& depth) 
        : sf::Sprite(albedo, rectangle)
    {
        new (this) RenderComponent(albedo, rectangle, height, scale);
        m_normal = &normal; 
        m_normal->setSmooth(true);
        m_normal->setRepeated(true);
        m_depth = &depth; 
        m_depth->setSmooth(true);
        m_depth->setRepeated(true);
    }

    RenderComponent(sf::Texture& albedo, const sf::IntRect &rectangle, float height, float scale,  sf::Texture& normal, sf::Texture& depth, sf::Texture& material) 
        : sf::Sprite(albedo, rectangle)
    {
        new (this) RenderComponent(albedo, rectangle, height, scale, normal, depth);
        m_material = &material; 
        m_material->setSmooth(true);
        m_material->setRepeated(true);
    }

    RenderComponent(sf::Texture& albedo, const sf::IntRect &rectangle, float height, float scale,  sf::Texture& normal, sf::Texture& depth, float roughness, float metalness, float translucency) 
        : sf::Sprite(albedo, rectangle)
    {
        new (this) RenderComponent(albedo, rectangle, height, scale, normal, depth);
        this->roughness = roughness;
        this->metalness = metalness;
        this->translucency = translucency;
    }

    RenderComponent(sf::Texture& albedo, const sf::IntRect &rectangle, float height, float scale,  sf::Texture& normal, sf::Texture& depth, sf::Texture& material, bool moveable) 
        : sf::Sprite(albedo, rectangle)
    {
        new (this) RenderComponent(albedo, rectangle, height, scale, normal, depth, material);
        this->moveable = moveable;
    }

    RenderComponent& load() {
        sf::Sprite::setTexture(*(m_texture));
        sf::Sprite::setScale(sf::Vector2f(scale, scale));
        return *(this);
    }

    void prepare_shader(sf::Shader *shader) {
        if(shader != nullptr)
        {
            shader->setUniform("color_map",*m_texture);

            if(m_depth != nullptr) {
                shader->setUniform("depth_map",*m_depth);
                shader->setUniform("useDepthMap", true);
            } else {
                shader->setUniform("useDepthMap", false);
            }

            if(m_normal != nullptr) {
                shader->setUniform("normal_map",*m_normal);
                shader->setUniform("useNormalMap", true);
                
            } else {
                shader->setUniform("useNormalMap", false);
            }

            if(m_material != nullptr) {
                shader->setUniform("material_map",*m_material);
                shader->setUniform("enable_materialMap", true);
            } else {
                shader->setUniform("enable_materialMap", false);
            }
            
            shader->setUniform("p_roughness", roughness);
            shader->setUniform("p_metalness", metalness);
            shader->setUniform("p_translucency", translucency);
            shader->setUniform("height", ((float)height * (float)getScale().x));
        }
    }

    float height {10.0f};
    float scale {1.0f};
    bool is_selected{false};
    bool moveable{true};
    
    sf::Texture* m_texture{nullptr};
    sf::Texture* m_depth{nullptr};
    sf::Texture* m_normal{nullptr};
    sf::Texture* m_material{nullptr};
    
    float roughness{0.f};
    float metalness{0.f};
    float translucency{0.f};

};

