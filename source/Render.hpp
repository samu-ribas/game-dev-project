#include <SFML/Graphics.hpp>

class Render{
public:
	Render(sf::RenderWindow& win);
	sf::RenderWindow& getWindow();
private:
	sf::RenderWindow& m_window;
};