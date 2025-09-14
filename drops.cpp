#include "drops.hpp"
#include "world.hpp"

Drops::Drops(Inventory& inventory) : m_inventory(inventory) {

	m_sprite.setOrigin(8, 8);
}

Drops::~Drops() {

}

void Drops::update(sf::Time dt) {
	float dtf = dt.asSeconds(); // no, its not what it looks like

	for(size_t i = 0; i < m_drops.size(); i++) {
		
		// increment timer
		m_drops[i].timer += dtf; 
	}
}

void Drops::draw(sf::RenderWindow& window, World* world) {
	
	Assets& m_assets = world->getAssets();

	// draw all the drops
	Items::Type type_old = Items::Type::NONE;
	Items::Category cat_old = Items::Category::NONE;

	for(size_t i = 0; i < m_drops.size(); i++) {
		
		// update texture rect if needed
		if(m_drops[i].item.item != type_old) {
			auto v = m_assets.getAnimation(Items::getItemData(m_drops[i].item.item).anim);
			m_sprite.setTextureRect(v[0]);
			type_old = m_drops[i].item.item;
		}

		// update texture if needed
		if(Items::getItemData(m_drops[i].item.item).category != cat_old) {
			if(Items::getItemData(m_drops[i].item.item).category == Items::Category::ITEM) 
				m_sprite.setTexture(m_assets.getTexture(Assets::texture_items));
			else m_sprite.setTexture(m_assets.getTexture(Assets::texture_blocks));

			cat_old = Items::getItemData(m_drops[i].item.item).category;
		}

		// position and draw
		m_sprite.setPosition(m_drops[i].pos + sf::Vector2f(0, sin(m_drops[i].timer*DROP_ANIM_SPEED)*DROP_MOVEMENT/2));

		// check if inside window
		sf::FloatRect view = sf::FloatRect(window.getView().getCenter()-window.getView().getSize()/2.f, window.getView().getSize());
		
		if(m_sprite.getGlobalBounds().intersects(view)) {
			window.draw(m_sprite);
		}
	}
}

void Drops::addDrop(sf::Vector2f pos, islot item) {
	m_drops.push_back({pos, item});
}

std::vector<Drops::Drop>& Drops::getDrops() {
	return m_drops;
}