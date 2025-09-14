#include "blocks.hpp"

namespace Blocks {

	const std::array<BlockData, COUNT> block_data = {
		// NONE
		BlockData(NONE, Assets::anim_count, false, 1, 0, "", "", false, false, false, false),

		// DIRT
		BlockData(DIRT, Assets::anim_block_dirt, true, 0.85, 0, "Dirt", "A block of dirt."),

		// STONE
		BlockData(STONE, Assets::anim_block_stone, true, 1.35, 0, "Stone", "A block of stone."),

		// GRASS
		BlockData(STONE, Assets::anim_block_grass, true, 1, 0, "Grass", "A block of grass."),

		// STUMP
		BlockData(STUMP, Assets::anim_block_stump, false, 1, 0, "Stump", "\"Legends say a little orange creature can come out.\""),

		// BARK
		BlockData(BARK, Assets::anim_block_bark, false, 1, 0, "Bark", "A block of bark."),

		// LEAF
		BlockData(BARK, Assets::anim_block_leaf, false, 0, 0, "Leaf", "A block of leaves.", false),

		// PERMF
		BlockData(PERMF, Assets::anim_block_permf, true, 1.15, 0, "Permafrost", "A block of permafrost."),

		// SNOW
		BlockData(SNOW, Assets::anim_block_snow, true, 1.2, 0, "Snow", "A block of snow."),

		// ICE
		BlockData(ICE, Assets::anim_block_ice, true, 1.4, 0, "Ice", "A block of ice."),

		// SLEAF
		BlockData(SLEAF, Assets::anim_block_sleaf, false, 0, 0, "Snow Leaf", "A block of snowy leaves.", false),

		// SAND
		BlockData(SAND, Assets::anim_block_sand, true, 0.65, 0, "Sand", "A block of sand."),

		// SANDS
		BlockData(SANDS, Assets::anim_block_sands, true, 1.4, 0, "Sandstone", "A block of sandstone."),

		// CACTI
		BlockData(CACTI, Assets::anim_block_cacti, false, 1, 0, "Cacti", "A block of cacti."),

		// WOOD
		BlockData(WOOD, Assets::anim_block_wood, true, 1, 0, "Wood", "A block of wood."),

		// LEAD
		BlockData(LEAD, Assets::anim_block_lead, true, 1.85, 0, "Lead", "A block of lead."),

		// GOLD
		BlockData(GOLD, Assets::anim_block_gold, true, 2, 1, "Gold", "A block of gold."),

		// COAL
		BlockData(COAL, Assets::anim_block_coal, true, 1.6, 0, "Coal", "\"A good fuel soruce.\""),

		// TITAN
		BlockData(TITAN, Assets::anim_block_titanium, true, 3.2, 2, "Titanium", "A block of titanium."),

		// RGOLD
		BlockData(RGOLD, Assets::anim_block_rose_gold, true, 3, 2, "Rose Gold", "A block of rose gold."),

		// VINESM
		BlockData(VINESM, Assets::anim_block_vinesmite, true, 3.5, 3, "Vinesmite", "A block of vinesmite."),

		// STONEBR
		BlockData(STONEBR, Assets::anim_block_stone_brick, true, 1.5, 0, "Stone Brick", "A block of stone bricks."),

		// STONERB
		BlockData(STONERB, Assets::anim_block_ruined_stone_brick, true, 1.5, 0, "Ruined Stone Brick", "\"Aged like milk.\""),

		// TORCH
		BlockData(TORCH, Assets::anim_block_torch, false, 0.35, 0, "Torch", "\"For all your advenuturing needs.\"", true, true, true),

		// CHEST
		BlockData(CHEST, Assets::anim_block_chest, false, 1.5, 0, "Chest", "\"A good place to store your stuff.\""),

		// HELLSTONE
		BlockData(HELL, Assets::anim_block_hellstone, true, 3, 4, "Hellstone", "\"Stone heated by the planet's core.\""),

		// CRYSTAL
		BlockData(CRYSTAL, Assets::anim_block_crystalium, true, 4, 5, "Crystalium", "\"A glowing crystal formed under the\nplanet's most intense conditios.\"", true, false, true),

		// WATER
		BlockData(WATER, Assets::anim_block_water, false, 0, 0, "", "", false, true, false, true),

		// LAVA
		BlockData(LAVA, Assets::anim_block_lava, false, 0, 0, "", "", false, true, true, true),

		// SIGN
		BlockData(SIGN, Assets::anim_block_sign, false, 0.55, 0, "Sign", "\"A good place to write down your thoughts.\"", true, true),

		// INVIN
		BlockData(INVIN, Assets::anim_block_invincible, true, INFINITY, -1, "Unobtanium", "Cheating now, aren't we...\nDon't worry about it, just have fun!"),

	};

	const std::array<sf::Color, COUNT> block_color = {
		sf::Color(BACKGROUND_COLOR[0], BACKGROUND_COLOR[1], BACKGROUND_COLOR[2]), // NONE
		sf::Color(0x8B, 0x45, 0x13), // DIRT
		sf::Color(0x65, 0x65, 0x65), // STONE
		sf::Color(0x00, 0x80, 0x00), // GRASS
		sf::Color(0x8B, 0x45, 0x13), // STUMP
		sf::Color(0x8B, 0x45, 0x13), // BARK
		sf::Color(0x00, 0x80, 0x00), // LEAF
		sf::Color(0xFF, 0xFF, 0xFF), // PERMF
		sf::Color(0xFF, 0xFF, 0xFF), // SNOW
		sf::Color(129, 205, 0xFF), // ICE
		sf::Color(0x00, 0x80, 0x00), // SLEAF
		sf::Color(231, 211, 124), // SAND
		sf::Color(209, 184, 74), // SANDS
		sf::Color(0x00, 0x80, 0x00), // CACTI
		sf::Color(0x8B, 0x45, 0x13), // WOOD
		sf::Color(49, 91, 175), // LEAD
		sf::Color(0xFF, 0xD7, 0x00), // GOLD
		sf::Color(38, 43, 68), // COAL
		sf::Color(230, 243, 255), // TITAN
		sf::Color(181, 80, 136), // RGOLD
		sf::Color(73, 151, 56), // VINESM
		sf::Color(0x75, 0x75, 0x75), // STONEBR
		sf::Color(0x75, 0x75, 0x75), // STONERB
		sf::Color(0xFF, 0x80, 0x00), // TORCH
		sf::Color(0x8B, 0x45, 0x13), // CHEST
		sf::Color(211, 0, 0), // HELLSTONE
		sf::Color(174, 18, 188), // CRYSTAL

		sf::Color(23, 110, 194), // WATER
		sf::Color(255, 138, 0), // LAVA

		sf::Color(0x00, 0x00, 0x00), // SIGN

		sf::Color(0x00, 0x00, 0x00), // INVIN
	};


	const BlockData& getBlockData(Type t) {
		return block_data[t];
	}


	const sf::Color& getBlockColor(Type t) {
		return block_color[t];
	}
	
}