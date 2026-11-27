#include "Render.hpp"

#include <string>

Render::Render(sf::RenderWindow& win)
	: window(win)
	, playerSprite(playerTexture)
	, projectileShape(BULLET_RADIUS)
	, scoreText(scoreFont, "", 24)
	, scoreSprite(scoreTexture.getTexture())
{
	projectileShape.setOrigin({BULLET_RADIUS, BULLET_RADIUS});
	projectileShape.setFillColor({255, 220, 80});

	hudView = window.getDefaultView();
	scoreText.setPosition({16.0f, 16.0f});
	scoreText.setFillColor(sf::Color::White);
	scoreText.setOutlineColor(sf::Color::Black);
	scoreText.setOutlineThickness(2.0f);
}

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
	if (!scoreFont.openFromFile(MENU_FONT) &&
		!scoreFont.openFromFile("crazyFont.otf")) {
		return false;
	}
	if (!scoreTexture.resize(window.getSize())) {
		return false;
	}
	// The sprite is constructed before the render texture is resized, so its
	// default texture rectangle is initially empty. Refresh it after resize.
	scoreSprite.setTexture(scoreTexture.getTexture(), true);
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
			if(tileMap.isWall(col, row))
				tile.setFillColor({60, 60, 70});
			else
				tile.setFillColor({25, 25, 30});

			window.draw(tile);
		}
	}
}

void Render::drawPlayer(Player& player){
	if(!player.active){
		return ;
	}
	const sf::Vector2u tex = playerTexture.getSize();
	if(tex.x > 0 && tex.y > 0){
		playerSprite.setScale({ 2.f * player.collision.halfExtents.x / (float)tex.x,
		                        2.f * player.collision.halfExtents.y / (float)tex.y });
	}
	playerSprite.setPosition({ player.body.position.x , player.body.position.y });
	window.draw(playerSprite);
}

void Render::drawProjectiles(const Player& player)
{
	for (int i = 0; i < player.projectileCapacity(); ++i) {
		const Projectile& projectile = player.projectileAt(i);
		if (!projectile.active) {
			continue;
		}

		projectileShape.setPosition({
			projectile.body.position.x,
			projectile.body.position.y
		});
		window.draw(projectileShape);
	}
}

void Render::drawScore(int score, float hp)
{
	if (scoreNeedsRedraw || score != lastScore || hp != lastHp) {
		scoreText.setString("hp: " + std::to_string(static_cast<int>(hp)) +
		                    "   Score: " + std::to_string(score));

		scoreTexture.clear(sf::Color::Transparent);
		scoreTexture.draw(scoreText);
		scoreTexture.display();

		lastScore = score;
		lastHp = hp;
		scoreNeedsRedraw = false;
	}

	// The window is cleared every frame, so the cached score image still has
	// to be composed on the current frame even when its text did not change.
	window.setView(hudView);
	scoreSprite.setPosition({0.0f, 0.0f});
	window.draw(scoreSprite);
	window.setView(mapView);
}

void Render::endFrame()
{
	window.display();
}
