#include "world.hpp"


void World::generate(WorldMeta data, Inventory& inventory) 
{
	m_worker = new std::thread([this, data, &inventory]() {

		m_status = "Initializing world generation...";

		reset();
		inventory.reset();

		m_hardmode = data.hardmode;
		if(m_hardmode) {
			m_timer = (DAY_LENGTH/4)+(DAY_LENGTH/7);
		}

		srand(std::hash<std::string>{}(data.seed)); // set seed 

		m_blocksprite.setTexture(m_assets.getTexture(Assets::texture_blocks));
		
		// variables for use while generating
		short vertical_offset = 0; 
		short underground_start = vertical_offset+UNDERGROUND_OFFSET;

		unsigned short mountain = MOUNTAIN_DISTANCE + (rand()%(MOUNTAIN_DISTANCE/2))-MOUNTAIN_DISTANCE/4;
		char ismountain = 0;

		unsigned short snow = SNOW_DISTANCE + (rand()%(BIOME_RANDOMPOS/2))-BIOME_RANDOMPOS/4;
		unsigned short sand = DESERT_DISTANCE + (rand()%(BIOME_RANDOMPOS/2))-BIOME_RANDOMPOS/4;
		unsigned short castle = CASTLE_DISTANCE + (rand()%(BIOME_RANDOMPOS/2))-BIOME_RANDOMPOS/4;

		// update snow and sand positions
		m_biomebounds[(size_t)Biome::SNOW][0] = snow - SNOW_SIZE/2;
		m_biomebounds[(size_t)Biome::SNOW][1] = snow + SNOW_SIZE/2;
		m_biomebounds[(size_t)Biome::DESERT][0] = sand - SNOW_SIZE/2;
		m_biomebounds[(size_t)Biome::DESERT][1] = sand + SNOW_SIZE/2;

		// lambda to build boxes
		auto buildBox = [this](unsigned int x, unsigned int y, unsigned int x2, unsigned int y2, Blocks::Type type, bool fill = false, Blocks::Type filltype = Blocks::Type::NONE) {
			for(unsigned int x3 = x; x3 <= x2; x3++) {
				for(unsigned int y3 = y; y3 <= y2; y3++) {

					// if not border, dont overwrite
					if(x3 != x && x3 != x2 && y3 != y && y3 != y2) {
						if(!fill) continue;
						if(fill) tile(x3, y3) = filltype;
					}
					else tile(x3, y3) = type;
				}
			}
		};

		m_status = "Generating biomes...";

		// initial pass, biomes and such
		int tree_countdown = 0;
		for (unsigned int x = 0; x < WORLD_WIDTH; x++) {
			if(tree_countdown > 0) tree_countdown--;

			// occasionally change the vertical offset
			if(!(rand()%2)) {

				if(x > mountain) {
					ismountain = 1;
					mountain += MOUNTAIN_DISTANCE + (rand()%(MOUNTAIN_DISTANCE/2))-MOUNTAIN_DISTANCE/4;
				}

				if(ismountain == 0) {
					vertical_offset += (rand()%5) - 2;
					if(vertical_offset > 6) vertical_offset = 6;
					else if(vertical_offset < -6) vertical_offset = -6;
				} else {
				
					char offset = (rand()%7) - 1;
					if(offset > 3) offset = 3; 

					if(ismountain == 1) vertical_offset -= offset;
					else vertical_offset += offset;
					
					if(vertical_offset > 6) {
						vertical_offset = 6;

						// mountain dipped, disable mountain generation entirely
						if(!(rand()%3) && ismountain == -1)
							ismountain = 0;
							
					} else if(vertical_offset < -30) {
						vertical_offset = -30;

						// mountain peak generated, bring it down
						if(rand()%3)
							ismountain = -1;
					}

				}

				if(ismountain == 0)
					underground_start = vertical_offset+(UNDERGROUND_OFFSET+rand()%3);
				else {
					short offset = vertical_offset+MOUNTAIN_STONE_QUANTITY;
					if(offset > (short)UNDERGROUND_OFFSET) offset = UNDERGROUND_OFFSET; // stone generates higher in mountains
					underground_start = vertical_offset + offset + rand()%3;
				}
			}

			// variables to be calculated per collumn
			bool first = true;
			bool tree = false;

			// should there be a tree in this collumn
			if(ismountain == 0 && tree_countdown==0) {
				tree = !(rand()%TREE_CHANCE);
				
				if(tree) tree_countdown = rand()%2 + 5;
			}

			for (unsigned int y = 0; y < WORLD_HEIGHT; y++) {

				bool grass = false;

				// keep top of the world air
				if(y > WORLD_HEIGHT/AIR_FRACTION + vertical_offset) {

					// underworld generation
					if(y >= UNDERWORLD_DISTANCE + rand()%6) {

						bool island = false;
						const unsigned short center = UNDERWORLD_DISTANCE + (WORLD_HEIGHT-UNDERWORLD_DISTANCE)/2;
						if(y > center - UNDERWORLD_CHANNEL_HEIGHT/2 + rand()%6 && y < center + UNDERWORLD_CHANNEL_HEIGHT/2 + rand()%6) {
							island = (rand()%UNDERWORLD_FISLAND_CHANCE == 0);
							if(tile(x, y) != Blocks::Type::HELL) {
								tile(x, y) = Blocks::Type::NONE;
							}

							tilebg(x, y) = Blocks::Type::HELL;

						} else {
							if(y < center) {
								tile(x, y) = Blocks::Type::HELL;
								tilebg(x, y) = Blocks::Type::HELL;
							}
							else {
								tile(x, y) = Blocks::Type::LAVA;
								tilebg(x, y) = Blocks::Type::HELL;
							}
						}
						
						// floating islands
						if(island) {

							bool molten = (rand()%5 == 0);

							for(size_t x2 = x - UNDERWORLD_FISLAND_SIZE/2 + rand()%UNDERWORLD_FISLAND_VAR; x2 < x + UNDERWORLD_FISLAND_SIZE/2 + rand()%UNDERWORLD_FISLAND_VAR; x2++) {
								for(size_t y2 = y - UNDERWORLD_FISLAND_SIZE/4 + rand()%UNDERWORLD_FISLAND_VAR; y2 < y + UNDERWORLD_FISLAND_SIZE/4 + rand()%UNDERWORLD_FISLAND_VAR; y2++) {
									if(x2 < WORLD_WIDTH && y2 < WORLD_HEIGHT) {
										if(!molten) tile(x2, y2) = Blocks::Type::HELL;
										else tile(x2, y2) = Blocks::Type::HELL; // temp
									}
								}
							}
						}

						continue;
					}

					bool snow_now = x > snow - SNOW_SIZE/2-rand()%5 && x < snow + SNOW_SIZE/2+rand()%5;
					bool sand_now = x > sand - DESERT_SIZE/2-rand()%5 && x < sand + DESERT_SIZE/2+rand()%5;

					// place stone after a certain depth
					if(y > WORLD_HEIGHT/AIR_FRACTION + underground_start) {
						tile(x, y) = sand_now ? Blocks::SANDS : snow_now ? Blocks::ICE : Blocks::Type::STONE;
						if(ismountain == 0 || y > WORLD_HEIGHT/AIR_FRACTION)
							tilebg(x,y) = sand_now ? Blocks::SANDS : snow_now ? Blocks::ICE : Blocks::Type::STONE;
					} else if(first) {
						tile(x, y) = sand_now ? Blocks::SAND : snow_now ? Blocks::SNOW : Blocks::GRASS;
						grass = true;
					}
					else
						tile(x, y) = sand_now ? Blocks::SAND : snow_now ? Blocks::PERMF : Blocks::Type::DIRT;
				
					if(grass && tree) {
						size_t tree_top = y-TREE_HEIGHT-rand()%3;
						for (size_t y2 = tree_top; y2 < y; y2++) {
							if(!sand_now) {
								if(y2 == y-1)
									tile(x, y2) = Blocks::STUMP;
								else
									tile(x, y2) = Blocks::BARK;
							} else {
								// cacti
								tile(x, y2) = Blocks::CACTI;
							}
						}

						// place leaves on the tree
						size_t leaf_center = tree_top - LEAVES_SIZE/2;
						for (size_t x2 = x-LEAVES_SIZE/2; x2 <= x+LEAVES_SIZE/2; x2++) {

							if(sand_now) // don't waste time with leaves on cacti
								break;

							for (size_t y2 = leaf_center-LEAVES_SIZE/2; y2 <= leaf_center+LEAVES_SIZE/2; y2++) {

								// bounds checking
								if(x2 >= WORLD_WIDTH || y2 >= WORLD_HEIGHT)
									continue;

								// if near the corners, don't place leaves
								unsigned int val = (x2 == x+LEAVES_SIZE/2) + (y2 == leaf_center+LEAVES_SIZE/2) + (x2 == x-LEAVES_SIZE/2) + (y2 == leaf_center-LEAVES_SIZE/2);
								if(val >= 2)
									continue;

								// randomness in tree gen
								else if(val == 1)
									if(!(rand()%7))
										continue;

								if(tile(x2, y2) == Blocks::Type::NONE) {
									tile(x2, y2) = snow_now ? Blocks::SLEAF :Blocks::LEAF;
								}
							}
						}

						tree = false;
					}
				
					first = false;
				}

			}
		}

		m_status = "Generating caves/ore...";

		// prereqs for cave gen
		sf::Vector2u cave_loc;
		sf::Vector2u cave_dir;

		// second pass, place ores/dig caves
		for (unsigned int x = 0; x < WORLD_WIDTH; x++) {
			for (unsigned int y = 0; y < WORLD_HEIGHT; y++) {
				auto& t = tile(x,y);

				if(t == Blocks::Type::STONE || t == Blocks::Type::SANDS || t == Blocks::Type::ICE || t == Blocks::Type::HELL) {

					// chance to generate ore
					if(rand()%ORE_CHANCE == 0) {

						Blocks::Type ore = Blocks::Type::NONE;

						// pick ore
						if(tile(x,y) == Blocks::Type::STONE) {
							if(rand()%VINESMITE_CHANCE == 0 && y > (WORLD_HEIGHT/AIR_FRACTION) + VINESMITE_DEPTH) {
								ore = Blocks::VINESM;
							} else if(rand()%TITANIUM_CHANCE == 0 && y > (WORLD_HEIGHT/AIR_FRACTION) + TITANIUM_DEPTH) {
								ore = Blocks::TITAN;
							} else if(rand()%GOLD_CHANCE == 0 && y > (WORLD_HEIGHT/AIR_FRACTION) + GOLD_DEPTH) {
								ore = Blocks::GOLD;
							} else if(rand()%LEAD_CHANCE == 0 && y > (WORLD_HEIGHT/AIR_FRACTION) + LEAD_DEPTH) {
								ore = Blocks::LEAD;
							} else {
								ore = Blocks::COAL;
							}
						} else if(tile(x,y) == Blocks::Type::SANDS) {
							if(rand()%ROSEGOLD_CHANCE == 0 && y > (WORLD_HEIGHT/AIR_FRACTION) + ROSEGOLD_DEPTH) {
								ore = Blocks::RGOLD;
							} else if(rand()%LEAD_CHANCE == 0 && y > (WORLD_HEIGHT/AIR_FRACTION) + LEAD_DEPTH) {
								ore = Blocks::LEAD;
							} else {
								ore = Blocks::COAL;
							}
						} else if(tile(x,y) == Blocks::Type::ICE) {
							if(rand()%ROSEGOLD_CHANCE == 0 && y > (WORLD_HEIGHT/AIR_FRACTION) + ROSEGOLD_DEPTH) {
								ore = Blocks::RGOLD;
							} else {
								ore = Blocks::COAL;
							}
						} else if(tile(x,y) == Blocks::Type::HELL) {
							ore = Blocks::CRYSTAL;
						}

						// generate ore patch
						int ore_patch_size = ORE_PATCH_SIZE/2;

						if(ore == Blocks::COAL) {
							ore_patch_size += 1;
						} else if(ore == Blocks::CRYSTAL) {
							ore_patch_size = 1;
						}

						for(size_t x2 = x-ore_patch_size; x2 <= x+ore_patch_size; x2++) {
							for(size_t y2 = y-ore_patch_size; y2 <= y+ore_patch_size; y2++) {
								
								// bounds checking
								if(x2 >= WORLD_WIDTH || y2 >= WORLD_HEIGHT)
									continue;

								if(rand()%ORE_PLACE_CHANCE)
									tile(x2, y2) = ore;
							}
						}
					}

					// chance to dig a cave
					if(rand()%CAVE_CHANCE == 0 && y > (WORLD_HEIGHT/AIR_FRACTION)-2 && t != Blocks::Type::HELL) {

						// update location
						cave_loc = sf::Vector2u(x, y);

						// set direction
						cave_dir = sf::Vector2u((rand()%3)-1, (rand()%3)-1);

						// dig a cave
						for(size_t i = 0; i < CAVE_SIZE; i++) {

							for(size_t x2 = cave_loc.x-CAVE_PATCH_SIZE/2; x2 <= cave_loc.x+CAVE_PATCH_SIZE/2; x2++) {
								for(size_t y2 = cave_loc.y-CAVE_PATCH_SIZE/2; y2 <= cave_loc.y+CAVE_PATCH_SIZE/2; y2++) {
									
									// bounds checking
									if(x2 >= WORLD_WIDTH || y2 >= WORLD_HEIGHT)
										continue;

									if(y2 < (WORLD_HEIGHT/AIR_FRACTION)-2)
										continue;

									// dig
									if(rand()%5 && Blocks::getBlockData(tile(x2, y2)).solid && tile(x2, y2-1) != Blocks::Type::STUMP)
										tile(x2, y2) = Blocks::Type::NONE;
								}
							}

							// pick a random direction
							cave_loc.x += (rand()%3)-1;
							cave_loc.y += (rand()%3)-1;

							// minor influence on direction
							if(rand()%2) {
								if(rand()%2) {
									cave_loc.x += cave_dir.x;
								} else {
									cave_loc.y += cave_dir.y;
								}
							}
						}
					}
				}
			}
		}

		m_status = "Generating structures...";

		// third pass, structre generation
		unsigned int guaranteed_lake = rand()%WORLD_WIDTH;
		for (unsigned int x = 0; x < WORLD_WIDTH; x++) {

			// lake chance and x is outside of guaranteed lake
			if(rand()%LAKE_CHANCE == 0 || x == guaranteed_lake) {

				// generate lake
				int center_x = x + rand()%LAKE_ROUND_DIST - LAKE_ROUND_DIST/2;
				int center_y = (WORLD_HEIGHT/AIR_FRACTION) + rand()%LAKE_ROUND_DIST/2 - LAKE_ROUND_DIST/4;
				for(size_t x2 = x-LAKE_SIZE/2; x2 <= x+LAKE_SIZE/2; x2++) {
					for(size_t y2 = (WORLD_HEIGHT/AIR_FRACTION) - MOUNTAIN_STONE_QUANTITY+5; y2 <= (WORLD_HEIGHT/AIR_FRACTION)+LAKE_DEPTH; y2++) {
						
						// bounds checking
						if(x2 >= WORLD_WIDTH || y2 >= WORLD_HEIGHT)
							continue;

						// round corners of lake by checking distance from center
						if((x2-center_x)*(x2-center_x) + (y2-center_y)*(y2-center_y) > LAKE_SIZE*LAKE_SIZE/4)
							continue;

						if(tile(x2, y2) != Blocks::Type::NONE) {
							tile(x2, y2) = Blocks::Type::WATER;
						}
					}
				}
			}

			for(unsigned int y = 0; y < WORLD_HEIGHT; y++) {
				if(y > (WORLD_HEIGHT/AIR_FRACTION) + GOLD_DEPTH && y < UNDERWORLD_DISTANCE && rand()%ABHOUSE_CHANCE == 0) {
					
					// generate abandoned house
					Blocks::Type t = Blocks::WOOD;
					if(rand()%3 == 0) t = Blocks::DIRT;
					else if(rand()%2) {
						t = Blocks::STONERB;
						if(rand()%3 == 0) t = Blocks::STONEBR;
					}

					buildBox(x-ABHOUSE_SIZE/2, y-ABHOUSE_SIZE/3, x+ABHOUSE_SIZE/2, y+ABHOUSE_SIZE/3, t, true);


					// torches
					if(rand()%3 == 0) tile(x-ABHOUSE_SIZE/2+2, y-ABHOUSE_SIZE/3 + 2) = Blocks::TORCH;
					if(rand()%3 == 0) tile(x+ABHOUSE_SIZE/2-2, y-ABHOUSE_SIZE/3 + 2) = Blocks::TORCH;

					// chest
					int offset = rand()%5 - 2;
					tile(x+offset, y + ABHOUSE_SIZE/3 - 1) = Blocks::CHEST;
					appendChest(x+offset, y + ABHOUSE_SIZE/3 - 1);

					// loot
					Inventory& inv = getChest(x+offset, y + ABHOUSE_SIZE/3 - 1);
					
					// loot tables change based on type
					if(t == Blocks::DIRT) {
						inv.append({Items::Type::DIRT, (unsigned int)rand()%22+5}, rand()%INVENTORY_SIZE);
						inv.append({Items::Type::STONE, (unsigned int)rand()%22+5}, rand()%INVENTORY_SIZE);
						inv.append({Items::Type::WOOD, (unsigned int)rand()%22+5}, rand()%INVENTORY_SIZE);
						if(rand()%2) inv.append({Items::Type::TORCH, (unsigned int)rand()%4+2}, rand()%INVENTORY_SIZE);
						if(rand()%2) inv.append({Items::Type::COAL, (unsigned int)rand()%6+3}, rand()%INVENTORY_SIZE);
						if(rand()%12 == 0) inv.append({Items::Type::STREYE, 1u}, rand()%INVENTORY_SIZE);
					} else if(t == Blocks::WOOD) {
						inv.append({Items::Type::LEADB, (unsigned int)rand()%3+5}, rand()%INVENTORY_SIZE);
						inv.append({Items::Type::COAL, (unsigned int)rand()%8+5}, rand()%INVENTORY_SIZE);
						inv.append({Items::Type::STONE, (unsigned int)rand()%12+5}, rand()%INVENTORY_SIZE);
						inv.append({Items::Type::WOOD, (unsigned int)rand()%12+5}, rand()%INVENTORY_SIZE);
						inv.append({Items::Type::BARK, (unsigned int)rand()%3+5}, rand()%INVENTORY_SIZE);
						inv.append({Items::Type::STUMP, (unsigned int)rand()%2+1}, rand()%INVENTORY_SIZE);
						if(rand()%3) inv.append({Items::Type::TORCH, (unsigned int)rand()%6+5}, rand()%INVENTORY_SIZE);
						if(rand()%9 == 0) inv.append({Items::Type::STREYE, 1u}, rand()%INVENTORY_SIZE);
					} else if(t == Blocks::STONEBR || t == Blocks::STONERB) {

						// nerfed castle loot + eye
						inv.append({Items::Type::LEADB, (unsigned int)rand()%3+5}, rand()%INVENTORY_SIZE);
						inv.append({Items::Type::COAL, (unsigned int)rand()%8+5}, rand()%INVENTORY_SIZE);
						inv.append({Items::Type::STONE, (unsigned int)rand()%12+5}, rand()%INVENTORY_SIZE);
						inv.append({Items::Type::STONERB, (unsigned int)rand()%45+20}, rand()%INVENTORY_SIZE);
						if(rand()%3) inv.append({Items::Type::TORCH, (unsigned int)rand()%6+5}, rand()%INVENTORY_SIZE);
						if(rand()%2 == 0) inv.append({Items::Type::GOLDB, (unsigned int)rand()%3+6}, rand()%INVENTORY_SIZE);
						if(rand()%3 == 0) inv.append({Items::Type::TITANB, (unsigned int)rand()%5+4}, rand()%INVENTORY_SIZE);
						if(rand()%3 == 0) inv.append({Items::Type::ROSEB, (unsigned int)rand()%5+4}, rand()%INVENTORY_SIZE);

						if(t == Blocks::STONEBR) {

							// lead armor
							if(rand()%3) {
								char c = rand()%3;
								if(c == 0) inv.append({Items::Type::LHELMET, 1}, rand()%INVENTORY_SIZE);
								else if(c == 1) inv.append({Items::Type::LCHESTPLATE, 1}, rand()%INVENTORY_SIZE);
								else inv.append({Items::Type::LLEGGINGS, 1}, rand()%INVENTORY_SIZE);
							} 

							// gold armor
							if(rand()%2 == 0) {
								char c = rand()%3;
								if(c == 0) inv.append({Items::Type::GHELMET, 1}, rand()%INVENTORY_SIZE);
								else if(c == 1) inv.append({Items::Type::GCHESTPLATE, 1}, rand()%INVENTORY_SIZE);
								else inv.append({Items::Type::GLEGGINGS, 1}, rand()%INVENTORY_SIZE);
							}

							// titanium armor
							if(rand()%3 == 0) {
								char c = rand()%3;
								if(c == 0) inv.append({Items::Type::THELMET, 1}, rand()%INVENTORY_SIZE);
								else if(c == 1) inv.append({Items::Type::TCHESTPLATE, 1}, rand()%INVENTORY_SIZE);
								else inv.append({Items::Type::TLEGGINGS, 1}, rand()%INVENTORY_SIZE);
							}

							if(rand()%4 == 0) inv.append({Items::Type::MSHIELD, 1}, rand()%INVENTORY_SIZE);

							// bones
							if(rand()%3) inv.append({Items::Type::BONE, (unsigned int)rand()%3 + 1}, rand()%INVENTORY_SIZE);

						}

						if(rand()%6 == 0) inv.append({Items::Type::STREYE, 1u}, rand()%INVENTORY_SIZE);
					}

				}
			}
		}

		// generate castle biome
		std::array<std::pair<sf::Vector2u, sf::Vector2u>, CASTLE_HALWAY_COUNT> castle_paths;

		unsigned int left = (castle-CASTLE_SIZE/2);
		unsigned int right = (castle+CASTLE_SIZE/2);
		unsigned int top = (WORLD_HEIGHT/AIR_FRACTION-CASTLE_SIZE/2);
		unsigned int bottom = (WORLD_HEIGHT/AIR_FRACTION+CASTLE_SIZE/2);

		m_biomebounds[(size_t)Biome::CASTLE][0] = left;
		m_biomebounds[(size_t)Biome::CASTLE][1] = right;

		// create lines
		for(unsigned int i = 0; i < CASTLE_HALWAY_COUNT; i++) {
			sf::Vector2u start(left + rand()%(CASTLE_SIZE), top + rand()%(CASTLE_SIZE));
			sf::Vector2u end(left + rand()%(CASTLE_SIZE), top + rand()%(CASTLE_SIZE));

			// add to list
			castle_paths[i] = std::make_pair(start, end);
		}

		for (unsigned int x = left; x <= right; x++) {

			for (unsigned int y = top; y <= bottom; y++) {
				
				// bounds checking
				if(x >= WORLD_WIDTH || y >= WORLD_HEIGHT)
					continue;

				// top pattern
				if(y <= top+2) {
					if(x/5 %2 == 0) {
						continue;
					}
				}

				// generate rubble and stone
				if(rand()%4 == 0) {
					tile(x, y) = Blocks::STONEBR;
				} else if(rand()%3 == 0) {
					tile(x, y) = Blocks::STONERB;
				} else if(rand()%2 == 0) {
					tile(x, y) = Blocks::STONE;
				} else {
					tile(x, y) = Blocks::NONE;
				}
				
					
				// generate castle walls
				if(rand()%3 == 0) {
					tilebg(x, y) = Blocks::STONEBR;
				} else {
					tilebg(x, y) = Blocks::STONERB;
				}

			}
		}

		// find and remove castle hallways
		for(unsigned int i = 0; i < CASTLE_HALWAY_COUNT; i++) {

			// get pos on current y
			sf::Vector2f pos = sf::Vector2f(castle_paths[i].first);
			sf::Vector2f dir = sf::Vector2f(castle_paths[i].second) - pos;
			dir /= sqrtf(dir.x*dir.x + dir.y*dir.y);

			// while in range
			while(pos.x >= left && pos.x <= right && pos.y >= top && pos.y <= bottom) {

				pos += dir;

				for(unsigned int x = pos.x - CASTLE_HALWAY_WIDTH/2; x < pos.x + CASTLE_HALWAY_WIDTH/2; x++) {
					for(unsigned int y = pos.y + CASTLE_HALWAY_WIDTH/2; y > pos.y - CASTLE_HALWAY_WIDTH/2; y--) {
						if(y >= top && y <= bottom && x >= left && x <= right) {

							auto t = tile(x, y);
							if(t == Blocks::NONE || t == Blocks::TORCH || t == Blocks::CHEST)
								continue;

							// remove wall / add loot
							if((rand()%CASTLE_CHEST_CHANCE == 0) && Blocks::getBlockData(tile(x, y+1)).solid) {

								// chest block
								tile(x, y) = Blocks::CHEST;
								appendChest(x, y);

								// add loot
								Inventory& inv = m_chests[std::make_pair(x, y)];

								// add loot
								inv.append({Items::Type::LEADB, (unsigned int)rand()%3+5}, rand()%INVENTORY_SIZE);
								inv.append({Items::Type::COAL, (unsigned int)rand()%8+5}, rand()%INVENTORY_SIZE);
								inv.append({Items::Type::STONE, (unsigned int)rand()%12+5}, rand()%INVENTORY_SIZE);
								inv.append({Items::Type::STONERB, (unsigned int)rand()%45+20}, rand()%INVENTORY_SIZE);
								if(rand()%2 == 0) inv.append({Items::Type::GOLDB, (unsigned int)rand()%3+6}, rand()%INVENTORY_SIZE);
								if(rand()%3 == 0) inv.append({Items::Type::TITANB, (unsigned int)rand()%5+4}, rand()%INVENTORY_SIZE);
								if(rand()%3 == 0) inv.append({Items::Type::ROSEB, (unsigned int)rand()%5+4}, rand()%INVENTORY_SIZE);
								if(rand()%4 == 0) inv.append({Items::Type::VINESM, (unsigned int)rand()%3+3}, rand()%INVENTORY_SIZE);

								// lead armor
								if(rand()%2) {
									char c = rand()%3;
									if(c == 0) inv.append({Items::Type::LHELMET, 1}, rand()%INVENTORY_SIZE);
									else if(c == 1) inv.append({Items::Type::LCHESTPLATE, 1}, rand()%INVENTORY_SIZE);
									else inv.append({Items::Type::LLEGGINGS, 1}, rand()%INVENTORY_SIZE);
								} 

								// gold armor
								if(rand()%2 == 0) {
									char c = rand()%3;
									if(c == 0) inv.append({Items::Type::GHELMET, 1}, rand()%INVENTORY_SIZE);
									else if(c == 1) inv.append({Items::Type::GCHESTPLATE, 1}, rand()%INVENTORY_SIZE);
									else inv.append({Items::Type::GLEGGINGS, 1}, rand()%INVENTORY_SIZE);
								}

								// titanium armor
								if(rand()%2 == 0) {
									char c = rand()%3;
									if(c == 0) inv.append({Items::Type::THELMET, 1}, rand()%INVENTORY_SIZE);
									else if(c == 1) inv.append({Items::Type::TCHESTPLATE, 1}, rand()%INVENTORY_SIZE);
									else inv.append({Items::Type::TLEGGINGS, 1}, rand()%INVENTORY_SIZE);
								}

								// bones
								if(rand()%3) inv.append({Items::Type::BONE, (unsigned int)rand()%3 + 1}, rand()%INVENTORY_SIZE);

								// flame cores
								if(rand()%3 == 0) inv.append({Items::Type::FCORE, 1}, rand()%INVENTORY_SIZE);

								// accessories
								if(rand()%4 == 0) {
									char c = rand()%2;
									if(c == 0) inv.append({Items::Type::MSHIELD, 1}, rand()%INVENTORY_SIZE);
									else inv.append({Items::Type::RHEART, 1}, rand()%INVENTORY_SIZE);
								}

							} else if(rand()%CASTLE_TORCH_CHANCE == 0) {
								tile(x, y) = Blocks::TORCH;
							} else {
								tile(x, y) = Blocks::NONE;
							}
						}
					}
				}
			}
		}
		
		// remove invalid chests
		for(unsigned int x = left; x <= right; x++) {
			for(unsigned int y = top; y <= bottom; y++) {
				if(tile(x, y) == Blocks::CHEST && (tile(x, y+1) == Blocks::NONE || tile(x, y+1) == Blocks::TORCH)) {
					tile(x, y) = Blocks::NONE;
					removeChest(x, y);
				}
			}
		}


		m_status = "Finishing up...";

		// surround world edges with walls
		for(unsigned int x = 0; x < WORLD_WIDTH; x++) {
			tile(x, 0) = Blocks::INVIN;
			tile(x, WORLD_HEIGHT-1) = Blocks::INVIN;
		}
		
		for(unsigned int y = 0; y < WORLD_HEIGHT; y++) {
			tile(0, y) = Blocks::INVIN;
			tile(WORLD_WIDTH-1, y) = Blocks::INVIN;
		}

		// save world to file
		save(data, inventory);

		m_status = "";

		// were done here
		m_done = true;
	});
}