#include <SFML/Graphics.hpp>

#include "Global.hpp"
#include "Render.hpp"
#include "TileMap.hpp"
#include "Dungeon2D.hpp"
#include "CollisionManager.hpp"
#include "NPC.hpp"

struct GameWorld {
    Render render;
    Dungeon2D dungeon;
    TileMap tileMap;
    Player player;
    CollisionManager colManager;
    int score = 0;
	
	GameWorld(sf::RenderWindow& win) 
        : render(win),
          dungeon(MAP_WIDTH, MAP_HEIGHT),
          colManager(MAP_WIDTH * TILE_SIZE, MAP_HEIGHT * TILE_SIZE, GRID_CELL_SIZE) 
    {}
};

bool init(GameWorld& world);
void update(GameWorld& world, float dt, const std::vector<const Entity*>& entities, std::vector<const Entity*>& visible);
void renderFrame(GameWorld& world);

int main ()
{
	sf::RenderWindow win(sf::VideoMode({RESOLUTION_WIDTH, RESOLUTION_HEIGHT}), "SFML window");
	sf::Clock clock;
	GameWorld world(win);

	std::vector<const Entity*> entities{ &world.player };   // + NPCs
	std::vector<const Entity*> visible;

	if(!init(world))	return -1;


	while(world.render.getWindow().isOpen())
	{
		while(const std::optional event = world.render.getWindow().pollEvent())
		{
			if(event->is<sf::Event::Closed>())
				world.render.getWindow().close();
		}
		const float elapsed = clock.restart().asSeconds();
		float dt = (FIXED_DT < elapsed) ? FIXED_DT : elapsed;

		if(dt > 0){
			update(world, dt, entities, visible);
		}
		renderFrame(world);
	}

	return 0;
}

bool init(GameWorld& world)
{
	world.render.getWindow().setFramerateLimit(FRAME_RATE);
	world.dungeon.generate(SEED);
	world.tileMap.loadFromDungeon(world.dungeon);

	if(!world.render.loadResources()) return false;

	int startCol = 0, startRow = 0;
	world.tileMap.randomEmptyCell(startCol, startRow);
	world.player.body.position = world.tileMap.toWorld(startCol, startRow);
	world.render.setupMapView(MAP_ZOOM);
	return true;
}

void update(GameWorld& world, float dt, const std::vector<const Entity*>& entities, std::vector<const Entity*>& visible)
{
	if(world.render.getWindow().hasFocus()){
		world.player.input(world.render.getWindow());
	}
	world.colManager.moveAndSlide(world.player, world.tileMap, dt);
	world.colManager.rebuild(entities);
	world.player.updateProjectiles(dt, world.colManager, world.tileMap);
	world.colManager.findInRadius(world.player.body.position, DEFAULT_PLAYER_VISION, visible, &world.player);
}

void renderFrame(GameWorld& world)
{
	world.render.updateCamera(world.player, world.tileMap);
	world.render.beginFrame();
	world.render.drawMap(world.tileMap);
	world.render.drawPlayer(world.player);
	world.render.drawProjectiles(world.player);
	world.render.drawScore(world.score, world.player.hp);
	world.render.endFrame();
}
