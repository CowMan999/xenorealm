#include "craft.hpp"

Craft::Craft(Inventory& inventory) : m_inventory(inventory) {
	
	// initialize crafting recipes

	// conversions / bi-directional
	m_craftingrecipes.push_back({Items::GRASS, 1,  {{Items::DIRT, 1}}});
	m_craftingrecipes.push_back({Items::DIRT,  1, {{Items::GRASS, 1}}});
	m_craftingrecipes.push_back({Items::PERMF, 1,  {{Items::SNOW, 1}}});
	m_craftingrecipes.push_back({Items::SNOW,  1, {{Items::PERMF, 1}}});

	// basic blocks
	m_craftingrecipes.push_back({Items::SANDS, 1, {
		{Items::SAND, 2},
	}});

	m_craftingrecipes.push_back({Items::PERMF, 2, {
		{Items::DIRT, 1},
		{Items::ICE, 1},
	}});

	m_craftingrecipes.push_back({Items::WOOD, 2, {
		{Items::BARK, 1},
	}});

	m_craftingrecipes.push_back({Items::TORCH, 6, {
		{Items::COAL, 1},
		{Items::WOOD, 2},
	}});

	m_craftingrecipes.push_back({Items::STONEBR, 2, {
		{Items::STONE, 2},
	}});

	m_craftingrecipes.push_back({Items::CHEST, 1, {
		{Items::WOOD, 8},
		{Items::LEADB, 6},
	}});

	m_craftingrecipes.push_back({Items::SIGN, 1, {
		{Items::WOOD, 6},
	}});

	// ores
	m_craftingrecipes.push_back({Items::LEADB, 1, {
		{Items::LEAD, 1},
		{Items::COAL, 1},
	}});

	m_craftingrecipes.push_back({Items::GOLDB, 1, {
		{Items::GOLD, 1},
		{Items::COAL, 1},
	}});

	m_craftingrecipes.push_back({Items::TITANB, 1, {
		{Items::TITAN, 1},
		{Items::COAL, 1},
	}});

	m_craftingrecipes.push_back({Items::ROSEB, 2, {
		{Items::RGOLD, 1},
		{Items::GOLDB, 1},
		{Items::COAL, 1},
	}});

	m_craftingrecipes.push_back({Items::VINEB, 1, {
		{Items::VINESM, 1},
		{Items::COAL, 1},
	}});

	// tools and gear
	m_craftingrecipes.push_back({Items::LSWORD, 1, { {Items::WOOD, 4}, {Items::LEADB, 12} }});
	m_craftingrecipes.push_back({Items::LPICKAXE, 1, { {Items::WOOD, 6}, {Items::LEADB, 14} }});
	m_craftingrecipes.push_back({Items::LAXE, 1, { {Items::WOOD, 3}, {Items::LEADB, 10} }});
	m_craftingrecipes.push_back({Items::LHELMET, 1, { {Items::LEADB, 12} }});
	m_craftingrecipes.push_back({Items::LCHESTPLATE, 1, { {Items::LEADB, 16} }});
	m_craftingrecipes.push_back({Items::LLEGGINGS, 1, { {Items::LEADB, 12} }});

	m_craftingrecipes.push_back({Items::GSWORD, 1, { {Items::WOOD, 4}, {Items::GOLDB, 10} }});
	m_craftingrecipes.push_back({Items::GPICKAXE, 1, { {Items::WOOD, 6}, {Items::GOLDB, 12} }});
	m_craftingrecipes.push_back({Items::GAXE, 1, { {Items::WOOD, 3}, {Items::GOLDB, 10} }});
	m_craftingrecipes.push_back({Items::GHELMET, 1, { {Items::GOLDB, 12} }});
	m_craftingrecipes.push_back({Items::GCHESTPLATE, 1, { {Items::GOLDB, 14} }});
	m_craftingrecipes.push_back({Items::GLEGGINGS, 1, { {Items::GOLDB, 12} }});

	m_craftingrecipes.push_back({Items::NATUREBL, 1, { {Items::WOOD, 4}, {Items::LEAF, 10}, {Items::GOLDB, 5}, {Items::TITANB, 5} }});

	m_craftingrecipes.push_back({Items::TSWORD, 1, { {Items::LEADB, 6}, {Items::TITANB, 10} }});
	m_craftingrecipes.push_back({Items::TPICKAXE, 1, { {Items::LEADB, 8}, {Items::TITANB, 12} }});
	m_craftingrecipes.push_back({Items::TAXE, 1, { {Items::LEADB, 6}, {Items::TITANB, 10} }});
	m_craftingrecipes.push_back({Items::THELMET, 1, { {Items::LEADB, 6}, {Items::TITANB, 10} }});
	m_craftingrecipes.push_back({Items::TCHESTPLATE, 1, { {Items::LEADB, 8}, {Items::TITANB, 14} }});
	m_craftingrecipes.push_back({Items::TLEGGINGS, 1, { {Items::LEADB, 6}, {Items::TITANB, 12} }});

	m_craftingrecipes.push_back({Items::RSWORD, 1, { {Items::LEADB, 6}, {Items::ROSEB, 18} }});
	m_craftingrecipes.push_back({Items::RPICKAXE, 1, { {Items::LEADB, 8}, {Items::ROSEB, 20} }});
	m_craftingrecipes.push_back({Items::RAXE, 1, { {Items::LEADB, 6}, {Items::ROSEB, 16} }});
	m_craftingrecipes.push_back({Items::RHELMET, 1, { {Items::ROSEB, 18} }});
	m_craftingrecipes.push_back({Items::RCHESTPLATE, 1, { {Items::ROSEB, 22} }});
	m_craftingrecipes.push_back({Items::RLEGGINGS, 1, { {Items::ROSEB, 18} }});

	m_craftingrecipes.push_back({Items::UHELMET, 1, { {Items::BRAIN, 1}, {Items::BONE, 7}, {Items::LEADB, 6}}});
	m_craftingrecipes.push_back({Items::UCHESTPLATE, 1, { {Items::BRAIN, 1}, {Items::BATWING, 7}, {Items::GOLDB, 8}}});
	m_craftingrecipes.push_back({Items::ULEGGINGS, 1, { {Items::CRAWLERLEG, 2}, {Items::BATWING, 4}, {Items::LEADB, 6}}});

	m_craftingrecipes.push_back({Items::BOW, 1, { {Items::TITANB, 10}, {Items::LEADB, 6} }});
	m_craftingrecipes.push_back({Items::RGBOW, 1, { {Items::ROSEB, 16}, {Items::LEADB, 12} }});

	m_craftingrecipes.push_back({Items::TSIGIL, 1, { {Items::STUMP, 1}, {Items::WOOD, 10}, {Items::ROSEB, 2}, {Items::TITANB, 2}, {Items::LEAF, 10} }});

	m_craftingrecipes.push_back({Items::VSWORD, 1, { {Items::VINEB, 12}, {Items::WOOD, 8}, {Items::TCORE, 1} }});
	m_craftingrecipes.push_back({Items::VPICKAXE, 1, { {Items::VINEB, 14}, {Items::WOOD, 10}, {Items::TCORE, 1} }});
	m_craftingrecipes.push_back({Items::VAXE, 1, { {Items::VINEB, 12}, {Items::WOOD, 8}, {Items::TCORE, 1} }});
	m_craftingrecipes.push_back({Items::VHELMET, 1, { {Items::VINEB, 13}, {Items::TCORE, 1} }});
	m_craftingrecipes.push_back({Items::VCHESTPLATE, 1, { {Items::VINEB, 15}, {Items::TCORE, 1} }});
	m_craftingrecipes.push_back({Items::VLEGGINGS, 1, { {Items::VINEB, 14}, {Items::TCORE, 1} }});
	m_craftingrecipes.push_back({Items::VBOW, 1, { {Items::VINEB, 14}, {Items::TCORE, 1} }});

	m_craftingrecipes.push_back({Items::FSWORD, 1, { {Items::VSWORD, 1}, {Items::FCORE, 1} }});
	m_craftingrecipes.push_back({Items::FPICKAXE, 1, { {Items::VPICKAXE, 1}, {Items::FCORE, 1} }});
	m_craftingrecipes.push_back({Items::FAXE, 1, { {Items::VAXE, 1}, {Items::FCORE, 1} }});
	m_craftingrecipes.push_back({Items::FHELMET, 1, { {Items::VHELMET, 1}, {Items::FCORE, 1} }});
	m_craftingrecipes.push_back({Items::FCHESTPLATE, 1, { {Items::VCHESTPLATE, 1}, {Items::FCORE, 1} }});
	m_craftingrecipes.push_back({Items::FLEGGINGS, 1, { {Items::VLEGGINGS, 1}, {Items::FCORE, 1} }});
	m_craftingrecipes.push_back({Items::FBOW, 1, { {Items::VBOW, 1}, {Items::FCORE, 1} }});

	m_craftingrecipes.push_back({Items::FSKULLS, 1, { {Items::BONE, 5}, {Items::FCORE, 1} }});

	m_craftingrecipes.push_back({Items::CSHARD, 4, {{Items::CRYSTAL, 2}, {Items::HELL, 3}}});
	m_craftingrecipes.push_back({Items::CSWORD, 1, { {Items::CSCALE, 3}, {Items::CSHARD, 16}, {Items::ROSEB, 8} }});
	m_craftingrecipes.push_back({Items::CPICKAXE, 1, { {Items::CSCALE, 2}, {Items::CSHARD, 18}, {Items::ROSEB, 10} }});
	m_craftingrecipes.push_back({Items::CAXE, 1, { {Items::CSCALE, 1}, {Items::CSHARD, 16}, {Items::ROSEB, 8} }});
	m_craftingrecipes.push_back({Items::CHELMET, 1, { {Items::CSCALE, 2}, {Items::CSHARD, 17}}});
	m_craftingrecipes.push_back({Items::CCHESTPLATE, 1, { {Items::CSCALE, 2}, {Items::CSHARD, 19}}});
	m_craftingrecipes.push_back({Items::CLEGGINGS, 1, { {Items::CSCALE, 2}, {Items::CSHARD, 17}}});

	m_craftingrecipes.push_back({Items::CBLASTER, 1, { {Items::CSHARD, 16}, {Items::CSCALE, 3}, {Items::ROSEB, 8} }});
	m_craftingrecipes.push_back({Items::CSTAFF, 1, { {Items::FSTAFF, 1}, {Items::CSCALE, 4}, {Items::CSHARD, 12} }});

	m_craftingrecipes.push_back({Items::HELLSEAL, 1, { {Items::CSHARD, 3}, {Items::CSCALE, 1}, {Items::HELL, 8}, {Items::TITANB, 6}, {Items::VINEB, 3} }});
}

Craft::~Craft() {

}

Craft::Code Craft::craft(Recipe r) {

	// remove items
	for (auto& slot : r.req) {
		if(!m_inventory.remove({slot.item, slot.count})) {
			return Code::NOITEMS;
		}
	}

	// add crafted item
	bool suc = m_inventory.append({r.item, r.count});
	if(suc) {
		return Code::OK;
	} else {
		return Code::FULLINV;
	}
}

const std::vector<Recipe> Craft::list() {
	std::vector<Recipe> recipes;

	for (auto& recipe : m_craftingrecipes) {

		// if player can craft this, display
		bool craftable = true;
		for (auto& slot : recipe.req) {
			if(!(m_inventory.count(slot.item) >= slot.count)) {
				craftable = false;
				break;
			}
		}
	
		// append to list
		if(craftable)
			recipes.push_back(recipe);
	}

	return recipes;
}

const std::vector<Recipe> Craft::imp() {
	std::vector<Recipe> recipes;

	for (auto& recipe : m_craftingrecipes) {

		// if player can craft this, display
		bool craftable = true;
		for (auto& slot : recipe.req) {
			if(!(m_inventory.count(slot.item) >= slot.count)) {
				craftable = false;
				break;
			}
		}
	
		// append to list
		if(!craftable)
			recipes.push_back(recipe);
	}

	return recipes;
}

const std::vector<Recipe>& Craft::all() {
	return m_craftingrecipes;
}

Recipe::Recipe(Items::Type item, unsigned short c, std::vector<islot> req) : item(item), req(req), count(c) {

}