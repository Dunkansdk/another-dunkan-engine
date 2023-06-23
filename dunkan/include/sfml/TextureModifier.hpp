#pragma once

#include <SFML/Graphics.hpp>

// TODO: Should be Singleton.
class TextureModifier {

private:
    sf::Shader m_blurShader;

    const std::string blur_fragShader = \
    "uniform sampler2D map_texture;" \
    "uniform vec2 p_offset;" \
    "void main()" \
    "{" \
    "    gl_FragColor =  gl_Color * "
	"			   (texture2D(map_texture, gl_TexCoord[0].xy + p_offset * 1.0)	* 0.000003 + "
	"				texture2D(map_texture, gl_TexCoord[0].xy + p_offset * 0.8)	* 0.000229 + "
	"				texture2D(map_texture, gl_TexCoord[0].xy + p_offset * 0.6)	* 0.005977 + "
	"				texture2D(map_texture, gl_TexCoord[0].xy + p_offset * 0.4)	* 0.060598 + "
	"    			texture2D(map_texture, gl_TexCoord[0].xy + p_offset * 0.2)	* 0.24173 + "
	"				texture2D(map_texture, gl_TexCoord[0].xy)	* 0.382925 + "
	"			    texture2D(map_texture, gl_TexCoord[0].xy - p_offset * 1.0)	* 0.000003 + "
	"				texture2D(map_texture, gl_TexCoord[0].xy - p_offset * 0.8)	* 0.000229 + "
	"				texture2D(map_texture, gl_TexCoord[0].xy - p_offset * 0.6)	* 0.005977 + "
	"				texture2D(map_texture, gl_TexCoord[0].xy - p_offset * 0.4)	* 0.060598 + "
	"    			texture2D(map_texture, gl_TexCoord[0].xy - p_offset * 0.2)	* 0.24173); "
    "}";

public:

    TextureModifier()
    {
        m_blurShader.loadFromMemory(blur_fragShader,sf::Shader::Fragment);
        m_blurShader.setUniform("map_texture", sf::Shader::CurrentTexture);
    }

    void BlurTexture(sf::Texture* texture, float blur)
    {
        sf::RectangleShape rect;
        rect.setTexture(texture);
        rect.setSize(sf::Vector2f(texture->getSize().x,
                                texture->getSize().y));

        sf::RenderTexture renderer;
        renderer.create(texture->getSize().x, texture->getSize().y);

        sf::Shader *blurShader = &m_blurShader;

        blurShader->setUniform("p_offset",sf::Vector2f(blur/texture->getSize().x,0));
        renderer.draw(rect,blurShader);
        renderer.display();

        rect.setTexture(&renderer.getTexture());
        blurShader->setUniform("p_offset",sf::Vector2f(0,blur/texture->getSize().y));
        renderer.draw(rect,blurShader);
        renderer.display();

        texture->update(renderer.getTexture().copyToImage());
    }

};