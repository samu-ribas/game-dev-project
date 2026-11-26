#include <SFML/Graphics.hpp>
#include <iostream>

#include "Global.hpp"
#include "Render.hpp"
#include "TileMap.hpp"
#include "Dungeon2D.hpp"
#include "CollisionManager.hpp"
#include "NPC.hpp"

bool init(Render& render, Dungeon2D& dungeon, TileMap& tileMap, Player& player, CollisionManager& colManager);
void update(TileMap& tileMap, Player& player, Render& render, float dt, CollisionManager& collisions, const std::vector<const Entity*>& entities, std::vector<const Entity*>& visible);
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
	CollisionManager collisions(MAP_WIDTH * TILE_SIZE, MAP_HEIGHT * TILE_SIZE, GRID_CELL_SIZE);
	std::vector<const Entity*> entities{ &player };   // + NPCs
	std::vector<const Entity*> visible;

	if(!init(render, dungeon, tileMap, player, collisions))	return -1;


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
			update(tileMap, player, render, dt, collisions, entities, visible);
		}
		renderFrame(render, tileMap, player);
	}

	return 0;
}

bool init(Render& render, Dungeon2D& dungeon, TileMap& tileMap, Player& player, CollisionManager& colManager)
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

void update(TileMap& tileMap, Player& player, Render& render, float dt,
            CollisionManager& collisions,
            const std::vector<const Entity*>& entities,
            std::vector<const Entity*>& visible)
{
	if(render.getWindow().hasFocus()){
		player.input(render.getWindow());
	}
	collisions.moveAndSlide(player, tileMap, dt);
	collisions.rebuild(entities);
	player.updateProjectiles(dt, collisions, tileMap);
	collisions.findInRadius(player.body.position, DEFAULT_PLAYER_VISION, visible, &player);
}

void renderFrame(Render& render, TileMap& tileMap, Player& player)
{
	render.updateCamera(player, tileMap);
	render.beginFrame();
	render.drawMap(tileMap);
	render.drawPlayer(player);
	render.drawProjectiles(player);
	render.endFrame();
}
