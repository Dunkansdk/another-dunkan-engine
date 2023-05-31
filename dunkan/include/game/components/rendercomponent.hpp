#pragma once

#include <cstddef>
#include <iostream>
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/Texture.hpp"
#include "SFML/System/Vector2.hpp"
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>

#define GL_SILENCE_DEPRECATION

struct RenderComponent : public sf::Sprite {

    void set_texture(std::string filename)
    {
        if(m_texture.loadFromFile(filename)) {
            std::cout << m_texture.getSize().x << std::endl;
            sf::Sprite::setTextureRect(sf::IntRect(0, 0, m_texture.getSize().x, m_texture.getSize().y));
            this->height = 400.f;
            sf::Sprite::setTexture(m_texture);
            sf::Sprite::setScale(sf::Vector2f(1.0f, 1.0f));
            this->scale = 1.0f;
        }
    }

    void set_3D_textures(std::string depth, std::string normal)
    {
        if(!m_depth.loadFromFile(depth)) {
            std::cout << "Error loading the depth map\n";
        }
        if(!m_normal.loadFromFile(normal)) {
            std::cout << "Error loading the depth map\n";
        }
    }

    sf::Texture& get_texture() {
        return m_texture;
    }

    sf::Texture& depth_texture() {
        return m_depth;
    }

    sf::Texture& normal_texture() {
        return m_normal;
    }

    float height;
    float scale;
    bool is_selected{false};

private:
    sf::Texture m_texture;
    sf::Texture m_depth;
    sf::Texture m_normal;
};

