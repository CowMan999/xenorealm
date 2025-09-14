#include "world.hpp"

sf::Sprite& World::getMinimap(sf::Vector2f ppos, bool large, bool fogofwar, sf::IntRect* viewrect_large) {
	
	if(m_largemapold != large) {
		m_largemapold = large;

		m_mapupdated = true;
	}

	sf::IntRect viewrect;

	if(!large) {
		viewrect = sf::IntRect(ppos.x/TILE_SIZE - MINIMAP_SIZE/2, ppos.y/TILE_SIZE - MINIMAP_SIZE/2, MINIMAP_SIZE, MINIMAP_SIZE);

		if(viewrect.left < 0) viewrect.left = 0;
		if(viewrect.top < 0) viewrect.top = 0;
		if(viewrect.left + viewrect.width > (int)WORLD_WIDTH) viewrect.left = WORLD_WIDTH - viewrect.width;
		if(viewrect.top + viewrect.height > (int)WORLD_HEIGHT) viewrect.top = WORLD_HEIGHT - viewrect.height;

		m_minimap_sprite.setTextureRect(viewrect);
		m_minimap_sprite.setScale(MINIMAP_SCALE, MINIMAP_SCALE);
	} else {
		viewrect = *viewrect_large;
	
		// find the largest scale that fits the screen
		float scale = getMinimapScale(viewrect.width);

		m_minimap_sprite.setTextureRect(viewrect);
		m_minimap_sprite.setScale(scale,scale);
	}

	// fog of war
	sf::Vector2u ppos_chunk = sf::Vector2u((ppos / (float)TILE_SIZE) / (float)MAP_VISITED_CHUNKS);
	if(m_oldpchunkpos != ppos_chunk) {
		m_oldpchunkpos = ppos_chunk;
		m_mapupdated = true;
	}

	for(int x = ppos_chunk.x-MAP_CLEAR_DISTANCE; x < (int)ppos_chunk.x+(int)MAP_CLEAR_DISTANCE; x++) {
		for(int y = ppos_chunk.y-(int)MAP_CLEAR_DISTANCE; y < (int)ppos_chunk.y+(int)MAP_CLEAR_DISTANCE; y++) {
			
			if(x < 0 || y < 0 || x >= (int)WORLD_WIDTH/MAP_VISITED_CHUNKS || y >= (int)WORLD_HEIGHT/MAP_VISITED_CHUNKS) continue;
			
			bool& visited = m_mapvisited[x][y];
			if(!visited) {
				visited = true;
			}
		}
	}
		
	m_minimap.clear(sf::Color(BACKGROUND_COLOR[0], BACKGROUND_COLOR[1], BACKGROUND_COLOR[2]));
	m_minimap.draw(m_minimap_sprite_clear);

	// circle
	sf::RectangleShape rect;

	rect.setOrigin(sf::Vector2f(0, 0));
	rect.setSize(sf::Vector2f(1, 1));

	for(auto& block : m_mapblocks) {
		rect.setPosition(sf::Vector2f(block.first.first, block.first.second));

		if(rect.getGlobalBounds().intersects(sf::FloatRect(m_minimap_sprite.getTextureRect()))) {
			rect.setFillColor(Blocks::getBlockColor(block.second));

			if(m_mapbg[block.first]) {
				short r = rect.getFillColor().r, g = rect.getFillColor().g, b = rect.getFillColor().b;
				r -= BG_WALLS_COLOR; g -= BG_WALLS_COLOR; b -= BG_WALLS_COLOR;
				if(r < 0) {r = 0;} if(g < 0) {g = 0;} if(b < 0) {b = 0;}
				rect.setFillColor(sf::Color(r, g, b));
			}

			m_minimap.draw(rect);
		}
	}

	// draw objects
	for(auto& obj : m_mapobjs) {
		if(obj.second == MapObj::ENEMY) {
			rect.setSize(sf::Vector2f(3, 3));
			rect.setOrigin(1.5f, 1.5f);
			rect.setFillColor(sf::Color::Red);
		} else if(obj.second == MapObj::BOSS) {
			rect.setSize(sf::Vector2f(5, 5));
			rect.setOrigin(2.5f, 2.5f);
			rect.setFillColor(sf::Color::Magenta);
		}

		rect.setPosition(obj.first / (float)TILE_SIZE);
		m_minimap.draw(rect);
	}

	m_mapobjs.clear();


	rect.setSize(sf::Vector2f(3, 3));
	rect.setFillColor(sf::Color::Green);
	rect.setOrigin(1.5f, 1.5f);
	rect.setPosition(ppos.x/TILE_SIZE, ppos.y/TILE_SIZE);
	
	m_minimap.draw(rect);

	// fog
	if(m_mapupdated) {
		rect.setOrigin(0, 0);
		rect.setSize(sf::Vector2f(1, 1));
		rect.setFillColor(sf::Color::Black);

		m_minimap_fog.clear(sf::Color::Transparent);

		auto rect_screen = m_minimap_sprite.getTextureRect();
		rect_screen.width /= MAP_VISITED_CHUNKS; rect_screen.height /= MAP_VISITED_CHUNKS;
		rect_screen.left /= MAP_VISITED_CHUNKS; rect_screen.top /= MAP_VISITED_CHUNKS;
		rect_screen.top -= 2; rect_screen.left -= 2;
		rect_screen.width += 4; rect_screen.height += 4;

		if(rect_screen.left < 0) { rect_screen.left = 0; }
		if(rect_screen.top < 0) { rect_screen.top = 0; }
		if(rect_screen.left+rect_screen.width > (int)WORLD_WIDTH/MAP_VISITED_CHUNKS) { rect_screen.width = WORLD_WIDTH/MAP_VISITED_CHUNKS-rect_screen.left; }
		if(rect_screen.top+rect_screen.height > (int)WORLD_HEIGHT/MAP_VISITED_CHUNKS) { rect_screen.height = WORLD_HEIGHT/MAP_VISITED_CHUNKS-rect_screen.top; }

		for (size_t x = rect_screen.left; x < (size_t)rect_screen.left+rect_screen.width; x++) {
			for (size_t y = rect_screen.top; y < (size_t)rect_screen.top+rect_screen.height; y++) {

				if(x >= WORLD_WIDTH/MAP_VISITED_CHUNKS || y >= WORLD_HEIGHT/MAP_VISITED_CHUNKS) {
					continue;
				}

				if(!m_mapvisited[x][y]) {
					rect.setPosition(x, y);
					m_minimap_fog.draw(rect);
				}
			}
		}

		m_minimap_fog.display();

		m_mapupdated = false;
	}

	if(fogofwar)
		m_minimap.draw(m_minimap_fog_sprite);

	m_minimap.display();

	return m_minimap_sprite;
}

void World::generateMinimap() {

	m_mapblocks.clear();
	m_mapobjs.clear();

	m_minimap.clear();
	m_minimap_img.create(WORLD_WIDTH, WORLD_HEIGHT, sf::Color::Transparent);

	for(size_t x = 0; x < WORLD_WIDTH; x++) {
		for(size_t y = 0; y < WORLD_HEIGHT; y++) {

			if(tile(x, y) == Blocks::Type::NONE && tilebg(x, y) == Blocks::Type::NONE) {
				continue;
			}

			// get color
			sf::Color color;

			bool bg = false;

			Blocks::Type t = tile(x, y);
			Blocks::Type tb = tilebg(x, y);

			// bg tiles
			bg = t == Blocks::Type::NONE;

			// get color
			if(!bg) color = Blocks::getBlockColor(t);
			else color = Blocks::getBlockColor(tb);

			// darken accordingly
			if(bg) {
				short r = color.r, g = color.g, b = color.b;
				r -= BG_WALLS_COLOR; g -= BG_WALLS_COLOR; b -= BG_WALLS_COLOR;
				if(r < 0) {r = 0;} if(g < 0) {g = 0;} if(b < 0) {b = 0;}

				color = sf::Color(r, g, b);
			}

			// set color
			m_minimap_img.setPixel(x, y, color);
		}
	}

	m_minimap_tex.loadFromImage(m_minimap_img);
	m_minimap_sprite_clear.setTexture(m_minimap_tex);

	m_minimap.create(WORLD_WIDTH, WORLD_HEIGHT);
	m_minimap.clear(sf::Color(BACKGROUND_COLOR[0], BACKGROUND_COLOR[1], BACKGROUND_COLOR[2]));

	m_minimap_fog.create(WORLD_WIDTH/MAP_VISITED_CHUNKS, WORLD_HEIGHT/MAP_VISITED_CHUNKS);
	m_minimap_fog_sprite.setTexture(m_minimap_fog.getTexture());
	m_minimap_fog_sprite.setScale(MAP_VISITED_CHUNKS, MAP_VISITED_CHUNKS);


	// weird flipping bug, fixed with inverted view
	m_minimap.setView(sf::View(sf::FloatRect(0, WORLD_HEIGHT, WORLD_WIDTH, -(int)WORLD_HEIGHT)));

	m_minimap_sprite.setTexture(m_minimap.getTexture());
	m_minimap_sprite.setScale(MINIMAP_SCALE, MINIMAP_SCALE);
}

void World::addToMinimap(std::pair<sf::Vector2f, MapObj> obj) {
	m_mapobjs.push_back(obj);
}

float World::getMinimapScale(float width) {
	return (m_window.getSize().x - MINIMAP_FULLSCREEN_BORDER*2) / width;
}
