#include <SFML/Graphics.hpp>
#include <array>
#include <vector>

#include "assets.hpp" 
#include "items.hpp"
#include "const.hpp"
#include "assert.hpp"
#include "tools.hpp"

#pragma once

struct islot {
	Items::Type item = Items::Type::NONE;
	unsigned int count = 0;

	// future data can be put into this struct later, for modifiers, etc
};

class Inventory {

public:
	Inventory();
	~Inventory();

	islot& operator[](size_t i) {
		return m_inventory[i];
	}

	islot& operator[](Armor::Category i) {
		return m_armor[(size_t)i];
	}


	bool append(islot s);
	bool append(islot s, size_t pos);
	bool remove(islot s);
	void remove(Armor::Category s);
	void remove(size_t s, size_t count = 0);
	void select(size_t i);
	islot& acc(size_t i);
	islot& selectedS();
	size_t selected();

	size_t count(Items::Type item);

	void swap(size_t i, size_t j);

	// armor overload
	void swap(size_t i, Armor::Category j);

	// multi-inventory swap
	void multswap(islot& s, islot& d);

	void reset();

private:
	std::array<islot, INVENTORY_SIZE> m_inventory {};
	std::array<islot, (size_t)Armor::Category::COUNT> m_armor {};
	std::array<islot, ACCESSORY_COUNT> m_accessories {};

	size_t m_selected = 0;

};