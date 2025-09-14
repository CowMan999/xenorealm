#include <imgui-SFML.h>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <imgui_internal.h>

#include <SFML/Graphics.hpp>

#include <bitset>
#include <filesystem>
#include <fstream>
#include <atomic>

#include "assert.hpp"
#include "assets.hpp"
#include "const.hpp"
#include "inventory.hpp"
#include "craft.hpp"
#include "player.hpp"
#include "world.hpp"
#include "tools.hpp"
#include "settings.hpp"

#pragma once

enum class MenuState {
	LOAD,
	MAIN,
	SELECT,
	NEW,
	OPTIONS,
	PAUSE,
	PLAY,
	DEAD,
	MAP,
	SETTINGS,
	CRAFTING,
	CREDITS,
};

class Menu {
public:
	Menu(sf::RenderWindow& window, Assets& assets, Inventory& inventory, Craft& craft, Player& player, Drops& drops, Enemies& enemies, World& world, Settings& settings);
	~Menu();

	void update(sf::Time dt);
	void camUpdate();
	void handleEvent(sf::Event& e);

	void setState(MenuState state);
	MenuState getState();

	bool& mouseAvailable();

	// callback for loading info related to world selection
	bool initWorldSelect();
	void appendWorld(const WorldMeta& world);

	bool& createWorld();
	WorldMeta& worldMetadata();

	bool save();
	bool quit();
	void tutorial();
	bool enemySpawns() {
		return !m_tutorial;
	}

	void style();

	bool loadFont();

	bool hogKeyboard();

	bool mapFullscreen();
	void ingameGui(float scale);

	std::string longToTimestamp(size_t timestamp);

	bool loadTitleBg();
private:
	sf::RenderWindow& m_window;
	MenuState m_state;

	Assets& m_assets;
	Inventory& m_inventory;
	Craft& m_craft;
	Player& m_player;
	Drops& m_drops;
	Enemies& m_enemies;
	World& m_world;
	Settings& m_settings;

	sf::Sprite m_inventoryrenderer;
	sf::Sprite m_swapitem;
	sf::View m_view;

	sf::Texture m_cursortexture;
	sf::Sprite m_cursorsprite;

	// helper functions
	void centeredText(const char* text);

	size_t m_selected = 0;

	// jeez templates are confusing
	template<int T> std::bitset<T> centeredButtonRow(const std::array<const char*, T> titles, const std::array<ImVec2, T> sizes); 
	template<int T> std::bitset<T> centeredButtonRowE(const std::array<const char*, T> titles, const ImVec2 size); 
	void centerByWidth(const float width);

	bool m_inventory_a = 0;
	islot* m_swaping = nullptr;
	int m_swapingt = 0;
	float m_swaptimer = 0;

	bool m_mousea = true;
	bool m_mousea_old = true;
	bool m_crafted = false;
	bool m_save = false, m_quit = false;

	std::vector<WorldMeta> m_worlds;
	WorldMeta m_selectedworld;
	bool m_createworld = false;

	MenuState m_prevstate;
	Items::Type m_tooltip_item = Items::NONE;

	bool m_block_picker = false;

	bool m_mapopen = false;
	bool m_mapholding = false;

	std::atomic_bool m_fontloaded = false;
	ImFont *m_fonttiny = nullptr, * m_fontsmall = nullptr, *m_fontlarge = nullptr;

	sf::IntRect m_maprect = sf::IntRect(0, 0, WORLD_WIDTH, WORLD_HEIGHT);

	sf::Vector2f m_map_pos = sf::Vector2f(0, 0);
	float m_mapzoom = 1;

	sf::Vector2f m_map_drag = sf::Vector2f(-1, -1);
	sf::Vector2f m_map_pos_old = sf::Vector2f(-1, -1);

	sf::Vector2f m_size_scales = sf::Vector2f(1, 1);
	bool m_signediting = false;

	sf::Texture m_titlebg;
	sf::Sprite m_titlebgsprite;

	bool m_tutorial = false;
	unsigned short m_tutorial_step = 0;

};

// this is the implementation of the template function, it has to be in the header file
template<int T> std::bitset<T> Menu::centeredButtonRow(const std::array<const char*, T> titles, const std::array<ImVec2, T> sizes)
{
	float culm = 0;
	float winwidth = ImGui::GetWindowWidth();
	std::bitset<T> bitset;
	for (size_t i = 0; i < sizes.size(); i++)
		culm += sizes[i].x;
	ImGui::SetCursorPosX((winwidth-(culm+ImGui::GetStyle().ItemSpacing.x*(titles.size()-1)))*0.5f);
	for (size_t i = 0; i < titles.size(); i++)
	{
		bitset[i] = ImGui::Button(titles[i], sizes[i]);
		if(i != titles.size()-1)
			ImGui::SameLine();
	}

	return bitset;
}

// wrapper function for ease of use
template<int T> std::bitset<T> Menu::centeredButtonRowE(const std::array<const char*, T> titles, const ImVec2 size) {
	
	std::array<ImVec2, T> sizes;
	for (size_t i = 0; i < T; i++) // yes, its a hack, but it works
		sizes[i] = size;
	return centeredButtonRow<T>(titles, sizes);
}