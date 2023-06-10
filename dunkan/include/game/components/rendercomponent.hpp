#pragma once

#include <cstddef>
#include <iostream>
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/Texture.hpp"
#include "SFML/System/Vector2.hpp"
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>

struct RenderComponent : public sf::Sprite {

    void set_texture(std::string filename)
    {
        if(m_texture.loadFromFile(filename)) {
            std::cout << m_texture.getSize().x << std::endl;
            sf::Sprite::setTextureRect(sf::IntRect(0, 0, m_texture.getSize().x, m_texture.getSize().y));
            this->height = 10.f;
            sf::Sprite::setTexture(m_texture);
            sf::Sprite::setScale(sf::Vector2f(1.0f, 1.0f));
            m_texture.setSmooth(true);
            m_texture.setRepeated(true);
            this->scale = 1.0f;
        }
    }

    void set_3D_textures(std::string depth, std::string normal)
    {
        if(!m_depth.loadFromFile(depth)) {
            std::cout << "Error loading the depth map\n";
        } else {
            m_depth.setSmooth(true);
            m_depth.setRepeated(true);
        }
        if(!m_normal.loadFromFile(normal)) {
            std::cout << "Error loading the depth map\n";
        } else {
            m_normal.setSmooth(true);
            m_normal.setRepeated(true);
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

    void prepare_shader(sf::Shader *shader) {
        if(shader != nullptr)
        {
            shader->setUniform("color_map",m_texture);
            shader->setUniform("depth_map",m_depth);
            shader->setUniform("useDepthMap", true);
            shader->setUniform("normal_map", m_normal);
            shader->setUniform("useNormalMap", true);
            shader->setUniform("height", ((float)height*(float)getScale().x));
        }
    }

    float height;
    float scale;
    bool is_selected{false};

private:
    sf::Texture m_texture;
    sf::Texture m_depth;
    sf::Texture m_normal;
};

