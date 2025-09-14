#include "world.hpp"

// i wish i wrote more comments when i knew what the fuck any of this ment

std::string& CompressedWorldData::get() {
	
	// write world sizes
	size_t sizefg = m_tiles.size();
	size_t sizebg = m_tilesbg.size();
	m_data += std::string((char*)&sizefg, sizeof(sizefg));

	// write world data
	for(size_t i = 0; i < sizefg; i++) {
		m_data += std::string((char*)&m_tiles[i].first, sizeof(m_tiles[i].first));
		m_data += std::string((char*)&m_tiles[i].second, sizeof(m_tiles[i].second));
	}

	m_data += std::string((char*)&sizebg, sizeof(sizebg));

	for(size_t i = 0; i < sizebg; i++) {
		m_data += std::string((char*)&m_tilesbg[i].first, sizeof(m_tilesbg[i].first));
		m_data += std::string((char*)&m_tilesbg[i].second, sizeof(m_tilesbg[i].second));
	}
	
	return m_data;
}

void CompressedWorldData::compress(World& world) {
	
	unsigned short count = 0;
	Blocks::Type type = Blocks::Type::COUNT;
	for(size_t x = 0; x < WORLD_WIDTH; x++) {
		for(size_t y = 0; y < WORLD_HEIGHT; y++) {
			Blocks::Type t = world.tile(x, y);

			if(t != type || (x == WORLD_WIDTH-1 && y == WORLD_HEIGHT-1) || count == 0xFFFF) {
				if(count > 0) {
					m_tiles.push_back({type, count});
				}
				type = t;
				count = 1;
			} else {
				count++;
			}
		}
	}

	count = 0;
	type = Blocks::Type::COUNT;

	for(size_t x = 0; x < WORLD_WIDTH; x++) {
		for(size_t y = 0; y < WORLD_HEIGHT; y++) {
			Blocks::Type t = world.tilebg(x, y);

			if(t != type) {
				if(count > 0) {
					m_tilesbg.push_back({type, count});
				}
				type = t;
				count = 1;
			} else {
				count++;
			}
		}
	}

}

void CompressedWorldData::set(std::string& data) {

	std::stringstream stream(data);

	// get world size
	size_t sizefg, sizebg;
	stream.read((char*)&sizefg, sizeof(sizefg));

	// buffer for reading data
	char* buf_c = new char[sizeof(compressedunit::first)];
	char* buf_s = new char[sizeof(compressedunit::second)];

	// get world data
	for(size_t i = 0; i < sizefg; i++) {
		stream.read(buf_c, sizeof(m_tiles[i].first));
		stream.read(buf_s, sizeof(m_tiles[i].second));
		m_tiles.push_back({*((unsigned short*)buf_c), *((unsigned short*)buf_s)});
	}

	stream.read((char*)&sizebg, sizeof(sizebg));

	for(size_t i = 0; i < sizebg; i++) {
		stream.read(buf_c, sizeof(m_tilesbg[i].first));
		stream.read(buf_s, sizeof(m_tilesbg[i].second));
		m_tilesbg.push_back({*((unsigned short*)buf_c), *((unsigned short*)buf_s)});
	}

	delete[] buf_c;
	delete[] buf_s;
}

void CompressedWorldData::decompress(World& world) {
	
	size_t x = 0, y = 0;
	for(size_t i = 0; i < m_tiles.size(); i++) {
		for(size_t j = 0; j < m_tiles[i].second; j++) {
			world.tile(x, y) = (Blocks::Type)m_tiles[i].first;
			if(y++; y >= WORLD_HEIGHT) {
				y = 0;
				x++;
			}
		}
	}

	x = 0, y = 0;
	for(size_t i = 0; i < m_tilesbg.size(); i++) {
		for(size_t j = 0; j < m_tilesbg[i].second; j++) {
			world.tilebg(x, y) = (Blocks::Type)m_tilesbg[i].first;
			if(y++; y >= WORLD_HEIGHT) {
				y = 0;
				x++;
			}
		}
	}

}