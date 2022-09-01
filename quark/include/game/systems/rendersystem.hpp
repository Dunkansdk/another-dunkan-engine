#pragma once

#include "game/types.hpp"

struct RenderSystem {

    void update(EntityManager& entity_manager) {
        entity_manager.forall([](Entity&) {
            // 
        })

        // begin
        // draw
        // end
    }
}