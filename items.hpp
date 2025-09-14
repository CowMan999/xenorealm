#include "assets.hpp"
#include "blocks.hpp"

#pragma once

class Player;
class Drops;
class Enemies;
class Inventory;
class Particles;
class Projectiles;

namespace Items
{
	enum Type { // remember you goon put items BEFORE blocks
		NONE,
		
		// items
		SWORD,
		PICKAXE,
		AXE,
		LSWORD, // lead gear
		LPICKAXE,
		LAXE,
		LHELMET,
		LCHESTPLATE,
		LLEGGINGS,
		GSWORD, // gold gear
		GPICKAXE,
		GAXE,
		GHELMET,
		GCHESTPLATE,
		GLEGGINGS,
		TSWORD, // titanium gear
		TPICKAXE,
		TAXE,
		THELMET,
		TCHESTPLATE,
		TLEGGINGS,
		RSWORD, // rose gold gear
		RPICKAXE,
		RAXE,
		RHELMET,
		RCHESTPLATE,
		RLEGGINGS,

		LEADB, // bars
		GOLDB,
		TITANB,
		ROSEB,
		
		BOW,
		RGBOW,

		BATWING,
		BRAIN,
		BONE,
		CRAWLERLEG,
		MUSHROOM,

		UHELMET,
		UCHESTPLATE,
		ULEGGINGS,

		TSIGIL,
		TCORE, // tree core

		// phase two game items

		VINEB, // bars

		VSWORD, // vinesmite gear
		VPICKAXE,
		VAXE,
		VHELMET,
		VCHESTPLATE,
		VLEGGINGS,
		VBOW,
		
		FCORE,
		FSWORD,
		FPICKAXE,
		FAXE,
		FHELMET,
		FCHESTPLATE,
		FLEGGINGS,
		FBOW,

		FSKULLS, // flame skull summon

		MSHIELD, // magic shield
		ANCMASK, // ancient mask
		RHEART, // red heart
		STREYE, // strange eye

		FSTAFF, // flame staff

		CSHARD, // crystal shard
		CSWORD,
		CPICKAXE,
		CAXE,
		CHELMET,
		CCHESTPLATE,
		CLEGGINGS,
		CBLASTER,
		CSTAFF,
		NATUREBL, // nature's blessing
		HELLSEAL, // hell's seal
		HELLWINGS, 
		
		CSCALE,  // crystal scale
 
		ITEMCOUNT,

		// blocks, copy-paste from blocks.hpp
		DIRT = 10000,
		STONE,
		GRASS,
		STUMP,
		BARK,
		LEAF,
		PERMF, // permafrost
		SNOW,
		ICE,
		SLEAF, // snow leaf
		SAND,
		SANDS, // sandstone
		CACTI,
		WOOD,
		LEAD,
		GOLD,
		COAL,
		TITAN, // titanium
		RGOLD, // rose gold
		VINESM, // vinesmite ore
		STONEBR, // stone brick
		STONERB, // ruinded stone brick
		TORCH,
		CHEST,
		HELL, // hellstone
		CRYSTAL,

		WATER,
		LAVA,

		SIGN,

		INVIN,
		COUNT = ITEMCOUNT + Blocks::COUNT
	};
};


typedef std::function<void(Items::Type, Player*, Drops*, Enemies*, Inventory*, Particles*, Projectiles*, float)> Ability;
typedef std::function<bool(Items::Type, Player*, Drops*, Enemies*, Inventory*, Particles*, Projectiles*, float)> AbilityB;
#define ABILITY_PARAMS [[maybe_unused]] Items::Type type, [[maybe_unused]] Player* p , [[maybe_unused]] Drops* d, [[maybe_unused]] Enemies* e, [[maybe_unused]] Inventory* i, [[maybe_unused]] Particles* pt, [[maybe_unused]] Projectiles* pj, [[maybe_unused]] float dt

namespace Items {

	enum class Category {
		NONE,
		BLOCK,
		ITEM
	};

	struct ItemData {
		Type type;
		Assets::Animation anim;
		const char* name;
		const char* data;
		bool swing;
		bool consumable;

		Category category = Category::ITEM;
		AbilityB ability = [](ABILITY_PARAMS) -> bool {return true;};

		ItemData(Type t, Assets::Animation a, const char* n, const char* d, bool s, bool c = false, AbilityB ab = [](ABILITY_PARAMS) -> bool {return true;});
		ItemData() = default;
	};
	
	ItemData& getItemData(Type t);

	Type fromBlock(Blocks::Type t);
	Blocks::Type toBlock(Type t);
	bool innit(); // oi oi, innit bruv, a bloody fookin' fine day if i do bloody say so myself innit
};