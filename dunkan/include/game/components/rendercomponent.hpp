#pragma once

#include <iostream>
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/Texture.hpp"
#include <SFML/Graphics.hpp>

struct RenderComponent : public sf::Sprite {

    void set_texture(std::string filename)
    {
        if(texture.loadFromFile(filename)) {
            sf::Sprite::setTextureRect(sf::IntRect(0, 0, texture.getSize().x, texture.getSize().y));
            sf::Sprite::setTexture(texture);
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
        return texture;
    }

    sf::Texture& depth_texture() {
        return m_depth;
    }

    sf::Texture& normal_texture() {
        return m_normal;
    }

private:
    sf::Texture texture;
    sf::Texture m_depth;
    sf::Texture m_normal;
};

