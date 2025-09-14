#include <SFML/Graphics.hpp>
#include <cmath>

#include "assets.hpp"
#include "inventory.hpp"
#include "const.hpp"
#include "assert.hpp"

#pragma once

class World;

class Drops {
public:
	Drops(Inventory& inventory);
	~Drops();

	struct Drop {
		sf::Vector2f pos;
		islot item;
		float timer = 0;
	};

	void update(sf::Time dt);
	void addDrop(sf::Vector2f pos, islot item);
	void draw(sf::RenderWindow& window, World* world);

	std::vector<Drop>& getDrops();

private:
	Inventory& m_inventory;

	sf::Sprite m_sprite;


	std::vector<Drop> m_drops;
};