#include "inventory.hpp"

Inventory::Inventory() {

}

Inventory::~Inventory() {}

bool Inventory::append(islot s) {

	// check if item is stackable
	if(!Tools::isTool(s.item) && !Armor::isArmor(s.item)) {

		// iterate through inventory, search for valid slot to insert item
		for(size_t i = 0; i < INVENTORY_SIZE; i++) {
			if(m_inventory[i].item == s.item) {

				m_inventory[i].count += s.count;
				return true;
			}
		}
	}

	// iterate through inventory once more, search for empty slot to insert item
	for(size_t i = 0; i < INVENTORY_SIZE; i++) {
		if(m_inventory[i].item == Items::Type::NONE) {
			m_inventory[i] = s;
			return true;
		}
	}

	return false;
}

bool Inventory::append(islot s, size_t pos) {
	if(m_inventory[pos].item == Items::Type::NONE) {
		m_inventory[pos] = s;
		return true;
	} else if(m_inventory[pos].item == s.item && !Tools::isTool(m_inventory[pos].item) && !Armor::isArmor(m_inventory[pos].item)) {
		m_inventory[pos].count += s.count;
		return true;
	}

	return false;
}


bool Inventory::remove(islot s) {

	// iterate through inventory once more, search for valid slot with desired quantity of items to remove
	for(size_t i = 0; i < INVENTORY_SIZE; i++) {
		if(m_inventory[i].item == s.item) {
			if(m_inventory[i].count >= s.count)
				m_inventory[i].count -= s.count;
			else
				continue;
			
			if(m_inventory[i].count == 0) {
				m_inventory[i] = {Items::Type::NONE, 0};
			}
			
			return true;
		}
	}

	return false;
}

void Inventory::remove(Armor::Category s) {
	m_armor[(size_t)s] = {Items::Type::NONE, 0};
}

void Inventory::remove(size_t s, size_t count) {
	if(count == 0) {
		m_inventory[s] = {Items::Type::NONE, 0};
		return;
	}
	else {
		m_inventory[s].count -= count;

		if(m_inventory[s].count == 0) {
			m_inventory[s] = {Items::Type::NONE, 0};
		}
	}
}

void Inventory::swap(size_t i, size_t j) {

	if(i == j)
		return;

	if(m_inventory[i].item != m_inventory[j].item || Tools::isTool(m_inventory[i].item) || Armor::isArmor(m_inventory[i].item)) {
		islot temp = m_inventory[i];
		m_inventory[i] = m_inventory[j];
		m_inventory[j] = temp;
	} else {
		m_inventory[j].count += m_inventory[i].count;
		m_inventory[i] = {Items::Type::NONE, 0};
	}
}

void Inventory::swap(size_t i, Armor::Category j) {
	islot temp = m_inventory[i];
	m_inventory[i] = m_armor[(size_t)j];
	m_armor[(size_t)j] = temp;
}


void Inventory::select(size_t i) {
	m_selected = i;
}

islot& Inventory::selectedS() {
	return (m_selected != INVENTORY_SIZE ? m_inventory[m_selected] : m_armor[(size_t)Armor::Category::ACCESSORIES]);
}

size_t Inventory::selected() {
	return m_selected;
}

size_t Inventory::count(Items::Type item) {
	size_t count = 0;

	for(size_t i = 0; i < INVENTORY_SIZE; i++) {
		if(m_inventory[i].item == item) {
			count += m_inventory[i].count;
		}
	}

	return count;
}

islot& Inventory::acc(size_t i) {
	return m_accessories[i];
}

void Inventory::multswap(islot& s, islot& d) {
	if(&s == &d)
		return;
		
	if(s.item != d.item || Tools::isTool(s.item) || Armor::isArmor(s.item)) {
		islot temp = s;
		s = d;
		d = temp;
	} else {
		s.count += d.count;
		d = {Items::Type::NONE, 0};
	}
}


void Inventory::reset() {
	for(size_t i = 0; i < INVENTORY_SIZE; i++) {
		m_inventory[i] = {Items::Type::NONE, 0};
	}

	for(size_t i = 0; i < (size_t)Armor::Category::ACCESSORIES; i++) {
		m_armor[i] = {Items::Type::NONE, 0};
	}

	for(size_t i = 0; i < ACCESSORY_COUNT; i++) {
		m_accessories[i] = {Items::Type::NONE, 0};
	}

	m_selected = 0;

	m_inventory[0] = {Items::Type::SWORD, 1};
	m_inventory[1] = {Items::Type::PICKAXE, 1};
	m_inventory[2] = {Items::Type::AXE, 1};

}