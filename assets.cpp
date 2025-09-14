#include "assets.hpp"

Assets::Assets() {
	
	// hardcoded texture names
	m_texture_names[texture_player] = "./assets/spritesheets/player.png";
	m_texture_names[texture_blocks] = "./assets/spritesheets/blocks.png";
	m_texture_names[texture_items] = "./assets/spritesheets/items.png";
	m_texture_names[texture_enemies] = "./assets/spritesheets/enemies.png";
	m_texture_names[texture_effects] = "./assets/spritesheets/effects.png";
	m_texture_names[texture_bosses] = "./assets/spritesheets/bosses.png";
	m_texture_names[texture_light] = "./assets/effects/light.png";
	
	// hardcoded animation names
	m_animation_names[anim_player_idle] = "./assets/player/idle.dat";
	m_animation_names[anim_player_run] = "./assets/player/run.dat";
	m_animation_names[anim_player_jump] = "./assets/player/jump.dat";
	m_animation_names[anim_player_idleatk] = "./assets/player/idleatk.dat";
	m_animation_names[anim_player_runatk] = "./assets/player/runatk.dat";
	m_animation_names[anim_player_jumpatk] = "./assets/player/jumpatk.dat";

	m_animation_names[anim_armor_lead] = "./assets/armor/lead.dat";
	m_animation_names[anim_armor_gold] = "./assets/armor/gold.dat";
	m_animation_names[anim_armor_titanium] = "./assets/armor/titanium.dat";
	m_animation_names[anim_armor_rosegold] = "./assets/armor/rosegold.dat";
	m_animation_names[anim_armor_undead] = "./assets/armor/undead.dat";
	m_animation_names[anim_armor_vinesmite] = "./assets/armor/vinesmite.dat";
	m_animation_names[anim_armor_flame] = "./assets/armor/flame.dat";
	m_animation_names[anim_armor_crystal] = "./assets/armor/crystal.dat";
	m_animation_names[anim_armor_leadatk] = "./assets/armor/leadatk.dat";
	m_animation_names[anim_armor_goldatk] = "./assets/armor/goldatk.dat";
	m_animation_names[anim_armor_titaniumatk] = "./assets/armor/titaniumatk.dat";
	m_animation_names[anim_armor_rosegoldatk] = "./assets/armor/rosegoldatk.dat";
	m_animation_names[anim_armor_undeadatk] = "./assets/armor/undeadatk.dat";
	m_animation_names[anim_armor_vinesmiteatk] = "./assets/armor/vinesmiteatk.dat";
	m_animation_names[anim_armor_flameatk] = "./assets/armor/flameatk.dat";
	m_animation_names[anim_armor_crystalatk] = "./assets/armor/crystalatk.dat";

	m_animation_names[anim_zombie] = "./assets/enemies/zombie.dat";
	m_animation_names[anim_bat] = "./assets/enemies/bat.dat";
	m_animation_names[anim_crawler] = "./assets/enemies/crawler.dat";
	m_animation_names[anim_skeleton] = "./assets/enemies/skeleton.dat";
	m_animation_names[anim_dustdevil] = "./assets/enemies/dustdevil.dat";
	m_animation_names[anim_snowflake] = "./assets/enemies/snowflake.dat";
	m_animation_names[anim_mushroom] = "./assets/enemies/mushroom.dat";
	m_animation_names[anim_treespirit] = "./assets/enemies/treespirit.dat";
	m_animation_names[anim_flameskeleton] = "./assets/enemies/flameskeleton.dat";
	m_animation_names[anim_bloodserpent] = "./assets/enemies/bloodserpent.dat";
	m_animation_names[anim_firespirit] = "./assets/enemies/firespirit.dat";
	m_animation_names[anim_eye] = "./assets/enemies/eye.dat";

	m_animation_names[anim_treegolem_idle] = "./assets/bosses/treegolemidle.dat";
	m_animation_names[anim_treegolem_run] = "./assets/bosses/treegolemrun.dat";
	m_animation_names[anim_treegolem_shrink] = "./assets/bosses/treegolemshrink.dat";

	m_animation_names[anim_flameskull] = "./assets/bosses/flameskull.dat";
	m_animation_names[anim_flameskull_enraged] = "./assets/bosses/flameskullenraged.dat";

	m_animation_names[anim_demonicvisionary] = "./assets/bosses/demonicvisionary.dat";

	m_animation_names[animation_effect_puff] = "./assets/effects/puff.dat";
	m_animation_names[animation_effect_blood] = "./assets/effects/blood.dat";
	m_animation_names[animation_effect_arrow] = "./assets/effects/arrow.dat";
	m_animation_names[animation_effect_tri] = "./assets/effects/tri.dat";
	m_animation_names[animation_effect_fireball] = "./assets/effects/fireball.dat";
	m_animation_names[animation_effect_shard] = "./assets/effects/shard.dat";
	m_animation_names[animation_effect_chunk] = "./assets/effects/chunk.dat";

	// block/item animations
	constexpr auto offset = anim_block_dirt;
	item_anims[anim_block_dirt-offset] = sf::IntRect(0, 0, 16, 16);
	item_anims[anim_block_stone-offset] = sf::IntRect(16, 0, 16, 16);
	item_anims[anim_block_grass-offset] = sf::IntRect(32, 0, 16, 16);
	item_anims[anim_block_stump-offset] = sf::IntRect(48, 0, 16, 16);
	item_anims[anim_block_bark-offset] = sf::IntRect(64, 0, 16, 16);
	item_anims[anim_block_leaf-offset] = sf::IntRect(80, 0, 16, 16);
	item_anims[anim_block_permf-offset] = sf::IntRect(0, 16, 16, 16);
	item_anims[anim_block_snow-offset] = sf::IntRect(32, 16, 16, 16);
	item_anims[anim_block_ice-offset] = sf::IntRect(16, 16, 16, 16);
	item_anims[anim_block_sleaf-offset] = sf::IntRect(48, 16, 16, 16);
	item_anims[anim_block_sand-offset] = sf::IntRect(64, 16, 16, 16);
	item_anims[anim_block_sands-offset] = sf::IntRect(80, 16, 16, 16);
	item_anims[anim_block_cacti-offset] = sf::IntRect(96, 16, 16, 16);
	item_anims[anim_block_wood-offset] = sf::IntRect(96, 0, 16, 16);
	item_anims[anim_block_lead-offset] = sf::IntRect(0, 32, 16, 16);
	item_anims[anim_block_gold-offset] = sf::IntRect(16, 32, 16, 16);
	item_anims[anim_block_coal-offset] = sf::IntRect(32, 32, 16, 16);
	item_anims[anim_block_titanium-offset] = sf::IntRect(48, 32, 16, 16);
	item_anims[anim_block_rose_gold-offset] = sf::IntRect(64, 32, 16, 16);
	item_anims[anim_block_vinesmite-offset] = sf::IntRect(80, 32, 16, 16);
	item_anims[anim_block_stone_brick-offset] = sf::IntRect(112, 0, 16, 16);
	item_anims[anim_block_ruined_stone_brick-offset] = sf::IntRect(112, 16, 16, 16);
	item_anims[anim_block_torch-offset] = sf::IntRect(96, 32, 16, 16);
	item_anims[anim_block_chest-offset] = sf::IntRect(112, 32, 16, 16);
	item_anims[anim_block_hellstone-offset] = sf::IntRect(16, 48, 16, 16);
	item_anims[anim_block_crystalium-offset] = sf::IntRect(48, 48, 16, 16);

	item_anims[anim_block_water-offset] = sf::IntRect(64, 48, 16, 16);
	item_anims[anim_block_lava-offset] = sf::IntRect(32, 48, 16, 16);
	item_anims[anim_block_sign-offset] = sf::IntRect(80, 48, 16, 16);

	item_anims[anim_block_invincible-offset] = sf::IntRect(0, 48, 16, 16);

	item_anims[anim_item_sword-offset] = sf::IntRect(0, 0, 16, 16);
	item_anims[anim_item_pickaxe-offset] = sf::IntRect(16, 0, 16, 16);
	item_anims[anim_item_axe-offset] = sf::IntRect(32, 0, 16, 16);

	item_anims[anim_item_leadsword-offset] = sf::IntRect(0, 16, 16, 16);
	item_anims[anim_item_leadpickaxe-offset] = sf::IntRect(16, 16, 16, 16);
	item_anims[anim_item_leadaxe-offset] = sf::IntRect(32, 16, 16, 16);
	item_anims[anim_item_leadhelmet-offset] = sf::IntRect(48, 16, 16, 16);
	item_anims[anim_item_leadchestplate-offset] = sf::IntRect(48, 32, 16, 16);
	item_anims[anim_item_leadleggings-offset] = sf::IntRect(48, 48, 16, 16);

	item_anims[anim_item_goldsword-offset] = sf::IntRect(0, 32, 16, 16);
	item_anims[anim_item_goldpickaxe-offset] = sf::IntRect(16, 32, 16, 16);
	item_anims[anim_item_goldaxe-offset] = sf::IntRect(32, 32, 16, 16);
	item_anims[anim_item_goldhelmet-offset] = sf::IntRect(64, 16, 16, 16);
	item_anims[anim_item_goldchestplate-offset] = sf::IntRect(64, 32, 16, 16);
	item_anims[anim_item_goldleggings-offset] = sf::IntRect(64, 48, 16, 16);

	item_anims[anim_item_titaniumsword-offset] = sf::IntRect(0, 48, 16, 16);
	item_anims[anim_item_titaniumpickaxe-offset] = sf::IntRect(16, 48, 16, 16);
	item_anims[anim_item_titaniumaxe-offset] = sf::IntRect(32, 48, 16, 16);
	item_anims[anim_item_titaniumhelmet-offset] = sf::IntRect(80, 16, 16, 16);
	item_anims[anim_item_titaniumchestplate-offset] = sf::IntRect(80, 32, 16, 16);
	item_anims[anim_item_titaniumleggings-offset] = sf::IntRect(80, 48, 16, 16);

	item_anims[anim_item_rosegoldsword-offset] = sf::IntRect(0, 64, 16, 16);
	item_anims[anim_item_rosegoldpickaxe-offset] = sf::IntRect(16, 64, 16, 16);
	item_anims[anim_item_rosegoldaxe-offset] = sf::IntRect(32, 64, 16, 16);
	item_anims[anim_item_rosegoldhelmet-offset] = sf::IntRect(112, 16, 16, 16);
	item_anims[anim_item_rosegoldchestplate-offset] = sf::IntRect(112, 32, 16, 16);
	item_anims[anim_item_rosegoldleggings-offset] = sf::IntRect(112, 48, 16, 16);

	item_anims[anim_item_leadbar-offset] = sf::IntRect(48, 0, 16, 16);
	item_anims[anim_item_goldbar-offset] = sf::IntRect(64, 0, 16, 16);
	item_anims[anim_item_titaniumbar-offset] = sf::IntRect(80, 0, 16, 16);
	item_anims[anim_item_rosegoldbar-offset] = sf::IntRect(48, 64, 16, 16);

	item_anims[anim_item_bow-offset] = sf::IntRect(80, 64, 16, 16);
	item_anims[anim_item_rosegoldbow-offset] = sf::IntRect(64, 64, 16, 16);

	item_anims[anim_item_batwing-offset] = sf::IntRect(96, 0, 16, 16);
	item_anims[anim_item_brain-offset] = sf::IntRect(112, 0, 16, 16);
	item_anims[anim_item_bone-offset] = sf::IntRect(112, 64, 16, 16);
	item_anims[anim_item_crawlerleg-offset] = sf::IntRect(96, 64, 16, 16);
	item_anims[anim_item_mushroom-offset] = sf::IntRect(112, 80, 16, 16);

	item_anims[anim_item_undeadhelmet-offset] = sf::IntRect(96, 16, 16, 16);
	item_anims[anim_item_undeadchestplate-offset] = sf::IntRect(96, 32, 16, 16);
	item_anims[anim_item_undeadleggings-offset] = sf::IntRect(96, 48, 16, 16);

	item_anims[anim_item_treesigil-offset] = sf::IntRect(96, 80, 16, 16);
	item_anims[anim_item_treecore-offset] = sf::IntRect(80, 80, 16, 16);

	item_anims[anim_item_vinesmitebar-offset] = sf::IntRect(48, 80, 16, 16);
	item_anims[anim_item_vinesmitesword-offset] = sf::IntRect(0, 80, 16, 16);
	item_anims[anim_item_vinesmitepickaxe-offset] = sf::IntRect(16, 80, 16, 16);
	item_anims[anim_item_vinesmiteaxe-offset] = sf::IntRect(32, 80, 16, 16);
	item_anims[anim_item_vinesmitehelmet-offset] = sf::IntRect(128, 16, 16, 16);
	item_anims[anim_item_vinesmitechestplate-offset] = sf::IntRect(128, 32, 16, 16);
	item_anims[anim_item_vinesmiteleggings-offset] = sf::IntRect(128, 48, 16, 16);
	item_anims[anim_item_vinesmitebow-offset] = sf::IntRect(64, 80, 16, 16);

	item_anims[anim_item_flamecore-offset] = sf::IntRect(48, 96, 16, 16);

	item_anims[anim_item_flamesword-offset] = sf::IntRect(0, 96, 16, 16);
	item_anims[anim_item_flamepickaxe-offset] = sf::IntRect(16, 96, 16, 16);
	item_anims[anim_item_flameaxe-offset] = sf::IntRect(32, 96, 16, 16);
	item_anims[anim_item_flamehelmet-offset] = sf::IntRect(144, 16, 16, 16);
	item_anims[anim_item_flamechestplate-offset] = sf::IntRect(144, 32, 16, 16);
	item_anims[anim_item_flameleggings-offset] = sf::IntRect(144, 48, 16, 16);
	item_anims[anim_item_flamebow-offset] = sf::IntRect(64, 96, 16, 16);

	item_anims[anim_item_flameskull_summon-offset] = sf::IntRect(80, 96, 16, 16);

	item_anims[anim_item_magic_shield-offset] = sf::IntRect(128, 64, 16, 16);
	item_anims[anim_item_ancient_mask-offset] = sf::IntRect(144, 64, 16, 16);
	item_anims[anim_item_red_heart-offset] = sf::IntRect(128, 80, 16, 16);
	item_anims[anim_item_strange_eye-offset] = sf::IntRect(144, 80, 16, 16);

	item_anims[anim_item_flamestaff-offset] = sf::IntRect(96, 96, 16, 16);

	item_anims[anim_item_crystalshard-offset] = sf::IntRect(128, 0, 16, 16);
	item_anims[anim_item_crystalsword-offset] = sf::IntRect(0, 112, 16, 16);
	item_anims[anim_item_crystalpickaxe-offset] = sf::IntRect(16, 112, 16, 16);
	item_anims[anim_item_crystalaxe-offset] = sf::IntRect(32, 112, 16, 16);
	item_anims[anim_item_crystalhelmet-offset] = sf::IntRect(160, 16, 16, 16);
	item_anims[anim_item_crystalchestplate-offset] = sf::IntRect(160, 32, 16, 16);
	item_anims[anim_item_crystalleggings-offset] = sf::IntRect(160, 48, 16, 16);

	item_anims[anim_item_crystalblaster-offset] = sf::IntRect(64, 112, 16, 16);
	item_anims[anim_item_crystalstaff-offset] = sf::IntRect(48, 112, 16, 16);

	item_anims[anim_item_natureblessing-offset] = sf::IntRect(112, 96, 16, 16);
	item_anims[anim_item_hellseal-offset] = sf::IntRect(128, 96, 16, 16);
	item_anims[anim_item_hellwings-offset] = sf::IntRect(80, 112, 32, 32);

	item_anims[anim_item_crystalscale-offset] = sf::IntRect(144, 0, 16, 16);
}

Assets::~Assets() {

}

// async load with info given to outside
void Assets::load() {
	m_worker = new std::thread([this]() {
		m_status = status_loading;

		for (size_t i = 0; i < texture_count; i++)
		{
			// don't load certain sprites normaly
			if(i == texture_blocks) {
				bool done = m_textures[i].loadFromFile(m_texture_names[i]);

				if(!done) {
					m_id_fail_tex = i;
					break;
				}
			} else { // use collision detection loading function
				bool done = Collision::createTextureAndBitmask(m_textures[i], m_texture_names[i]);

				if(!done) {
					m_id_fail_tex = i;
					break;
				}
			}
		}

		for (size_t i = 0; i < anim_count; i++)
		{
			if(i >= anim_block_dirt) { // don't load item animations from files, get them from the global array
				m_animations[i].push_back(item_anims[i-anim_block_dirt]);
				continue;
			}

			bool done = loadAnimation(m_animations[i], m_animation_names[i]);
			
			if(!done) {
				m_id_fail_anim = i;
				break;
			}
		}
		
		size_t last = 0;

		try {

			for(size_t i = 0; i < m_callbacks.size(); i++) {
				last = i;
				if(!m_callbacks[i]()) {
					m_callbacks_fail = i;
				}

				if(m_callbacks_fail != -1) {
					break;
				}
			}

		} catch(std::exception& e) {
			m_callbacks_fail = last;
			m_what = e.what();
		}

		if(m_id_fail_tex == static_cast<size_t>(-1) && m_id_fail_anim == static_cast<size_t>(-1) && m_callbacks_fail == -1)
			m_status = status_done;
		else m_status = status_failed;
	});

	//std::this_thread::sleep_for(std::chrono::milliseconds(100));

}

Assets::Status Assets::getStatus() {
	return m_status;
}

void Assets::cleanup()
{
	m_worker->join();
	delete m_worker;
	m_status = status_none;
}

std::string Assets::getFail() {
	std::string msg;
	if(m_id_fail_tex != static_cast<size_t>(-1))
		msg = m_texture_names[m_id_fail_tex];
	else if(m_id_fail_anim != static_cast<size_t>(-1))
		msg = m_animation_names[m_id_fail_anim];
	else if(m_callbacks_fail != -1)
		msg = "Predefined callback failed: " + m_callbacknames[m_callbacks_fail] + " " + m_what;
	else msg = "unknown error";

	return msg;
}

sf::Texture& Assets::getTexture(Texture tex) {
	return m_textures[tex];
}

std::vector<sf::IntRect>& Assets::getAnimation(Animation anim) {
	if(anim >= anim_count) {
		return m_animations[anim_block_dirt];
	}

	return m_animations[anim];
}

bool Assets::loadAnimation(std::vector<sf::IntRect>& out, const char* name) {
	try {
		std::ifstream file(name);

		ASSERT(file.is_open());

		std::string line;
		while(std::getline(file, line)) {
			std::stringstream stream(line);

			unsigned int x{}, y{}, w{}, h{};
			stream >> x >> y >> w >> h;
			out.push_back(sf::IntRect(x, y, w, h));
		}

	} catch(const std::exception& e) {
		return false;
	}
	return true;
}

void Assets::addCallback(std::function<bool(void)> callback, std::string name) {
	m_callbacks.push_back(callback);
	m_callbacknames.push_back(name);
}
