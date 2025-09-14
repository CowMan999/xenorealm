#include <SFML/Graphics.hpp>
#include "sfcollider.hpp"

#include "assert.hpp"
#include "const.hpp"
#include "menu.hpp"
#include "assets.hpp"
#include "world.hpp"
#include "player.hpp"
#include "items.hpp"
#include "inventory.hpp"
#include "drops.hpp"
#include "craft.hpp"
#include "enemies.hpp"
#include "particle.hpp"
#include "settings.hpp"
#include "projectiles.hpp"

#pragma once


class Game {
public:
	Game();
	~Game();
	
	void mainloop();

private:
	sf::RenderWindow m_window;
	sf::Event m_event;
	
	sf::Time m_deltatime;
	sf::Clock m_clock;

	bool m_loadedworld = false; 
	
	sf::Sprite m_bg;
	sf::Texture m_bgtexture;

	// homemade classes
	Inventory m_inventory;
	Settings m_settings;
	Assets m_assets;
	World m_world;
	Drops m_drops;
	Craft m_craft;
	Particles m_particles;
	Projectiles m_projectiles;
	Enemies m_enemies;
	Player m_player;
	Menu m_menu;

	void handleEvent();
};