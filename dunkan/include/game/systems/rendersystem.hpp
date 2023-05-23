#pragma once

#include "SFML/Graphics/RenderWindow.hpp"
#include "game/types.hpp"

#if defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

struct RenderSystem {

    void update(EntityManager&, sf::RenderWindow&);

};


