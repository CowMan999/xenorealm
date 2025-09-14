#include <SFML/Graphics.hpp>
#include <vector>

#include "assert.hpp"
#include "assets.hpp"
#include "random.hpp"

#pragma once

struct Particle
{
	sf::Vector2f pos;
	sf::Vector2f vel;
	sf::Color color;
	float size;
	float life;
	int mlife;
	int angle;
	Assets::Animation anim;
	float gravity = 0;
};

struct ParticleInit
{
	sf::Vector2f pos;
	unsigned int count;
	unsigned int count_var;
	float radius;
	float life;
	float life_var;
	float size;
	float size_var;
	float speed;
	float speed_var;
	float gravity;
	sf::Color color;
	Assets::Animation anim;
	bool outwards = 1;
	bool inwards = 0;
	sf::Vector2f dir = sf::Vector2f(0, 0);
};

// template particle init for easy use
const extern ParticleInit PARTICLE_PUFF;
const extern ParticleInit PARTICLE_BLOOD;

class Particles
{
public:
	Particles(Assets& assets);
	~Particles();

	void update(sf::Time dt);
	void draw(sf::RenderWindow& window);

	void create(ParticleInit init);

	bool ready();

	void clear();

private:
	Assets& m_assets;
	std::vector<Particle> m_particles;
	Assets::Animation m_anim;

	sf::Sprite m_sprite;
};