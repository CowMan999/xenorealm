#include "world.hpp"
#include "drops.hpp"
#include "audio.hpp"

World::World(Assets& assets, sf::RenderWindow& window) : m_assets(assets), m_window(window) {

	// init sprite
	m_blocksprite.setScale(TILE_SIZE / 16, TILE_SIZE / 16);

	// allocate memory for tiles
	m_tiles = new std::array<std::array<Blocks::Type, WORLD_HEIGHT>, WORLD_WIDTH>;
	m_tilesbg = new std::array<std::array<Blocks::Type, WORLD_HEIGHT>, WORLD_WIDTH>;

	m_brightnessupdated = new std::array<std::array<bool, WORLD_HEIGHT>, WORLD_WIDTH> {0};
	m_werelights = new std::array<std::array<bool, WORLD_HEIGHT>, WORLD_WIDTH> {0};

	m_liquiddirection = new std::array<std::array<bool, WORLD_HEIGHT>, WORLD_WIDTH> {0};

	m_lightmap.create(WORLD_WIDTH*2, WORLD_HEIGHT*2);
	m_lightmap.setSmooth(true);
	m_lightmap_sprite.setTexture(m_lightmap.getTexture());
	m_lightmap_sprite.setScale(TILE_SIZE/2, TILE_SIZE/2);
}

World::~World() {
	delete m_tiles;
}

void World::update(sf::Time dt, Drops* drops) {
	float fdt = dt.asSeconds();

	m_timer += (m_day ? fdt : -fdt);

	// day and night cycle
	if(m_timer > DAY_LENGTH/2) {
		m_timer = DAY_LENGTH/2;

		m_day = false;
	} else if(m_timer < 0) {
		m_timer = 0;

		m_day = true;
	}

	// audio
	auto p = m_audio.playing();
	bool d = p == Audio::Song::DAY;
	bool n = p == Audio::Song::NIGHT;

	if(p == Audio::Song::COUNT || d || n || p == Audio::Song::MENU) {
		if(!night() && !d) {
			m_audio.play(Audio::Song::DAY);
		} else if(night() && !n) {
			m_audio.play(Audio::Song::NIGHT);
		}
	}

	// block updates
	m_bupdatet += fdt;
	if(m_bupdatet > WORLD_TICK) {
		m_bupdatet -= WORLD_TICK;

		sf::Vector2f viewpos = m_window.getView().getCenter() - m_window.getView().getSize()/2.f;
		sf::Vector2f viewsize = m_window.getView().getSize();

		//bool check = true;
	
		viewpos /= (float)TILE_SIZE;
		viewsize /= (float)TILE_SIZE;
	
		// extra space for updating
		viewpos.x -= WORLD_UPDATE_PAST_SCREEN_RANGE;
		viewpos.y -= WORLD_UPDATE_PAST_SCREEN_RANGE;
		viewsize.x += WORLD_UPDATE_PAST_SCREEN_RANGE*2; 
		viewsize.y += WORLD_UPDATE_PAST_SCREEN_RANGE*2;
	
		for(int x = viewpos.x; x < (viewpos.x+viewsize.x); x++) {
			for(int y = viewpos.y; y < (viewpos.y+viewsize.y); y++) {
				if(x < 0 || y < 0 || x >= (int)WORLD_WIDTH || y >= (int)WORLD_HEIGHT) {
					continue;
				}


				Blocks::Type& t = tile(x,y);
	
				// update trees if broken
				bool check = true;
				Blocks::Type& t2 = tile(x,y-1, check);
				if(check && (t2 == Blocks::BARK || t2 == Blocks::STUMP || t2 == Blocks::CACTI) && !Blocks::getBlockData(t).relevant) {
					drops->addDrop(sf::Vector2f(x + 0.5,y-1)*(float)TILE_SIZE, islot(Items::fromBlock(t2), 1));

					// leaf removal
					if(auto type_above = tile(x,y-2, check); t2 == Blocks::BARK && (type_above == Blocks::LEAF || type_above == Blocks::SLEAF) && check) {
						int nx = x, ny = y-1;
						
						for(int x2 = nx - LEAVES_SIZE/2; x2 <= (int)nx + (int)LEAVES_SIZE/2; x2++) {
							for(int y2 = ny - (LEAVES_SIZE); y2 <= (int)ny + (int)1; y2++) {
								if(x2 < 0 || y2 < 0 || x2 >= (int)WORLD_WIDTH || y2 >= (int)WORLD_HEIGHT) { // box with tree max size
									continue;
								}

								Blocks::Type& ti = tile(x2,y2);
								if(ti == Blocks::LEAF || ti == Blocks::SLEAF) {
									Items::Type item = Items::fromBlock(ti);
									ti = Blocks::NONE;
									updateMapTile(x2, y2);
									drops->addDrop(sf::Vector2f(x2 + 0.5 + Random::getf(-0.35,0.35),y2 + Random::getf(-0.35,0.35))*(float)TILE_SIZE, islot(item, 1));
								}
							}
						}
					}

					t2 = Blocks::NONE;
					updateMapTile(x, y-1);
				}

				// update liquids
				if(Blocks::getBlockData(t).liquid) {

					// liquids falls down
					Blocks::Type& td = tile(x,y+1, check);
					if(check && (!Blocks::getBlockData(td).relevant) && !Blocks::getBlockData(td).liquid) {
						td = t, t = Blocks::NONE;
						updateMapTile(x, y);
						updateMapTile(x, y+1);
						(*m_liquiddirection)[x][y+1] = (*m_liquiddirection)[x][y];
					}

					// liquid spreads
					bool check2;
					Blocks::Type& tr = tile(x+1,y, check);
					Blocks::Type& tl = tile(x-1,y, check2);
					bool rvalid = check && !Blocks::getBlockData(tr).relevant && !Blocks::getBlockData(tr).liquid;
					bool lvalid = check2 && !Blocks::getBlockData(tl).relevant && !Blocks::getBlockData(tl).liquid;

					if(Random::getb()) {
						if(rvalid && (*m_liquiddirection)[x][y] == 1) {
							tr = t, t = Blocks::NONE;
							updateMapTile(x, y);
							updateMapTile(x+1, y);
							(*m_liquiddirection)[x-1][y] = 1;
						} else if(rvalid) {
							(*m_liquiddirection)[x][y] = 1;
						}

						else if(lvalid && (*m_liquiddirection)[x][y] == 0) {
							tl = t, t = Blocks::NONE;
							updateMapTile(x, y);
							updateMapTile(x-1, y);
							(*m_liquiddirection)[x-1][y] = 0;
						} else if(lvalid) {
							(*m_liquiddirection)[x][y] = 0;
						}
					} else {
						if(lvalid && (*m_liquiddirection)[x][y] == 0) {
							tl = t, t = Blocks::NONE;
							updateMapTile(x, y);
							updateMapTile(x-1, y);
							(*m_liquiddirection)[x-1][y] = 0;
						} else if(lvalid) {
							(*m_liquiddirection)[x][y] = 0;
						}

						else if(rvalid && (*m_liquiddirection)[x][y] == 1) {
							tr = t, t = Blocks::NONE;
							updateMapTile(x, y);
							updateMapTile(x+1, y);
							(*m_liquiddirection)[x+1][y] = 1;
						} else if(rvalid) {
							(*m_liquiddirection)[x][y] = 1;
						}
					}
				}
			}
		}
	}
}
void World::draw() {

	// figure out render region
	sf::Vector2f viewpos = m_window.getView().getCenter() - m_window.getView().getSize() / 2.f;
	sf::Vector2f viewsize = m_window.getView().getSize();

	Blocks::Type type_old = Blocks::Type::NONE;

	// draw paralaxed bg first
	m_window.draw(m_bg);
	if(m_bgi.getColor() != m_bg.getColor())
		m_window.draw(m_bgi);

	// long story short, cull everything off screen and don't waste time updating block when its not needed, and calculate brightness of each block via distance to a light source.

	// calculate brightness
	calculateBrightness(viewpos, viewsize);

	for (size_t i = 0; i < (size_t)Shader::COUNT; i++) {
		updateShader((Shader)i, viewpos);
	}

	// draw bg first
	for (size_t x = (viewpos.x)/TILE_SIZE; x < (viewpos.x+viewsize.x)/TILE_SIZE; x++) {
		for (size_t y = (viewpos.y)/TILE_SIZE; y < (viewpos.y+viewsize.y)/TILE_SIZE; y++) {
			Blocks::Type type = tilebg(x, y);
			Blocks::Type typef = tile(x, y);
			sf::Vector2u pos(x, y);

			if((typef == Blocks::Type::NONE || Blocks::getBlockData(typef).alpha) && type != Blocks::Type::NONE) {
				m_blocksprite.setPosition(sf::Vector2f(pos*TILE_SIZE));
				if(type != type_old) {
					m_blocksprite.setTextureRect(m_assets.getAnimation(Blocks::getBlockData(type).anim)[0]);
					type_old = type;
				}

				float val = ((float)255)-BG_WALLS_COLOR;
				if(val < 0) {val = 0;}
				m_blocksprite.setColor(sf::Color(val, val, val, 255));
				m_window.draw(m_blocksprite);
			} 
		}
	}

	// draw blocks
	for (size_t x = (viewpos.x)/TILE_SIZE; x < (viewpos.x+viewsize.x)/TILE_SIZE; x++) {
		for (size_t y = (viewpos.y)/TILE_SIZE; y < (viewpos.y+viewsize.y)/TILE_SIZE; y++) {
			Blocks::Type type = tile(x, y);
			sf::Vector2u pos(x, y);

			if(type != Blocks::Type::NONE) {
				m_blocksprite.setPosition(sf::Vector2f(pos * TILE_SIZE));
				if(type != type_old) {
					m_blocksprite.setTextureRect(m_assets.getAnimation(Blocks::getBlockData(type).anim)[0]);
					type_old = type;
				}

				m_blocksprite.setColor(sf::Color(255, 255, 255, 255));

				if(type == Blocks::WATER || type == Blocks::LAVA) {
					if(type == Blocks::WATER) {
						m_window.draw(m_blocksprite, &getShader(Shader::WATER));
					} else {
						m_window.draw(m_blocksprite, &getShader(Shader::LAVA));
					}

				} else {
					m_window.draw(m_blocksprite);
				}
			}
		}
	}

}

Blocks::Type& World::tile(size_t x, size_t y) {

	// check if out of bounds
	if(x >= WORLD_WIDTH || y >= WORLD_HEIGHT)
		return m_outofbounds;

	return (*m_tiles)[x][y];
}

Blocks::Type& World::tilebg(size_t x, size_t y) {

	// check if out of bounds
	if(x >= WORLD_WIDTH || y >= WORLD_HEIGHT)
		return m_outofbounds;

	return (*m_tilesbg)[x][y];
}

Blocks::Type& World::tile(size_t x, size_t y, bool& check) {

	// check if out of bounds
	if(x >= WORLD_WIDTH || y >= WORLD_HEIGHT) {
		check = false;
		return m_outofbounds;
	}
	check = true;

	return (*m_tiles)[x][y];
}

Blocks::Type& World::tilebg(size_t x, size_t y, bool& check) {

	// check if out of bounds
	if(x >= WORLD_WIDTH || y >= WORLD_HEIGHT) {
		check = false;
		return m_outofbounds;
	}
	check = true;

	return (*m_tilesbg)[x][y];
}

sf::Vector2f World::getSpawn() {

	if(m_spawn == sf::Vector2f()) {
		// set spawn in the middle of the world, atop the highest block
		size_t ypos = 0;
		for(size_t y = 1; y < WORLD_HEIGHT; y++) {
			if(Blocks::getBlockData(tile(WORLD_WIDTH/2, y)).solid) {
				if(y-1 > WORLD_HEIGHT) ypos = 0;
				else ypos = y-1;
				break;
			}
		}
		m_spawn = sf::Vector2f((WORLD_WIDTH/2+.5)*TILE_SIZE, ypos*TILE_SIZE);
	}

	return m_spawn;
}

bool World::done() {
	if(m_done) {
		m_worker->join();
		delete m_worker;
		m_done = false;
		return true;
	} else {
		return false;
	}
}

void World::save(WorldMeta data, Inventory& inventory) {
	size_t num = 0;
	std::string filename;
	if(data.path == std::filesystem::path()) {
		do {
			filename = "data/worlds/world" + std::to_string(num); 
			num++;
		} while(std::filesystem::exists(filename));
	} else {
		filename = data.path.string();
	}

	std::ofstream file(filename, std::ios::binary);
	m_recent = data;
	m_recent.path = filename;
	m_inventory_r = &inventory;
	
	// write world meta
	file << data.name.size() << ' ' << data.name << ' ' << data.seed.size() << ' ' << data.seed;
	m_worldplaytime += std::round(m_gametime.restart().asSeconds());
	file.write(reinterpret_cast<char*>(&m_worldplaytime), sizeof(size_t));
	file.write(reinterpret_cast<char*>(&m_hardmode), sizeof(bool));
	

	std::vector<sf::Vector2u> chests;
	std::vector<sf::Vector2u> signs;

	// write world data
	for(size_t x = 0; x < WORLD_WIDTH; x++) {
		for(size_t y = 0; y < WORLD_HEIGHT; y++) {
			// save chest/sign data for later
			if(tile(x, y) == Blocks::Type::CHEST) {
				chests.push_back(sf::Vector2u(x, y));
			} else if(tile(x, y) == Blocks::Type::SIGN) {
				signs.push_back(sf::Vector2u(x, y));
			}
		}
	}
	
	// compress via compressor
	CompressedWorldData compressor;
	compressor.compress(*this);

	// calling all cryptogaphers...
	std::string& bin_dump = compressor.get();

	// write compressed data
	size_t size = bin_dump.size();

	file.write(reinterpret_cast<const char*>(&size), sizeof(size));
	file.write(bin_dump.c_str(), size);

	// write map data
	file.write(reinterpret_cast<const char*>(&m_mapvisited), sizeof(m_mapvisited));

	// write chests
	size = chests.size();
	file.write(reinterpret_cast<const char*>(&size), sizeof(size));
	for(size_t i = 0; i < size; i++) {
		file.write(reinterpret_cast<const char*>(&chests[i].x), sizeof(chests[i].x));
		file.write(reinterpret_cast<const char*>(&chests[i].y), sizeof(chests[i].y));
		Inventory inv = m_chests[{chests[i].x, chests[i].y}];
		file.write(reinterpret_cast<char*>(&inv), sizeof(inv));
	}

	// write signs
	size = signs.size();
	file.write(reinterpret_cast<const char*>(&size), sizeof(size));
	for(size_t i = 0; i < size; i++) {
		file.write(reinterpret_cast<const char*>(&signs[i].x), sizeof(signs[i].x));
		file.write(reinterpret_cast<const char*>(&signs[i].y), sizeof(signs[i].y));
		std::string str = m_signs[{signs[i].x, signs[i].y}];
		size_t len = str.size();
		file.write(reinterpret_cast<char*>(&len), sizeof(len));
		file.write(str.c_str(), len);
	}

	// write inventory
	file.write(reinterpret_cast<const char*>(&inventory), sizeof(inventory));

	// write biome/time data
	file.write(reinterpret_cast<char*>(&m_biomebounds), sizeof(m_biomebounds));
	file.write(reinterpret_cast<char*>(&m_spawn), sizeof(m_spawn));
	file.write(reinterpret_cast<char*>(&m_timer), sizeof(m_timer));
	file.write(reinterpret_cast<char*>(&m_day), sizeof(m_day));

	

	file.close();
}

void World::load(WorldMeta data, Inventory& inventory) {

	m_worker = new std::thread([this, data, &inventory]() {
	
		m_status = "Clearing world...";
		reset();

		m_blocksprite.setTexture(m_assets.getTexture(Assets::texture_blocks));
		m_light.setTexture(m_assets.getTexture(Assets::texture_light));
		m_light.setOrigin(sf::Vector2f(m_assets.getTexture(Assets::texture_light).getSize()/(unsigned int)2) - sf::Vector2f(1, 1));
		std::ifstream file(data.path, std::ios::binary);

		m_recent = data;
		m_inventory_r = &inventory;

		// skip world meta
		size_t len;
		file >> len;
		file.ignore(len+1);
		file >> len;
		file.ignore(len+1);

		// read world playtime, albeit part of world meta
		file.read(reinterpret_cast<char*>(&m_worldplaytime), sizeof(size_t));
		file.read(reinterpret_cast<char*>(&m_hardmode), sizeof(bool));

		// read compressed data
		CompressedWorldData decompressor;

		m_status = "Reading data...";

		size_t size;
		file.read(reinterpret_cast<char*>(&size), sizeof(size));
		char* bin_dump = new char[size];
		file.read(bin_dump, size); 

		std::string str(bin_dump, size);

		m_status = "Decompressing...";

		decompressor.set(str);
		decompressor.decompress(*this);

		delete[] bin_dump;

		m_status = "Inventory/Chest/Map data...";

		// read map data
		file.read(reinterpret_cast<char*>(&m_mapvisited), sizeof(m_mapvisited));


		// read chests
		unsigned int x, y;
		file.read(reinterpret_cast<char*>(&size), sizeof(size));
		for(size_t i = 0; i < size; i++) {
			file.read(reinterpret_cast<char*>(&x), sizeof(x));
			file.read(reinterpret_cast<char*>(&y), sizeof(y));
			Inventory inv;
			file.read(reinterpret_cast<char*>(&inv), sizeof(inv));
			m_chests[{x, y}] = inv;
		}

		// read signs
		file.read(reinterpret_cast<char*>(&size), sizeof(size));
		for(size_t i = 0; i < size; i++) {
			file.read(reinterpret_cast<char*>(&x), sizeof(x));
			file.read(reinterpret_cast<char*>(&y), sizeof(y));
			size_t len;
			file.read(reinterpret_cast<char*>(&len), sizeof(len));
			std::string str(len, '\0');
			file.read(&str[0], len);
			m_signs[{x, y}] = str;
		}

		// read inventory
		file.read(reinterpret_cast<char*>(&inventory), sizeof(inventory));
		inventory.select(0);

		

		// read biome/time/spawn data
		file.read(reinterpret_cast<char*>(&m_biomebounds), sizeof(m_biomebounds));
		file.read(reinterpret_cast<char*>(&m_spawn), sizeof(m_spawn));
		file.read(reinterpret_cast<char*>(&m_timer), sizeof(m_timer));
		file.read(reinterpret_cast<char*>(&m_day), sizeof(m_day));

		file.close();

		m_status = "Generating map...";
		
		// generate minimap
		generateMinimap();

		m_gametime.restart();
		m_status = "";
		m_done = true;

	});

}

WorldMeta World::recentWorld() {
	return m_recent;
}

int World::getGroundHeight(int x, size_t y) {
	for(; y < WORLD_HEIGHT; y++) {
		if(Blocks::getBlockData(tile(x, y)).solid) {
			return y;
		}
	}
	return 0;
}

void World::reset() {
	for(size_t x = 0; x < WORLD_WIDTH; x++) {
		for(size_t y = 0; y < WORLD_HEIGHT; y++) {
			tile(x, y) = Blocks::Type::NONE;
			tilebg(x, y) = Blocks::Type::NONE;
			brightnessUpdated(x, y) = false;
			wasLight(x, y) = false;
			m_mapvisited[x/MAP_VISITED_CHUNKS][y/MAP_VISITED_CHUNKS] = false;
		}
	}

	m_lightmap.clear();

	m_timer = 0;
	m_hardmode = false;
	m_spawn = {0, 0};
	m_worldplaytime = 0;
	m_gametime.restart();
	m_audio.reset();
}


sf::Color World::getSkyColor() {
	float day_len2 = DAY_LENGTH/2;
	float t = (day_len2-m_timer)/day_len2;

	// speed up t at the beginning and end of the day
	if(t < 0.25f) t += t-0.25f;
	else if(t > 0.75f) t += t-0.75f;

	// caps to avoid color overflow
	if(t < 0) t = 0;
	else if(t > 1) t = 1;

	return sf::Color(BACKGROUND_COLOR[0]*t, BACKGROUND_COLOR[1]*t, BACKGROUND_COLOR[2]*t);
}

sf::Color World::getSkyColorGrayscale() {
	float day_len2 = DAY_LENGTH/2;
	float t = (day_len2-m_timer)/day_len2;

	// speed up t at the beginning and end of the day
	if(t < 0.25f) t += t-0.25f;
	else if(t > 0.75f) t += t-0.75f;

	// caps to avoid color overflow
	if(t < 0) t = 0;
	else if(t > 1) t = 1;

	return sf::Color(t*255, t*255, t*255);
}

bool World::night() {
	return m_timer > (float)DAY_LENGTH/4;
}

Biome World::getBiome(unsigned int x) {
	for (size_t i = 0; i < (size_t)Biome::COUNT; i++)
		if(x > m_biomebounds[i][0] && x < m_biomebounds[i][1]) return (Biome)i;
		
	return Biome::FOREST;
}

void World::calculateBrightness(sf::Vector2f viewpos, sf::Vector2f viewsize) {
	
	int x_start = (viewpos.x)/TILE_SIZE - ZERO_LIGHT_DISTANCE, x_end = (viewpos.x+viewsize.x)/TILE_SIZE + ZERO_LIGHT_DISTANCE + 1;
	int y_start = (viewpos.y)/TILE_SIZE - ZERO_LIGHT_DISTANCE, y_end = (viewpos.y+viewsize.y)/TILE_SIZE + ZERO_LIGHT_DISTANCE + 1;

	// clamp
	if(x_end > (int)WORLD_WIDTH) x_end = WORLD_WIDTH;
	if(y_end > (int)WORLD_HEIGHT) y_end = WORLD_HEIGHT;
	if(x_start < 0) x_start = 0;
	if(y_start < 0) y_start = 0;

	// was light hanlding
	for(size_t x = x_start; x < (size_t)x_end; x++) {
		for(size_t y = y_start; y < (size_t)y_end; y++) {

			bool& bup = brightnessUpdated(x, y);

			if(bup) continue;


			// light tiles 
			auto& t = tile(x, y);
			auto& tb = tilebg(x, y);
			bool light = false;
			if(Blocks::getBlockData(t).light || (t == Blocks::Type::NONE && tb == Blocks::Type::NONE)) {
				light = true;
			}

			if(wasLight(x, y) && !light) {

				sf::RectangleShape rect(sf::Vector2f(ZERO_LIGHT_DISTANCE*8+1, ZERO_LIGHT_DISTANCE*8+1));
				rect.setFillColor(sf::Color(0, 0, 0, 255));
				rect.setOrigin(ZERO_LIGHT_DISTANCE*2+1, ZERO_LIGHT_DISTANCE*2+1);
				rect.setPosition(x*2, y*2);
				m_lightmap.draw(rect);

				for(int i = x-ZERO_LIGHT_DISTANCE*4; i < (int)x+ZERO_LIGHT_DISTANCE*4; i++) {
					for(int j = y-ZERO_LIGHT_DISTANCE*4; j < (int)y+ZERO_LIGHT_DISTANCE*4; j++) {
						brightnessUpdated(i, j) = false;
					}
				}

				wasLight(x, y) = false;
			} else if(!wasLight(x, y) && light) {
				wasLight(x, y) = true;
			}

		}
	}


	// draw lightmap to alphamaped renertexure
	for (size_t x = x_start; x < (size_t)x_end; x++) {
		for (size_t y = y_start; y < (size_t)y_end; y++) {
			
			bool& bup = brightnessUpdated(x, y);

			if(bup) continue;

			Blocks::Type& t = tile(x, y);
			Blocks::Type& tb = tilebg(x, y);

			bool light = false;

			// light tiles 
			if(Blocks::getBlockData(t).light || (t == Blocks::Type::NONE && tb == Blocks::Type::NONE)) {
				light = true;
			} 

			// update lighting of other tiles
			if(light) {

				// check if all surroundings are lights (don't draw light if so)
				bool all = true;
				for(int i = x-1; i <= (int)x+1; i++) {
					for(int j = y-1; j <= (int)y+1; j++) {
						if(i == (int)x && j == (int)y) continue;
						if(!wasLight(i, j)) {
							all = false;
							break;
						}
					}
					if(!all) break;
				}

				if(!all) {
					m_light.setPosition(x*2, y*2);
					m_lightmap.draw(m_light, sf::BlendMax);
				} else {
					sf::RectangleShape rect({2, 2});
					rect.setFillColor(sf::Color(255, 255, 255, 255));
					rect.setPosition(x*2, y*2);
					m_lightmap.draw(rect);
				}
				

			}

			bup = true;
		}
	}
	m_lightmap.display();

}

bool& World::brightnessUpdated(size_t x, size_t y) {

	// check if out of bounds
	if(x >= WORLD_WIDTH || y >= WORLD_HEIGHT)
		return m_outofbounds_bu;

	return (*m_brightnessupdated)[x][y];
}

bool& World::wasLight(size_t x, size_t y) {

	// check if out of bounds
	if(x >= WORLD_WIDTH || y >= WORLD_HEIGHT)
		return m_outofbounds_bu;

	return (*m_werelights)[x][y];
}

void World::appendChest(size_t x, size_t y) {
	m_chests[{x, y}] = Inventory();
}

bool World::removeChest(size_t x, size_t y) {

	Inventory& inv = m_chests[{x, y}];

	// check if chest is empty
	for(size_t i = 0; i < INVENTORY_SIZE; i++)
	{
		if(inv[i].item != Items::Type::NONE)
			return false;
	}
	
	m_chests.erase({x, y});

	return true;
}

Inventory& World::getChest(size_t x, size_t y) {
	return m_chests[{x, y}];
}

const std::string& World::getStatus() {
	return m_status;
}

bool World::loadBg(sf::RenderWindow* window) {
	if(!getBgTexture(Biome::FOREST).loadFromFile("./assets/backgrounds/background_forest.png")) return false;
	if(!getBgTexture(Biome::SNOW).loadFromFile("./assets/backgrounds/background_snow.png")) return false;
	if(!getBgTexture(Biome::DESERT).loadFromFile("./assets/backgrounds/background_desert.png")) return false;
	if(!getBgTexture(Biome::CASTLE).loadFromFile("./assets/backgrounds/background_castle.png")) return false;

	for(size_t i = 0; i < (size_t)Biome::FCOUNT; i++) {
		m_bgtexture[i].setRepeated(true);
	}

	m_bg.setTexture(getBgTexture(Biome::FOREST));

	updateBg(*window);

	m_audio.load();

	return true;
}

void World::updateBg(sf::RenderWindow& window) {
	float y = window.getView().getSize().y;
	float val = y / getBgTexture(m_bg_biome).getSize().y;
	val *= PARALAX_BGSIZEINC;
	val *= 1080.f / m_window.getSize().y;
	m_bg.setScale(val, val);
	m_bg.setTextureRect(sf::IntRect(m_bgpos.x, 0, window.getSize().x, getBgTexture(m_bg_biome).getSize().y));
}

void World::paralax() {

	sf::Vector2f pos = m_window.getView().getCenter();
	Biome b = getBiome(pos.x/TILE_SIZE);
	if(m_bg_biome != b) {
		if(m_bgiclock.getElapsedTime().asSeconds() < 1) {
			m_bg.setTexture(getBgTexture(m_bg_biome));
		}

		m_bgi.setTexture(getBgTexture(b));
		m_bgiclock.restart();
		m_bg_biome = b;
	}
	
	pos.x /= (float)PARALAX_DIVISOR_X;
	m_bgpos.x = pos.x;

	m_bg.setTextureRect(sf::IntRect(m_bgpos.x, 0, m_bg.getTextureRect().width, getBgTexture(m_bg_biome).getSize().y));

	// calculate how bg should be vertically positioned
	float y =  pos.y - (WORLD_HEIGHT/AIR_FRACTION - 75)*TILE_SIZE;
	y /= (float)PARALAX_DIVISOR_Y;
	m_bgpos.y = y;

	sf::Color c = getSkyColorGrayscale();
	if(c.r < 60) {
		c.r = 60; c.g = 60; c.b = 60;
	}

	m_bg.setColor(c);

	sf::Vector2f viewpos = m_window.getView().getCenter() - m_window.getView().getSize()/2.f;
	sf::Vector2f viewsize = m_window.getView().getSize();

	float rem = m_bgpos.x - (int)m_bgpos.x;
	float vy = viewpos.y - m_bgpos.y;

	m_bg.setPosition(viewpos.x - (rem * m_bg.getScale().x), vy);

	if(m_bg.getPosition().y > viewpos.y)
		m_bg.setPosition(m_bg.getPosition().x, viewpos.y);
	else if(m_bg.getPosition().y + m_bg.getGlobalBounds().height < viewpos.y + viewsize.y)
		m_bg.setPosition(m_bg.getPosition().x, viewpos.y + viewsize.y - m_bg.getGlobalBounds().height);

	m_bgi.setPosition(m_bg.getPosition());
	m_bgi.setScale(m_bg.getScale());
	int a = 0 + m_bgiclock.getElapsedTime().asSeconds() * 255;
	if(a > 255) a = 255;
	c.a = a;
	m_bgi.setColor(c);
	m_bgi.setTextureRect(m_bg.getTextureRect());
	
	if(m_bgi.getColor() == m_bg.getColor()) {
		m_bgi.setColor(sf::Color::Transparent);
		m_bg.setTexture(getBgTexture(b));
	}
}

sf::Texture& World::getBgTexture(Biome biome) {
	return m_bgtexture[(size_t)biome+1];
}

Assets& World::getAssets() {
	return m_assets;
}

void World::updateMapTile(size_t x, size_t y) {
	m_mapblocks[{x, y}] = tile(x, y);
	m_mapbg[{x, y}] = false;
	
	if(tile(x, y) == Blocks::NONE && tilebg(x, y) != Blocks::NONE) {
		m_mapbg[{x, y}] = true;
		m_mapblocks[{x, y}] = tilebg(x, y);
	}
	brightnessUpdated(x, y) = false;

	if(tile(x, y) == Blocks::SIGN) {
		m_signs[{x, y}] = "";
	}
}


// create pointer to a matrix of tile-pointers
// DON'T FORGET TO DELETE IT with the func
Blocks::Type*** World::tilematrix(size_t x, size_t y, size_t w, size_t h) {
	Blocks::Type*** matrix = new Blocks::Type**[w];
	for(size_t i = 0; i < w; i++)
		matrix[i] = new Blocks::Type*[h];

	for(size_t x2 = 0; x2 < w; x2++) {
		for(size_t y2 = 0; y2 < h; y2++) {
			if(x+x2 >= WORLD_WIDTH || y+y2 >= WORLD_HEIGHT) {
				matrix[x2][y2] = &m_outofbounds;
				continue;
			}
			matrix[x2][y2] = &(*m_tiles)[x+x2][y+y2];
		}
	}

	return matrix;
}

// DON'T FORGET TO DELETE IT with the func
Blocks::Type*** World::tilebgmatrix(size_t x, size_t y, size_t w, size_t h) {
	Blocks::Type*** matrix = new Blocks::Type**[w*h];
	for(size_t i = 0; i < w; i++)
		matrix[i] = new Blocks::Type*[h];

	for(size_t x2 = 0; x2 < w; x2++) {
		for(size_t y2 = 0; y2 < h; y2++) {
			if(x+x2 >= WORLD_WIDTH || y+y2 >= WORLD_HEIGHT) {
				matrix[x2][y2] = &m_outofbounds;
				continue;
			}
			matrix[x2][y2] = &(*m_tilesbg)[x+x2][y+y2];
		}
	}

	return matrix;
}

void World::deletetilematrix(Blocks::Type*** matrix, size_t w) {
	for(size_t i = 0; i < w; i++)
		delete[] matrix[i];
	delete[] matrix;
}

bool World::calculateLightMatrix() {
	sf::Image img; img.create(ZERO_LIGHT_DISTANCE*4+2, ZERO_LIGHT_DISTANCE*4+2);
	for (int x = -ZERO_LIGHT_DISTANCE*2-1; x < ZERO_LIGHT_DISTANCE*2+1; x++) {
		for (int y = -ZERO_LIGHT_DISTANCE*2-1; y < ZERO_LIGHT_DISTANCE*2+1; y++) {
			sf::Vector2i dist = sf::Vector2i(x, y);
			float val = sqrtf(dist.x*dist.x + dist.y*dist.y);

			// calculate brightness based on distance
			val = val/(ZERO_LIGHT_DISTANCE*2+1);
			if(val > 1) {val = 1;} if(val < 0) {val = 0;}
			float bright = std::lerp(255, 0, val);

			img.setPixel(x+ZERO_LIGHT_DISTANCE*2+1, y+ZERO_LIGHT_DISTANCE*2+1, sf::Color(bright, bright, bright));
		}
	}

	img.saveToFile("light.png");
	return true;
}

bool World::compileShaders() {
	
	getShader(Shader::WATER).loadFromFile("./assets/shaders/water.frag", sf::Shader::Fragment);
	getShader(Shader::LAVA).loadFromFile("./assets/shaders/water.frag", sf::Shader::Fragment);

	for(size_t i = 0; i < (size_t)Shader::COUNT; i++) {
		if(!getShader((Shader)i).isAvailable()) {
			throw std::runtime_error("Shader " + std::to_string(i) + " failed to compile");
			return false;
		}
	}

	// load shader textures
	sf::Sprite spr;
	sf::RenderTexture rt;
	rt.create(TILE_SIZE/2, TILE_SIZE/2);
	spr.setTexture(m_assets.getTexture(Assets::texture_blocks));

	for(size_t i = 0; i < m_shadertexturetypes.size(); i++) {
		spr.setTextureRect(m_assets.getAnimation(Blocks::getBlockData(m_shadertexturetypes[i]).anim)[0]);
		rt.draw(spr); rt.display();
		m_shadertextures[m_shadertexturetypes[i]] = rt.getTexture();
		m_shadertextures[m_shadertexturetypes[i]].setRepeated(true);
	}

	getShader(Shader::WATER).setUniform("tex", m_shadertextures[Blocks::WATER]);
	getShader(Shader::LAVA).setUniform("tex", m_shadertextures[Blocks::LAVA]);

	for(size_t i = 0; i < (size_t)Shader::COUNT; i++) {
		getShader((Shader)i).setUniform("scale", (float)m_window.getSize().x / 1920);
	}

	return true;
}

Audio& World::getAudio() {
	return m_audio;
}

void World::updateShader(Shader shader, sf::Vector2f pos) {
	getShader(shader).setUniform("time", m_shaderclock.getElapsedTime().asSeconds());
	getShader(shader).setUniform("res", sf::Glsl::Vec2(m_window.getSize()));
	getShader(shader).setUniform("pos", pos);
}

void World::drawLight() {
	m_window.draw(m_lightmap_sprite, sf::RenderStates(sf::BlendMultiply));
}

std::string& World::getSign(size_t x, size_t y) {
	return m_signs[{x, y}];
}

bool World::hardMode() {
	return m_hardmode;
}