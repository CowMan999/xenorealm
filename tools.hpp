#include "items.hpp"

#pragma once

namespace Tools
{
	enum class Category {
		NONE = -1,
		PICKAXE,
		AXE,
		SWORD,
		SPECIAL, // bows, staffs, anything that needs a lambda for an ability

		COUNT
	};

	struct ToolData {
		Items::Type type;
		Category category;

		float speed = 1;
		unsigned int damage = 1;
		unsigned int level = 0;
		Ability ability = [](ABILITY_PARAMS){};
	};

	bool isTool(Items::Type t);
	const ToolData& getToolData(Items::Type t);

}

namespace Armor
{
	enum class Category {
		NONE = -1,
		HELMET,
		CHESTPLATE,
		LEGGINGS,
		ACCESSORIES,

		COUNT
	};

	struct ArmorData {
		Items::Type type;
		Category category;

		float defence = 1;
		Assets::Animation anim = Assets::anim_armor_lead;
		Ability ability = [](ABILITY_PARAMS){};
	};

	bool isArmor(Items::Type t);
	const ArmorData& getArmorData(Items::Type t);

}

