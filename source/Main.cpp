#include <SFML/Graphics.hpp>
#include "Global.hpp"
#include "Render.hpp"
#include "TileMap.hpp"
#include "Dungeon2D.hpp"

void init(Render& render, Dungeon2D& dungeon, TileMap& tileMap);

int main ()
{
	sf::RenderWindow win(sf::VideoMode({RESOLUTION_WIDTH, RESOLUTION_HEIGHTS}), "SFML window");
	
	// instâncias de classes
	Render render(win);
	Dungeon2D dungeon(MAP_WIDTH, MAP_HEIGHT);

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

void init(Render& render, Dungeon2D& dungeon, TileMap& tileMap, )
{
	render.getWindow().setFramerateLimit(FRAME_RATE);
	dungeon.generate(SEED);
	tileMap.loadFromDungeon(dungeon);

	int startCol = 0, startRow = 0;
	tileMap.randomEmptyCell(startCol, startRow);
}