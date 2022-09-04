#include "engine/engine.hpp"

namespace ADE {

    Engine::Engine() {
        m_name = "Default title";
    }

    Engine::Engine(const std::string& name) {
        m_name = name;
    }

    Engine::~Engine() { }

    bool Engine::init() {
        sf::VideoMode videoMode = sf::VideoMode(800, 600);

        if(!videoMode.isValid()) {
            std::cout << "Invalid resolution" << "\n";
            return false;
        }

        sf::ContextSettings context;
        context.depthBits = 24;
        context.antialiasingLevel = 2;
        context.sRgbCapable = false;

        m_window.create(videoMode, m_name, sf::Style::Close, context);

        return true;
    }

    int Engine::run() {
        m_running = true;
/*
        if(!init())
        {
            std::cout << "Could not initialize application" << "\n";
            return 1;
        }
        std::cout << "Starting application" << "\n";
*/

        return loop();
    }

    bool Engine::loop()
    {
        sf::Clock clock;
        clock.restart();

/*
        std::cout << "isRunning: " << m_running << "\n";

        while(m_running)
        {
            sf::Time elapsed_time = clock.restart();
            m_window.clear();
            m_window.display();
        }

        return 0;
*/
        return m_running;
    }

    void Engine::clear_screen() {
        m_window.clear();
    }

    void Engine::end_scene() {
        m_window.display();
    }

}
