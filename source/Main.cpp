#include <SFML/Graphics.hpp>
#include "Global.hpp"
#include "Render.hpp"
#include "TileMap.hpp"
#include "Dungeon2D.hpp"

void init(Render& render, Dungeon2D& dungeon, TileMap& tileMap);
void update(float dt);
void renderFrame(Render& render, TileMap& tileMap);

int main ()
{
	sf::RenderWindow win(sf::VideoMode({RESOLUTION_WIDTH, RESOLUTION_HEIGHT}), "SFML window");
	
	// instâncias de classes
	Render render(win);
	Dungeon2D dungeon(MAP_WIDTH, MAP_HEIGHT);
	TileMap tileMap;
	sf::Clock clock;

	init(render, dungeon, tileMap);

	while(render.getWindow().isOpen())
	{
		while(const std::optional event = render.getWindow().pollEvent())
		{
			if(event->is<sf::Event::Closed>())
				render.getWindow().close();
		}
		const float elapsed = clock.restart().asSeconds();
		float dt = (FIXED_DT < elapsed) ? FIXED_DT : elapsed;

		if(dt > 0){
			update(dt);
		}
		renderFrame(render, tileMap);
	}

	return 0;
}

void init(Render& render, Dungeon2D& dungeon, TileMap& tileMap)
{
	render.getWindow().setFramerateLimit(FRAME_RATE);
	dungeon.generate(SEED);
	tileMap.loadFromDungeon(dungeon);

	int startCol = 0, startRow = 0;
	tileMap.randomEmptyCell(startCol, startRow);
	render.setupMapView(MAP_ZOOM);
}

void update(float dt)
{

}

void renderFrame(Render& render, TileMap& tileMap)
{
	render.beginFrame();
	render.drawMap(tileMap);
	render.endFrame();
}