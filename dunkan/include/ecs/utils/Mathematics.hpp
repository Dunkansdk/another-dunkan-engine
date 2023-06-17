#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <iostream>

float ComputeDotProduct(sf::Vector3f v, sf::Vector3f w)
{
    return v.x*w.x + v.y*w.y + v.z*w.z;
}

float ComputeDotProduct(sf::Vector2f v, sf::Vector2f w)
{
    return v.x*w.x + v.y*w.y;
}

float ComputeSquareDistance(sf::Vector3f v, sf::Vector3f w)
{
    return ComputeDotProduct(v-w,v-w);
}

float ComputeSquareDistance(sf::Vector2f v, sf::Vector2f w)
{
    return ComputeDotProduct(v-w,v-w);
}

sf::Vector2f Normalize(sf::Vector2f v)
{
    float norm = std::sqrt(ComputeDotProduct(v,v));
    if(norm != 0)
        return v/norm;
    return v;
}

sf::Vector3f Normalize(sf::Vector3f v)
{
    float norm = std::sqrt(ComputeDotProduct(v,v));
    if(norm != 0)
        return v/norm;
    return v;
}

