// constants

// game informtion
inline const char* GAME_NAME = "XenoRealm";
inline const char* GAME_VERSION = "0.0.1";
inline const char* GAME_DATA = "xenorealm DevelopmentBuild-0.0.1\n" "" __DATE__ " " __TIME__; 
inline const unsigned int TILE_SIZE = 32;
inline const unsigned int MIN_FRAMERATE = 20;
inline const bool TRAILER_BUILD = false;

// player related constants
inline const unsigned int PLAYER_ACCEL = 22 * TILE_SIZE;
inline const float PLAYER_DECEL = 0.06 * TILE_SIZE;
inline const unsigned int PLAYER_JUMP = 26.75 * TILE_SIZE;
inline const unsigned int GRAVITY = 50 * TILE_SIZE;
inline const float PLAYER_ANIM_SPEED = 0.05f;
inline const unsigned int SWING_ANIM_SPEED = 6;
inline const float ITEM_SIZE = 1.5f;
inline const unsigned int ITEM_OFFSET = 26;
inline const float INVIS_PLAYER_DURATION = 0.55f;
inline const unsigned int PLAYER_MAX_HEALTH = 100;
inline const float PLAYER_HEALTH_REGEN = 0.75f;
inline const bool PLAYER_COLLIDES = true;
inline const float PLAYER_BUILD_TIMER = 0.15f;

// general world info
inline const unsigned int WORLD_WIDTH = 2500;
inline const unsigned int WORLD_HEIGHT = 700;
inline const float AIR_FRACTION = 3.5;
inline const unsigned char BACKGROUND_COLOR[3] = {58, 148, 255};
inline const unsigned char BG_WALLS_COLOR = 120;
inline const unsigned char MAP_VISITED_CHUNKS = 10;
inline const unsigned char MAP_CLEAR_DISTANCE = 3;
inline const unsigned int DAY_LENGTH = 480;
inline const int ZERO_LIGHT_DISTANCE = 13;
inline const unsigned int PARALAX_DIVISOR_X = 75;
inline const unsigned int PARALAX_DIVISOR_Y = 20;
inline const float PARALAX_BGSIZEINC = 1.25f;
inline const unsigned int MINIMAP_SIZE = 90;
inline const unsigned int MINIMAP_SCALE = 2;
inline const unsigned int MINIMAP_FULLSCREEN_BORDER = 100;
inline const unsigned int MINIMAP_SPEED = 1000;
inline const unsigned int MINIMAP_ZSPEED = 6;
inline const unsigned int MINIMAP_MIN_ZOOM = 10;
inline const float WORLD_TICK = 1.f/20.f;
inline const float WORLD_UPDATE_PAST_SCREEN_RANGE = 8;


// world generation info, pass one
inline const unsigned char TREE_CHANCE = 18;
inline const unsigned int TREE_HEIGHT = 8;
inline const unsigned int LEAVES_SIZE = 6;
inline const unsigned int UNDERGROUND_OFFSET = 9;
inline const unsigned int MOUNTAIN_DISTANCE = 400;
inline const unsigned int MOUNTAIN_STONE_QUANTITY = 15;
inline const unsigned int SNOW_DISTANCE = 800;
inline const unsigned int SNOW_SIZE = 300;
inline const unsigned int DESERT_DISTANCE = 1700;
inline const unsigned int DESERT_SIZE = 300;
inline const unsigned int BIOME_RANDOMPOS = 400;
inline const unsigned int UNDERWORLD_DISTANCE = 600;
inline const unsigned int UNDERWORLD_CHANNEL_HEIGHT = 65;
inline const unsigned int UNDERWORLD_FISLAND_CHANCE = 850;
inline const unsigned int UNDERWORLD_FISLAND_SIZE = 23;
inline const unsigned int UNDERWORLD_FISLAND_VAR = 4;

// world generation info, pass two
inline const unsigned int ORE_CHANCE = 615;
inline const unsigned int ORE_PLACE_CHANCE = 3;
inline const unsigned int ORE_PATCH_SIZE = 4;
inline const unsigned int LEAD_CHANCE = 2;
inline const unsigned int LEAD_DEPTH = 5;
inline const unsigned int GOLD_CHANCE = 3;
inline const unsigned int GOLD_DEPTH = 110;
inline const unsigned int TITANIUM_CHANCE = 4;
inline const unsigned int TITANIUM_DEPTH = 170;
inline const unsigned int ROSEGOLD_CHANCE = 5;
inline const unsigned int ROSEGOLD_DEPTH = 220;
inline const unsigned int VINESMITE_CHANCE = 5;
inline const unsigned int VINESMITE_DEPTH = 250;
inline const unsigned int CAVE_CHANCE = 2050;
inline const unsigned int CAVE_SIZE = 245;
inline const unsigned int CAVE_PATCH_SIZE = 4;

// world generation info, pass three
inline const unsigned int CASTLE_DISTANCE = 400;
inline const unsigned int CASTLE_SIZE = 200;
inline const unsigned int CASTLE_HALWAY_COUNT = 22;
inline const unsigned int CASTLE_HALWAY_WIDTH = 4;
inline const unsigned int CASTLE_TORCH_CHANCE = 180;
inline const unsigned int CASTLE_CHEST_CHANCE = 45;
inline const unsigned int LAKE_CHANCE = 375;
inline const unsigned int LAKE_SIZE = 40;
inline const unsigned int LAKE_DEPTH = 12;
inline const unsigned int LAKE_ROUND_DIST = 6;
inline const unsigned int ABHOUSE_CHANCE = 10595;
inline const unsigned int ABHOUSE_SIZE = 9;

// inventory/item info
inline const unsigned int INVENTORY_ROWS = 4;
inline const unsigned int INVENTORY_SIZE = 32; // divisible by INVENTORY_ROWS
inline const float DROP_ANIM_SPEED = 2.5f;
inline const unsigned int DROP_MOVEMENT = 20;
inline const unsigned int DROP_SPEED = 200;
inline const float BREAK_TIME = 0.315f;
inline const unsigned int BREAK_PLACE_DISTANCE = 10;
inline const unsigned int ACCESSORY_COUNT = 3;

// enemy info
inline const unsigned int ENEMY_HPBAR_WIDTH = 30;
inline const unsigned int ENEMY_HPBAR_HEIGHT = 6;
inline const float ENEMY_SPAWN_TIME_DAY = 9.25f;
inline const float ENEMY_SPAWN_TIME_NIGHT = 3.8f;
inline const float ENEMY_SPAWN_TIME_DANGER = 4.6f;
inline const unsigned int ENEMY_SPAWN_CHANCE = 4;
inline const unsigned int ENEMY_SPAWN_MAX_DAY = 4;
inline const unsigned int ENEMY_SPAWN_MAX_NIGHT = 9;
inline const unsigned int ENEMY_SPAWN_MAX_DANGER = 13;
inline const unsigned int KNOCKBACK = 12*TILE_SIZE;
inline const float INVIS_DURATION = 0.35f;
inline const unsigned int ZOMBIE_ACCEL = 6*TILE_SIZE;
inline const float ZOMBIE_DECEL = 0.04*TILE_SIZE;
inline const unsigned int ZOMBIE_JUMP = 26.75*TILE_SIZE;
inline const unsigned int BAT_ACCEL = 11*TILE_SIZE;
inline const float BAT_DECEL = 0.03*TILE_SIZE;
inline const unsigned int CRAWLER_ACCEL = 9*TILE_SIZE;
inline const float CRAWLER_DECEL = 0.0385*TILE_SIZE;
inline const unsigned int CRAWLER_JUMP = 19*TILE_SIZE;
inline const unsigned int SKELETON_ACCEL = 9*TILE_SIZE;
inline const float SKELETON_DECEL = 0.0385*TILE_SIZE;
inline const unsigned int SKELETON_JUMP = 28*TILE_SIZE;
inline const unsigned int DUSTDEVIL_ACCEL = 5*TILE_SIZE;
inline const float DUSTDEVIL_DECEL = 0.04*TILE_SIZE;
inline const unsigned int DUSTDEVIL_JUMP = 22*TILE_SIZE;
inline const unsigned int SNOWFLAKE_ACCEL = 7*TILE_SIZE;
inline const float SNOWFLAKE_DECEL = 0.03*TILE_SIZE;
inline const unsigned int MUSHROOM_ACCEL = 6*TILE_SIZE;
inline const float MUSHROOM_DECEL = 0.0235*TILE_SIZE;
inline const unsigned int MUSHROOM_JUMP = 18*TILE_SIZE;
inline const unsigned int BLOODSERPENT_ACCEL = 5*TILE_SIZE;
inline const float BLOODSERPENT_DECEL = 0.025*TILE_SIZE;
inline const unsigned int EYE_ACCEL = 45*TILE_SIZE;
inline const float EYE_DECEL = 0.0315*TILE_SIZE;
inline const float HARD_MODE_INC = 1.5f;


// boss info
inline const unsigned int TREEGOLEM_ACCEL = 14.5*TILE_SIZE;
inline const float TREEGOLEM_DECEL = 0.045*TILE_SIZE;
inline const unsigned int TREEGOLEM_JUMP = 35*TILE_SIZE;
inline const float TREEGOLEM_ATTACK_TIME = 7;
inline const float TREEGOLEM_SUMMON_TIME = 1.95;
inline const float TREEGOLEM_ANIMATION_SPEED = 0.4f;
inline const float TREEGOLEM_MEGAJUMP_TIME = 5.5f;
inline const float FLAMESKULL_ACCEL = 12*TILE_SIZE;
inline const float FLAMESKULL_DECEL = 0.05*TILE_SIZE;
inline const float FLAMESKULL_ATTACK_TIME = 6;
inline const float FLAMESKULL_BAYBLADE_ROTATE = 694.20f; // im funny irl I promise
inline const float FLAMESKULL_SUMMON_TIME = 1.65;
inline const float FLAMESKULL_SHOOT_TIME = 0.115;
inline const float FLAMESKULL_LAUNCH_STRENGTH = 1;
inline const float DEMONICVISIONARY_ACCEL = 8*TILE_SIZE;
inline const float DEMONICVISIONARY_DECEL = 0.05*TILE_SIZE;
inline const unsigned int DEMONICVISIONARY_EYECOUNT = 6;
inline const unsigned int DEMONICVISIONARY_EYECOUNTMAX = 32;
inline const float DEMONICVISIONARY_EYETIME = 0.55f;
inline const unsigned int DEMONICVISIONARY_EYESHOOTTIME = 15;
inline const float DEMONICVISIONARY_ANIMTIME = 0.15f;
inline const int DEMONICVISIONARY_EYEDIST = 125;

#pragma once
#include <iostream>