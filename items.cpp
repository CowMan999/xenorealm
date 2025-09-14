#include "items.hpp"

#include "enemies.hpp"
#include "items.hpp"
#include "player.hpp"
#include "inventory.hpp"
#include "particle.hpp"

const AbilityB func_mushroom = [](ABILITY_PARAMS) -> bool {
	if(p->healTimer() == 0) {
		p->setHealth(p->getHealth() + 20);
		p->setHealTimer(30);

		ParticleInit init = PARTICLE_PUFF;
		init.pos = p->getPos();
		init.color = sf::Color(255, 80, 80, 255);
		init.life = 1000;
		init.size = 0.75f;

		pt->create(init);
		return true;
	}

	return false;
};

const AbilityB func_treesigil = [](ABILITY_PARAMS) -> bool {
	auto& vector = e->vector();

	for(auto& enemy : vector) {
		if(enemy->isBoss()) {
			return false;			
		}
	}

	Enemy* boss = e->spawn(EnemyType::TREEGOLEM, p->getPos() - sf::Vector2f(0, e->spawnDistance().y));
	if(p->getWorld().hardMode()) {
		boss->maxHealth() *= HARD_MODE_INC;
		boss->health() *= HARD_MODE_INC;
	}
	
	ParticleInit init = PARTICLE_PUFF;
	init.pos = p->getPos();
	init.color = sf::Color(80, 255, 80, 255);
	init.life = 1000;
	init.size = 0.75f;

	pt->create(init);

	return true;
};

const AbilityB func_flameskullsummon = [](ABILITY_PARAMS) -> bool {
	auto& vector = e->vector();

	for(auto& enemy : vector) {
		if(enemy->isBoss()) {
			return false;			
		}
	}

	Enemy* boss = e->spawn(EnemyType::FLAMESKULL, p->getPos() - sf::Vector2f(0, e->spawnDistance().y));
	if(p->getWorld().hardMode()) {
		boss->maxHealth() *= HARD_MODE_INC;
		boss->health() *= HARD_MODE_INC;
	}

	ParticleInit init = PARTICLE_PUFF;
	init.pos = p->getPos();
	init.color = sf::Color(255, 137, 17, 255);
	init.life = 1000;
	init.size = 0.75f;

	pt->create(init);

	return true;
};

const AbilityB func_hellseal = [](ABILITY_PARAMS) -> bool {
	auto& vector = e->vector();

	for(auto& enemy : vector) {
		if(enemy->isBoss()) {
			return false;			
		}
	}

	if((p->getPos()/(float)TILE_SIZE).y < UNDERWORLD_DISTANCE) {
		return false;
	}

	Enemy* boss = e->spawn(EnemyType::DEMONICVISIONARY, p->getPos() - sf::Vector2f(0, e->spawnDistance().y));
	if(p->getWorld().hardMode()) {
		boss->maxHealth() *= HARD_MODE_INC;
		boss->health() *= HARD_MODE_INC;
	}

	ParticleInit init = PARTICLE_PUFF;
	init.anim = Assets::animation_effect_blood;
	init.pos = p->getPos();
	init.color = sf::Color(255, 0, 0, 255);
	init.life = 1000;
	init.size = 0.75f;

	pt->create(init);

	return true;
};

namespace Items {

	bool loaded = 0;
	std::array<ItemData, COUNT> item_data = { };

	bool innit() {

		item_data = {
			ItemData(Type::NONE, Assets::anim_count, "Blank", "", false),

			ItemData(SWORD, Assets::anim_item_sword, "Sword", "\"A warrior's blade; it needs a good sharpening.\"", true),
			ItemData(PICKAXE, Assets::anim_item_pickaxe, "Pickaxe", "\"An old pickaxe that lacks shine.\"", true),
			ItemData(AXE, Assets::anim_item_axe, "Axe", "\"A dull axe that can still chop trees.\"", true),
	
			ItemData(LSWORD, Assets::anim_item_leadsword, "Lead Sword", "\"A strong blade forged of lead.\"", true),
			ItemData(LPICKAXE, Assets::anim_item_leadpickaxe, "Lead Pickaxe", "\"A pickaxe with the strength of lead.\"", true),
			ItemData(LAXE, Assets::anim_item_leadaxe, "Lead Axe", "\"A sharp axe made with lead.\"", true),
			ItemData(LHELMET, Assets::anim_item_leadhelmet, "Lead Helmet", "A lead helmet.", false),
			ItemData(LCHESTPLATE, Assets::anim_item_leadchestplate, "Lead Chestplate", "A lead chestplate.", false),
			ItemData(LLEGGINGS, Assets::anim_item_leadleggings, "Lead Legings", "A pair of lead leggings.", false),
	
			ItemData(GSWORD, Assets::anim_item_goldsword, "Gold Sword", "\"Midas' prefered weapon.\"", true),
			ItemData(GPICKAXE, Assets::anim_item_goldpickaxe, "Gold Pickaxe", "\"Shines with a blinding light.\"", true),
			ItemData(GAXE, Assets::anim_item_goldaxe, "Gold Axe", "\"Pointed to perfection.\"", true),
			ItemData(GHELMET, Assets::anim_item_goldhelmet, "Gold Helmet", "A gold helmet.", false),
			ItemData(GCHESTPLATE, Assets::anim_item_goldchestplate, "Gold Chestplate", "A gold chestplate.", false),
			ItemData(GLEGGINGS, Assets::anim_item_goldleggings, "Gold Legings", "A pair of gold leggings.", false),
	
			ItemData(TSWORD, Assets::anim_item_titaniumsword, "Titanium Sword", "\"A lightwight yet powerful blade.\"", true),
			ItemData(TPICKAXE, Assets::anim_item_titaniumpickaxe, "Titanium Pickaxe", "\"The strength of titanium bashes through stone.\"", true),
			ItemData(TPICKAXE, Assets::anim_item_titaniumaxe, "Titanium Axe", "\"Slices through trees like butter.\"", true),
			ItemData(GHELMET, Assets::anim_item_titaniumhelmet, "Titanium Helmet", "\"You shoot me down...\"", false),
			ItemData(GCHESTPLATE, Assets::anim_item_titaniumchestplate, "Titanium Chestplate", "\"...but I won't fall...\"", false),
			ItemData(GLEGGINGS, Assets::anim_item_titaniumleggings, "Titanium Legings", "\"I am TITANIUM!\"", false),
	
			ItemData(RSWORD, Assets::anim_item_rosegoldsword, "Rose Gold Sword", "\"Midas seem jealous.\"", true),
			ItemData(RPICKAXE, Assets::anim_item_rosegoldpickaxe, "Rose Gold Pickaxe", "\"Break blocks with style.\"", true),
			ItemData(RAXE, Assets::anim_item_rosegoldaxe, "Rose Gold Axe", "\"Elegance, unmatched.\"", true),
			ItemData(GHELMET, Assets::anim_item_rosegoldhelmet, "Rose Gold Helmet", "A rose gold helmet.", false),
			ItemData(GCHESTPLATE, Assets::anim_item_rosegoldchestplate, "Rose Gold Chestplate", "Wearing the full set will increase\nyour mobility and glamour.", false),
			ItemData(GLEGGINGS, Assets::anim_item_rosegoldleggings, "Rose Gold Legings", "A pair of rose gold leggings.", false),
	
			ItemData(LEADB, Assets::anim_item_leadbar, "Lead Bar", "A bar of lead ore.", false),
			ItemData(GOLDB, Assets::anim_item_goldbar, "Gold Bar", "\"24-carat magic!\"", false),
			ItemData(TITANB, Assets::anim_item_titaniumbar, "Titanium Bar", "\"Durable and lightweight.\"", false),
			ItemData(ROSEB, Assets::anim_item_rosegoldbar, "Rose Gold Bar", "\"Everything is better pink!\"", false),
	
			ItemData(BOW, Assets::anim_item_bow, "Bow", "\"A simple bow.\"", false),
			ItemData(RGBOW, Assets::anim_item_rosegoldbow, "Rose Gold Bow", "\"A bow made of rose gold.\"", false),
	
			ItemData(BATWING, Assets::anim_item_batwing, "Bat Wing", "A bat's wing.", false),
			ItemData(BRAIN, Assets::anim_item_brain, "Brain", "\"It's strangely smooth.\"", false),
			ItemData(BONE, Assets::anim_item_bone, "Bone", "A bone.", false),
			ItemData(CRAWLERLEG, Assets::anim_item_crawlerleg, "Flower Petal", "\"A flower's fallen petal.\"", false),
			ItemData(MUSHROOM, Assets::anim_item_mushroom, "Mushroom", "\"I hear they are magic...\"\nHeals 20 HP.", false, true, func_mushroom),
	
			ItemData(UHELMET, Assets::anim_item_undeadhelmet, "Undead Helmet", "\"A grotesque helmet with a... brain?\"", false),
			ItemData(UCHESTPLATE, Assets::anim_item_undeadchestplate, "Undead Chestplate", "Wearing the full set will slow you down,\nbut regenerate more of your health.", false),
			ItemData(ULEGGINGS, Assets::anim_item_undeadleggings, "Undead Legings", "\"Thrilling in apperance.\"", false),
	
			ItemData(TSIGIL, Assets::anim_item_treesigil, "Nature's Sigil", "\"Awakens a powerful beast from the forest.\"", false, true, func_treesigil),
			ItemData(TCORE, Assets::anim_item_treecore, "Nature's Core", "\"A powerful force of nature, It can fuse vinesmite into strong equipment.\"", false),
	
			ItemData(VINEB, Assets::anim_item_vinesmitebar, "Vinesmite Bar", "A bar of vinesmite.", false),
			ItemData(VSWORD, Assets::anim_item_vinesmitesword, "Vinesmite sword", "\"Mother nature's blade.\"", true),
			ItemData(VPICKAXE, Assets::anim_item_vinesmitepickaxe, "Vinesmite Pickaxe", "\"A natural disaster when it comes to breaking blocks.\"", true),
			ItemData(VAXE, Assets::anim_item_vinesmiteaxe, "Vinesmite Axe", "\"A little counterintuitive, isn't it?\"", true),
			ItemData(VHELMET, Assets::anim_item_vinesmitehelmet, "Vinesmite Helmet", "\"A helmet made of vinesmite.\"", false),
			ItemData(VCHESTPLATE, Assets::anim_item_vinesmitechestplate, "Vinesmite Chestplate", "\"A chestplate made of vinesmite.\"", false),
			ItemData(VLEGGINGS, Assets::anim_item_vinesmiteleggings, "Vinesmite Legings", "\"A pair of leggings made of vinesmite.\"", false),
			ItemData(VBOW, Assets::anim_item_vinesmitebow, "Vinesmite Bow", "\"Shoots two arrows with every shot.\"", false),
			
			ItemData(FCORE, Assets::anim_item_flamecore, "Flame Core", "\"A star in the palm of your hand, It can upgrade vinesmite equipment.\"", false),
			ItemData(FSWORD, Assets::anim_item_flamesword, "Flame Sword", "\"A sword that burns with the power of the sun.\"", true),
			ItemData(FPICKAXE, Assets::anim_item_flamepickaxe, "Flame Pickaxe", "\"Melts and Mines!\"", true),
			ItemData(FAXE, Assets::anim_item_flameaxe, "Flame Axe", "An axe made of vinesmite and a flame core.", true),
			ItemData(FHELMET, Assets::anim_item_flamehelmet, "Flame Helmet", "\"Converts you into a red-head.\"", false),
			ItemData(FCHESTPLATE, Assets::anim_item_flamechestplate, "Flame Chestplate", "Wearing the full set will negate knockback.", false),
			ItemData(FLEGGINGS, Assets::anim_item_flameleggings, "Flame Legings", "\"Radiant protection.\"", false),
			ItemData(FBOW, Assets::anim_item_flamebow, "Flame Bow", "\"Sends out balls of fire.\"", false),
	
			ItemData(FSKULLS, Assets::anim_item_flameskull_summon, "Flame Skull", "\"A skull with the power to awaken an undead monster.\"", false, true, func_flameskullsummon),
	
			ItemData(MSHIELD, Assets::anim_item_magic_shield, "Magic Shield", "\"Now blocks Lynels!\"", false),
			ItemData(ANCMASK, Assets::anim_item_ancient_mask, "Ancient Mask", "\"Holds an endless void within.\"\nShoots vengeful spirits towards the cursor.", false),
			ItemData(RHEART, Assets::anim_item_red_heart, "Red Heart", "\"Full of DETERMINATION.\"\nIncreases health regeneration.", false),
			ItemData(STREYE, Assets::anim_item_strange_eye, "Strange Eye", "\"It stares at you suspiciously.\"", false),
			
			ItemData(FSTAFF, Assets::anim_item_flamestaff, "Flame Staff", "\"A magical staff capable of engulfing any foe in flames.\"", true),
	
			ItemData(CSHARD, Assets::anim_item_crystalshard, "Crystal Shard", "\"A shard of crystalium.\"", false),
			ItemData(CSWORD, Assets::anim_item_crystalsword, "Crystal Sword", "\"A sword pulsing with energy.\"", true),
			ItemData(CPICKAXE, Assets::anim_item_crystalpickaxe, "Crystal Pickaxe", "\"Cracks rock like an egg.\"", true),
			ItemData(CAXE, Assets::anim_item_crystalaxe, "Crystal Axe", "\"Trees don't stand a chance.\"", true),
			ItemData(CHELMET, Assets::anim_item_crystalhelmet, "Crystal Helmet", "\"A helmet made of crystalium.\"", false),
			ItemData(CCHESTPLATE, Assets::anim_item_crystalchestplate, "Crystal Chestplate", "Wearing the full set allows\nyou to jump one extra time", false),
			ItemData(CLEGGINGS, Assets::anim_item_crystalleggings, "Crystal Leggings", "\"A pair of leggings made of crystalium.\"", false),
	
			ItemData(CBLASTER, Assets::anim_item_crystalblaster, "Crystal Blaster", "\"Fires high-speed crystals at the opponent.\"", false),
			ItemData(CSTAFF, Assets::anim_item_crystalstaff, "Crystal Staff", "\"A magical staff capable of firing a barrage of crystals.\"", true),

			ItemData(NATUREBL, Assets::anim_item_natureblessing, "Nature's Blessing", "\"A powerful accessory with the power of nature.\"\nIncreases movement speed and health regen slightly.", false),
			ItemData(HELLSEAL, Assets::anim_item_hellseal, "The Seal of Hell", "\"Seals the power of the underworld within,\nwith the ability to let it lose.\"\nMust be used in the underworld.", false, true, func_hellseal),
			ItemData(HELLWINGS, Assets::anim_item_hellwings, "Hell Wings", "\"Wings of a demonic entity.\"\nAllows flight.", false),

			ItemData(CSCALE, Assets::anim_item_crystalscale, "Crystal Scale", "\"The scale of a reptilian overgrown with crystal.\"", false),
			ItemData(ITEMCOUNT, Assets::anim_count, "Blank", "", false),
		};

		// load item_data array
		for(size_t i = Blocks::DIRT; i < Blocks::COUNT; i++) {
			auto block = Blocks::getBlockData(static_cast<Blocks::Type>(i));
			item_data[ITEMCOUNT+i] = ItemData(fromBlock((Blocks::Type)i), block.anim, block.name, block.data, false);
			item_data[ITEMCOUNT+i].category = Category::BLOCK;
		}

		return true;
	}

	ItemData& getItemData(Type t) {
		int pos = (t <= ITEMCOUNT ? t : (t - DIRT) + ITEMCOUNT+1);
		return item_data[pos];
	}

	ItemData::ItemData(Type t, Assets::Animation a, const char* n, const char* d, bool s, bool c, AbilityB ab) : type(t), anim(a), name(n), data(d), swing(s), consumable(c), ability(ab) {
		if(t >= DIRT && t < COUNT) category = Category::BLOCK;
		else category = Category::ITEM;
	}

	Type fromBlock(Blocks::Type t) {
		return (Type)((int)t+(int)DIRT-1);
	}

	Blocks::Type toBlock(Type t) {
		return (Blocks::Type)((int)t-(int)DIRT+1);
	}

}