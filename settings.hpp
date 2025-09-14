#include <SFML/Graphics.hpp>

#include <thread>
#include <filesystem>
#include <fstream>

#include "const.hpp"

#pragma once

class Settings {
public:
	Settings(sf::RenderWindow& window);
	~Settings();

	bool load();
	void save();

	int& framerate();
	int& volume();
	int& major();
	int& minor();

	void apply();

	enum Flag {
		FULLSCREEN,
		VSYNC,
		DEBUG,
		GAMEDATA,
		METRICS,
		STACKTOOL,
		STYLEEDIT,
		SPEED,
		INVISPAUSE,
		FOGOFWAR,
		CUSTOMCURSOR,
		ANTIALIASING,
		ENEMYSPAWNING,

		COUNT
	};

	std::array<bool, COUNT> m_flags;
	bool& getFlag(Flag flag);

private:
	int m_framerate;
	int m_volume;

	int m_version_major, m_version_major_old;
	int m_version_minor, m_version_minor_old;

	bool m_fullscreen_old;
	bool m_antialiasing_old;

	bool m_first = true;

	sf::RenderWindow& m_window;
};