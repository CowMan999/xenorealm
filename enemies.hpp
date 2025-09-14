#include <SFML/Graphics.hpp>
#include "sfcollider.hpp"

#include "assets.hpp"
#include "const.hpp"
#include "world.hpp"
#include "drops.hpp"
#include "random.hpp"
#include "particle.hpp"
#include "settings.hpp"
#include "projectiles.hpp"

class Player;

#pragma once

// enum for enemies
enum class EnemyType {
	ZOMBIE,
	BAT,
	CRAWLER,
	SKELETON,
	DUSTDEVIL,
	SNOWFLAKE,
	MUSHROOM,
	TREESPIRIT,
	FLAMESKELETON,
	BLOODSERPENT,
	FIRESPIRIT,
	EYE,

	// bosses
	TREEGOLEM,
	FLAMESKULL,
	DEMONICVISIONARY,
};

#define ENEMY_PARAMS Assets& assets, World& world, Drops& drops, Particles& particles, Projectiles& projectiles, sf::Vector2f pos

// abstracted to death for now, child classes contain next to everything
class Enemy {
public:
	Enemy(ENEMY_PARAMS, float hp, float defence, float damage, sf::FloatRect off, EnemyType type);
	virtual ~Enemy();
	
	// don't call this manualy, called on enemy death; ofc you can just spontaniously combust an enemy with this tho ;)
	virtual void died() = 0;
	virtual void update(float dt, Player* player, Enemies& enemies) = 0;

	void draw(sf::RenderWindow& window);
	
	bool damage(float dmg, sf::Vector2f dir, bool projectile = false);
	EnemyType type();

	bool collided(sf::Sprite& sprite);

	void updateBase(float dt, sf::Vector2f oldpos, float vel_off = 0, unsigned char quant = 1, std::function<void()> hor_col = [](){}, std::function<void()> landed = [](){});

	// info about the enemy
	float& health();
	float& defence();
	float& attack();
	sf::FloatRect bounds();
	bool right();
	sf::Vector2f& position();
	sf::Vector2f& velocity();
	bool isBoss();
	float& maxHealth();


protected:
	sf::Sprite m_sprite;

	Assets& m_assets;
	World& m_world;
	Drops& m_drops;
	Particles& m_particles;
	Projectiles& m_projectiles;

	float m_health, m_defence, m_damage;
	bool boss = false; // always false for now, but ill use this for boss enemies later

	// enemies define this
	sf::FloatRect m_bounds;

	bool m_hitable = true;
	sf::Vector2f m_position;
	sf::Vector2f m_velocity;
	float m_invuln = 0;

	bool m_boss = false;
	bool m_solid = true;

	float m_knockback_resist = 1;

	unsigned char m_quant = 1;

private:
	sf::FloatRect m_offset;
	EnemyType m_type;
	float m_maxhealth = 0;

};

class Boss : public Enemy {
public:
	Boss(ENEMY_PARAMS, float hp, float defence, float damage, sf::FloatRect off, EnemyType type);
	virtual ~Boss();

	virtual void died() = 0;
	virtual void update(float dt, Player* player, Enemies& enemies) = 0;

private:
	Audio::Song m_theme = Audio::Song::BATTLE;
};

class Enemies {
public:
	Enemies(Assets& assets, Settings& settings, World& world, Drops& drops, Particles& particles, Projectiles& projectiles, sf::Vector2u win);
	~Enemies();

	void update(sf::Time deltatime, Player* player, sf::RenderWindow& window);
	void draw(sf::RenderWindow& window);

	void reset();

	const std::vector<Enemy*>& vector();

	Enemy* spawn(EnemyType enemy, sf::Vector2f pos);
	sf::Vector2f spawnDistance();

	bool shoudCredits();

private:
	std::vector<Enemy*> m_enemies;
	Assets& m_assets;
	Settings& m_settings;
	World& m_world;
	Drops& m_drops;
	Particles& m_particles;
	Projectiles& m_projectiles;

	sf::Vector2f m_spawndistance;

	float m_spawntimer = 0;

	float m_endgametimer = 0;
	bool m_endgame = 0;
	bool m_finalboss = 0;

	sf::RectangleShape m_hpbar, m_hpbarbg;
};

// specific enemy classes
class Zombie : public Enemy {
public:
	Zombie(ENEMY_PARAMS, float hp = 18, float def = 1, float dam = 20, sf::FloatRect off = sf::FloatRect(6, 6, -12, -12), EnemyType type = EnemyType::ZOMBIE);
	virtual ~Zombie();

	void update(float dt, Player* player, Enemies& enemies) override;
	void died() override;

protected:
	Assets::Animation m_anim = Assets::anim_zombie;
	float m_speed = ZOMBIE_ACCEL;
	float m_decel = ZOMBIE_DECEL;
	float m_jump = ZOMBIE_JUMP;
	bool m_advance_jump = true;

	std::function<void(float, Player*)> m_lambda = [this]([[maybe_unused]] float dt, [[maybe_unused]] Player* player){};

private:
	float m_animtimer = 0;
	float m_invuln = 0;

	bool m_jumping = false;

};

class Bat : public Enemy {
public:
	Bat(ENEMY_PARAMS, float hp = 12, float def = 0, float dam = 15, sf::FloatRect off = sf::FloatRect(2, 2, -4, -4), EnemyType type = EnemyType::BAT);
	virtual ~Bat();

	void update(float dt, Player* player, Enemies& enemies) override;
	void died() override;

protected:
	Assets::Animation m_anim = Assets::anim_bat;
	float m_speed = BAT_ACCEL;
	float m_decel = BAT_DECEL;
	
	std::function<void(float, Player*)> m_lambda = [this]([[maybe_unused]] float dt, [[maybe_unused]] Player* player){};

private:
	float m_animtimer = 0;
	float m_invuln = 0;
	std::vector<sf::Vector2u> m_loc;
	sf::Vector2u m_oldpos;
	sf::Vector2u m_bestpos;

};

class Crawler : public Zombie {
public:
	Crawler(ENEMY_PARAMS, float hp = 14, float def = 0, float dam = 12, sf::FloatRect off = sf::FloatRect(2, 0, -4, 0), EnemyType type = EnemyType::CRAWLER);
	virtual ~Crawler();

	void died() override;
};

class Skeleton : public Zombie {
public:
	Skeleton(ENEMY_PARAMS, float hp = 26, float def = 2, float dam = 25, sf::FloatRect off = sf::FloatRect(2, 2, -4, -4), EnemyType type = EnemyType::SKELETON);
	virtual ~Skeleton();

	void died() override;
};

class DustDevil : public Zombie {
public:
	DustDevil(ENEMY_PARAMS, float hp = 36, float def = 2, float dam = 35, sf::FloatRect off = sf::FloatRect(2, 2, -4, -4), EnemyType type = EnemyType::DUSTDEVIL);
	virtual ~DustDevil();

	void died() override;
private:
	float m_part_timer = 0;
	float m_proj_timer = 0;

};

class Snowflake : public Bat {
public:
	Snowflake(ENEMY_PARAMS, float hp = 24, float def = 1, float dam = 30, sf::FloatRect off = sf::FloatRect(2, 2, -4, -4), EnemyType type = EnemyType::SNOWFLAKE);
	virtual ~Snowflake();

	void died() override;
private:
	float m_part_timer = 0;
	float m_proj_timer = 0;

};

class Mushroom : public Zombie {
public:
	Mushroom(ENEMY_PARAMS, float hp = 20, float def = 0, float dam = 18, sf::FloatRect off = sf::FloatRect(2, 0, -4, 0), EnemyType type = EnemyType::MUSHROOM);
	virtual ~Mushroom();

	void died() override;
};

// boss minion
class TreeSpirit : public Bat {
public:
	TreeSpirit(ENEMY_PARAMS, float hp = 16, float def = 1, float dam = 24, sf::FloatRect off = sf::FloatRect(0, 0, 0, 0), EnemyType type = EnemyType::TREESPIRIT);
	virtual ~TreeSpirit();

	void died() override;
private:
	float m_part_timer = 0;

};

// boss classes
class TreeGolem : public Boss {
public:
	TreeGolem(ENEMY_PARAMS, float hp = 325, float def = 2, float dam = 30, sf::FloatRect off = sf::FloatRect(6, 0, -12, 0), EnemyType type = EnemyType::TREEGOLEM);
	virtual ~TreeGolem();

	void update(float dt, Player* player, Enemies& enemies) override;
	void died() override;

private:
	char m_attack_use = 4;
	float m_attack_timer = 0;
	float m_summon_timer = 0;
	float m_megajump_timer = 0;

	bool m_grounded = true;
	bool m_smash = false;
};

// more enemies
class FlameSkeleton : public Skeleton {
public:
	FlameSkeleton(ENEMY_PARAMS, float hp = 35, float def = 2, float dam = 40, sf::FloatRect off = sf::FloatRect(2, 2, -4, -4), EnemyType type = EnemyType::FLAMESKELETON);
	virtual ~FlameSkeleton();

	void died() override;
private:
	float m_part_timer = 0;
	float m_proj_timer = 0;
};

// skull boss
class FlameSkull : public Boss {
public:
	FlameSkull(ENEMY_PARAMS, float hp = 750, float def = 3, float dam = 27.5, sf::FloatRect off = sf::FloatRect(6, 0, -12, 0), EnemyType type = EnemyType::FLAMESKULL);
	virtual ~FlameSkull();

	void update(float dt, Player* player, Enemies& enemies) override;
	void died() override;

private:
	char m_attack_use = 0;
	float m_attack_timer = 0;
	float m_summon_timer = 0;
	float m_baseshot_timer = 0;

	bool m_phase2 = false;
	bool m_shoot = true;

	size_t m_projcount = 0;
};

// underworld enemies
class BloodSerpent : public Bat {
public:
	BloodSerpent(ENEMY_PARAMS, float hp = 70, float def = 3, float dam = 45, sf::FloatRect off = sf::FloatRect(0, 4, 0, -8), EnemyType type = EnemyType::BLOODSERPENT);
	virtual ~BloodSerpent();

	void died() override;
private:
	float m_part_timer = 0;
};

class FireSpirit : public TreeSpirit {
public:
	FireSpirit(ENEMY_PARAMS, float hp = 40, float def = 2, float dam = 45, sf::FloatRect off = sf::FloatRect(0, 0, 0, 0), EnemyType type = EnemyType::FIRESPIRIT);
	virtual ~FireSpirit();

	void died() override;
private:
	float m_part_timer = 0;
	float m_dash_timer = 0;
	float m_old_speed = 0;

	bool m_dashing = false;
};

// underworld boss
class DemonicVisionary : public Boss {
public:
	DemonicVisionary(ENEMY_PARAMS, float hp = 2250, float def = 1.5f, float dam = 65, sf::FloatRect off = sf::FloatRect(0, 0, 0, 0), EnemyType type = EnemyType::DEMONICVISIONARY);
	virtual ~DemonicVisionary();

	void update(float dt, Player* player, Enemies& enemies) override;
	void died() override;

private:
	char m_attack_use = 0;
	float m_attack_timer = 0;
	float m_eye_timer = 0;
	unsigned int m_eye_shoot_timer = 0;
	float m_anim_timer = 0;
	unsigned char m_anim_frame;

	bool m_phase2 = false;

	sf::Vector2f m_dashtarget = sf::Vector2f(0, 0);
};

// minion
class Eye : public Enemy {
public:
	Eye(ENEMY_PARAMS, float hp = 25, float def = 1, float dam = 45, sf::FloatRect off = sf::FloatRect(0, 0, 0, 0), EnemyType type = EnemyType::EYE);
	virtual ~Eye();

	void update(float dt, Player* player, Enemies& enemies) override;
	void died() override;

	void target(sf::Vector2f target) { m_target = target; }
	void chase() { m_chase = true; m_speed = EYE_ACCEL / 1.85f;}
	bool chasing() { return m_chase; }
	void intangible() { m_solid = false; }


protected:
	float m_speed = EYE_ACCEL;
	float m_decel = EYE_DECEL;
	bool m_chase = false;
	sf::Vector2f m_target = sf::Vector2f(0, 0);
	
	std::function<void(float, Player*)> m_lambda = [this]([[maybe_unused]] float dt, [[maybe_unused]] Player* player){};

private:
	float m_animtimer = 0;
	float m_invuln = 0;
	std::vector<sf::Vector2u> m_loc;
	sf::Vector2u m_oldpos;
	sf::Vector2u m_bestpos;
};