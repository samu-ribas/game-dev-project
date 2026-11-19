#include "Render.hpp"

Render::Render(sf::RenderWindow& win) : m_window(win) {}

void Render::setupMapView(float zoom)
{
	if(zoom == 0) return;

	const sf::Vector2u windowSize = m_window.getSize();
	m_mapView.setSize({ (float)windowSize.x / zoom, (float)windowSize.y / zoom });
	m_mapView.setCenter(m_mapView.getSize() * 0.5f);
	m_window.setView(m_mapView);
}

sf::RenderWindow& Render::getWindow()
{
	return m_window;
}

void Render::beginFrame()
{
	m_window.clear({100, 0, 0});
}

void Render::updateCamera()
{

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

			m_window.draw(tile);
		}
	}

}

void Render::endFrame()
{
	m_window.display();
}