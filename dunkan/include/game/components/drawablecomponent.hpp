#pragma once

#include "SFML/Graphics/Drawable.hpp"
#include "SFML/Graphics/Rect.hpp"
#include "SFML/Graphics/RenderStates.hpp"
#include "SFML/Graphics/RenderTarget.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/Texture.hpp"
#include "SFML/Graphics/Transformable.hpp"
#include "SFML/Graphics/Vertex.hpp"
#include "SFML/Graphics/VertexArray.hpp"

struct DrawableComponent : protected sf::Drawable, public sf::Transformable {

public:
    void render(sf::RenderWindow& window) {
        this->draw(window, states);
    }

private:
    sf::RenderStates states;
    sf::Vertex         m_vertices[4]; ///< Vertices defining the sprite's geometry
    const sf::Texture* m_texture;     ///< Texture of the sprite
    sf::IntRect       m_textureRect; ///< Rectangle defining the area of the source texture to display

    ////////////////////////////////////////////////////////////
    void setTexture(const sf::Texture& texture, bool resetRect)
    {
        // Recompute the texture area if requested, or if there was no valid texture & rect before
        if (resetRect || (!m_texture && (m_textureRect == sf::IntRect())))
            setTextureRect(sf::IntRect(0, 0, texture.getSize().x, texture.getSize().y));

        // Assign the new texture
        m_texture = &texture;
    }


    ////////////////////////////////////////////////////////////
    void setTextureRect(const sf::IntRect& rectangle)
    {
        if (rectangle != m_textureRect)
        {
            m_textureRect = rectangle;
            updatePositions();
            updateTexCoords();
        }
    }


    ////////////////////////////////////////////////////////////
    void setColor(const sf::Color& color)
    {
        // Update the vertices' color
        m_vertices[0].color = color;
        m_vertices[1].color = color;
        m_vertices[2].color = color;
        m_vertices[3].color = color;
    }


    ////////////////////////////////////////////////////////////
    const sf::Texture* getTexture() const
    {
        return m_texture;
    }


    ////////////////////////////////////////////////////////////
    const sf::IntRect& getTextureRect() const
    {
        return m_textureRect;
    }


    ////////////////////////////////////////////////////////////
    const sf::Color& getColor() const
    {
        return m_vertices[0].color;
    }


    ////////////////////////////////////////////////////////////
    sf::FloatRect getLocalBounds() const
    {
        float width = static_cast<float>(std::abs(m_textureRect.width));
        float height = static_cast<float>(std::abs(m_textureRect.height));

        return sf::FloatRect(0.f, 0.f, width, height);
    }


    ////////////////////////////////////////////////////////////
    sf::FloatRect getGlobalBounds() const
    {
        return getTransform().transformRect(getLocalBounds());
    }


    ////////////////////////////////////////////////////////////
    void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        if (m_texture)
        {
            states.transform *= getTransform();
            states.texture = m_texture;
            target.draw(m_vertices, 4, sf::TriangleStrip, states);
        }
    }


    ////////////////////////////////////////////////////////////
    void updatePositions()
    {
        sf::FloatRect bounds = getLocalBounds();

        m_vertices[0].position = sf::Vector2f(0, 0);
        m_vertices[1].position = sf::Vector2f(0, bounds.height);
        m_vertices[2].position = sf::Vector2f(bounds.width, 0);
        m_vertices[3].position = sf::Vector2f(bounds.width, bounds.height);
    }


    ////////////////////////////////////////////////////////////
    void updateTexCoords()
    {
        float left   = static_cast<float>(m_textureRect.left);
        float right  = left + m_textureRect.width;
        float top    = static_cast<float>(m_textureRect.top);
        float bottom = top + m_textureRect.height;

        m_vertices[0].texCoords = sf::Vector2f(left, top);
        m_vertices[1].texCoords = sf::Vector2f(left, bottom);
        m_vertices[2].texCoords = sf::Vector2f(right, top);
        m_vertices[3].texCoords = sf::Vector2f(right, bottom);
    }
///     virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
///     {
///         // You can draw other high-level objects
///         target.draw(m_sprite, states);
///
///         // ... or use the low-level API
///         states.texture = &m_texture;
///         target.draw(m_vertices, states);
///
///         // ... or draw with OpenGL directly
///         glBegin(GL_QUADS);
///         ...
///         glEnd();
///     }
///
///     sf::Sprite m_sprite;
///     sf::Texture m_texture;
///     sf::VertexArray m_vertices;

};

