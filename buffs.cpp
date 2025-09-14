#include "buffs.hpp"

Buffs::Buffs() {

}

void Buffs::add(Buff::Type type, float value, float duration) {
	
	// check if buff already exists at id
	if(m_buffs.size() > m_id && m_buffs[m_id].type == type && m_buffs[m_id].value == value) {
		m_buffs[m_id].duration = duration;
		m_buffs[m_id].active = true;
		m_id++;
		return;
	}

	Buff buff;
	buff.type = type;
	buff.value = value;
	buff.duration = duration;
	buff.active = true;

	m_buffs.push_back(buff);

	m_id++;
}

void Buffs::update(float dt) {
	m_id = 0;

	for(size_t i = 0; i < m_buffs.size(); i++) {
		Buff& buff = m_buffs[i];

		if(!buff.active) {
			m_buffs.erase(m_buffs.begin() + i);
			i--;
		}

		if(buff.duration == 0.0f) {
			if(buff.active) {
				buff.active = false;
			}
		}

		if(buff.duration > 0) {
			buff.duration -= dt;
			if(buff.duration <= 0) {
				buff.active = false;
			}
		}
	}

}

std::vector<Buff>& Buffs::vector() {
	return m_buffs;
}

void Buffs::clear() {
	m_buffs.clear();
}