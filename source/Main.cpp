#include <SFML/Graphics.hpp>
#include "Global.hpp"
#include "Render.hpp"
#include "TileMap.hpp"
#include "Dungeon2D.hpp"

bool init(Render& render, Dungeon2D& dungeon, TileMap& tileMap, Player& player);
void update(TileMap& tileMap, Player& player, Render& render, float dt);
void renderFrame(Render& render, TileMap& tileMap, Player& player);

int main ()
{
	sf::RenderWindow win(sf::VideoMode({RESOLUTION_WIDTH, RESOLUTION_HEIGHT}), "SFML window");
	
	// instâncias de classes
	Render render(win);
	Dungeon2D dungeon(MAP_WIDTH, MAP_HEIGHT);
	TileMap tileMap;
	Player player;
	sf::Clock clock;

	if(!init(render, dungeon, tileMap, player))	return -1;

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
			update(tileMap, player, render, dt);
		}
		renderFrame(render, tileMap, player);
	}

	return 0;
}

bool init(Render& render, Dungeon2D& dungeon, TileMap& tileMap, Player& player)
{
	render.getWindow().setFramerateLimit(FRAME_RATE);
	dungeon.generate(SEED);
	tileMap.loadFromDungeon(dungeon);
	if(!render.loadResources()) return false;

	int startCol = 0, startRow = 0;
	tileMap.randomEmptyCell(startCol, startRow);
	player.body.position = tileMap.toWorld(startCol, startRow);
	render.setupMapView(MAP_ZOOM);
	return true;
}

void update(TileMap& tileMap, Player& player, Render& render, float dt)
{
	player.handleInput(render, dt);
}

void renderFrame(Render& render, TileMap& tileMap, Player& player)
{
	render.updateCamera(player, tileMap);
	render.beginFrame();
	render.drawMap(tileMap);
	render.drawPlayer(player);
	render.endFrame();
}