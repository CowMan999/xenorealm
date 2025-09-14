#include "audio.hpp"

bool Audio::load() {
	m_songpaths = {
		"assets/audio/menu.mp3",
		"assets/audio/day.mp3",
		"assets/audio/night.mp3",
		"assets/audio/battle.mp3"
	};

	if(!m_soundbuffers[(size_t)Sound::BREAK].loadFromFile("assets/audio/break.wav")) {
		throw std::runtime_error("Failed to load sound file: assets/audio/break.wav");
	} if(!m_soundbuffers[(size_t)Sound::HIT].loadFromFile("assets/audio/hit.wav")) {
		throw std::runtime_error("Failed to load sound file: assets/audio/hit.wav");
	} if(!m_soundbuffers[(size_t)Sound::JUMP].loadFromFile("assets/audio/jump.wav")) {
		throw std::runtime_error("Failed to load sound file: assets/audio/jump.wav");
	} if(!m_soundbuffers[(size_t)Sound::GET].loadFromFile("assets/audio/get.wav")) {
		throw std::runtime_error("Failed to load sound file: assets/audio/get.wav");
	} if(!m_soundbuffers[(size_t)Sound::SHOOT].loadFromFile("assets/audio/shoot.wav")) {
		throw std::runtime_error("Failed to load sound file: assets/audio/shoot.wav");
	}

	for(size_t i = 0; i < (size_t)Sound::COUNT; i++) {
		m_sounds[i].setBuffer(m_soundbuffers[i]);
	}

	m_music.setLoop(true);

	// load once sounds are a thing
	return true;
}

void Audio::play(Song song) {


	if(m_playing == song) {
		return;
	} else if(m_playing != Song::COUNT) {
		m_songlocations[(size_t)m_playing] = m_music.getPlayingOffset();
	}

	m_past = m_playing;

	if(!m_music.openFromFile(m_songpaths[(size_t)song])) {
		throw std::runtime_error("Failed to load music file: " + m_songpaths[(size_t)song]);
	}
	m_music.play();

	if(m_songlocations[(size_t)song] != sf::Time::Zero) {
		m_music.setPlayingOffset(m_songlocations[(size_t)song]);
	}

	m_playing = song;
}

void Audio::play(Sound sound) {
	m_sounds[(size_t)sound].play();
}

void Audio::pause() {
	m_music.pause();
}

void Audio::stop() {
	m_music.stop();
}

void Audio::reset() {
	stop();
	m_playing = Song::COUNT;
	for(auto& i : m_songlocations) {
		i = sf::Time::Zero;
	}
}

void Audio::setVolume(float volume) {
	m_music.setVolume(volume);
	
	for(auto& i : m_sounds) {
		i.setVolume(volume);
	}
}

Audio::Song Audio::playing() {
	if(m_music.getStatus() == sf::Music::Status::Playing) {
		return m_playing;
	} else {
		return Song::COUNT;
	}
}

void Audio::revert() {
	if(m_past != Song::COUNT) {
		play(m_past);
	}
}