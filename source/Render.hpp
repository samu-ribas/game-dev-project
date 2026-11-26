#ifndef RENDER_HPP
#define RENDER_HPP

#include <SFML/Graphics.hpp>
#include "TileMap.hpp"
#include "Player.hpp"

class Render{
public:
	Render(sf::RenderWindow& win);
	
	void setupMapView(float zoom);
	sf::RenderWindow& getWindow();
	bool loadResources();

	void beginFrame();
	void updateCamera(const Player& player, const TileMap& tileMap);
	void drawMap(TileMap& tileMap);
	void drawPlayer(Player& player);
	void drawProjectiles(const Player& player);
	void endFrame();

private:
	sf::RenderWindow& window;
	sf::View mapView;

	sf::Texture playerTexture;
	sf::Sprite playerSprite;
	sf::CircleShape projectileShape;
};

#endif //RENDER_HPP
