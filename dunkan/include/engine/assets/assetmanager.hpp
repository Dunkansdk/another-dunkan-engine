#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>

namespace ADE {

    struct AssetManager {

        static sf::Texture load_texture(std::string file)
        {
            sf::Texture texture;

            if(!texture.loadFromFile(file)) {
                std::cout << "Error loading texture: " << file << "\n";
            }

            return texture;
        }

    };

}
