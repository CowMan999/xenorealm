#include "tools.hpp"

#include "enemies.hpp"
#include "items.hpp"
#include "player.hpp"
#include "inventory.hpp"
#include "particle.hpp"

// special weapon/armor abilities
const Ability undead_chestplate = [](ABILITY_PARAMS) {

	static float timer = 0;
	static float timer2 = 0;
	timer += dt; timer2 += dt;

	if((*i)[Armor::Category::HELMET].item == Items::Type::UHELMET && (*i)[Armor::Category::LEGGINGS].item == Items::Type::ULEGGINGS) {
		
		// set equiped, do bonus
		if(timer >= 6) {
			timer -= 6;
			if(p->getHealth() < PLAYER_MAX_HEALTH) {
				p->setHealth(p->getHealth() + 3.5f);

				ParticleInit init = PARTICLE_PUFF;
				init.pos = p->getPos();
				init.color = sf::Color(80, 255, 80, 255);
				init.life = 1000;
				init.size = 0.75f;

				pt->create(init);
			}
		}

		if(timer2 >= 0.75) {
			timer2 -= 0.75;

			if(p->running()) {
				p->getBuffs().add(Buff::SPEED, 0.775f);


				ParticleInit init = PARTICLE_PUFF;
				init.pos = p->getPos() + sf::Vector2f(0, TILE_SIZE);
				init.color = sf::Color(80, 255, 80, 255);
				init.count = 1;
				init.count_var = 0;
				init.outwards = false;

				init.dir = sf::Vector2f(0, 1);

				pt->create(init);
			}
		}
	}
};

const Ability rosegold_chestplate = [](ABILITY_PARAMS) {

	static float timer = 0;
	timer += dt;

	if((*i)[Armor::Category::HELMET].item == Items::Type::RHELMET && (*i)[Armor::Category::LEGGINGS].item == Items::Type::RLEGGINGS) {

		p->getBuffs().add(Buff::SPEED, 1.15f);
		p->getBuffs().add(Buff::JUMP, 1.15f);


		if(timer >= 0.35) {
			timer -= 0.35;

			if(p->running()) {

				ParticleInit init = PARTICLE_PUFF;
				init.pos = p->getPos() + sf::Vector2f(0, TILE_SIZE);
				init.color = sf::Color(212, 109, 65, 255);
				init.count = 1;
				init.count_var = 0;
				init.outwards = false;

				init.dir = sf::Vector2f(0, 1);

				pt->create(init);
			}
		}
	}
};

const Ability flame_chestplate = [](ABILITY_PARAMS) {

	static float timer = 0;

	if((*i)[Armor::Category::HELMET].item == Items::Type::FHELMET && (*i)[Armor::Category::LEGGINGS].item == Items::Type::FLEGGINGS) {
		timer += dt;
		
		// no knockback
		p->getBuffs().add(Buff::KNOCKBACK_RES, INFINITY);

		if(timer >= 0.25) {
			timer -= 0.25;

			ParticleInit init = PARTICLE_PUFF;
			init.pos = p->getPos() + sf::Vector2f(0, TILE_SIZE);
			init.color = sf::Color(255, 137, 17, 255);
			init.life = 900;
			init.size = 0.8;
			init.speed = 75;
			init.gravity = -750;
			init.count = 3;
			init.count_var = 1;


			pt->create(init);
			
		}
	}
};

const Ability crystal_chestplate = [](ABILITY_PARAMS) {

	static float timer = 0;

	if((*i)[Armor::Category::HELMET].item == Items::Type::CHELMET && (*i)[Armor::Category::LEGGINGS].item == Items::Type::CLEGGINGS) {
		timer += dt;
		
		// add jump
		p->getBuffs().add(Buff::JUMP_COUNT, 1);

		static unsigned int old_jump = false;

		if(old_jump != p->jumpcount()) {
			
			if(p->jumpcount() < old_jump) {
				ParticleInit init = PARTICLE_PUFF;
				init.anim = Assets::animation_effect_shard;
				init.pos = p->getPos() + sf::Vector2f(0, TILE_SIZE);
				init.color = sf::Color(174, 18, 188);
				init.life = 900;
				init.size = 0.8;
				init.speed = 75;
				init.gravity = 650;
				init.count = 5;
				init.count_var = 1;
				
				pt->create(init);
			}

			old_jump = p->jumpcount();
		}
	}
};

// bow
const Ability basic_bow = [](ABILITY_PARAMS) {
	
	static float timer = 0;
	timer += dt;


	if(timer >= 1/Tools::getToolData(type).speed) {
		if(type != Items::FSTAFF && type != Items::CSTAFF) timer -= 1/Tools::getToolData(type).speed;
		else timer -= 1/Tools::getToolData(type).speed * Random::getf(.5, .95);

		int count = Tools::getToolData(type).level;
		float off = (float)(count-1)/2.f;
		for(int i = 0; i < count; i++) {
			
			// create arrow
			Projectile proj = PROJECTILE_ARROW;

			if(type == Items::FBOW || type == Items::FSTAFF) {
				proj.anim = Assets::animation_effect_fireball;
				proj.color = sf::Color(255, 100, 10);
				if(type != Items::FSTAFF)
					proj.speed *= 1.35f;
				else proj.speed = Random::get(230, 400);
			} else if(type == Items::CBLASTER || type == Items::CSTAFF) {
				proj.anim = Assets::animation_effect_shard;
				proj.color = sf::Color(174, 18, 188);
				if(type != Items::CSTAFF)
					proj.speed *= 1.25f;
				else proj.speed = Random::get(220, 280);
			}

			// get angle from p's direction, rotate 90 degrees and recreate vector
			float angle = atan2(p->direction().y, p->direction().x) + 3.14159f/2.f;
			sf::Vector2f ndir = sf::Vector2f(cos(angle), sin(angle));

			sf::Vector2f offset = sf::Vector2f(((float)i - off)*ndir.x, ((float)i - off)*ndir.y)*10.f;

			proj.pos = p->getIPos() + offset;
			proj.dir = p->direction();
			proj.damage = Tools::getToolData(type).damage;

			p->getAudio().play(Audio::Sound::SHOOT);
			pj->add(proj);
		}
	}
};

const Ability ancient_mask = [](ABILITY_PARAMS) {

	static float timer = 0;
	static float timer2 = 0;
	timer += dt; timer2 += dt;

	// shoot "vengeful spirit"
	if(timer >= 0.85) {
		timer -= 0.85;

		if(p->attacking() && i->selectedS().item != Items::NONE) {
			Projectile proj = PROJECTILE_ARROW;
			proj.anim = Assets::animation_effect_fireball;
			proj.color = sf::Color(235, 235, 235, 255);
			proj.damage = 8.5f;
			proj.size *= 1.65;
			proj.dir = p->direction();
			proj.pos = p->getIPos();

			p->getAudio().play(Audio::Sound::SHOOT);
			pj->add(proj);
		}
	}

	if(timer2 >= 0.25) {
		timer2 -= 0.25;

		ParticleInit init = PARTICLE_PUFF;
		init.pos = p->getPos() + sf::Vector2f(0, TILE_SIZE);
		init.color = sf::Color(0, 0, 0, 255);
		init.count = 2;
		init.count_var = 0;
		init.gravity = 200;

		pt->create(init);
	}
	
};

const Ability red_heart = [](ABILITY_PARAMS) {
	p->setHealth(p->getHealth() + 0.35f*dt);
};

const Ability nature_blessing = [](ABILITY_PARAMS) {
	p->getBuffs().add(Buff::SPEED, 1.15f);
	p->getBuffs().add(Buff::JUMP, 1.15f);
	p->setHealth(p->getHealth() + 0.125f*dt);
};

const Ability hell_wings = [](ABILITY_PARAMS) {
	p->getBuffs().add(Buff::JUMP_COUNT, 3);
};


namespace Tools {
	const std::map<Items::Type, ToolData> toolData = {
		{Items::Type::SWORD, {Items::Type::SWORD, Category::SWORD, 1, 3}},
		{Items::Type::PICKAXE, {Items::Type::PICKAXE, Category::PICKAXE, 1, 0, 0}},
		{Items::Type::AXE, {Items::Type::AXE, Category::AXE, 1}},

		{Items::Type::LSWORD, {Items::Type::LSWORD, Category::SWORD, 1.65, 5}},
		{Items::Type::LPICKAXE, {Items::Type::LPICKAXE, Category::PICKAXE, 1.25, 0, 1}},
		{Items::Type::LAXE, {Items::Type::LAXE, Category::AXE, 1.35}},

		{Items::Type::GSWORD, {Items::Type::GSWORD, Category::SWORD, 2.5, 7}},
		{Items::Type::GPICKAXE, {Items::Type::GPICKAXE, Category::PICKAXE, 1.6, 0, 2}},
		{Items::Type::GAXE, {Items::Type::GAXE, Category::AXE, 1.65}},

		{Items::Type::TSWORD, {Items::Type::TSWORD, Category::SWORD, 3, 10}}, 
		{Items::Type::TPICKAXE, {Items::Type::TPICKAXE, Category::PICKAXE, 1.9, 0, 3}},
		{Items::Type::TAXE, {Items::Type::TAXE, Category::AXE, 2}},

		{Items::Type::RSWORD, {Items::Type::RSWORD, Category::SWORD, 3.25, 12}},
		{Items::Type::RPICKAXE, {Items::Type::RPICKAXE, Category::PICKAXE, 2, 0, 3}},
		{Items::Type::RAXE, {Items::Type::RAXE, Category::AXE, 2.1}},

		{Items::Type::BOW, {Items::Type::BOW, Category::SPECIAL, 2.1, 5, 1, basic_bow}},
		{Items::Type::RGBOW, {Items::Type::RGBOW, Category::SPECIAL, 2.25, 7, 1, basic_bow}},
		
		{Items::Type::VSWORD, {Items::Type::VSWORD, Category::SWORD, 3.15, 15}},
		{Items::Type::VPICKAXE, {Items::Type::VPICKAXE, Category::PICKAXE, 2.3, 0, 4}},
		{Items::Type::VAXE, {Items::Type::VAXE, Category::AXE, 2.3}},

		{Items::Type::VBOW, {Items::Type::VBOW, Category::SPECIAL, 2.4, 6, 2, basic_bow}},

		{Items::Type::FSWORD, {Items::Type::FSWORD, Category::SWORD, 3.15, 17}},
		{Items::Type::FPICKAXE, {Items::Type::FPICKAXE, Category::PICKAXE, 2.5, 0, 5}},
		{Items::Type::FAXE, {Items::Type::FAXE, Category::AXE, 2.5}},

		{Items::Type::FBOW, {Items::Type::FBOW, Category::SPECIAL, 4.5, 8, 1, basic_bow}},
		{Items::Type::FSTAFF, {Items::Type::FSTAFF, Category::SPECIAL, 5, 9, 1, basic_bow}},

		{Items::Type::CSWORD, {Items::Type::CSWORD, Category::SWORD, 3.15, 20}},
		{Items::Type::CPICKAXE, {Items::Type::CPICKAXE, Category::PICKAXE, 2.7, 0, 6}},
		{Items::Type::CAXE, {Items::Type::CPICKAXE, Category::AXE, 2.7}},

		{Items::Type::CBLASTER, {Items::Type::FBOW, Category::SPECIAL, 4.25, 5, 3, basic_bow}},
		{Items::Type::CSTAFF, {Items::Type::CSTAFF, Category::SPECIAL, 4.25, 9, 2, basic_bow}},

	};

	bool isTool(Items::Type t) {
		return toolData.find(t) != toolData.end();
	}

	const ToolData& getToolData(Items::Type t) {
		return toolData.at(t);
	}

}


namespace Armor {
	const std::map<Items::Type, ArmorData> armorData = {
		{Items::Type::LHELMET, {Items::Type::LHELMET, Category::HELMET, 1, Assets::anim_armor_lead}},
		{Items::Type::LCHESTPLATE, {Items::Type::LCHESTPLATE, Category::CHESTPLATE, 2, Assets::anim_armor_lead}},
		{Items::Type::LLEGGINGS, {Items::Type::LLEGGINGS, Category::LEGGINGS, 1, Assets::anim_armor_lead}},

		{Items::Type::GHELMET, {Items::Type::GHELMET, Category::HELMET, 2, Assets::anim_armor_gold}},
		{Items::Type::GCHESTPLATE, {Items::Type::GCHESTPLATE, Category::CHESTPLATE, 3, Assets::anim_armor_gold}},
		{Items::Type::GLEGGINGS, {Items::Type::GLEGGINGS, Category::LEGGINGS, 1, Assets::anim_armor_gold}},

		{Items::Type::THELMET, {Items::Type::THELMET, Category::HELMET, 4, Assets::anim_armor_titanium}},
		{Items::Type::TCHESTPLATE, {Items::Type::TCHESTPLATE, Category::CHESTPLATE, 4, Assets::anim_armor_titanium}},
		{Items::Type::TLEGGINGS, {Items::Type::TLEGGINGS, Category::LEGGINGS, 4, Assets::anim_armor_titanium}},

		{Items::Type::RHELMET, {Items::Type::RHELMET, Category::HELMET, 3, Assets::anim_armor_rosegold}},
		{Items::Type::RCHESTPLATE, {Items::Type::RCHESTPLATE, Category::CHESTPLATE, 4, Assets::anim_armor_rosegold, rosegold_chestplate}},
		{Items::Type::RLEGGINGS, {Items::Type::RLEGGINGS, Category::LEGGINGS, 3, Assets::anim_armor_rosegold}},

		{Items::Type::UHELMET, {Items::Type::UHELMET, Category::HELMET, 2, Assets::anim_armor_undead}},
		{Items::Type::UCHESTPLATE, {Items::Type::UCHESTPLATE, Category::CHESTPLATE, 3, Assets::anim_armor_undead, undead_chestplate}},
		{Items::Type::ULEGGINGS, {Items::Type::ULEGGINGS, Category::LEGGINGS, 2, Assets::anim_armor_undead}},

		{Items::Type::VHELMET, {Items::Type::VHELMET, Category::HELMET, 5, Assets::anim_armor_vinesmite}},
		{Items::Type::VCHESTPLATE, {Items::Type::VCHESTPLATE, Category::CHESTPLATE, 5, Assets::anim_armor_vinesmite}},
		{Items::Type::VLEGGINGS, {Items::Type::VLEGGINGS, Category::LEGGINGS, 5, Assets::anim_armor_vinesmite}},

		{Items::Type::FHELMET, {Items::Type::FHELMET, Category::HELMET, 6, Assets::anim_armor_flame}},
		{Items::Type::FCHESTPLATE, {Items::Type::FCHESTPLATE, Category::CHESTPLATE, 6, Assets::anim_armor_flame, flame_chestplate}},
		{Items::Type::FLEGGINGS, {Items::Type::FLEGGINGS, Category::LEGGINGS, 5, Assets::anim_armor_flame}},

		// accessories
		{Items::Type::MSHIELD, {Items::Type::MSHIELD, Category::ACCESSORIES, 3, Assets::anim_count}},
		{Items::Type::ANCMASK, {Items::Type::ANCMASK, Category::ACCESSORIES, 1, Assets::anim_count, ancient_mask}},
		{Items::Type::RHEART, {Items::Type::RHEART, Category::ACCESSORIES, 0, Assets::anim_count, red_heart}},
		{Items::Type::STREYE, {Items::Type::STREYE, Category::ACCESSORIES, 0, Assets::anim_count}},

		{Items::Type::CHELMET, {Items::Type::CHELMET, Category::HELMET, 7, Assets::anim_armor_crystal}},
		{Items::Type::CCHESTPLATE, {Items::Type::CCHESTPLATE, Category::CHESTPLATE, 7, Assets::anim_armor_crystal, crystal_chestplate}},
		{Items::Type::CLEGGINGS, {Items::Type::CLEGGINGS, Category::LEGGINGS, 6, Assets::anim_armor_crystal}},

		{Items::Type::NATUREBL, {Items::Type::NATUREBL, Category::ACCESSORIES, 2, Assets::anim_count, nature_blessing}},
		{Items::Type::HELLWINGS, {Items::Type::HELLWINGS, Category::ACCESSORIES, 1, Assets::anim_count, hell_wings}},
	};

	bool isArmor(Items::Type t) {
		return armorData.find(t) != armorData.end();
	}

	const ArmorData& getArmorData(Items::Type t) {
		return armorData.at(t);
	}

}