#include "game.hpp"

// mega constructor
Game::Game() : m_window(), m_settings(m_window), m_world(m_assets, m_window), m_drops(m_inventory), m_craft(m_inventory), m_particles(m_assets), m_projectiles(m_window, m_assets, m_world), m_enemies(m_assets, m_settings, m_world, m_drops, m_particles, m_projectiles, m_window.getSize()), m_player(m_window, m_assets, m_world, m_inventory, m_drops, m_enemies, m_particles, m_projectiles, m_menu.mouseAvailable()), m_menu(m_window, m_assets, m_inventory, m_craft, m_player, m_drops, m_enemies, m_world, m_settings) {
	
	// setup loading callbacks
	m_assets.addCallback(std::bind(&Menu::loadFont, &m_menu), "FontLoader");
	m_assets.addCallback(std::bind(&Menu::initWorldSelect, &m_menu), "WorldSelectLoader");
	m_assets.addCallback(std::bind(&Player::ready, &m_player), "PlayerLoader");
	m_assets.addCallback(&Items::innit, "ItemsLoader");
	m_assets.addCallback(std::bind(&Particles::ready, &m_particles), "ParticlesLoader");
	m_assets.addCallback(std::bind(&World::compileShaders, &m_world), "ShaderCompiler");
	m_assets.addCallback(std::bind(&World::loadBg, &m_world, &m_window), "BackgroundLoader");
	m_assets.addCallback(std::bind(&Menu::loadTitleBg, &m_menu), "TitleBackgroundLoader");
	// m_assets.addCallback(std::bind(&World::calculateLightMatrix, &m_world), "LightMatrixLoader"); // generates light png, its shipped with the game

	// create worker thread to load all assets
	m_assets.load();
}

Game::~Game() {
	
}

void Game::mainloop() {
	
	// main game loop
	while(m_window.isOpen()) {
		
		// update delta time
		m_deltatime = m_clock.restart();
		if(m_deltatime.asSeconds() > 1.f/MIN_FRAMERATE) m_deltatime = sf::seconds(1.f/MIN_FRAMERATE); // limit framerate to MIN_FRAMERATE

		// event loop
		while(m_window.pollEvent(m_event)) {
			handleEvent();
		}
		
		// load world if needed
		if(m_menu.createWorld()) {

			WorldMeta meta = m_menu.worldMetadata();

			if(!m_loadedworld) {

				// new world
				if(meta.path == std::filesystem::path()) {
					m_world.generate(meta, m_inventory);
				} else {

					// load world
					m_world.load(meta, m_inventory);
				}
				m_loadedworld = true;
			}

			if(m_world.done()) {
				m_player.setSpawn(m_world.getSpawn());
				m_player.reset();
				m_menu.createWorld() = false;

				WorldMeta meta = m_menu.worldMetadata();

				bool wnew = meta.path == std::filesystem::path();

				meta.path = m_world.recentWorld().path;
				meta.abs = std::filesystem::absolute(meta.path);
				if(wnew) m_menu.appendWorld(meta);

				if(wnew) m_menu.setState(MenuState::SELECT);
				else m_menu.setState(MenuState::PLAY);
				m_loadedworld = false;

				m_menu.worldMetadata() = WorldMeta();

				// first subframe to mitigate midframe
				m_player.update(sf::seconds(0), false);
				m_menu.camUpdate();

			}
		}

		// render loop
		if(m_menu.getState() == MenuState::PLAY || m_menu.getState() == MenuState::PAUSE || m_menu.getState() == MenuState::DEAD) {
			m_window.clear(m_world.getSkyColor());
		} else {
			m_window.clear(); // dont bother with colours in menus
		}

		// update and render world
		auto state = m_menu.getState();
		if(state == MenuState::PLAY) {

			m_world.update(m_deltatime, &m_drops);
			if(m_menu.enemySpawns())
				m_enemies.update(m_deltatime, &m_player, m_window);
			m_player.update(m_deltatime, !m_menu.hogKeyboard());
			m_drops.update(m_deltatime);
			m_particles.update(m_deltatime);
			m_projectiles.update(m_deltatime);

			m_menu.camUpdate();
			m_world.paralax();
			m_world.draw();
			m_drops.draw(m_window, &m_world);
			m_enemies.draw(m_window);
			m_projectiles.draw();
			m_particles.draw(m_window);
			m_player.draw();
			m_world.drawLight();
		}

		// draw everything
		else if(state == MenuState::PAUSE || state == MenuState::DEAD || state == MenuState::MAP) {
			m_menu.camUpdate();
			m_bg.setPosition(m_window.getView().getCenter() - m_window.getView().getSize() / 2.f);
			m_window.draw(m_bg);
			m_world.draw();
			m_drops.draw(m_window, &m_world);
			m_enemies.draw(m_window);
			m_projectiles.draw();
			m_particles.draw(m_window);
			m_player.draw();
			m_world.drawLight();
		}

		// update and render menu
		m_menu.update(m_deltatime);

		// menu transitions
		if(m_menu.save() || m_menu.quit()) {
			WorldMeta meta = m_world.recentWorld();

			m_world.save(meta, m_inventory);
			m_menu.initWorldSelect();
		}

		m_window.display();
	}

	if(m_world.recentWorld().path != std::filesystem::path())
		m_world.save(m_world.recentWorld(), m_inventory);

	std::filesystem::remove_all("./tmp");
}

void Game::handleEvent() {
	
	// pass event to menu and thus imgui
	m_menu.handleEvent(m_event);

	// react to event as normal after passing to imgui
	switch(m_event.type) {
		
		case sf::Event::Closed:
			m_window.close();
			break;

		case sf::Event::Resized:
			m_world.updateBg(m_window);

		default:
			break;
	}
}