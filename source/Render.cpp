#include "Render.hpp"

Render::Render(sf::RenderWindow& win) : m_window(win) {}

sf::RenderWindow& Render::getWindow(){
	return m_window;
}