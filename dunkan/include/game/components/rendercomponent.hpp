#pragma once

#include <iostream>
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/Texture.hpp"
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>

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

    void setScale(float scale) {
        sf::Sprite::setScale(sf::Vector2f(scale, scale));
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

    ImVec4 color = ImVec4(114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 200.0f / 255.0f);

private:
    sf::Texture texture;
    sf::Texture m_depth;
    sf::Texture m_normal;
};

