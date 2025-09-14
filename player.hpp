#include <SFML/Graphics.hpp>
#include <sfcollider.hpp>
#include <cmath>

#include "assets.hpp"
#include "const.hpp"
#include "world.hpp"
#include "buffs.hpp"
#include "drops.hpp"
#include "tools.hpp"
#include "enemies.hpp"
#include "particle.hpp"
#include "inventory.hpp"
#include "projectiles.hpp"


#pragma once

class Player {
public:
	Player(sf::RenderWindow& window, Assets& assets, World& world, Inventory& inventory, Drops& drop, Enemies& enemies, Particles& particles, Projectiles& projectiles, bool& mouse);
	~Player();

	void update(sf::Time dt, bool keyboard);
	bool ready();

	void draw();
	bool right();

	void setSpawn(sf::Vector2f s);
	sf::FloatRect getBounds();
	sf::Vector2f getPos();
	sf::Vector2f getIPos();

	float getHealth();
	bool alive();

	void reset();

	// statistical functions for abillity lambdas
	void setHealth(float h);
	void setHealTimer(float h);

	// data retrieval for abillity lambdas
	bool running();
	bool attacking();
	bool jumping();
	unsigned int jumpcount();
	bool hitable();
	sf::Vector2f direction();
	sf::Vector2f velocity();
	float directionDeg();
	float healTimer();
	float air();

	Audio& getAudio() { return m_world.getAudio(); };
	World& getWorld() { return m_world; };

	bool chestOpen();
	sf::Vector2i& getChestOpen();

	Buffs& getBuffs();

private:
	sf::Sprite m_sprite;
	sf::RenderWindow& m_window;

	Assets& m_assets;
	World& m_world;
	Inventory& m_inventory;
	Drops& m_drops;
	Enemies& m_enemies;
	Particles& m_particles;
	Projectiles& m_projectiles;
	Buffs m_buffs;

	// physics and data storage
	sf::Vector2f m_position;
	sf::Vector2f m_position_old;
	sf::Vector2f m_velocity;
	sf::Vector2f m_spawn;
	sf::Vector2i m_oldtile;

	bool m_jumping = true;
	bool m_holding_jump = false;
	bool m_running = false;

	sf::Vector2i m_chest_open = sf::Vector2i(-1, -1);

	float m_animtimer = 0;
	float m_timer = 0;
	float m_breaktimer = 0;
	float m_healtimer = 0;

	bool m_attacking = false;
	sf::Sprite m_item;
	Items::Category m_itold = Items::Category::NONE;
	Assets::Animation m_iaold = Assets::Animation::anim_count;
	bool m_iright = true;
	bool& m_mousea;

	float m_health = 100;
	bool m_alive = true;

	float m_invuln = 0;
	bool m_hitable = true;

	bool m_consumed = false;

	float m_buildtimer = 0;
	
	sf::Vector2f m_direction = sf::Vector2f(0, 0);
	float m_directionangle = 0;

	float m_air = 100;

	std::deque<std::pair<sf::Vector2u, Blocks::BlockData>> m_colliders;

	// armor renderers
	sf::Sprite m_armor_helmet;
	sf::Sprite m_armor_chestplate;
	sf::Sprite m_armor_leggings;

	sf::RectangleShape m_arm;
	sf::RectangleShape m_armsleave;

	unsigned int m_jumpcount = 0;

	// functions for orginizations sake
	void inputs(float dt);
	void physics(float dt);
	void collision();
	void animate();
	void interact(float dt); 
};