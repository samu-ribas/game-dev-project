#include "Render.hpp"

Render::Render(sf::RenderWindow& win) : window(win), playerSprite(playerTexture) {}

namespace{
	float limitRange (float value, float min, float max){
		if(value < min){
			return min;
		}else if(value > max){
			return max;
		}
		return value;
	}
}


void Render::setupMapView(float zoom)
{
	if(zoom == 0) return;

	const sf::Vector2u windowSize = window.getSize();
	mapView.setSize({ (float)windowSize.x / zoom, (float)windowSize.y / zoom });
	mapView.setCenter(mapView.getSize() * 0.5f);
	window.setView(mapView);
}

sf::RenderWindow& Render::getWindow()
{
	return window;
}

bool Render::loadResources(){
	if (!playerTexture.loadFromFile(PLAYER_TEX)) {
		return false;
	}
	playerSprite.setTexture(playerTexture, true);
	const sf::Vector2u texSize = playerTexture.getSize();
	if (texSize.x > 0 && texSize.y > 0) {
		playerSprite.setOrigin({ texSize.x * 0.5f, texSize.y * 0.5f });

	}
	return true;
}

void Render::beginFrame()
{
	window.clear({100, 0, 0});
}

void Render::updateCamera(const Player& player, const TileMap& tileMap)
{
	float mapWidth  = (float)tileMap.width()  * TILE_SIZE;
	float mapHeight = (float)tileMap.height() * TILE_SIZE;

	sf::Vector2f viewSize = mapView.getSize();
	float halfW = viewSize.x * 0.5f;
    float halfH = viewSize.y * 0.5f;

	float camX = player.body.position.x;
	float camY = player.body.position.y;

    if(mapWidth > viewSize.x)
        camX = limitRange(camX, halfW, mapWidth - halfW);
    else
        camX = mapWidth * 0.5f;

    if(mapHeight > viewSize.y)
        camY = limitRange(camY, halfH, mapHeight - halfH);
    else
        camY = mapHeight * 0.5f;

    mapView.setCenter({ camX, camY });
    window.setView(mapView);
}

void Render::drawMap(TileMap& tileMap)
{
	sf::RectangleShape tile({TILE_SIZE, TILE_SIZE});

	for(int row = 0 ; row < tileMap.height() ; row++){
		for(int col = 0 ; col < tileMap.width() ; col++){
			tile.setPosition({(float)col * TILE_SIZE , (float)row * TILE_SIZE});
			if(tileMap.isWall(col, row)){
				tile.setFillColor({60, 60, 70});
			}else{
				tile.setFillColor({25, 25, 30});
			}

			window.draw(tile);
		}
	}
}

void Render::drawPlayer(Player& player){
	if(!player.active){
		return ;
	}
	playerSprite.setPosition({ player.body.position.x ,
                               player.body.position.y });
	window.draw(playerSprite);
}

void Render::endFrame()
{
	window.display();
}