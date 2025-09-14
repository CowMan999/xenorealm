#include <SFML/Audio.hpp>

#pragma once

class Audio {

public:
	enum class Song {
		MENU,
		DAY,
		NIGHT,
		BATTLE,

		COUNT
	};

	enum class Sound {
		BREAK,
		HIT,
		JUMP,
		GET,
		SHOOT,

		COUNT
	};

	bool load();
	void play(Song song);
	void play(Sound song);
	void pause();
	void stop();
	void reset();
	void revert();
	
	void setVolume(float v);
	Song playing();

private:
	sf::Music m_music;

	std::array<std::string, (size_t)Song::COUNT> m_songpaths;
	std::array<sf::Time, (size_t)Song::COUNT> m_songlocations;
	std::array<sf::SoundBuffer, (size_t)Sound::COUNT> m_soundbuffers {};
	std::array<sf::Sound, (size_t)Sound::COUNT> m_sounds;

	Song m_playing = Song::COUNT;
	Song m_past = Song::COUNT;
	
};