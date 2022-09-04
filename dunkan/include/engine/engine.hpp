#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>

namespace ADE {

	struct Engine {
			Engine();
			Engine(const std::string&);
			virtual ~Engine();

			bool init();
			int run();
			// int loop();
			bool loop();
			void stop();

			void clear_screen();
			void end_scene();

		private:
			bool m_running;
            std::string m_name;

     	    sf::RenderWindow m_window;

	};

}
