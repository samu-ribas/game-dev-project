#include <SFML/Graphics.hpp>
#include "Render.hpp"

void init(Render& render);

int main ()
{
	sf::RenderWindow win(sf::VideoMode({1100, 800}), "SFML window");
	Render render(win);

	init(render);

	while(render.getWindow().isOpen())
	{
		while(const std::optional event = win.pollEvent())
		{
			if(event->is<sf::Event::Closed>())
				render.getWindow().close();
		}
	}

	return 0;
}

void init(Render& render)
{
	render.getWindow().setFramerateLimit(60);
}