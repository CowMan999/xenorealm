#include "enemies.hpp"
#include "player.hpp"

TreeGolem::TreeGolem(ENEMY_PARAMS, float hp, float def, float atk, sf::FloatRect off, EnemyType type) : Boss(assets, world, drops, particles, projectiles, pos, hp, def, atk, off, type) {
	m_sprite.setTextureRect(m_assets.getAnimation(Assets::anim_treegolem_idle)[0]);
	m_sprite.setOrigin(24, 24);
	m_sprite.setScale(3, 3);

	// take quarter knockback
	m_knockback_resist = 8;
}

TreeGolem::~TreeGolem() {

}

void TreeGolem::update(float dt, Player* player, Enemies& enemies) {
	m_attack_timer += dt;
	sf::Vector2f oldpos = m_position;
	sf::IntRect rect;

	bool jump = false;

	// walk phase
	if(m_attack_use == 0) {
		
		// move towards player
		if(player->getPos().x > m_position.x) m_velocity.x += TREEGOLEM_ACCEL * dt;
		else m_velocity.x -= TREEGOLEM_ACCEL * dt;

		// walk animation
		auto& vector = m_assets.getAnimation(Assets::anim_treegolem_run);
		rect = vector[static_cast<int>(m_attack_timer / TREEGOLEM_ANIMATION_SPEED) % vector.size()];

		if(m_attack_timer > TREEGOLEM_ATTACK_TIME) {
			m_attack_use = 1;
			m_attack_timer = 0;
		}
	}

	// summon phase
	else if(m_attack_use == 1) {
		rect = m_assets.getAnimation(Assets::anim_treegolem_idle)[0];
		m_summon_timer += dt;

		if(m_summon_timer > TREEGOLEM_SUMMON_TIME) {
			m_summon_timer -= TREEGOLEM_SUMMON_TIME;

			// spawn enemies
			if(Random::get(0, 2) == 0) enemies.spawn(EnemyType::MUSHROOM, m_position);
			else enemies.spawn(EnemyType::TREESPIRIT, m_position);
			
		}

		if(m_attack_timer > TREEGOLEM_ATTACK_TIME) {
			m_attack_use = 3;
			m_attack_timer = 0;
		}
	}

	// mjump phase
	else if(m_attack_use == 3) {

		m_megajump_timer += dt;
		m_grounded = false;

		if(m_solid) {
			m_solid = false;
		}

		// move towards player
		if(player->getPos().x > m_position.x) m_velocity.x += TREEGOLEM_ACCEL*1.85f * dt;
		else m_velocity.x -= TREEGOLEM_ACCEL*1.85f * dt;

		if(player->getPos().y-300 < m_position.y) m_velocity.y -= TREEGOLEM_ACCEL*5.15f * dt;

		// anim
		auto& vector = m_assets.getAnimation(Assets::anim_treegolem_shrink);
		float p = m_megajump_timer / TREEGOLEM_MEGAJUMP_TIME;
		if(p > 1) p = 1;
		rect = vector[static_cast<int>(p * vector.size()-1)];

		if(m_megajump_timer > TREEGOLEM_MEGAJUMP_TIME) {
			m_megajump_timer = 0;
			m_attack_use = 4;
			m_attack_timer = 0;
		}
	} else { // phase 4, just fall
		m_megajump_timer += dt;

		if(!m_grounded) {
			if(player->getPos().x > m_position.x) m_velocity.x += TREEGOLEM_ACCEL * dt;
			else m_velocity.x -= TREEGOLEM_ACCEL * dt;
		} else if(!m_smash) {

			// particles
			ParticleInit init = PARTICLE_PUFF;
			init.pos = m_position;
			init.life = 1500;
			init.size = 1;
			init.speed = 95;
			init.count = 20;
			init.count_var = 3;

			m_particles.create(init);

			m_smash = true;
		}

		if(!m_solid) {
			m_solid = true;
		}

		auto& vector = m_assets.getAnimation(Assets::anim_treegolem_shrink);
		rect = vector[vector.size()-1];

		if(m_megajump_timer > TREEGOLEM_MEGAJUMP_TIME/1.65f) {
			m_megajump_timer = 0;
			m_attack_use = 0;
			m_attack_timer = 0;

			jump = true;
			m_smash = false;
		}
	}

	// physics
	if(m_attack_use != 3) m_velocity.y += GRAVITY * dt;
	else if(m_attack_use == 4) m_velocity.y += GRAVITY * dt*1.45f;
	else m_velocity.y += GRAVITY * dt;

	// deceleration
	m_velocity -= m_velocity * static_cast<float>(TREEGOLEM_DECEL) * dt;
	m_position += m_velocity * dt;
	m_sprite.setPosition(m_position);


	// collision
	updateBase(dt, oldpos, 0, 4, [&jump](){
		jump = true;
	}, [this](){
		m_grounded = true;
	});

	// jump
	if(jump && m_grounded && m_attack_use == 0) {
		m_velocity.y = -(float)TREEGOLEM_JUMP;
		m_grounded = false;
	}

	// animation
	if(m_attack_use != 1 && m_attack_use != 4) {
		if(m_velocity.x > 0) m_sprite.setScale(3, 3);
		else if(m_velocity.x < 0) m_sprite.setScale(-3, 3);
	}

	m_sprite.setTextureRect(rect);

}

void TreeGolem::died() {
	ParticleInit init = PARTICLE_BLOOD;
	init.pos = m_position;
	init.life = 1500;
	init.size = 1;
	init.speed = 145;
	init.count = 30;
	init.count_var = 3;
	init.outwards = true;
	init.size = 1;

	m_particles.create(init);

	m_drops.addDrop(m_position-sf::Vector2f(20,0), {Items::TCORE, 2});
	m_drops.addDrop(m_position+sf::Vector2f(20,0), {Items::VINESM, (unsigned int)Random::get(8,11)});
}

FlameSkull::FlameSkull(ENEMY_PARAMS, float hp, float def, float atk, sf::FloatRect off, EnemyType type) : Boss(assets, world, drops, particles, projectiles, pos, hp, def, atk, off, type) {
	m_sprite.setOrigin(24, 24);
	m_sprite.setScale(3, 3);

	// take no knockback
	m_knockback_resist = INFINITY;

	// anim
	m_sprite.setTextureRect(m_assets.getAnimation(Assets::anim_flameskull)[0]);

	// not solid
	m_solid = false;
}

FlameSkull::~FlameSkull() {

}

void FlameSkull::update(float dt, Player* player, Enemies& enemies) {
	m_attack_timer += dt;
	
	// do jack shit attack
	if(m_attack_use == 0) {

		sf::Vector2f destpos = player->getPos();

		if(m_shoot) {
			destpos.y -= 9*TILE_SIZE;
		}

		// point direction
		float angle = atan2(destpos.y - m_position.y, destpos.x - m_position.x);

		// move towards player
		m_velocity.x += cos(angle) * FLAMESKULL_ACCEL * dt * (m_phase2 ? 1.255f : 1);
		m_velocity.y += sin(angle) * FLAMESKULL_ACCEL * dt * (m_phase2 ? 1.255f : 1);

		// point towards player
		m_sprite.setRotation(angle * 180 / 3.14159265358979323846f);
		

		if(m_attack_timer > (m_shoot ? FLAMESKULL_ATTACK_TIME*1.25f : FLAMESKULL_ATTACK_TIME*0.75f)) {
			m_attack_timer = 0;
			m_attack_use = 1;
			m_summon_timer = 0;
		}
	}

	// bayblade burst
	if(m_attack_use == 1) {

		m_summon_timer += dt;

		// point direction
		m_sprite.rotate(FLAMESKULL_BAYBLADE_ROTATE * dt);

		// summon enemies or shoot
		if(m_shoot) {
			if(m_summon_timer > (m_phase2 ? FLAMESKULL_SHOOT_TIME*0.8f : FLAMESKULL_SHOOT_TIME)) {
				m_summon_timer -= (m_phase2 ? FLAMESKULL_SHOOT_TIME*0.8f : FLAMESKULL_SHOOT_TIME);

				Projectile proj = PROJECTILE_ARROW;
				proj.pos = m_position;
				proj.size = 2.f;
				proj.damage = attack()*1.15f;
				proj.anim = Assets::animation_effect_fireball;
				proj.speed = 750;
				proj.color = m_phase2 ? sf::Color::Red : sf::Color(255, 137, 17, 255);

				m_projcount++;

				if(m_projcount == 25) {
					m_projcount = 0;
					sf::Vector2f dir = player->getPos() - m_position;
					proj.dir = dir / sqrtf(dir.x*dir.x + dir.y*dir.y);
					proj.color = m_phase2 ? sf::Color(255, 137, 17, 255) : sf::Color::Red;
					proj.speed = 800;
					proj.gravity = 0;
					proj.size = 3.5f;
					proj.solid = false;
				} else {
					proj.dir = sf::Vector2f((float)Random::get(-100, 100)/100.f, -0.65f);
				}

				proj.playerproj = false;

				m_projectiles.add(proj);
			}
		} else {
			if(m_summon_timer > (m_phase2 ? FLAMESKULL_SUMMON_TIME*0.8f : FLAMESKULL_SUMMON_TIME)) {
				m_summon_timer = 0;

				// spawn enemies
				sf::Vector2f dir = player->getPos() - m_position;

				enemies.spawn((Random::get(0,1) == 1 ? EnemyType::SKELETON : EnemyType::FLAMESKELETON), m_position);
				enemies.vector().back()->velocity() = dir*FLAMESKULL_LAUNCH_STRENGTH;
			}
		}

		if(m_attack_timer > FLAMESKULL_ATTACK_TIME) {
			m_attack_timer = 0;
			m_attack_use = 0;

			m_shoot = !m_shoot;
			
			m_summon_timer = 0;
		}
	}

	// phase change
	if(m_phase2 == false && ((m_health < 325 && !m_world.hardMode()) || (m_health < 600 && m_world.hardMode()))) {
		m_phase2 = true;

		ParticleInit init = PARTICLE_BLOOD;
		init.pos = m_position;
		init.life = 1500;
		init.size = 1;
		init.speed = 145;
		init.count = 30;
		init.count_var = 3;
		init.outwards = true;
		init.size = 1;

		m_defence += 1.75f;
		m_damage *= 1.35f;

		m_particles.create(init);

		m_sprite.setTextureRect(m_assets.getAnimation(Assets::anim_flameskull_enraged)[0]);
	}


	if((m_shoot && m_attack_use == 0) || (m_phase2 && (m_attack_use == 0 || (m_attack_use == 1 && !m_shoot)))) {
		m_baseshot_timer += dt;

		float m = 1.3f;

		if(m_attack_use == 1 && !m_shoot) {
			m = 1.75;
		}

		if(m_baseshot_timer > (m_phase2 ? FLAMESKULL_SHOOT_TIME*m : FLAMESKULL_SHOOT_TIME) * 7.5f) {
			m_baseshot_timer -= (m_phase2 ? FLAMESKULL_SHOOT_TIME*m : FLAMESKULL_SHOOT_TIME) * 7.5f;

			Projectile proj = PROJECTILE_ARROW;
			proj.playerproj = false;
			proj.pos = m_position;
			proj.size = 2.25f;
			proj.damage = attack();
			proj.anim = Assets::animation_effect_fireball;
			proj.speed = 750;
			proj.gravity = 0;
			proj.color = m_phase2 ? sf::Color(255, 137, 17, 255) : sf::Color::Red;

			float a = atan2(player->getPos().y - m_position.y, player->getPos().x - m_position.x);

			proj.dir = sf::Vector2f(cos(a), sin(a));

			m_projectiles.add(proj);
		}
	}

	sf::Vector2f oldpos = m_position;

	m_velocity -= m_velocity * FLAMESKULL_DECEL * dt;
	m_position += m_velocity * dt;

	// update base
	updateBase(dt, oldpos, 0, 4);
}

void FlameSkull::died() {
	ParticleInit init = PARTICLE_BLOOD;
	init.pos = m_position;
	init.life = 1500;
	init.size = 1;
	init.speed = 145;
	init.count = 30;
	init.count_var = 3;
	init.outwards = true;
	init.size = 1;

	m_particles.create(init);

	m_drops.addDrop(m_position, {Items::BONE, (unsigned int)Random::get(3,6)});
	int r = Random::get(0, 1);
	if(r == 0) m_drops.addDrop(m_position+sf::Vector2f(20,0), {Items::ANCMASK, 1});
	else m_drops.addDrop(m_position-sf::Vector2f(20,0), {Items::FSTAFF, 1});

}

DemonicVisionary::DemonicVisionary(ENEMY_PARAMS, float hp, float def, float atk, sf::FloatRect off, EnemyType type) : Boss(assets, world, drops, particles, projectiles, pos, hp, def, atk, off, type) {
	m_sprite.setOrigin(24, 24);
	m_sprite.setScale(4, 4);

	// take no knockback
	m_knockback_resist = INFINITY;

	// anim
	m_sprite.setTextureRect(m_assets.getAnimation(Assets::anim_demonicvisionary)[0]);

	// not solid
	m_solid = false;
}

DemonicVisionary::~DemonicVisionary() {

}

void DemonicVisionary::update(float dt, Player* player, Enemies& enemies) {
	
	
	if(!m_phase2) m_attack_timer += dt;
	else m_attack_timer += dt*2.5f;
	
	m_anim_timer += dt;
	m_eye_timer += dt;

	// follow player attack
	if(m_attack_use == 0) {

		sf::Vector2f destpos = player->getPos();

		// point direction
		float angle = atan2(destpos.y - m_position.y - (m_phase2 ? 0 : 5*TILE_SIZE), destpos.x - m_position.x);

		// move towards player
		m_velocity.x += cos(angle) * DEMONICVISIONARY_ACCEL * dt;
		m_velocity.y += sin(angle) * DEMONICVISIONARY_ACCEL * dt;

		if(m_attack_timer > 5) {
			m_attack_use = 1;
			m_attack_timer = 0;
		}
	}

	// summon eyes
	else if(m_attack_use == 1) {

		// count
		size_t quant = 0;
		auto& vector = enemies.vector();
		for(size_t i = 0; i < vector.size(); i++) {
			if(vector[i]->type() == EnemyType::EYE) {
				quant++;
			}
		}

		for(size_t i = 0; i < (m_phase2 ? DEMONICVISIONARY_EYECOUNT-Random::get(2,3) : DEMONICVISIONARY_EYECOUNT); i++) {
			if(quant >= DEMONICVISIONARY_EYECOUNTMAX) break;
			quant++;
			
			if(!m_phase2) enemies.spawn(EnemyType::EYE, m_position);
			else {
				sf::Vector2f pos = player->getPos();
				sf::View v = m_world.window().getView();
				sf::FloatRect view(v.getCenter() - v.getSize()/2.f, v.getSize());

				while(view.contains(pos) || Blocks::getBlockData(m_world.tile(pos.x/TILE_SIZE, pos.y/TILE_SIZE)).solid) {
					pos = player->getPos() + sf::Vector2f(Random::get(v.getSize().x*-1.5, v.getSize().x*1.5), Random::get(v.getSize().y*-1.5, v.getSize().y*1.5));
				}

				Eye* e = (Eye*)enemies.spawn(EnemyType::EYE, pos);
				e->damage(e->maxHealth()-5, sf::Vector2f(0,0));
				e->chase();
				e->intangible();
			}
		}

		// dash attack prep
		m_dashtarget = player->getPos();
		for(size_t i = 0; i < vector.size(); i++) {
			if(vector[i]->type() == EnemyType::EYE) {
				((Eye*)vector[i])->target(m_dashtarget);
			}
		}

		m_attack_use = 2;
	}

	// dash attack
	else if(m_attack_use == 2) {

		const float mul = 3.35f;

		// move in dir
		if(m_dashtarget.x > m_position.x)
			m_velocity.x += DEMONICVISIONARY_ACCEL * dt * mul;
		else m_velocity.x -= DEMONICVISIONARY_ACCEL * dt * mul;
		if(m_dashtarget.y > m_position.y)
			m_velocity.y += DEMONICVISIONARY_ACCEL * dt * mul;
		else m_velocity.y -= DEMONICVISIONARY_ACCEL * dt * mul;

		if(m_attack_timer > 3) {
			m_attack_use = 0;
			m_attack_timer = 0;
		}
	}


	// manage eyes
	if(m_eye_timer >= DEMONICVISIONARY_EYETIME && !m_phase2) {
		m_eye_timer -= DEMONICVISIONARY_EYETIME;
		if(m_attack_use != 2) {
			m_eye_shoot_timer++;

			const int dist = DEMONICVISIONARY_EYEDIST;
			auto& vector = enemies.vector();
			for(size_t i = 0; i < vector.size(); i++) {
				if(vector[i]->type() == EnemyType::EYE) {
					((Eye*)vector[i])->target({Random::getf(m_position.x-dist, m_position.x+dist), Random::getf(m_position.y-dist, m_position.y+dist)});
				}
			}

			if(m_eye_shoot_timer >= DEMONICVISIONARY_EYESHOOTTIME) {
				m_eye_shoot_timer = 0;
				size_t quant = 0;

				// shoot
				for(size_t i = 0; i < vector.size(); i++) {
					if(vector[i]->type() == EnemyType::EYE && !((Eye*)vector[i])->chasing()) {
						quant++;
						((Eye*)vector[i])->chase();
						if(quant == 3) {
							break;
						}
					}
				}
			}
		}

	}

	// phase change
	if(health() < maxHealth()*0.35 && !m_phase2) {
		m_phase2 = true;
		m_attack_timer = 0;
		m_attack_use = 0;
		
		auto& frame = m_assets.getAnimation(Assets::anim_demonicvisionary)[m_assets.getAnimation(Assets::anim_demonicvisionary).size()-1];
		m_sprite.setTextureRect(frame);
		m_sprite.setOrigin(frame.width/2, frame.height/2);

		// kill eyes
		auto& vector = enemies.vector();
		for(size_t i = 0; i < vector.size(); i++) {
			if(vector[i]->type() == EnemyType::EYE) {
				vector[i]->damage(9999, sf::Vector2f(0,0));
			}
		}

		// particle effects
		ParticleInit init = PARTICLE_BLOOD;
		init.pos = m_position;
		init.life = 1500;
		init.size = 1;
		init.speed = 145;
		init.count = 30;
		init.count_var = 3;
		init.outwards = true;
		init.size = 1;

		m_particles.create(init);
	}

	// animate
	if(!m_phase2) {
		if(m_anim_timer > DEMONICVISIONARY_ANIMTIME) {
			m_anim_timer -= DEMONICVISIONARY_ANIMTIME;
			m_anim_frame++;

			if(m_anim_frame >= m_assets.getAnimation(Assets::anim_demonicvisionary).size()-1) {
				m_anim_frame = 0;
			}

			auto& frame = m_assets.getAnimation(Assets::anim_demonicvisionary)[m_anim_frame];
			m_sprite.setTextureRect(frame);

			// width and height change over anim
			m_sprite.setOrigin(frame.width/2, frame.height/2);
		}
	} else {
		m_sprite.rotate(FLAMESKULL_BAYBLADE_ROTATE * dt);
	}

	if(m_velocity.x > 0) m_sprite.setScale(4, 4);
	else if(m_velocity.x < 0) m_sprite.setScale(-4, 4);

	sf::Vector2f oldpos = m_position;

	m_velocity -= m_velocity * DEMONICVISIONARY_DECEL * dt;
	m_position += m_velocity * dt;

	// update base
	updateBase(dt, oldpos, 0, 4);
}

void DemonicVisionary::died() {
	m_drops.addDrop(m_position, {Items::HELLWINGS, 1});
}