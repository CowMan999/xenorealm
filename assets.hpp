#include <SFML/Graphics.hpp>
#include <sfcollider.hpp>
#include <SFML/Audio.hpp>

#include <fstream>
#include <sstream>
#include <functional>
#include <thread>

#include "assert.hpp"

#pragma once

class Assets {

public:
	Assets();
	~Assets();

	// async load function
	void load();
	void cleanup();

	enum Status {
		status_none = -1,
		status_loading,
		status_done,
		status_failed,
	};

	enum Texture {
		texture_player,
		texture_blocks,
		texture_items,
		texture_enemies,
		texture_effects,
		texture_light,
		texture_bosses,

		texture_count
	};

	enum Animation {
		anim_player_idle,
		anim_player_run,
		anim_player_jump,
		anim_player_idleatk,
		anim_player_runatk,
		anim_player_jumpatk,

		anim_armor_lead,
		anim_armor_gold,
		anim_armor_titanium,
		anim_armor_rosegold,
		anim_armor_undead,
		anim_armor_vinesmite,
		anim_armor_flame,
		anim_armor_crystal,
		anim_armor_leadatk,
		anim_armor_goldatk,
		anim_armor_titaniumatk,
		anim_armor_rosegoldatk,
		anim_armor_undeadatk,
		anim_armor_vinesmiteatk,
		anim_armor_flameatk,
		anim_armor_crystalatk,


		anim_zombie,
		anim_bat,
		anim_crawler,
		anim_skeleton,
		anim_dustdevil,
		anim_snowflake,
		anim_mushroom,
		anim_treespirit,
		anim_flameskeleton,
		anim_bloodserpent,
		anim_firespirit,
		anim_eye,

		anim_treegolem_idle,
		anim_treegolem_run,
		anim_treegolem_shrink,

		anim_flameskull,
		anim_flameskull_enraged,

		anim_demonicvisionary,

		animation_effect_puff,
		animation_effect_blood,
		animation_effect_arrow,
		animation_effect_tri,
		animation_effect_fireball,
		animation_effect_shard,
		animation_effect_chunk,

		anim_block_dirt, // all blocks must come after this
		anim_block_stone,
		anim_block_grass,
		anim_block_stump,
		anim_block_bark,
		anim_block_leaf,
		anim_block_permf,
		anim_block_snow,
		anim_block_ice,
		anim_block_sleaf,
		anim_block_sand,
		anim_block_sands,
		anim_block_cacti, 
		anim_block_wood,
		anim_block_lead,
		anim_block_gold,
		anim_block_coal,
		anim_block_titanium,
		anim_block_rose_gold,
		anim_block_vinesmite,
		anim_block_stone_brick,
		anim_block_ruined_stone_brick,
		anim_block_torch,
		anim_block_chest,
		anim_block_hellstone,
		anim_block_crystalium,

		// liquids
		anim_block_water,
		anim_block_lava,

		anim_block_sign,

		anim_block_invincible,
		
		// put all items after here

		anim_item_sword,
		anim_item_pickaxe,
		anim_item_axe,
		anim_item_leadsword,
		anim_item_leadpickaxe,
		anim_item_leadaxe,
		anim_item_leadhelmet,
		anim_item_leadchestplate,
		anim_item_leadleggings,
		anim_item_goldsword,
		anim_item_goldpickaxe,
		anim_item_goldaxe,
		anim_item_goldhelmet,
		anim_item_goldchestplate,
		anim_item_goldleggings,
		anim_item_titaniumsword,
		anim_item_titaniumpickaxe,
		anim_item_titaniumaxe,
		anim_item_titaniumhelmet,
		anim_item_titaniumchestplate,
		anim_item_titaniumleggings,
		anim_item_rosegoldsword,
		anim_item_rosegoldpickaxe,
		anim_item_rosegoldaxe,
		anim_item_rosegoldhelmet,
		anim_item_rosegoldchestplate,
		anim_item_rosegoldleggings,
		
		anim_item_leadbar,
		anim_item_goldbar,
		anim_item_titaniumbar,
		anim_item_rosegoldbar,

		anim_item_bow,
		anim_item_rosegoldbow,

		anim_item_batwing,
		anim_item_brain,
		anim_item_bone,
		anim_item_crawlerleg,
		anim_item_mushroom,

		anim_item_undeadhelmet,
		anim_item_undeadchestplate,
		anim_item_undeadleggings,

		anim_item_treesigil,
		anim_item_treecore,

		anim_item_vinesmitebar,
		anim_item_vinesmitesword,
		anim_item_vinesmitepickaxe,
		anim_item_vinesmiteaxe,
		anim_item_vinesmitehelmet,
		anim_item_vinesmitechestplate,
		anim_item_vinesmiteleggings,
		anim_item_vinesmitebow,

		anim_item_flamecore,

		anim_item_flamesword,
		anim_item_flamepickaxe,
		anim_item_flameaxe,
		anim_item_flamehelmet,
		anim_item_flamechestplate,
		anim_item_flameleggings,
		anim_item_flamebow,

		anim_item_flameskull_summon,

		anim_item_magic_shield,
		anim_item_ancient_mask,
		anim_item_red_heart,
		anim_item_strange_eye,

		anim_item_flamestaff,

		anim_item_crystalshard,
		anim_item_crystalsword,
		anim_item_crystalpickaxe,
		anim_item_crystalaxe,
		anim_item_crystalhelmet,
		anim_item_crystalchestplate,
		anim_item_crystalleggings,

		anim_item_crystalblaster,
		anim_item_crystalstaff,

		anim_item_natureblessing,
		anim_item_hellseal,
		anim_item_hellwings,
		
		anim_item_crystalscale,

		anim_count
	};

	// get status to check when all textures are loaded
	Status getStatus();

	// error handling
	std::string getFail();

	// get textures/animations
	sf::Texture& getTexture(Texture tex);
	std::vector<sf::IntRect>& getAnimation(Animation anim);

	// loading callbacks (add functions to callback list to be called in load sequence)
	void addCallback(std::function<bool(void)> callback, std::string name);

private:
	bool loadAnimation(std::vector<sf::IntRect>& out, const char* name);

	std::array<sf::Texture, texture_count> m_textures;
	std::array<const char*, texture_count> m_texture_names;

	std::array<std::vector<sf::IntRect>, anim_count> m_animations; 
	std::array<sf::IntRect, anim_count-anim_block_dirt> item_anims;
	std::array<const char*, anim_count> m_animation_names;

	Status m_status = status_none;
	size_t m_id_fail_tex = -1;
	size_t m_id_fail_anim = -1;

	int m_callbacks_fail = -1;
	std::vector<std::function<bool(void)>> m_callbacks;
	std::vector<std::string> m_callbacknames;
	std::string m_what;
	std::thread* m_worker;
};