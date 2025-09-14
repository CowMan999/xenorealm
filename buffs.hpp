#include "assert.hpp"
#include <vector>

#pragma once

struct Buff {
public:
	enum Type {
		HEALTH,
		ATTACK,
		DEFENCE,
		SPEED,
		JUMP,
		KNOCKBACK_RES,
		JUMP_COUNT,
		COUNT
	};

	Type type;
	float value;
	bool active = false;
	float duration = 0.0f;
};

class Buffs {
public:
	Buffs();

	void add(Buff::Type type, float value, float duration = 0);
	void update(float dt);

	std::vector<Buff>& vector();

	void clear();

private:
	std::vector<Buff> m_buffs;
	size_t m_id = 0;

};