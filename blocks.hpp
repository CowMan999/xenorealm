#include "assets.hpp"
#include "const.hpp"
#include "math.h"

#pragma once

namespace Blocks {

	enum Type {
		NONE,
		DIRT,
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
		CRYSTAL, // crystalium

		WATER,
		LAVA,

		SIGN,

		INVIN, // invincible block
		COUNT
	};


	struct BlockData {
		Type type;
		Assets::Animation anim;
		bool solid;
		float hardness;
		unsigned int level;

		const char* name, *data;
		bool relevant = true;
		bool alpha = false;
		bool light = false;
		bool liquid = false;
	};
	
	const BlockData& getBlockData(Type t);

	const sf::Color& getBlockColor(Type t);

}