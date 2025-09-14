#include "settings.hpp"

// setting defaults
Settings::Settings(sf::RenderWindow& window) : m_window(window) {
	
	m_flags[FULLSCREEN] = true;
	m_flags[VSYNC] = true;

	// debug flags
	m_flags[DEBUG] = false;
	m_flags[GAMEDATA] = false;
	m_flags[METRICS] = false;
	m_flags[STACKTOOL] = false;
	m_flags[STYLEEDIT] = false;
	m_flags[SPEED] = false;
	m_flags[INVISPAUSE] = false;
	m_flags[FOGOFWAR] = true;
	m_flags[CUSTOMCURSOR] = true;
	m_flags[ANTIALIASING] = false;
	m_flags[ENEMYSPAWNING] = true;

	m_version_major = 4;
	m_version_minor = 6;

	m_framerate = 60;
	m_volume = 75;

	// load settings
	load();
}

Settings::~Settings() {
	save();
}

// getters
int& Settings::framerate() { return m_framerate;}
int& Settings::volume() { return m_volume;}
int& Settings::major() { return m_version_major;}
int& Settings::minor() { return m_version_minor;}

bool& Settings::getFlag(Settings::Flag flag) { return m_flags[flag]; }

bool Settings::load() {
	
	try {
		std::ifstream file("data/settings", std::ios::binary);

		for(size_t i = 0; i < COUNT; i++) {
			file.read((char*)&m_flags[i], sizeof(bool));
		}

		file.read((char*)&m_framerate, sizeof(int));
		file.read((char*)&m_volume, sizeof(int));
		file.read((char*)&m_version_major, sizeof(int));
		file.read((char*)&m_version_minor, sizeof(int));

		m_fullscreen_old = m_flags[FULLSCREEN];
		m_antialiasing_old = m_flags[ANTIALIASING];
		m_version_major_old = m_version_major;
		m_version_minor_old = m_version_minor;
		apply();

	} catch(...) {
		throw std::runtime_error("Failed to load settings.");
	}

	return true;
}

void Settings::save() {
	std::ofstream file("data/settings", std::ios::binary);
	for(size_t i = 0; i < COUNT; i++) {
		file.write((char*)&m_flags[i], sizeof(bool));
	}
	
	file.write((char*)&m_framerate, sizeof(int));
	file.write((char*)&m_volume, sizeof(int));
	file.write((char*)&m_version_major, sizeof(int));
	file.write((char*)&m_version_minor, sizeof(int));
	
	file.close();
}

void Settings::apply() {

	sf::Vector2u size;

	if(!m_first) {
		size = m_window.getSize();
	} else if(!m_flags[FULLSCREEN]) {
		sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
		size.x = desktop.width; size.y = desktop.height;
	} else {
		sf::VideoMode desktop = sf::VideoMode::getFullscreenModes()[0];
		size.x = desktop.width; size.y = desktop.height;
	}
	

	// recreate window with new settings
	if(m_fullscreen_old != m_flags[FULLSCREEN] || m_flags[ANTIALIASING] != m_antialiasing_old || m_version_major != m_version_major_old || m_version_minor != m_version_minor_old || m_first) {
		if(!m_first) m_window.close();

		sf::ContextSettings settings;
		settings.antialiasingLevel = m_flags[ANTIALIASING] ? 8 : 0;
		settings.majorVersion = m_version_major;
		settings.minorVersion = m_version_minor;
		//settings.attributeFlags = sf::ContextSettings::Core;
		settings.depthBits = 24;
		settings.stencilBits = 8;

		m_window.create(sf::VideoMode(size.x, size.y), GAME_NAME, (m_flags[FULLSCREEN] ? sf::Style::Fullscreen : sf::Style::Default), settings);
		m_fullscreen_old = m_flags[FULLSCREEN];
		m_antialiasing_old = m_flags[ANTIALIASING];
		m_version_major = m_window.getSettings().majorVersion;
		m_version_minor = m_window.getSettings().minorVersion;
		m_version_major_old = m_version_major;
		m_version_minor_old = m_version_minor;
		m_first = false;

	}

	m_window.setVerticalSyncEnabled(m_flags[VSYNC]);
	if(!m_flags[VSYNC]) m_window.setFramerateLimit(m_framerate);

	// use ingame cursor
	if(m_flags[CUSTOMCURSOR]){
		m_window.setMouseCursorGrabbed(true);
		m_window.setMouseCursorVisible(false);
	}else{
		m_window.setMouseCursorGrabbed(true);
		m_window.setMouseCursorVisible(true);
	}

}