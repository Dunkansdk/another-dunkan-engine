#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <cstdio>
#include <iostream>
#include <netinet/tcp.h>
#include <random>
#include <chrono>
#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/Window/Window.hpp"
#include "SFML/Window/WindowStyle.hpp"
#include "engine/entitymanager.hpp"
#include "engine/utils/typelist.hpp"

    struct CPosition { sf::Vector2f value; };
    struct CVelocity { sf::Vector2f value; };
    struct CAcceleration { sf::Vector2f value; };
    struct CLife { float value; };
    struct CRender {
        sf::RectangleShape shape;

        CRender() {
            shape.setFillColor(sf::Color::Red);
            shape.setSize(sf::Vector2f{5.f, 5.f});
        }
    };

    using MyComponents = ADE::META_TYPES::Typelist<CPosition, CVelocity, CAcceleration, CRender, CLife>;
    using MyTags = ADE::META_TYPES::Typelist<>;
    using MyManager = ADE::EntityManager<MyComponents, MyTags, 9999>;
    using Entity = MyManager::Entity;

    using SApplyVelocity = ADE::META_TYPES::Typelist<CPosition, CVelocity>;
    using SApplyAcceleration = ADE::META_TYPES::Typelist<CVelocity, CAcceleration>;
    using SRender = ADE::META_TYPES::Typelist<CPosition, CRender>;
    using SLife = ADE::META_TYPES::Typelist<CLife>;
    using SGrow = ADE::META_TYPES::Typelist<CRender, CLife>;
    using EMPTY_TAG = ADE::META_TYPES::Typelist<>;

float get_random(float min, float max)
{
    static std::default_random_engine e;
    static std::uniform_real_distribution<> dis(min, max); // rage 0 - 1
    return dis(e);
}

void update(MyManager& entity_manager, float time, sf::RenderWindow& window) {
    for(auto i(0u); i < 20; ++i) {
        auto& entity = entity_manager.create_entity();

        auto set_rnd_vec2([](auto& vec, auto val) {
            vec.x = get_random(-val, val);
            vec.y = get_random(-val, val);
        });

        auto& pos = entity_manager.add_component<CPosition>(entity).value;
        set_rnd_vec2(pos, 100.f);
        auto& vel = entity_manager.add_component<CVelocity>(entity).value;
        set_rnd_vec2(vel, 4.f);
        auto& acc = entity_manager.add_component<CAcceleration>(entity).value;
        set_rnd_vec2(acc, 0.4f);
        auto& life = entity_manager.add_component<CLife>(entity).value;
        life = get_random(13.f, 30.f);
        auto& shape = entity_manager.add_component<CRender>(entity).shape;
        shape.setFillColor(sf::Color(
            static_cast<int>(get_random(150, 255)),
            static_cast<int>(get_random(30, 70)),
            static_cast<int>(get_random(30, 70)), 100
        ));
    }

    // Systems
    //

    entity_manager.foreach<SApplyVelocity, EMPTY_TAG>
    ([&time](auto, auto& cPosition, auto& cVelocity) {
        cPosition.value += cVelocity.value * time;
    });

    entity_manager.foreach<SApplyAcceleration, EMPTY_TAG>
    ([&time](auto, auto& cVelocity, auto& cAcceleration) {
        cVelocity.value += cAcceleration.value * time;
    });

    entity_manager.foreach<SRender, EMPTY_TAG>
    ([](auto, auto& cPosition, auto& cRender) {
        auto& s(cRender.shape);
        s.setPosition(cPosition.value);
    });

    entity_manager.foreach<SLife, EMPTY_TAG>
    ([&time, &entity_manager](Entity& entity, auto& cLife) {
        cLife.value -= time * 2.f;

        if(cLife.value <= 0)
            entity_manager.kill(entity);
    });

    entity_manager.foreach<SGrow, EMPTY_TAG>
    ([&window](auto, auto& cRender, auto& cLife) {
        auto l(0.8f + (cLife.value * 0.3f));
        cRender.shape.setSize(sf::Vector2f{l, l});
        window.draw(cRender.shape);
    });

    entity_manager.refresh();
}

int main() {
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    sf::RenderWindow window(sf::VideoMode(800, 600), "Window", sf::Style::Default, settings);
    // window.setFramerateLimit(60);
    sf::View view;
    view.setCenter(0, 0); //!!!
    view.setSize(800, 600);
    window.setView(view);

    MyManager entity_manager;

    sf::Clock deltaClock;
    std::chrono::high_resolution_clock::time_point start;
    std::chrono::high_resolution_clock::time_point end;
    float fps;

    while (window.isOpen()) {
        sf::Time delta = deltaClock.restart();
        // Performed. Now perform GPU stuff...
        start = std::chrono::high_resolution_clock::now();

        window.clear();
        update(entity_manager, delta.asSeconds(), window);
        window.display();

        end = std::chrono::high_resolution_clock::now();
        fps = (float)1e9/(float)std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count();
        std::cout << "FPS: " << fps << " // Entities created: " << entity_manager.get_entities_count() << "\n";
    }

    return 0;
}
