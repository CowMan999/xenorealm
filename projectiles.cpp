#include "projectiles.hpp"

Projectiles::Projectiles(sf::RenderWindow& window, Assets& assets, World& world) : m_window(window), m_assets(assets), m_world(world) {

}

Projectiles::~Projectiles() {

}

void Projectiles::update(sf::Time dt) {
	for(size_t i = 0; i < m_projectiles.size(); i++) {
		m_projectiles[i].pos += m_projectiles[i].dir * m_projectiles[i].speed * dt.asSeconds();
		m_projectiles[i].dir.y += (m_projectiles[i].gravity/m_projectiles[i].speed) * dt.asSeconds();
		m_projectiles[i].life -= dt.asSeconds() * 1000.f;

		// update sprite
		sf::Vector2f old = m_projectiles[i].sprite.getPosition();
		m_projectiles[i].sprite.setPosition(m_projectiles[i].pos);

		// get movement direction
		sf::Vector2f dir = m_projectiles[i].pos - old;
		float angle = atan2(dir.y, dir.x) * 180.f / 3.14159265f;

		m_projectiles[i].sprite.setRotation(angle);

		bool colliding = m_projectiles[i].solid && Blocks::getBlockData(m_world.tile(m_projectiles[i].pos.x/TILE_SIZE, m_projectiles[i].pos.y/TILE_SIZE)).solid;

		if(m_projectiles[i].life <= 0 || colliding) {
			m_projectiles.erase(m_projectiles.begin() + i);
			i--;
		}
	}
}

void Projectiles::draw() {
	for(size_t i = 0; i < m_projectiles.size(); i++) {

		if(m_projectiles[i].first) {
			m_projectiles[i].first = false;
			continue;
		}

		// get colour
		float r = m_projectiles[i].color.r;
		float g = m_projectiles[i].color.g;
		float b = m_projectiles[i].color.b;

		m_projectiles[i].sprite.setColor(sf::Color(r, g, b, 255));

		m_window.draw(m_projectiles[i].sprite);
	}
}

void Projectiles::add(Projectile p) {
	p.mlife = p.life;
	p.sprite.setTexture(m_assets.getTexture(Assets::texture_effects));
	p.sprite.setColor(p.color);
	p.sprite.setScale(p.size, p.size);
	p.sprite.setTextureRect(m_assets.getAnimation(p.anim)[0]);
	p.sprite.setOrigin(8, 8);
	m_projectiles.push_back(p);
}

std::vector<Projectile>& Projectiles::vector() {
	return m_projectiles;
}

void Projectiles::clear() {
	m_projectiles.clear();
}

const Projectile PROJECTILE_ARROW = {
	true,
	sf::Vector2f(0, 0),
	sf::Vector2f(0, 0),
	sf::Color::White,
	0,
	1.35f,
	850,
	4500,
	Assets::animation_effect_arrow,
	500
};