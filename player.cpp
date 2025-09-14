#include "player.hpp"

Player::Player(sf::RenderWindow& window, Assets& assets, World& world, Inventory& inventory, Drops& drops, Enemies& enemies, Particles& particles, Projectiles& projectiles, bool& mouse) : m_window(window), m_assets(assets), m_world(world), m_inventory(inventory), m_drops(drops), m_enemies(enemies), m_particles(particles), m_projectiles(projectiles), m_mousea(mouse) {

	// init player sprite
	m_sprite.setOrigin(8, 16);

	m_armor_helmet.setOrigin(8, 16);
	m_armor_chestplate.setOrigin(8, 6);
	m_armor_leggings.setOrigin(8, -1);

	m_item.setOrigin(0, 8);
	m_sprite.setScale(2, 2);
	m_armor_helmet.setScale(2, 2);
	m_armor_chestplate.setScale(2, 2);
	m_armor_leggings.setScale(2, 2);


	m_arm.setFillColor(sf::Color(0xFF,0xE3,0xA1,0xFF));
	
}

Player::~Player() {

}

bool Player::ready() {
	m_sprite.setTexture(m_assets.getTexture(Assets::texture_player));
	m_sprite.setTextureRect(m_assets.getAnimation(Assets::anim_player_idle)[0]);
	m_armor_helmet.setTexture(m_assets.getTexture(Assets::texture_player));
	m_armor_chestplate.setTexture(m_assets.getTexture(Assets::texture_player));
	m_armor_leggings.setTexture(m_assets.getTexture(Assets::texture_player));

	return true;
}

void Player::update(sf::Time dt, bool keyboard) {

	float fdt = dt.asSeconds();
	m_timer += fdt;

	// movement buffs
	m_buffs.update(fdt);

	// calculate inputs and update velocity 
	if(keyboard) inputs(fdt);

	// move player based on said velocity
	physics(fdt);

	// check for collisions
	collision();

	// animate player
	m_animtimer += fdt;
	animate();

	// interact with world
	interact(fdt);

}

void Player::draw() {
	bool item = false;
	if(m_attacking && m_inventory.selectedS().item != Items::NONE) {
		item = true;
		m_window.draw(m_arm);
		m_window.draw(m_armsleave);
	}

	m_window.draw(m_sprite);

	for(size_t i = (size_t)Armor::Category::HELMET; i < (size_t)Armor::Category::ACCESSORIES; i++) {
		if(m_inventory[(Armor::Category)i].item != Items::NONE) {
			switch((Armor::Category)i) {
				case Armor::Category::HELMET: {
					m_window.draw(m_armor_helmet);
					break;
				} case Armor::Category::CHESTPLATE: {
					m_window.draw(m_armor_chestplate);
					break;
				} case Armor::Category::LEGGINGS: {
					m_window.draw(m_armor_leggings);
					break;
				} default:
					break;
			}
		}
	}

	if(item) {
		m_window.draw(m_item);
	}
	
}

void Player::inputs(float dt) {

	float speed = 1;
	float jump = 1;
	auto& vector = m_buffs.vector();
	for(auto& buff : vector) {
		if(buff.type == Buff::SPEED) {
			speed += buff.value / 5;
		} else if(buff.type == Buff::JUMP) {
			jump += buff.value / 15;
		}
	}

	if(sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
		m_velocity.x -= PLAYER_ACCEL*dt*speed; m_running = 1; speed = 1;
	} else if(sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
		m_velocity.x += PLAYER_ACCEL*dt*speed; m_running = 1; speed = 1;
	} else {
		m_running = 0;
	}

	if((sf::Keyboard::isKeyPressed(sf::Keyboard::Space) || sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) && !m_holding_jump && m_jumpcount > 0) {
		m_velocity.y = -(int)PLAYER_JUMP * jump; // boost value
		m_jumping = true; 
		m_jumpcount--;
		m_holding_jump = true;
		m_world.getAudio().play(Audio::Sound::JUMP);
	} else if(!sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && !sf::Keyboard::isKeyPressed(sf::Keyboard::W) && !sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
		m_holding_jump = false;
	}

	if(sf::Mouse::isButtonPressed(sf::Mouse::Left) && m_mousea) {
		m_attacking = true;
	} else {
		m_attacking = false;
	}

}

void Player::physics(float dt) {
	
	// deceleration 
	m_velocity -= m_velocity * static_cast<float>(PLAYER_DECEL) * dt;
	if(m_running == 0) m_velocity.x -= m_velocity.x * static_cast<float>(PLAYER_DECEL) * 1.75f * dt;

	// gravity
	m_velocity.y += GRAVITY * dt;

	// update position
	m_position_old = m_position;
	m_position += m_velocity * dt;

	m_sprite.setPosition(m_position);

	auto bounds = m_sprite.getGlobalBounds();
	bool changed = true;

	// keep player within map bounds at all costs
	if(bounds.top < 1*TILE_SIZE/2) {
		m_position.y = 1*TILE_SIZE/2 + bounds.height/2;
		m_velocity.y = 0;
	} else if(bounds.top+bounds.height > (WORLD_HEIGHT-.5)*TILE_SIZE) {
		m_position.y = (WORLD_HEIGHT-.5)*TILE_SIZE - bounds.height/2;
		m_velocity.y = 0;
	} else if(bounds.left < 1*TILE_SIZE/2) {
		m_position.x = 1*TILE_SIZE/2 + bounds.width/2;
		m_velocity.x = 0;
	} else if(bounds.left+bounds.width > (WORLD_WIDTH-.5)*TILE_SIZE) {
		m_position.x = (WORLD_WIDTH-.5)*TILE_SIZE - bounds.width/2;
		m_velocity.x = 0;
	} else {
		changed = false;
	}

	if(changed) m_sprite.setPosition(m_position);

}

// this is an ordeal to understand, better call the cryptographer
void Player::collision() {

	// get player hitbox
	sf::FloatRect hitbox = m_sprite.getGlobalBounds();

	// adjust
	hitbox.left += 4;
	hitbox.width -= 8;
	hitbox.top += 6;
	hitbox.height -= 12;

	// get player tile position
	sf::Vector2i tilepos = sf::Vector2i(m_position.x / TILE_SIZE, m_position.y / TILE_SIZE);

	// find surrounding tiles
	std::vector<std::pair<sf::FloatRect, Blocks::BlockData>> tiles;
	for (int x = tilepos.x-1; x < tilepos.x+2; x++) {

		if(x < 0 || x >= (int)WORLD_WIDTH) continue;

		for (int y = tilepos.y-1; y < tilepos.y+2; y++) {

			if(y < 0 || y >= (int)WORLD_HEIGHT) continue;

			tiles.push_back(std::make_pair(sf::FloatRect(x*TILE_SIZE, y*TILE_SIZE, TILE_SIZE, TILE_SIZE), Blocks::getBlockData(m_world.tile(x, y))));
		}
	}
	
	m_colliders.clear();

	bool colided = false;

	// check for collisions
	for (size_t i = 0; i < tiles.size(); i++)
	{
		if(tiles[i].second.solid && hitbox.intersects(tiles[i].first)) {

			bool within_x = m_position.x+hitbox.width/2 > tiles[i].first.left && m_position.x-hitbox.width/2 < tiles[i].first.left+TILE_SIZE;

			// get collision side
			if(m_position.y+hitbox.height/2 > tiles[i].first.top && within_x && 
				m_position_old.y+hitbox.height/2 <= tiles[i].first.top)
			{ 
				m_position.y = tiles[i].first.top - hitbox.height/2;
				m_jumping = false;
				m_velocity.y = 0;

				colided = true;

				m_colliders.push_back(std::make_pair(sf::Vector2u(tiles[i].first.getPosition()/(float)TILE_SIZE), tiles[i].second));
			}

			bool within_y = m_position.y+hitbox.height/2 > tiles[i].first.top && m_position.y-hitbox.height/2 < tiles[i].first.top+TILE_SIZE;

			if(m_position.x+hitbox.width/2 > tiles[i].first.left && within_y && 
				m_position_old.x+hitbox.width/2 <= tiles[i].first.left)
			{ 
				m_position.x = tiles[i].first.left - hitbox.width/2;
				m_velocity.x = 0;

				colided = true;

				m_colliders.push_back(std::make_pair(sf::Vector2u(tiles[i].first.getPosition()/(float)TILE_SIZE), tiles[i].second));
			}
			if(m_position.x-+hitbox.width/2 < tiles[i].first.left+TILE_SIZE && within_y && 
				m_position_old.x-hitbox.width/2 >= tiles[i].first.left+TILE_SIZE)
			{
				m_position.x = tiles[i].first.left + TILE_SIZE + hitbox.width/2;
				m_velocity.x = 0;

				colided = true;

				m_colliders.push_back(std::make_pair(sf::Vector2u(tiles[i].first.getPosition()/(float)TILE_SIZE), tiles[i].second));
			}

			// revaluate within_x
			within_x = m_position.x+hitbox.width/2 > tiles[i].first.left && m_position.x-hitbox.width/2 < tiles[i].first.left+TILE_SIZE;
			if(m_position.y-hitbox.height/2 < tiles[i].first.top+TILE_SIZE && within_x && 
				m_position_old.y-hitbox.height/2 >= tiles[i].first.top+TILE_SIZE)
			{ 
				m_position.y = tiles[i].first.top +TILE_SIZE + hitbox.height/2;
				m_velocity.y = 0;

				colided = true;

				m_colliders.push_back(std::make_pair(sf::Vector2u(tiles[i].first.getPosition()/(float)TILE_SIZE), tiles[i].second));
			}


		} else if(tiles[i].second.liquid && hitbox.intersects(tiles[i].first)) {
			m_colliders.push_back(std::make_pair(sf::Vector2u(tiles[i].first.getPosition()/(float)TILE_SIZE), tiles[i].second));
		}
	}

	// fix slight stuttering
	if(colided) {
		m_sprite.setPosition(m_position);
	}

	// advanced jump checking
	if(!Blocks::getBlockData(m_world.tile(hitbox.getPosition().x/TILE_SIZE, (hitbox.getPosition().y + hitbox.height+1)/TILE_SIZE)).solid &&
		!Blocks::getBlockData(m_world.tile((hitbox.getPosition().x+hitbox.getSize().x)/TILE_SIZE, (hitbox.getPosition().y + hitbox.height+1)/TILE_SIZE)).solid && !m_jumping)
	{
		m_jumping = true;
		m_jumpcount--;
	}

}

void Player::animate() {
	Assets::Animation anim = Assets::anim_count;

	// chose animation sequence
	if(!m_attacking || m_inventory.selectedS().item == Items::NONE) {
		if(m_jumping) {
			anim = Assets::anim_player_jump;
		} else if(m_running) {
			anim = Assets::anim_player_run;
		} else {
			anim = Assets::anim_player_idle;
		}
	} else { 
		if(m_inventory.selectedS().item != Items::NONE) {
			if(m_jumping) {
				anim = Assets::anim_player_jumpatk;
			} else if(m_running) {
				anim = Assets::anim_player_runatk;
			} else {
				anim = Assets::anim_player_idleatk;
			}
		}

		// item in hand, set texture
		auto category = Items::getItemData(m_inventory.selectedS().item).category;

		// new texture, update sprite
		if(m_itold != category) {
			if(category == Items::Category::BLOCK)
				m_item.setTexture(m_assets.getTexture(Assets::texture_blocks));
			else m_item.setTexture(m_assets.getTexture(Assets::texture_items));

			m_itold = category;
		}

		// set texture rect
		auto anim = Items::getItemData(m_inventory.selectedS().item).anim;
		if(m_iaold != anim) {
			m_item.setTextureRect(m_assets.getAnimation(anim)[0]);

			m_iaold = anim;
		}

		sf::Vector2f mousepos = m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window));

		// get direction from player position to mouse position via normalized vector
		sf::Vector2f dir = mousepos - m_position;
		dir = (dir / std::sqrt(dir.x*dir.x + dir.y*dir.y));
		m_direction = dir;

		// get angle from direction
		float angle = std::atan2(dir.y, dir.x) * 180 / 3.14159265;
		m_directionangle = angle;

		if(dir.x > 0.7) dir.x = 0.7;
		else if(dir.x < -0.7) dir.x = -0.7;
		if(dir.y > 0.7) dir.y = 0.7;
		else if(dir.y < -0.7) dir.y = -0.7;

		if(dir.x > 0) m_iright = true;
		else m_iright = false; 

		// set position
		m_item.setPosition(m_position.x, m_position.y);
		m_item.move(dir*(float)ITEM_OFFSET);
		
		// pythonic code
		float scale = (Tools::isTool(m_inventory.selectedS().item) && Tools::getToolData(m_inventory.selectedS().item).category == Tools::Category::SWORD ? 1.35f : 1);
		m_item.setScale((m_iright ? 1*ITEM_SIZE : -1*ITEM_SIZE)*scale, 1*ITEM_SIZE*scale);

		// procedural rotation for items flaged with the swing attr
		if(Items::getItemData(m_inventory.selectedS().item).swing) {

			// if its a tool, get the speed attribute
			float speed = 1;
			if(Tools::isTool(m_inventory.selectedS().item))
				speed = Tools::getToolData(m_inventory.selectedS().item).speed;

			m_item.setRotation((std::sin(m_timer*SWING_ANIM_SPEED*speed)*45) + angle - (m_iright ? 0 : 180));
		} else {
			m_item.setRotation(angle - (m_iright ? 0 : 180));
		}

		// connect arm to item
		m_arm.setPosition(m_position.x + (m_iright ? 3 : -3), m_position.y - 8);
		m_arm.setRotation(angle);

		// get arm length
		float armlength = sqrt(pow(m_item.getPosition().x - m_arm.getPosition().x, 2) + pow(m_item.getPosition().y - m_arm.getPosition().y, 2));
		if(armlength > 22) armlength = 22;
		m_arm.setSize(sf::Vector2f(armlength,2));
		
		// point arm to m_item.getposition
		float armangle = std::atan2(m_item.getPosition().y - m_arm.getPosition().y, m_item.getPosition().x - m_arm.getPosition().x) * 180 / 3.14159265;
		m_arm.setRotation(armangle);

		// sleave
		m_armsleave.setPosition(m_arm.getPosition());
		m_armsleave.setRotation(armangle);
		m_armsleave.setSize(sf::Vector2f(armlength/1.65f,2));
		switch (m_inventory[Armor::Category::CHESTPLATE].item)
		{
		case Items::LCHESTPLATE: m_armsleave.setFillColor(sf::Color(0x31,0x5B,0xAF,0xFF)); break;
		case Items::GCHESTPLATE: m_armsleave.setFillColor(sf::Color(0xFE,0xE7,0x61,0xFF)); break;
		case Items::TCHESTPLATE: m_armsleave.setFillColor(sf::Color(0xE6,0xF3,0xFF,0xFF)); break;
		case Items::UCHESTPLATE: m_armsleave.setFillColor(sf::Color(0x02,0x5E,0x86,0xFF)); break;
		case Items::RCHESTPLATE: m_armsleave.setFillColor(sf::Color(0xD4,0x6D,0xA5,0xFF)); break;
		case Items::VCHESTPLATE: m_armsleave.setFillColor(sf::Color(0x49,0x97,0x38,0xFF)); break;
		case Items::FCHESTPLATE: m_armsleave.setFillColor(sf::Color(0xFD,0x6C,0x05,0xFF)); break;
		case Items::CCHESTPLATE: m_armsleave.setFillColor(sf::Color(0xBB,0x1D,0xC9,0xFF)); break;

		default:
			m_armsleave.setFillColor(sf::Color(0xE6,0x77,0xD7,0xFF));
			break;
		}
	}
	
	if(m_attacking && m_inventory.selectedS().item != Items::NONE) {
		if(m_iright) {
			m_sprite.setScale(2, 2);
			m_armor_helmet.setScale(2, 2);
			m_armor_chestplate.setScale(2, 2);
			m_armor_leggings.setScale(2, 2);
		} else {
			m_sprite.setScale(-2, 2);
			m_armor_helmet.setScale(-2, 2);
			m_armor_chestplate.setScale(-2, 2);
			m_armor_leggings.setScale(-2, 2);
		}
	} else {
		if(m_velocity.x > 0) {
			m_sprite.setScale(2, 2);
			m_armor_helmet.setScale(2, 2);
			m_armor_chestplate.setScale(2, 2);
			m_armor_leggings.setScale(2, 2);
		} else if(m_velocity.x < 0) { // seems redundant (should use else), but makes player not turn on 0 vel (walking into block)
			m_sprite.setScale(-2, 2);
			m_armor_helmet.setScale(-2, 2);
			m_armor_chestplate.setScale(-2, 2);
			m_armor_leggings.setScale(-2, 2);
		}
	}


	// set animation frame from sequence
	auto& animation = m_assets.getAnimation(anim);
	int frame = std::round(m_animtimer/PLAYER_ANIM_SPEED);

	if((size_t)frame >= animation.size()) {
		m_animtimer = 0;
		frame = 0;
	}

	m_sprite.setTextureRect(animation[frame]); 

	// armor
	if(anim == Assets::anim_player_jump || anim == Assets::anim_player_jumpatk) {
		frame += m_assets.getAnimation(Assets::anim_player_run).size();
	}

	for(size_t i = (size_t)Armor::Category::HELMET; i < (size_t)Armor::Category::LEGGINGS+1; i++) {

		if(Armor::isArmor(m_inventory[(Armor::Category)i].item)) {
			int a = (int)Armor::getArmorData(m_inventory[(Armor::Category)i].item).anim;
			if(m_attacking && m_inventory.selectedS().item != Items::NONE) a += (int)Assets::anim_armor_leadatk - (int)Assets::anim_armor_lead;
			auto& armor_animation = m_assets.getAnimation((Assets::Animation)a);
			sf::IntRect rect = armor_animation[frame];
			if(m_jumping)
				rect = armor_animation[armor_animation.size()-1];
			if(i == (size_t)Armor::Category::HELMET) {
				rect.height = 10;
				m_armor_helmet.setTextureRect(rect);
				m_armor_helmet.setPosition(m_position.x, m_position.y);
			} else if(i == (size_t)Armor::Category::CHESTPLATE) {
				rect.top += 10;
				rect.height = 7;
				m_armor_chestplate.setTextureRect(rect);
				m_armor_chestplate.setPosition(m_position.x, m_position.y);
			} else {
				rect.top += 17;
				rect.height = 12;
				m_armor_leggings.setTextureRect(rect);
				m_armor_leggings.setPosition(m_position.x, m_position.y);
			}

		}
	}

}

void Player::interact(float dt) {
	
	// timer
	if(m_invuln > 0) {
		m_invuln -= dt;
		m_sprite.setColor(sf::Color(255, 255, 255, 128));
		sf::Color c = m_arm.getFillColor(); c.a = 128;
		m_arm.setFillColor(c);
		if(m_invuln <= 0) {
			m_invuln = 0;
			m_hitable = true;
			m_sprite.setColor(sf::Color(255, 255, 255, 255));
			sf::Color c = m_arm.getFillColor(); c.a = 255;
			m_arm.setFillColor(c);
		}
	}

	// hp regen
	if(m_health < PLAYER_MAX_HEALTH) {
		m_health += PLAYER_HEALTH_REGEN*dt;
	} else {
		m_health = PLAYER_MAX_HEALTH;
	}

	// heal timer
	if(m_healtimer > 0) {
		m_healtimer -= dt;
		if(m_healtimer <= 0) {
			m_healtimer = 0;
		}
	}

	if(m_buildtimer > 0) {
		m_buildtimer -= dt;
		if(m_buildtimer <= 0) {
			m_buildtimer = 0;
		}
	}

	// other buffs
	float b_attack = 1;
	float b_defence = 1;
	float b_knockback_res = 1;

	if(m_jumping == false) {
		m_jumpcount = 1;
	}

	auto& vector = m_buffs.vector();
	for(auto& buff : vector) {
		if(buff.type == Buff::ATTACK) {
			b_attack += buff.value;
		} else if(buff.type == Buff::DEFENCE) {
			b_defence += buff.value;
		} else if(buff.type == Buff::KNOCKBACK_RES) {
			b_knockback_res += buff.value;
		} else if(buff.type == Buff::JUMP_COUNT && m_jumping == false) {
			m_jumpcount += (unsigned int)buff.value;
		}
	}

	// handle item pickups
	sf::FloatRect hitbox;
	hitbox.width = 16;
	hitbox.height = 16;

	std::vector<Drops::Drop>& drops = m_drops.getDrops();

	// check collision with all drops, make em rain
	for(size_t i = 0; i < drops.size(); i++) {

		float old = drops[i].pos.y;
		drops[i].pos.y += DROP_SPEED*dt;

		if(Blocks::getBlockData(m_world.tile(drops[i].pos.x/TILE_SIZE, 1+(drops[i].pos.y/TILE_SIZE))).solid) {
			drops[i].pos.y = old;
		}

		// check if the player is close enough to pick up the item
		hitbox.left = drops[i].pos.x-8;
		hitbox.top = drops[i].pos.y-8;

		// append to inventory, remove from drops
		if(m_sprite.getGlobalBounds().intersects(hitbox)) {
			if(m_inventory.append(drops[i].item)) {
				m_world.getAudio().play(Audio::Sound::GET);
				drops.erase(drops.begin()+i);
				i--;
			}
		}
	}

	// call bonus lambdas from armor/accessories
	for (size_t i = (size_t)Armor::Category::HELMET; i < (size_t)Armor::Category::COUNT; i++) {
		
		if(i == (size_t)Armor::Category::ACCESSORIES) {
			for (size_t j = 0; j < ACCESSORY_COUNT; j++) {
				if(Armor::isArmor(m_inventory.acc(j).item)) {
					Armor::getArmorData(m_inventory.acc(j).item).ability(m_inventory.acc(j).item, this, &m_drops, &m_enemies, &m_inventory, &m_particles, &m_projectiles, dt);
				}
			}
		}
		
		else if(Armor::isArmor(m_inventory[(Armor::Category)i].item))
				Armor::getArmorData(m_inventory[(Armor::Category)i].item).ability(m_inventory[(Armor::Category)i].item, this, &m_drops, &m_enemies, &m_inventory, &m_particles, &m_projectiles, dt);
	}

	// react to certain blocks
	bool burning = false;
	bool drowning = false; // not actually drowning, just under water 
	static float particle_timer = 0;
	for (size_t i = 0; i < m_colliders.size(); i++)
	{
		if(m_colliders[i].second.type == Blocks::LAVA) {
			burning = true;
		} else if(m_colliders[i].second.type == Blocks::WATER && m_colliders[i].first.y < m_position.y/TILE_SIZE - 1) {
			drowning = true; 
		}
	}

	if(burning) {
		m_health -= 30*dt;
		particle_timer += dt;
		if(particle_timer > 0.2) {
			ParticleInit init = PARTICLE_PUFF;
			init.pos = m_position + sf::Vector2f(0, TILE_SIZE);
			init.color = sf::Color(255, 137, 17, 255);
			init.life = 900;
			init.size = 0.8;
			init.speed = 75;
			init.gravity = -750;
			init.count = 3;
			init.count_var = 1;

			m_particles.create(init);
			particle_timer -= 0.2;
		}
	}

	if(drowning) {
		m_air -= (1.f/7.f)*100 * dt; // 7 seconds to drown
		if(m_air <= 0) {
			m_air = 0;
			m_health -= 30*dt;
		}
		particle_timer += dt;
		if(particle_timer > 0.3) {
			ParticleInit init = PARTICLE_PUFF;
			init.pos = m_position - sf::Vector2f(0, TILE_SIZE) + sf::Vector2f(0, TILE_SIZE/2);
			init.color = sf::Color(255, 255, 255, 255);
			init.life = 600;
			init.size = 0.5;
			init.speed = 75;
			init.gravity = -550;
			init.count = 1;
			init.count_var = 1;

			m_particles.create(init);
			particle_timer -= 0.3;
		}
	} else {
		if(m_air < 100) { // replenish air
			m_air += (1.f/7.f)*100 * dt;
			if(m_air > 100) m_air = 100;
		}
	}
	

	// prerequisite variables for mining/placing
	bool mouse = sf::Mouse::isButtonPressed(sf::Mouse::Left);
	bool mouser = sf::Mouse::isButtonPressed(sf::Mouse::Right);
	sf::Vector2f mousepos = m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window));
	sf::Vector2i mouseposworld = sf::Vector2i(floor(mousepos.x/(float)TILE_SIZE), floor(mousepos.y/(float)TILE_SIZE));

	// check if distance from player is within range BREAK_PLACE_DISTANCE
	bool within_range = false;
	if(m_position.x/TILE_SIZE > mouseposworld.x - BREAK_PLACE_DISTANCE/2 && m_position.x/TILE_SIZE < mouseposworld.x + BREAK_PLACE_DISTANCE/2) {
		if(m_position.y/TILE_SIZE > mouseposworld.y - BREAK_PLACE_DISTANCE/2 && m_position.y/TILE_SIZE < mouseposworld.y + BREAK_PLACE_DISTANCE/2) {
			within_range = true;
		} 
	}

	// check if block overlaps with player or enemy
	bool block_overlap = false;
	sf::FloatRect rect(mouseposworld.x*TILE_SIZE, mouseposworld.y*TILE_SIZE, TILE_SIZE, TILE_SIZE);
	if(m_sprite.getGlobalBounds().intersects(rect)) {
		block_overlap = true;
	}

	const auto& enemy_vector = m_enemies.vector();
	for(size_t i = 0; i < enemy_vector.size(); i++) {
		if(enemy_vector[i]->bounds().intersects(rect)) {
			block_overlap = true;
		}
	}

	// clicking and mouse aint hoged by other class (we don't need a ronaldo)
	if(mouse && m_mousea) {
		m_chest_open = sf::Vector2i(-1, -1);

		if(within_range) {

			if(m_world.tile(mouseposworld.x, mouseposworld.y) != Blocks::NONE) { // clicking on block

				// a tool is selected
				if(Tools::isTool(m_inventory.selectedS().item)) {

					// break that shit
					auto& tool = Tools::getToolData(m_inventory.selectedS().item);
					auto& block = Blocks::getBlockData(m_world.tile(mouseposworld.x, mouseposworld.y));
					if(((tool.category == Tools::Category::PICKAXE && block.solid) || (tool.category == Tools::Category::AXE && !block.solid))) {

						// l bozo ur dinky ass beta pick can't even touch this sigma block xD
						if(tool.level >= block.level && !block.liquid) {

							m_breaktimer += dt;

							if(mouseposworld != m_oldtile) {
								m_breaktimer = 0;
								m_oldtile = mouseposworld;
							}


							if(m_breaktimer >= BREAK_TIME / Tools::getToolData(m_inventory.selectedS().item).speed * Blocks::getBlockData(m_world.tile(mouseposworld.x, mouseposworld.y)).hardness) {
								
								bool b = true;

								// if it's a chest, try destroy it
								if(m_world.tile(mouseposworld.x, mouseposworld.y) == Blocks::CHEST) {
									if(!m_world.removeChest(mouseposworld.x, mouseposworld.y)) {
										b = false;
									}
								}

								if(b)
								{
									m_breaktimer = 0;

									// drop item
									m_drops.addDrop(mousepos, {Items::fromBlock(m_world.tile(mouseposworld.x, mouseposworld.y)), 1});

									// particles
									ParticleInit init = PARTICLE_PUFF;
									init.pos = mousepos;
									init.anim = Assets::animation_effect_chunk;
									init.color = Blocks::getBlockColor(m_world.tile(mouseposworld.x, mouseposworld.y));
									init.life = 900;
									init.size = 1.15;
									init.speed = 75;
									init.gravity = 650;
									init.count = 6;
									init.count_var = 1;

									m_particles.create(init);


									m_world.tile(mouseposworld.x, mouseposworld.y) = Blocks::NONE;
									m_world.getAudio().play(Audio::Sound::BREAK);

									// update tile's brightness around it
									m_world.updateMapTile(mouseposworld.x, mouseposworld.y);
								}
							}
						}
					} 
				}

			}
			
			// o, thy user hath requested a tile'th to be filled with a block
			if(Items::getItemData(m_inventory.selectedS().item).category == Items::Category::BLOCK && m_buildtimer == 0) {

				if(!block_overlap || !Blocks::getBlockData(Items::toBlock(m_inventory.selectedS().item)).solid) {

					// check you aren't replacing a relevant block
					if(m_world.tile(mouseposworld.x, mouseposworld.y) == Blocks::NONE || (Blocks::getBlockData(m_world.tile(mouseposworld.x, mouseposworld.y)).relevant == false && Blocks::getBlockData(Items::toBlock(m_inventory.selectedS().item)).relevant == true)) {

						// check if there is an adjacent tile so it can be placed
						bool placable = false;
						if(m_inventory.selectedS().item != Items::TORCH) {
							for(char x = -1; x <= 1; x++) {
								for(char y = -1; y <= 1; y++) {

									if(x == 0 && y == 0) continue;

									if(m_world.tile(mouseposworld.x+x, mouseposworld.y+y) != Blocks::NONE && Blocks::getBlockData(m_world.tile(mouseposworld.x+x, mouseposworld.y+y)).relevant) {
										placable = true;
										break;
									}
								}

								if(placable) break;
							}
						} else { // special case, only placable if bg exists or block under is solid
							if(m_world.tile(mouseposworld.x, mouseposworld.y+1) != Blocks::NONE || m_world.tilebg(mouseposworld.x, mouseposworld.y) != Blocks::NONE) {
								placable = true;
							}
						}

						// after hours of work, the block can be placed
						if(placable) {
							m_world.tile(mouseposworld.x, mouseposworld.y) = Items::toBlock(m_inventory.selectedS().item);
							m_inventory.remove({m_inventory.selectedS().item, 1}); // man, constructing structs like this is so hot

							// lighting
							m_world.updateMapTile(mouseposworld.x, mouseposworld.y);

							// if it's a chest, create and store an inventory for it
							if(m_world.tile(mouseposworld.x, mouseposworld.y) == Blocks::CHEST) {
								m_world.appendChest(mouseposworld.x, mouseposworld.y);
							}

							m_buildtimer = PLAYER_BUILD_TIMER;
							m_world.getAudio().play(Audio::Sound::BREAK);
						}
					}
				}
			}
		}

		// attacking, also we don't give a shit about range
		if(Tools::isTool(m_inventory.selectedS().item) && (Tools::getToolData(m_inventory.selectedS().item).category == Tools::Category::SWORD || Tools::getToolData(m_inventory.selectedS().item).category == Tools::Category::SPECIAL)) {

			// use ability if special
			if(Tools::getToolData(m_inventory.selectedS().item).category == Tools::Category::SPECIAL) {
				Tools::getToolData(m_inventory.selectedS().item).ability(m_inventory.selectedS().item, this, &m_drops, &m_enemies, &m_inventory, &m_particles, &m_projectiles, dt);
			} else {

				// check if there is an enemy in range
				for(size_t i = 0; i < enemy_vector.size(); i++) {
					if(enemy_vector[i]->collided(m_item)) {

						// attack, don't forget buff value
						bool hit = enemy_vector[i]->damage(Tools::getToolData(m_inventory.selectedS().item).damage * b_attack, sf::Vector2f((right() ? 1 : -1), -1.f));

						if(hit) {
							// particle time
							ParticleInit init = PARTICLE_BLOOD;
							init.pos = enemy_vector[i]->position();
							init.dir = sf::Vector2f((right() ? 1 : -1), -0.65f);

							m_particles.create(init);
						}
					}
				}
			}
		} 
		
		// consumables
		else if(Items::getItemData(m_inventory.selectedS().item).consumable && !m_consumed) {

			m_consumed = true;

			// use it
			bool used = Items::getItemData(m_inventory.selectedS().item).ability(m_inventory.selectedS().item, this, &m_drops, &m_enemies, &m_inventory, &m_particles, &m_projectiles, dt);

			// remove if used
			if(used) {
				m_inventory.remove(m_inventory.selected(), 1);
			}
		} 
	} else if(mouser && m_mousea) { // chests
		if(within_range && m_world.tile(mouseposworld.x, mouseposworld.y) == Blocks::CHEST) {
			m_chest_open = sf::Vector2i(mouseposworld.x, mouseposworld.y);
		}
	} else {
		m_consumed = false;
	}

	if constexpr(PLAYER_COLLIDES) {

		// hit by enemy / projectile
		bool hit = false;
		Projectile* projectile = nullptr;
		Enemy* enemy = nullptr;

		for(size_t i = 0; i < enemy_vector.size(); i++) {
			if(enemy_vector[i]->collided(m_sprite) && m_hitable) {
				hit = true;
				enemy = enemy_vector[i];
			}
		}

		auto& projectiles = m_projectiles.vector();
		for(size_t i = 0; i < projectiles.size(); i++) {
			if(!projectiles[i].playerproj && Collision::pixelPerfectTest(projectiles[i].sprite, m_sprite) && m_hitable) {
				hit = true;

				projectile = &projectiles[i];
			}
		}

		if(hit) {

			int var = Random::get(-3, 3);
			float damage = 0;
			if(enemy != nullptr)
				damage = enemy->attack() + var;
			else
				damage = projectile->damage + var;
			if(damage < 1) damage = 1;

			// get player defence
			float defence = 0;
			for(size_t i = (size_t)Armor::Category::HELMET; i < (size_t)Armor::Category::ACCESSORIES; i++) {
				if(m_inventory[(Armor::Category)i].item != Items::NONE) {
					defence += Armor::getArmorData(m_inventory[(Armor::Category)i].item).defence;
				}
			}

			for(size_t i = 0; i < ACCESSORY_COUNT; i++) {
				if(m_inventory.acc(i).item != Items::NONE) {
					defence += Armor::getArmorData(m_inventory.acc(i).item).defence;
				}
			}
			
			defence *= b_defence;

			// calculate damage
			damage -= defence;
			if(damage < 1) damage = 1;
			m_health -= damage;

			m_hitable = false;

			m_invuln = INVIS_PLAYER_DURATION;
			if(enemy != nullptr)
				m_velocity = sf::Vector2f((enemy->right() ? 1 : -1) * (float)KNOCKBACK, -(float)KNOCKBACK);
			else
				m_velocity = sf::Vector2f(projectile->dir.x * (float)KNOCKBACK, -(float)KNOCKBACK);

			m_velocity *= 1/b_knockback_res;

			// particle time
			ParticleInit init = PARTICLE_BLOOD;
			init.pos = m_position;
			if(enemy != nullptr)
				init.dir = sf::Vector2f((enemy->right() ? -1 : 1), -0.65f);
			else
				init.dir = sf::Vector2f(projectile->dir.x, -0.65f);

			m_particles.create(init);
			m_world.getAudio().play(Audio::Sound::HIT);
		}
	}

	// check distance from chest
	if(chestOpen()) {
		float distance = sqrtf(powf(m_position.x/TILE_SIZE - m_chest_open.x, 2) + powf(m_position.y/TILE_SIZE - m_chest_open.y, 2));
		if(distance > BREAK_PLACE_DISTANCE/2) {
			m_chest_open = sf::Vector2i(-1, -1);
		}
	}
	
	// check health
	if(m_health <= 0) {
		m_health = 0;
		m_alive = false;
	}
}

void Player::setSpawn(sf::Vector2f s) {
	m_spawn = s;

	m_position = m_spawn;
	m_position_old = m_position;

	m_sprite.setPosition(m_position);
}

sf::FloatRect Player::getBounds() {
	return m_sprite.getGlobalBounds();
}

sf::Vector2f Player::getPos() {
	return m_position;
}

sf::Vector2f Player::getIPos() {
	return m_item.getPosition();
}

bool Player::right() {
	return m_iright;
}

float Player::getHealth() {
	return m_health;
}

bool Player::alive() {
	return m_alive;
}

void Player::reset() {
	m_health = 100;
	m_alive = true;
	m_position = m_spawn;
	m_position_old = m_position;
	m_sprite.setPosition(m_position);

	m_velocity = sf::Vector2f(0, 0);

	m_enemies.reset();
	m_particles.clear();
	m_projectiles.clear();
	m_drops.getDrops().clear();

	m_healtimer = 0;
	m_health = PLAYER_MAX_HEALTH;
}

// buff and stat functions
void Player::setHealth(float health) {m_health = health; if(m_health > 100) m_health = 100;}
void Player::setHealTimer(float duration) {m_healtimer = duration;}

// info functions
bool Player::running() {return m_running;}
bool Player::jumping() {return m_jumping;}
unsigned int Player::jumpcount() {return m_jumpcount;}
bool Player::hitable() {return m_hitable;}
bool Player::attacking() {return m_attacking;}
sf::Vector2f Player::direction() {return m_direction;}
sf::Vector2f Player::velocity() {return m_velocity;}
float Player::directionDeg() {return m_directionangle;}
float Player::healTimer() {return m_healtimer;}
float Player::air() {return m_air;}

bool Player::chestOpen() {
	return m_chest_open != sf::Vector2i(-1, -1);
}

sf::Vector2i& Player::getChestOpen() {
	return m_chest_open;
}

Buffs& Player::getBuffs() {
	return m_buffs;
}