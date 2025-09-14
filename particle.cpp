#include "particle.hpp"

Particles::Particles(Assets& assets) : m_assets(assets) {
	m_sprite.setOrigin(8, 8);
}

Particles::~Particles() {

}

bool Particles::ready() {
	m_sprite.setTexture(m_assets.getTexture(Assets::texture_effects));
	return true;
}

void Particles::update(sf::Time dt) {
	for(size_t i = 0; i < m_particles.size(); i++) {
		m_particles[i].pos += m_particles[i].vel * dt.asSeconds();
		m_particles[i].vel.y += m_particles[i].gravity * dt.asSeconds();
		m_particles[i].life -= dt.asSeconds() * 1000.f;

		if(m_particles[i].life <= 0) {
			m_particles.erase(m_particles.begin() + i);
			i--;
		}
	}
}

void Particles::draw(sf::RenderWindow& window) {
	sf::FloatRect view(window.getView().getCenter() - window.getView().getSize()/2.f, window.getView().getSize());
	for(size_t i = 0; i < m_particles.size(); i++) {
		m_sprite.setPosition(m_particles[i].pos);
		float a = m_particles[i].color.a;
		a = ((float)m_particles[i].life/(float)m_particles[i].mlife) * 255.f;
		m_sprite.setScale(m_particles[i].size, m_particles[i].size);
		m_sprite.setRotation(m_particles[i].angle);

		if(m_anim != m_particles[i].anim) {
			m_anim = m_particles[i].anim;
			m_sprite.setTextureRect(m_assets.getAnimation(m_anim)[0]);
		}

		if(m_sprite.getGlobalBounds().intersects(view)) {

			// get brightness based on world brightness
			float r = m_particles[i].color.r;
			float g = m_particles[i].color.g;
			float b = m_particles[i].color.b;

			m_sprite.setColor(sf::Color(r, g, b, a));

			window.draw(m_sprite);
		}
	}
}

void Particles::create(ParticleInit info) {
	
	unsigned int count = info.count + Random::get(0, info.count_var) - (float)info.count_var/2.f;
	
	ASSERT(!(info.inwards && info.outwards));

	for(size_t i = 0; i < count; i++) {
		
		Particle p;
		if(info.radius == 0) {
			p.pos = info.pos;
		} else {
			float x = Random::get(0, 100)/1.74532923716; // 100/2pi
			p.pos = info.pos + sf::Vector2f(sin(x), cos(x)) * info.radius * (float)Random::get(5, 100)/100.f;
		}

		p.life = info.life + Random::get(0, info.life_var) - (float)info.life_var/2.f;
		p.mlife = p.life;
		p.size = info.size + (float)Random::get(0, info.size_var*100)/100.f - (float)info.size_var/2.f;
		p.color = info.color;
		p.anim = info.anim;
		p.angle = Random::get(1, 360);
		p.gravity = info.gravity;

		// determine velocity
		if(info.outwards) {
			sf::Vector2f dir = p.pos - info.pos;
			
			// normalize
			dir = (dir / std::sqrt(dir.x*dir.x + dir.y*dir.y));

			// use speed
			p.vel = dir * (info.speed + Random::get(1, info.speed_var+1) - (float)info.speed_var/2.f);
		} else if(info.inwards) {
			sf::Vector2f dir = info.pos - p.pos;
			
			// normalize
			dir = (dir / std::sqrt(dir.x*dir.x + dir.y*dir.y));

			// use speed
			p.vel = dir * (info.speed + Random::get(1, info.speed_var+1) - (float)info.speed_var/2.f);
		} else {
			p.vel = info.dir * (info.speed + Random::get(1, info.speed_var+1) - (float)info.speed_var/2.f);
		}


		m_particles.push_back(p);
	}
}

void Particles::clear() {
	m_particles.clear();
}

const ParticleInit PARTICLE_PUFF = {
	sf::Vector2f(0, 0),
	16,
	4,
	20,
	500,
	125,
	0.7,
	0.2,
	125,
	45,
	0,
	sf::Color(255, 255, 255),
	Assets::animation_effect_puff,
	1,
	0,
	sf::Vector2f(0, 0)
};

const ParticleInit PARTICLE_BLOOD = {
	sf::Vector2f(0, 0),
	6,
	1,
	10,
	700,
	125,
	0.6,
	0.1,
	125,
	45,
	450,
	sf::Color(255, 0, 0),
	Assets::animation_effect_blood,
	0,
	0,
	sf::Vector2f(0, 0)
};