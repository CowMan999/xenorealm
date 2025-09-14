#include "inventory.hpp"

#pragma once

struct Recipe {
	Items::Type item;
	std::vector<islot> req;
	unsigned short count = 1;

	Recipe(Items::Type item, unsigned short count, std::vector<islot> req);
};

class Craft { 
public:

	enum class Code {
		OK,
		NOITEMS,
		FULLINV
	};

	Craft(Inventory& inventory);
	~Craft();

	Code craft(Recipe r);
	const std::vector<Recipe> list();
	const std::vector<Recipe> imp();
	const std::vector<Recipe>& all();

private:
	Inventory& m_inventory;

	std::vector<Recipe> m_craftingrecipes;
};