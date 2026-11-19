#include <SFML/Graphics.hpp>
#include "TileMap.hpp"

class Render{
public:
	Render(sf::RenderWindow& win);
	
	void setupMapView(float zoom);

	sf::RenderWindow& getWindow();
	void beginFrame();
	void updateCamera();
	void drawMap(TileMap& tileMap);
	void endFrame();

private:
	sf::RenderWindow& m_window;
	sf::View m_mapView;
};