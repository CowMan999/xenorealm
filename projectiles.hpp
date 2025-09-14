#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>

#include "assets.hpp"
#include "world.hpp"

#pragma once

struct Projectile {
	bool playerproj;
	sf::Vector2f pos;
	sf::Vector2f dir;
	sf::Color color;
	float damage;
	float size;
	float speed;
	float life;
	Assets::Animation anim;
	float gravity = 0;
	bool solid = true;
	sf::Sprite sprite = sf::Sprite(); // don't touch
	int mlife = 0; // don't touch
	bool first = true; // don't touch
};

const extern Projectile PROJECTILE_ARROW;

class Projectiles {
public:
	Projectiles(sf::RenderWindow& window, Assets& assets, World& world);
	~Projectiles();

	void update(sf::Time dt);
	void draw();

	void add(Projectile p);
	std::vector<Projectile>& vector();

	void clear();
	
private:
	sf::RenderWindow& m_window;
	Assets& m_assets;
	World& m_world;

	std::vector<Projectile> m_projectiles;
};
