#include "enemies.hpp"
#include "player.hpp"
	
#define PARAMS m_assets, m_world, m_drops, m_particles, m_projectiles, pos
#define TPARAMS assets, world, drops, particles, projectiles, pos
#define CPARAMS assets, world, drops, particles, projectiles, pos, hp, def, atk, off, type


Enemy::Enemy(ENEMY_PARAMS, float hp, float def, float atk, sf::FloatRect off, EnemyType type) :
	m_assets(assets), m_world(world), m_drops(drops), m_particles(particles), m_projectiles(projectiles), m_health(hp), m_defence(def), m_damage(atk), m_offset(off), m_type(type) {
	m_sprite.setTexture(m_assets.getTexture(Assets::texture_enemies));
	m_sprite.setPosition(pos);
	m_position = pos;
	m_maxhealth = hp;
}

Enemy::~Enemy() {
}

void Enemy::draw(sf::RenderWindow& window) {
	window.draw(m_sprite);
}

void Enemy::updateBase(float dt, sf::Vector2f oldpos, float vel_off, unsigned char quant, std::function<void()> hor_col, std::function<void()> landed) {
	if(m_invuln > 0) { 
		m_invuln -= dt;
		if(m_invuln <= 0) {
			m_invuln = 0;
			m_hitable = true;
		}
	}


	m_bounds = m_sprite.getGlobalBounds();
	m_bounds.left += m_offset.left; m_bounds.width += m_offset.width;
	m_bounds.top += m_offset.top; m_bounds.height += m_offset.height;

	// physics copied from player.cpp with slight edits, check there for readable version
	
	if(m_solid) {

		// find surrounding tiles
		sf::Vector2i tilepos = sf::Vector2i(m_position.x / TILE_SIZE, m_position.y / TILE_SIZE);
		std::vector<std::pair<sf::FloatRect, Blocks::BlockData>> tiles;
		for (int x = tilepos.x-quant; x <= tilepos.x+quant; x++) {
			if(x < 0 || x >= (int)WORLD_WIDTH) continue;
			for (int y = tilepos.y-quant; y <= tilepos.y+quant; y++) {
				if(y < 0 || y >= (int)WORLD_HEIGHT) continue;
				tiles.push_back(std::make_pair(sf::FloatRect(x*TILE_SIZE, y*TILE_SIZE, TILE_SIZE, TILE_SIZE), Blocks::getBlockData(m_world.tile(x, y))));
			}
		}

		// check for collisions
		for (size_t i = 0; i < tiles.size(); i++)
		{
			if(tiles[i].second.solid && m_bounds.intersects(tiles[i].first)) {
				bool within_x = m_position.x+m_bounds.width/2 > tiles[i].first.left && m_position.x-m_bounds.width/2 < tiles[i].first.left+TILE_SIZE;
				if(m_position.y+m_bounds.height/2 > tiles[i].first.top && within_x && oldpos.y+m_bounds.height/2 <= tiles[i].first.top) { 
					m_position.y = tiles[i].first.top - m_bounds.height/2; landed(); m_velocity.y = -m_velocity.y*vel_off;
				} bool within_y = m_position.y+m_bounds.height/2 > tiles[i].first.top && m_position.y-m_bounds.height/2 < tiles[i].first.top+TILE_SIZE;
				if(m_position.x+m_bounds.width/2 > tiles[i].first.left && within_y && oldpos.x+m_bounds.width/2 <= tiles[i].first.left) { 
					m_position.x = tiles[i].first.left - m_bounds.width/2; m_velocity.x = -m_velocity.x*vel_off; hor_col();
				} if(m_position.x-m_bounds.width/2 < tiles[i].first.left+TILE_SIZE && within_y && oldpos.x-m_bounds.width/2 >= tiles[i].first.left+TILE_SIZE) {
					m_position.x = tiles[i].first.left + TILE_SIZE + m_bounds.width/2; m_velocity.x = -m_velocity.x*vel_off; hor_col();
				} within_x = m_position.x+m_bounds.width/2 > tiles[i].first.left && m_position.x-m_bounds.width/2 < tiles[i].first.left+TILE_SIZE;
				if(m_position.y-m_bounds.height/2 < tiles[i].first.top+TILE_SIZE && within_x &&  oldpos.y-m_bounds.height/2 >= tiles[i].first.top+TILE_SIZE) { 
					m_position.y = tiles[i].first.top +TILE_SIZE + m_bounds.height/2; m_velocity.y = -m_velocity.y*vel_off;
				}
			}
		}
	}


	m_sprite.setPosition(m_position);
}

bool Enemy::damage(float dmg, sf::Vector2f dir, bool projectile) {
	if(m_hitable) {
		float damage = dmg - m_defence;
		if(damage < 1) damage = 1;
		m_health -= damage;
		if (m_health <= 0) {
			died();
		}

		if(!projectile) {
			m_hitable = false;
			m_invuln = INVIS_DURATION;
		}

		sf::Vector2f kn = dir*(float)KNOCKBACK * (float)1/m_knockback_resist;

		if(projectile) {
			kn *= 0.33f;
		}

		m_world.getAudio().play(Audio::Sound::HIT);

		m_velocity += kn;
		return true;
	} else {
		return false;
	}
}

float& Enemy::attack() {
	return m_damage;
}

float& Enemy::defence() {
	return m_defence;
}

float& Enemy::health() {
	return m_health;
}

EnemyType Enemy::type() {
	return m_type;
}

bool Enemy::collided(sf::Sprite& sprite) {
	return Collision::pixelPerfectTest(m_sprite, sprite);
}

bool Enemy::right() {
	return m_velocity.x > 0;
}

sf::FloatRect Enemy::bounds() {
	return m_bounds;
}

sf::Vector2f& Enemy::position() {
	return m_position;
}

sf::Vector2f& Enemy::velocity() {
	return m_velocity;
}

float& Enemy::maxHealth() {
	return m_maxhealth;
}


bool Enemy::isBoss() {
	return m_boss;
}

// boss class
Boss::Boss(ENEMY_PARAMS, float hp, float defence, float damage, sf::FloatRect off, EnemyType type) : Enemy(TPARAMS, hp, defence, damage, off, type) {
	m_boss = true;
	m_sprite.setTexture(m_assets.getTexture(Assets::texture_bosses));
	m_world.getAudio().play(m_theme);
}

Boss::~Boss() {
	m_world.getAudio().stop();
}


Enemies::Enemies(Assets& assets, Settings& settings, World& world, Drops& drops, Particles& particle, Projectiles& projectiles, sf::Vector2u win) : m_assets(assets), m_settings(settings), m_world(world), m_drops(drops), m_particles(particle), m_projectiles(projectiles) {
	m_spawndistance = sf::Vector2f((win/2u)+sf::Vector2u(5*TILE_SIZE, 3*TILE_SIZE)) * (win.x / 1920.f);

	m_hpbarbg.setSize(sf::Vector2f(ENEMY_HPBAR_WIDTH+4, ENEMY_HPBAR_HEIGHT+4));
	m_hpbarbg.setFillColor(sf::Color(0, 0, 0, 200));
	m_hpbarbg.setOrigin(m_hpbarbg.getSize()/2.f);

	m_hpbar.setSize(sf::Vector2f(ENEMY_HPBAR_WIDTH, ENEMY_HPBAR_HEIGHT));
	m_hpbar.setFillColor(sf::Color(255, 0, 0, 200));
	m_hpbar.setOrigin(m_hpbar.getSize()/2.f);
}


Enemies::~Enemies() {
}

bool Enemies::shoudCredits() {
	bool cred = m_endgametimer == 3;
	if(cred) reset();
	return cred;
}

void Enemies::update(sf::Time deltatime, Player* player, sf::RenderWindow& win) {

	float dt = deltatime.asSeconds();
	m_spawntimer += dt;
	bool underground = player->getPos().y > (WORLD_HEIGHT/(AIR_FRACTION/1.25f))*TILE_SIZE;
	bool underworld = player->getPos().y > (UNDERWORLD_DISTANCE)*TILE_SIZE;
	bool hardmode = m_world.hardMode();
	
	Biome b = m_world.getBiome(player->getPos().x/TILE_SIZE);
	bool desert = b == Biome::DESERT;
	bool tundra = b == Biome::SNOW;
	bool castle = b == Biome::CASTLE;
	
	// base spawn rates
	float dur = ENEMY_SPAWN_TIME_DAY;
	float cap = ENEMY_SPAWN_MAX_DAY;

	// increased spawn rates
	if(!underworld && (m_world.night() || underground || desert || tundra)) {
		dur = ENEMY_SPAWN_TIME_NIGHT;
		cap = ENEMY_SPAWN_MAX_NIGHT;
	} else if(castle || underworld) { // max spawn rates
		dur = ENEMY_SPAWN_TIME_DANGER;
		cap = ENEMY_SPAWN_MAX_NIGHT;
	}

	if(hardmode) {
		dur /= (HARD_MODE_INC*0.875);
		cap += 4;
	}

	if(m_world.getplaytime() < 60*8) {
		dur *= 2;
	}

	if(m_finalboss) {

		bool alive = false;
		for(unsigned int i = 0; i < m_enemies.size(); i++) {
			if(m_enemies[i]->isBoss()) {
				alive = true;
			}
		}

		if(!alive) {
			m_finalboss = false;
			m_endgame = true;
		}
	}

	if(m_endgame && m_endgametimer < 3) {
		m_endgametimer += dt;
		if(m_endgametimer > 3) {
			m_endgametimer = 3;
			m_endgame = false;
		}
	}

	// add enemies
	if(m_spawntimer >= dur) {
		
		m_spawntimer = 0;

		for(unsigned int i = 0; i < m_enemies.size(); i++) {
			if(m_enemies[i]->isBoss()) {
				return;
			}
		}

		if(m_world.getAudio().playing() == Audio::Song::BATTLE) {
			m_world.getAudio().stop();
		}
		
		bool enemyspawning = m_settings.getFlag(Settings::ENEMYSPAWNING);
		if(!enemyspawning && !m_settings.getFlag(Settings::DEBUG)) 
			enemyspawning = true;

		int chance = ENEMY_SPAWN_CHANCE-1;
		if(dur == ENEMY_SPAWN_TIME_DANGER) {
			chance = 1;
		}

		if(Random::get(0, chance) == 0 && m_enemies.size() < cap && enemyspawning) {

			// determine pos
			sf::Vector2f pos = win.getView().getCenter();
			bool left = Random::get(0, 1) == 0;
			
			// find valid y pos
			int x = pos.x/TILE_SIZE;
			int y = pos.y/TILE_SIZE;
			unsigned int count = 0;
			int mod_count = 0;
			bool top = (Random::get(0, 3) == 0) && underground;

			if(top) {
				y -= m_spawndistance.y/(TILE_SIZE);
				count = 5;
				while(Blocks::getBlockData(m_world.tile(x, y)).solid || Blocks::getBlockData(m_world.tile(x, y+1)).solid || Blocks::getBlockData(m_world.tile(x-1, y)).solid) {
					count++; x += ((count)%2 == 0 ? 1 : -1)*count;

					if(count > m_spawndistance.x/TILE_SIZE) {
						count = 0;
						x = pos.x/TILE_SIZE;
						y = pos.y/TILE_SIZE;
						top = false;
						break;
					}
				}
			}

			if(!top) {
				pos.x += (float)m_spawndistance.x * (left ? -1 : 1);
				x = pos.x/TILE_SIZE;
				while(Blocks::getBlockData(m_world.tile(x, y)).solid || Blocks::getBlockData(m_world.tile(x, y-1)).solid || Blocks::getBlockData(m_world.tile(x-1, y)).solid) {
					mod_count++;

					if(mod_count > 12+ abs(x-pos.x/TILE_SIZE)*2) {
						if(left) x--;
						else x++;
						mod_count = 0;
						count = 0;
					}

					count++; y += ((count)%2 == 0 ? 1 : -1)*count;
				}
			}

			pos = sf::Vector2f(x*TILE_SIZE, y*TILE_SIZE);
			
			Enemy* e = nullptr;

			// add enemy
			if(underworld) {
				int r = Random::get(0, 3);

				if(r == 0) e = spawn(EnemyType::FIRESPIRIT, pos);
				else if(r == 1) e = spawn(EnemyType::FLAMESKELETON, pos);
				else e = spawn(EnemyType::BLOODSERPENT, pos);
			} else {
				if(castle) {

					int r = Random::get(0, 3);

					if(r == 0) e = spawn(EnemyType::FLAMESKELETON, pos);
					else if(r == 1) e = spawn(EnemyType::TREESPIRIT, pos);
					else {
						e = spawn(EnemyType::BAT, pos);
						e->maxHealth() *= 1.35f;
						e->health() *= 1.35f;
						e->attack() *= 1.35f;
						e->defence() += 1;
						e = spawn(EnemyType::SKELETON, pos);
					}

				} else if(underground) {
					if(desert) {
						int r = Random::get(0, 3);

						if(r == 0) e = spawn(EnemyType::SKELETON, pos);
						else if(r == 1) e = spawn(EnemyType::BAT, pos);
						else e = spawn(EnemyType::DUSTDEVIL, pos);

					} else if(tundra) {

						if(Random::get(0, 3) == 0) e = spawn(EnemyType::SKELETON, pos);
						else e = spawn(EnemyType::SNOWFLAKE, pos);

					} else {

						int r = Random::get(0, 9);

						if(r < 4) e = spawn(EnemyType::SKELETON, pos);
						else if(r < 9) e = spawn(EnemyType::BAT, pos);
						else e = spawn(EnemyType::TREESPIRIT, pos);

					}
				} else {
					if(desert) {

						if(Random::get(0, 2) == 0) e = spawn(EnemyType::SKELETON, pos);
						else e = spawn(EnemyType::DUSTDEVIL, pos);

					} else if(tundra) {

						if(Random::get(0, 2) == 0) e = spawn(EnemyType::SKELETON, pos);
						else e = spawn(EnemyType::SNOWFLAKE, pos);

					} else {
						if(m_world.night()) {

							int r = Random::get(0, 6);

							if(r < 3) e = spawn(EnemyType::BAT, pos);
							else if(r < 6) e = spawn(EnemyType::ZOMBIE, pos);
							else e = spawn(EnemyType::TREESPIRIT, pos);

						} else {

							if(Random::get(0, 3) == 0) e = spawn(EnemyType::MUSHROOM, pos);
							else e = spawn(EnemyType::CRAWLER, pos);
						}
					}
				}
			}

			if(e != nullptr && hardmode) {
				e->maxHealth() *= HARD_MODE_INC;
				e->health() *= HARD_MODE_INC;
				e->attack() *= HARD_MODE_INC;
				e->defence() += 1;
			}
		}
	}
	

	for (size_t i = 0; i < m_enemies.size(); i++) {

		m_enemies[i]->update(dt, player, *this);

		// check for collision with projectiles
		auto& projectiles = m_projectiles.vector();
		for(size_t j = 0; j < projectiles.size(); j++) {
			if(projectiles[j].playerproj) {
				if(m_enemies[i]->collided(projectiles[j].sprite)) {

					sf::Vector2f dir;
					if(projectiles[j].dir.x > 0) {
						dir.x = 1;
					} else {
						dir.x = -1;
					}

					dir.y = -1;

					if(m_enemies[i]->damage(projectiles[j].damage, dir, true)) {
						
						// particle time
						ParticleInit init = PARTICLE_BLOOD;
						init.pos = m_enemies[i]->position();
						init.dir = sf::Vector2f(-dir.x, -0.65f);
						m_particles.create(init);
					}
					projectiles.erase(projectiles.begin() + j);
					j--;
					break;
				}
			}
		}

		// despawn enemies, pythagoras moment
		float x1 = m_enemies[i]->bounds().left;
		float y1 = m_enemies[i]->bounds().top;
		float x2 = player->getPos().x;
		float y2 = player->getPos().y;
		float dist = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
		float cap = m_spawndistance.x*1.85f;

		if(m_enemies[i]->isBoss()) {
			cap *= 2.5f;
		}

		if(dist > cap) {
			delete m_enemies[i];

			if(m_enemies[i]->isBoss()) {
				m_finalboss = false;
				m_endgame = false;
				m_endgametimer = 0;
			}

			// remove from vector
			m_enemies.erase(m_enemies.begin() + i); i--;
			continue;
		}


		// remove dead enemies
		if (m_enemies[i]->health() <= 0) {
			delete m_enemies[i];

			// remove from vector
			m_enemies.erase(m_enemies.begin() + i); i--;
		}
	}
}

Enemy* Enemies::spawn(EnemyType enemy, sf::Vector2f pos) {
	Enemy* e = nullptr;
	switch (enemy) {
		case EnemyType::ZOMBIE: e = new Zombie(PARAMS); break;
		case EnemyType::CRAWLER: e = new Crawler(PARAMS); break;
		case EnemyType::SKELETON: e = new Skeleton(PARAMS); break;	
		case EnemyType::BAT: e = new Bat(PARAMS); break;
		case EnemyType::DUSTDEVIL: e = new DustDevil(PARAMS); break;
		case EnemyType::SNOWFLAKE: e = new Snowflake(PARAMS); break;
		case EnemyType::MUSHROOM: e = new Mushroom(PARAMS); break;
		case EnemyType::TREESPIRIT: e = new TreeSpirit(PARAMS); break;
		case EnemyType::TREEGOLEM: e = new TreeGolem(PARAMS); break;
		case EnemyType::FLAMESKELETON: e = new FlameSkeleton(PARAMS); break;
		case EnemyType::FLAMESKULL: e = new FlameSkull(PARAMS); break;
		case EnemyType::BLOODSERPENT: e = new BloodSerpent(PARAMS); break;
		case EnemyType::FIRESPIRIT: e = new FireSpirit(PARAMS); break;
		case EnemyType::DEMONICVISIONARY: e = new DemonicVisionary(PARAMS); m_finalboss = true; break;
		case EnemyType::EYE: e = new Eye(PARAMS); break;
	}

	m_enemies.push_back(e);
	return e;
}

void Enemies::draw(sf::RenderWindow& window) {

	sf::FloatRect bounds(window.getView().getCenter() - window.getView().getSize() / 2.f, window.getView().getSize());

	for (auto& enemy : m_enemies) {
		if(enemy->bounds().intersects(bounds)) {
			enemy->draw(window);

			// draw healthbar
			sf::Vector2f pos = sf::Vector2f(enemy->position().x, enemy->bounds().top);
			pos.y -= TILE_SIZE*0.6f;
			m_hpbarbg.setPosition(pos);
			m_hpbar.setPosition(pos);
			m_hpbar.setScale(enemy->health() / enemy->maxHealth(), 1);
			
			sf::Color c = m_hpbarbg.getFillColor();
			m_hpbarbg.setFillColor(c);
			c.r = 255;
			m_hpbar.setFillColor(c);

			window.draw(m_hpbarbg);
			window.draw(m_hpbar);
		}
	}
}

const std::vector<Enemy*>& Enemies::vector() {
	return m_enemies;
}

void Enemies::reset() {
	for (auto& enemy : m_enemies) {
		delete enemy;
	}
	m_endgametimer = 0;
	m_endgame = false;
	m_finalboss = false;
	m_enemies.clear();

	if(m_world.getAudio().playing() == Audio::Song::BATTLE) {
		m_world.getAudio().stop();
	}
}

sf::Vector2f Enemies::spawnDistance() {
	return m_spawndistance;
}

// no more boilerplate code, enemy implementations
Zombie::Zombie(ENEMY_PARAMS, float hp, float def, float atk, sf::FloatRect off, EnemyType type) : Enemy(CPARAMS) {
	m_sprite.setOrigin(8, 16);
	m_sprite.setScale(2, 2);
}

Zombie::~Zombie() {

}

void Zombie::update(float dt, Player* player, [[maybe_unused]] Enemies& enemies) {
	
	sf::Vector2f oldpos = m_position;

	// move towards player
	if(player->getPos().x > m_position.x)
		m_velocity.x += m_speed * dt;
	else m_velocity.x -= m_speed * dt;
	m_velocity.y += GRAVITY * dt;

	// update rect
	m_velocity -= m_velocity * static_cast<float>(m_decel) * dt;
	m_position += m_velocity * dt;

	m_sprite.setPosition(m_position);

	bool hor_collision = false;

	// update base with lambdas
	updateBase(dt, oldpos, 0,  m_quant, [this, &hor_collision]() {hor_collision = true;}, [this](){m_jumping = false;});
	
	bool should_jump = false;
	if(hor_collision)
		should_jump = true;

	// advanced jump
	if(m_advance_jump) {
		if(m_position.y > player->getPos().y)
			should_jump = true;
		else if(Blocks::getBlockData(m_world.tile(m_position.x/TILE_SIZE, (m_bounds.getPosition().y + m_bounds.height+1)/TILE_SIZE)).solid == false && m_velocity.y == 0)
			should_jump = true;
	}

	if(should_jump && !m_jumping) {
		m_velocity.y -= m_jump; m_jumping = true;
	}

	// advanced jump checking
	if(!Blocks::getBlockData(m_world.tile(m_bounds.getPosition().x/TILE_SIZE, (m_bounds.getPosition().y + m_bounds.height+1)/TILE_SIZE)).solid &&
		!Blocks::getBlockData(m_world.tile((m_bounds.getPosition().x+m_bounds.getSize().x)/TILE_SIZE, (m_bounds.getPosition().y + m_bounds.height+1)/TILE_SIZE)).solid)
		m_jumping = true;

	
	m_animtimer += dt;

	// chose animation sequence
	// set animation frame from sequence
	auto& animation = m_assets.getAnimation(m_anim);
	int frame = std::round(m_animtimer/PLAYER_ANIM_SPEED);

	if((size_t)frame >= animation.size()) {
		m_animtimer = 0;
		frame = 0;
	}
	
	m_sprite.setTextureRect(animation[frame]);
	if(m_velocity.x < 0) {
		m_sprite.setScale(-2, 2);
	} else {
		m_sprite.setScale(2, 2);
	}

	m_lambda(dt, player);
}

void Zombie::died() {
	if(Random::get(0, 9) == 0) m_drops.addDrop(m_position, {Items::BRAIN, 1});
}

Bat::Bat(ENEMY_PARAMS, float hp, float def, float atk, sf::FloatRect off, EnemyType type) : Enemy(CPARAMS) {
	m_sprite.setOrigin(8, 8);
	m_sprite.setScale(2, 2);
}

Bat::~Bat() {

}


void Bat::update(float dt, Player* player, [[maybe_unused]] Enemies& enemies) {
	
	sf::Vector2f oldpos = m_position;

	if(m_solid) {
		
		// move towards player, incredibly dogshit implimentation of A* from http://csis.pace.edu/~benjamin/teaching/cs627/webfiles/Astar.pdf
		sf::Vector2u pos = sf::Vector2u(m_position.x/TILE_SIZE, m_position.y/TILE_SIZE);
		if(m_oldpos != pos) {
			m_oldpos = pos;

			m_loc.push_back(pos);

			// find surrounding tiles and score them
			std::vector<std::pair<sf::Vector2u, float>> tiles;
			for(size_t x = pos.x-1; x < pos.x+2; x++) {
				for(size_t y = pos.y-1; y < pos.y+2; y++) {

					// check if curent tile
					if(x == pos.x && y == pos.y)
						continue;

					if(!Blocks::getBlockData(m_world.tile(x, y)).solid) {

						bool use = true;
						for(size_t i = 0; i < m_loc.size(); i++) {
							if(m_loc[i].x == x && m_loc[i].y == y) {
								use = false;
								break;
							}
						}

						if(use) {
							// distance to player
							float dist = std::sqrt(std::pow(player->getPos().x - x*TILE_SIZE, 2) + std::pow(player->getPos().y - y*TILE_SIZE, 2));
							tiles.push_back(std::make_pair(sf::Vector2u(x, y), dist));
						}
					}
				}
			}

			sf::Vector2u old = m_bestpos;
			float best = INFINITY;
			for(size_t i = 0; i < tiles.size(); i++) {
				if(tiles[i].second < best) {
					best = tiles[i].second;
					m_bestpos = tiles[i].first;
				}
			}

			if(old != m_bestpos) {
				m_loc.clear();
				m_loc.push_back(pos);
			}
		}


		// move towards best tile
		if(m_bestpos.x*TILE_SIZE > m_position.x)
			m_velocity.x += m_speed * dt;
		else m_velocity.x -= m_speed * dt;
		if(m_bestpos.y*TILE_SIZE > m_position.y)
			m_velocity.y += m_speed * dt;
		else m_velocity.y -= m_speed * dt;

	} else {
		// move towards player
		if(player->getPos().x > m_position.x)
			m_velocity.x += m_speed * dt;
		else m_velocity.x -= m_speed * dt;
		if(player->getPos().y > m_position.y)
			m_velocity.y += m_speed * dt;
		else m_velocity.y -= m_speed * dt;
	}



	// update rect
	m_velocity -= m_velocity * static_cast<float>(m_decel) * dt;
	m_position += m_velocity * dt;
	m_sprite.setPosition(m_position);

	// update base
	updateBase(dt, oldpos, 0.75f, m_quant);


	m_animtimer += dt;

	// chose animation sequence
	// set animation frame from sequence
	auto& animation = m_assets.getAnimation(m_anim);
	int frame = std::round(m_animtimer/(PLAYER_ANIM_SPEED*4));

	if((size_t)frame >= animation.size()) {
		m_animtimer = 0;
		frame = 0;
	}
	
	m_sprite.setTextureRect(animation[frame]);
	if(m_velocity.x < 0) {
		m_sprite.setScale(-2, 2);
	} else {
		m_sprite.setScale(2, 2);
	}

	m_lambda(dt, player);
}

void Bat::died() {
	if(Random::get(0, 4) == 0) m_drops.addDrop(m_position, {Items::BATWING, (unsigned int)Random::get(1, 2)});
}

// ultra basic class derived from zombie, just weaker
Crawler::Crawler(ENEMY_PARAMS, float hp, float def, float atk, sf::FloatRect off, EnemyType type) : Zombie(CPARAMS) {

	m_sprite.setOrigin(8, 8);

	m_anim = Assets::anim_crawler;

	m_speed = CRAWLER_ACCEL;
	m_decel = CRAWLER_DECEL;
	m_jump = CRAWLER_JUMP;
	m_advance_jump = false;
}

Crawler::~Crawler() {
	
}

void Crawler::died() {
	if(Random::get(0, 6) == 0) m_drops.addDrop(m_position, {Items::CRAWLERLEG, 1});
}

Skeleton::Skeleton(ENEMY_PARAMS, float hp, float def, float atk, sf::FloatRect off, EnemyType type) : Zombie(CPARAMS) {

	m_anim = Assets::anim_skeleton;

	m_speed = SKELETON_ACCEL;
	m_decel = SKELETON_DECEL;
	m_jump = SKELETON_JUMP;
}

Skeleton::~Skeleton() {
	
}

void Skeleton::died() {
	if(Random::get(0, 3) == 0) m_drops.addDrop(m_position, {Items::BONE, (unsigned int)Random::get(1, 3)});
}

DustDevil::DustDevil(ENEMY_PARAMS, float hp, float def, float atk, sf::FloatRect off, EnemyType type) : Zombie(CPARAMS) {

	m_anim = Assets::anim_dustdevil;

	m_speed = DUSTDEVIL_ACCEL;
	m_decel = DUSTDEVIL_DECEL;
	m_jump = DUSTDEVIL_JUMP;

	m_lambda = [this](float dt, Player* player) {
		m_part_timer += dt;
		m_proj_timer += dt;
	
		if(m_part_timer > 0.3) {
			m_part_timer = 0;

			// particles
			ParticleInit init = PARTICLE_PUFF;
			init.pos = m_position;
			init.color = sf::Color(254, 231, 97, 255);
			init.life = 1750;
			init.size = 1;
			init.count = 6;
			init.count_var = 1;

			m_particles.create(init);
		}

		if(m_proj_timer > 1.95f) {
			m_proj_timer = 0;

			// projectile
			Projectile proj = PROJECTILE_ARROW;
			proj.pos = m_position;
			proj.playerproj = false;
			proj.anim = Assets::animation_effect_puff;
			proj.color = sf::Color(224, 201, 77, 255);
			proj.size = 2;
			proj.speed = 600;
			sf::Vector2f dir = player->getPos() - m_position;

			// normalize
			dir = dir / std::sqrt(dir.x*dir.x + dir.y*dir.y);
			proj.dir = dir;
			proj.damage = attack()/1.75f;
			proj.gravity = 275;

			m_projectiles.add(proj);
		}
	};
}

DustDevil::~DustDevil() {
	
}

void DustDevil::died() {
	m_drops.addDrop(m_position, {Items::SAND, (unsigned int)Random::get(6, 12)});
}

Snowflake::Snowflake(ENEMY_PARAMS, float hp, float def, float atk, sf::FloatRect off, EnemyType type) : Bat(CPARAMS) {

	m_anim = Assets::anim_snowflake;

	m_speed = SNOWFLAKE_ACCEL;
	m_decel = SNOWFLAKE_DECEL;

	m_lambda = [this](float dt, Player* player) {

		m_part_timer += dt;
		m_proj_timer += dt;

		if(m_part_timer > 0.2) {
			m_part_timer = 0;

			// particles
			ParticleInit init = PARTICLE_PUFF;
			init.pos = m_position;
			init.color = sf::Color(173, 250, 255, 255);
			init.life = 900;
			init.size = 1;
			init.speed = 75;
			init.gravity = 750;
			init.count = 3;
			init.count_var = 1;

			m_particles.create(init);
		}

		if(m_proj_timer > 2.15f) {
			m_proj_timer = 0;

			// projectile
			Projectile proj = PROJECTILE_ARROW;
			proj.pos = m_position;
			proj.playerproj = false;
			proj.anim = Assets::animation_effect_tri;
			proj.color = sf::Color(173, 250, 255, 255);
			proj.speed = 600;
			proj.size = 2;
			sf::Vector2f dir = player->getPos() - m_position;

			// normalize
			dir = dir / std::sqrt(dir.x*dir.x + dir.y*dir.y);
			proj.dir = dir;
			proj.damage = attack()/1.65f;
			proj.gravity = 275;

			m_projectiles.add(proj);
		}
	};
}

Snowflake::~Snowflake() {
	
}

void Snowflake::died() {
	m_drops.addDrop(m_position, {Items::ICE, (unsigned int)Random::get(6, 12)});
}

Mushroom::Mushroom(ENEMY_PARAMS, float hp, float def, float atk, sf::FloatRect off, EnemyType type) : Zombie(CPARAMS) {

	m_sprite.setOrigin(8, 8);

	m_anim = Assets::anim_mushroom;

	m_speed = MUSHROOM_ACCEL;
	m_decel = MUSHROOM_DECEL;
	m_jump = MUSHROOM_JUMP;
}

Mushroom::~Mushroom() {
	
}

void Mushroom::died() {
	if(Random::get(0, 2) == 0) m_drops.addDrop(m_position, {Items::MUSHROOM, 1});
}

TreeSpirit::TreeSpirit(ENEMY_PARAMS, float hp, float def, float atk, sf::FloatRect off, EnemyType type) : Bat(CPARAMS) {

	m_anim = Assets::anim_treespirit;

	m_speed = SNOWFLAKE_ACCEL;
	m_decel = SNOWFLAKE_DECEL;

	m_solid = false;

	m_lambda = [this](float dt, Player* player) {

		(void)player;
		m_part_timer += dt;

		if(m_part_timer > 0.3) {
			m_part_timer = 0;

			// particles
			ParticleInit init = PARTICLE_PUFF;
			init.pos = m_position;
			init.color = sf::Color(99, 199, 77, 255);
			init.life = 900;
			init.size = 1;
			init.speed = 75;
			init.gravity = 750;
			init.count = 3;
			init.count_var = 1;

			m_particles.create(init);
		}
	};
}

TreeSpirit::~TreeSpirit() {
	
}

void TreeSpirit::died() {
	
	// more particles
	ParticleInit init = PARTICLE_PUFF;
	init.pos = m_position;
	init.color = sf::Color(99, 199, 77, 255);
	init.life = 1500;
	init.size = 1;
	init.speed = 95;
	init.count = 20;
	init.count_var = 3;

	m_particles.create(init);
}

FlameSkeleton::FlameSkeleton(ENEMY_PARAMS, float hp, float def, float atk, sf::FloatRect off, EnemyType type) : Skeleton(CPARAMS) {

	m_anim = Assets::anim_flameskeleton;

	m_lambda = [this](float dt, Player* player) {

		m_part_timer += dt;
		m_proj_timer += dt;

		if(m_part_timer > 0.2) {
			m_part_timer = 0;

			// particles
			ParticleInit init = PARTICLE_PUFF;
			init.pos = m_position;
			init.color = sf::Color(255, 137, 17, 255);
			init.life = 900;
			init.size = 0.6;
			init.speed = 75;
			init.gravity = -750;
			init.count = 2;
			init.count_var = 1;

			m_particles.create(init);
		}

		if(m_proj_timer > 1.95f) {
			m_proj_timer = 0;

			// projectile
			Projectile proj = PROJECTILE_ARROW;
			proj.pos = m_position;
			proj.playerproj = false;
			proj.anim = Assets::animation_effect_fireball;
			proj.color = sf::Color(255, 137, 17, 255);
			proj.speed = 600;
			proj.size = 1.25;
			sf::Vector2f dir = player->getPos() - m_position;

			// normalize
			dir = dir / std::sqrt(dir.x*dir.x + dir.y*dir.y);
			proj.dir = dir;
			proj.damage = attack()/1.25f;
			proj.gravity = 275;

			m_projectiles.add(proj);
		}
	};
}

FlameSkeleton::~FlameSkeleton() {
	
}

void FlameSkeleton::died() {
	if(Random::get(0, 4) == 0)
		m_drops.addDrop(m_position, {Items::FCORE, 1});
	else if(Random::get(0, 2) == 0)
		m_drops.addDrop(m_position, {Items::BONE, (unsigned int)Random::get(1, 2)});
}

BloodSerpent::BloodSerpent(ENEMY_PARAMS, float hp, float def, float atk, sf::FloatRect off, EnemyType type) : Bat(CPARAMS) {
	
	m_anim = Assets::anim_bloodserpent;

	m_sprite.setOrigin(16, 8);

	m_speed = BLOODSERPENT_ACCEL;
	m_decel = BLOODSERPENT_DECEL;

	m_knockback_resist = 8.5f;

	m_quant = 2;

	m_lambda = [this](float dt, Player* player) {

		// rotate towards player
		if(player->getPos().x > m_position.x)
			m_sprite.setScale(2, 2);
		else
			m_sprite.setScale(-2, 2);

		m_part_timer += dt;

		if(m_part_timer > 0.3) {
			m_part_timer = 0;

			// particles
			ParticleInit init = PARTICLE_PUFF;
			init.pos = m_position;
			init.color = sf::Color(255, 0, 0, 255);
			init.life = 900;
			init.size = 1;
			init.speed = 75;
			init.gravity = 750;
			init.count = 3;
			init.count_var = 1;

			m_particles.create(init);
		}
	};
}

BloodSerpent::~BloodSerpent() {
	
}

void BloodSerpent::died() {
	if(Random::getb())
		m_drops.addDrop(m_position, {Items::CSCALE, (unsigned int)Random::get(1, 2)});

}

FireSpirit::FireSpirit(ENEMY_PARAMS, float hp, float def, float atk, sf::FloatRect off, EnemyType type) : TreeSpirit(CPARAMS) {
	
	m_anim = Assets::anim_firespirit;

	m_old_speed = m_speed;

	m_lambda = [this](float dt, Player* player) {

		(void)player;
		m_part_timer += dt;
		m_dash_timer += (m_dashing ? -dt : dt);

		// shit spin "animation" if dashing
		if(m_dashing) {
			m_sprite.setScale(2, 2);
			m_sprite.rotate(420 * dt);
		} else {
			m_sprite.setRotation(0);
		}

		if(m_part_timer > 0.3) {
			m_part_timer = 0;

			// particles
			ParticleInit init = PARTICLE_PUFF;
			init.pos = m_position;
			init.color = sf::Color(255, 137, 17, 255);
			init.life = 900;
			init.size = 1;
			init.speed = 75;
			init.gravity = 750;
			init.count = 3;
			init.count_var = 1;

			m_particles.create(init);
		}
		
		if(m_dash_timer > 2.5f && !m_dashing) {

			// dash
			m_speed = m_old_speed * 2;

			m_dashing = true;
		} else if(m_dash_timer < 0 && m_dashing) {

			// stop dash
			m_speed = m_old_speed;

			m_dashing = false;
		}
	};
}

FireSpirit::~FireSpirit() {
	
}

void FireSpirit::died() {
	m_drops.addDrop(m_position, {Items::HELL, (unsigned int)Random::get(2, 5)});
}


Eye::Eye(ENEMY_PARAMS, float hp, float def, float atk, sf::FloatRect off, EnemyType type) : Enemy(CPARAMS) {
	m_sprite.setOrigin(8, 8);
	m_sprite.setScale(2, 2);
	m_target = pos;

	m_sprite.setTextureRect(m_assets.getAnimation(Assets::anim_eye)[0]);	
}

Eye::~Eye() {

}


void Eye::update(float dt, Player* player, [[maybe_unused]] Enemies& enemies) {
	
	sf::Vector2f oldpos = m_position;

	// move towards target
	if(m_chase) {
		m_target = player->getPos();
	}
	
	if(m_target.x > m_position.x)
		m_velocity.x += m_speed * dt;
	else m_velocity.x -= m_speed * dt;
	if(m_target.y > m_position.y)
		m_velocity.y += m_speed * dt;
	else m_velocity.y -= m_speed * dt;

	// update rect
	m_velocity -= m_velocity * static_cast<float>(m_decel) * dt;
	m_position += m_velocity * dt;
	m_sprite.setPosition(m_position);

	// update base
	updateBase(dt, oldpos, 0.75f, m_quant);

	if(m_velocity.x > 0) m_sprite.setScale(2, 2);
	else if(m_velocity.x < 0) m_sprite.setScale(-2, 2);

	m_animtimer += dt;

	m_lambda(dt, player);
}

void Eye::died() {

}