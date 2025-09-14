#include <SFML/Graphics.hpp>
#include <functional>
#include <filesystem>
#include <sstream>
#include <cmath>
#include <deque>
#include <unordered_map>

#include "assets.hpp"
#include "const.hpp" 
#include "blocks.hpp"
#include "inventory.hpp"
#include "random.hpp"
#include "audio.hpp"

class Drops;

#pragma once

typedef std::pair<unsigned short, unsigned short> compressedunit;

struct WorldMeta {
	std::string name;
	std::filesystem::path path;
	std::filesystem::path abs;
	std::string seed;
	size_t gametime = 0;
	bool hardmode = 0;
};

enum class Biome {
	FOREST = -1,
	SNOW,
	DESERT,
	CASTLE,

	COUNT,
	FCOUNT
};

enum class MapObj {
	ENEMY,
	BOSS,
	BLOCK,

	COUNT
};

class World
{
public:
	World(Assets& assets, sf::RenderWindow& window);
	~World();

	void generate(WorldMeta data, Inventory& inventory);
	void draw();
	void drawLight();

	Blocks::Type& tile(size_t x, size_t y);
	Blocks::Type& tilebg(size_t x, size_t y);

	Blocks::Type& tile(size_t x, size_t y, bool& check);
	Blocks::Type& tilebg(size_t x, size_t y, bool& check);

	Blocks::Type*** tilematrix(size_t x, size_t y, size_t w, size_t h);
	Blocks::Type*** tilebgmatrix(size_t x, size_t y, size_t w, size_t h);
	void deletetilematrix(Blocks::Type*** matrix, size_t w);

	bool calculateLightMatrix();
	bool compileShaders();

	sf::RenderWindow& window() { return m_window;}

	sf::Vector2f getSpawn();
	bool done();

	void update(sf::Time dt, Drops* drops);

	void save(WorldMeta data, Inventory& inventory);
	void load(WorldMeta data, Inventory& inventory);

	WorldMeta recentWorld();

	int getGroundHeight(int x, size_t y = 0);

	sf::Color getSkyColor();
	sf::Color getSkyColorGrayscale();

	void settime(float time, bool day) {
		m_timer = time;
		m_day = day;
	};

	float getplaytime() {
		return m_gametime.getElapsedTime().asSeconds() + m_worldplaytime;
	};

	void reset();

	bool night();

	Biome getBiome(unsigned int x);

	bool& brightnessUpdated(size_t x, size_t y);
	bool& wasLight(size_t x, size_t y);

	void appendChest(size_t x, size_t y);

	// attempt to remove a chest at the given position
	bool removeChest(size_t x, size_t y);

	Inventory& getChest(size_t x, size_t y);
	std::string& getSign(size_t x, size_t y);

	const std::string& getStatus();

	bool loadBg(sf::RenderWindow* window);
	void updateBg(sf::RenderWindow& window);
	void paralax();

	sf::Texture& getBgTexture(Biome biome);

	void addToMinimap(std::pair<sf::Vector2f, MapObj> obj);
	sf::Sprite& getMinimap(sf::Vector2f ppos, bool large = false, bool fogofwar = true, sf::IntRect* viewrect = nullptr);
	float getMinimapScale(float width);
	constexpr float getMinimapRatio() {
		return WORLD_WIDTH / (float)WORLD_HEIGHT;
	};

	Assets& getAssets();

	void updateMapTile(size_t x, size_t y);

	Audio& getAudio();

	bool hardMode();

	enum class Shader {
		WATER,
		LAVA,

		COUNT
	};

	sf::Shader& getShader(Shader which) {
		return m_shaders[(size_t)which];
	};

	void updateShader(Shader which, sf::Vector2f pos);

	std::map<Blocks::Type, sf::Texture> m_shadertextures;
	std::array<Blocks::Type, 2> m_shadertexturetypes = {
		Blocks::Type::WATER,
		Blocks::Type::LAVA
	};

	sf::RenderTexture m_lightmap;
	sf::Sprite m_lightmap_sprite;
	sf::Sprite m_light;
	
	sf::Clock m_gametime;
	size_t m_worldplaytime = 0;

private:
	void calculateBrightness(sf::Vector2f viewpos, sf::Vector2f viewsize);
	void generateMinimap();

	Audio m_audio;
	Assets& m_assets;
	sf::RenderWindow& m_window;

	std::array<std::array<Blocks::Type, WORLD_HEIGHT>, WORLD_WIDTH>* m_tiles;
	std::array<std::array<Blocks::Type, WORLD_HEIGHT>, WORLD_WIDTH>* m_tilesbg;
	sf::Sprite m_blocksprite;

	sf::Vector2f m_spawn;

	Blocks::Type m_outofbounds = Blocks::Type::NONE;
	unsigned char m_outofbounds_b = 0;
	bool m_outofbounds_bu = 0;

	std::thread* m_worker;
	bool m_done = false;

	float m_timer = 0;

	WorldMeta m_recent;
	bool m_day = true;

	sf::Sprite m_bg, m_bgi;
	sf::Clock m_bgiclock;
	Biome m_bg_biome = Biome::FOREST;
	std::array<sf::Texture, (size_t)Biome::FCOUNT> m_bgtexture;

	std::array<std::array<unsigned int, 2>, (size_t)Biome::COUNT> m_biomebounds = {};

	Inventory* m_inventory_r;
	std::array<std::array<bool, WORLD_HEIGHT>, WORLD_WIDTH>* m_brightnessupdated;
	std::array<std::array<bool, WORLD_HEIGHT>, WORLD_WIDTH>* m_werelights;

	std::map<std::pair<unsigned int, unsigned int>, Inventory> m_chests;
	std::map<std::pair<unsigned int, unsigned int>, std::string> m_signs;

	std::string m_status;
	sf::Vector2f m_bgpos;

	// minimap sutff
	sf::Image m_minimap_img;
	sf::Texture m_minimap_tex;
	sf::RenderTexture m_minimap;
	sf::RenderTexture m_minimap_fog;
	sf::Sprite m_minimap_fog_sprite;
	sf::Sprite m_minimap_sprite;
	sf::Sprite m_minimap_sprite_clear;

	bool m_map = false;

	std::deque<std::pair<sf::Vector2f, MapObj>> m_mapobjs;
	std::map<std::pair<unsigned int, unsigned int>, Blocks::Type> m_mapblocks;
	std::map<std::pair<unsigned int, unsigned int>, bool> m_mapbg;
	std::array<std::array<bool, WORLD_HEIGHT/MAP_VISITED_CHUNKS>, WORLD_WIDTH/MAP_VISITED_CHUNKS> m_mapvisited;
	bool m_mapupdated = true, m_largemapold = false;
	sf::Vector2u m_oldpchunkpos;
	
	float m_bupdatet = 0;

	std::array<std::array<bool, WORLD_HEIGHT>, WORLD_WIDTH>* m_liquiddirection;

	Particles* m_particles;

	std::array<sf::Shader, (size_t)Shader::COUNT> m_shaders;

	sf::Clock m_shaderclock;

	bool m_hardmode = false;
};

struct CompressedWorldData {
	std::vector<compressedunit> m_tiles;
	std::vector<compressedunit> m_tilesbg;
	std::string m_data;

	void compress(World& world);
	void decompress(World& world);
	std::string& get();
	void set(std::string& data);
};