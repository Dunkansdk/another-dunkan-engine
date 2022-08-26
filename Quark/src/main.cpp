#include <thread>
#include <SFML/Graphics.hpp>

#ifdef __UNIX__
#include <X11/Xlib.h>
#endif

int main()
{
#ifdef __UNIX__
  // init X threads
  XInitThreads();
#endif

  sf::ContextSettings settings;
  settings.antialiasingLevel = 10;
  sf::RenderWindow window (
    sf::VideoMode(800, 600),
    "SFML & CMAKE",
    sf::Style::Titlebar | sf::Style::Close,
    settings
  );

  window.clear(sf::Color::Cyan);
  window.setFramerateLimit(120);
  window.setActive(false);

  while(window.isOpen()) {
    sf::Event event;
    while(window.pollEvent(event))
    {
      if(event.type == sf::Event::Closed)
        window.close();
    }

    window.clear();
    window.display();
  }

  return 0;
}
