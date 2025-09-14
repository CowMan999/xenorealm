#include "menu.hpp"

#define WINDOW_FLAGS ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus\

struct MenuSlot {
	islot* ref;
	size_t index;
	enum class Type {
		ITEM,
		ARMOR,
		ACCESSORY,
	} type;
	sf::Vector2i loc;
	bool main = false;
};

void Menu::ingameGui(float scale) {
	ImGui::Begin("##gamewindow", nullptr, ImGuiWindowFlags_NoBackground | WINDOW_FLAGS | ImGuiWindowFlags_NoDecoration);

	ImGui::SetWindowPos(ImVec2(50*scale, 50*scale));
	ImGui::SetWindowSize(ImVec2(m_window.getSize().x-50*scale, m_window.getSize().y));

	ImGui::SetCursorPosY(1);

	if(m_enemies.shoudCredits()) {
		setState(MenuState::CREDITS);
	}

	m_inventoryrenderer.setScale(scale, scale);

	// mouse set active for other classes, we can hog it by setting this to false
	m_mousea = m_mousea_old;

	// create list of inventory items to render
	std::vector<MenuSlot> renderitems;
	renderitems.reserve(INVENTORY_SIZE + 6);

	if(m_player.chestOpen()) {
		m_inventory_a = true;
	}

	for(size_t y = 0; y < (m_inventory_a ? INVENTORY_ROWS : 1); y++) {
		for(size_t x = 0; x < INVENTORY_SIZE/INVENTORY_ROWS; x++) {
			MenuSlot m;
			m.index = x + y*INVENTORY_SIZE/INVENTORY_ROWS;
			m.ref = &m_inventory[m.index];
			m.type = MenuSlot::Type::ITEM;
			m.loc = sf::Vector2i(sf::Vector2f(x, y)*52.f*scale); // 52 = 48 + 4
			m.main = true;

			renderitems.push_back(m);
		}
	}
	

	if(m_inventory_a) {
		for(size_t i = 0; i < 3; i++) {
			MenuSlot m;
			m.index = i;
			m.ref = &m_inventory[(Armor::Category)i];
			m.type = MenuSlot::Type::ARMOR;
			m.loc = sf::Vector2i(sf::Vector2f(452*scale, i*52.f*scale)); // 52 = 48 + 4

			renderitems.push_back(m);
		}

		for(size_t i = 0; i < 3; i++) {
			MenuSlot m;
			m.index = i;
			m.ref = &m_inventory.acc(i);
			m.type = MenuSlot::Type::ACCESSORY;
			m.loc = sf::Vector2i(sf::Vector2f(504*scale, i*52.f*scale));

			renderitems.push_back(m);
		}


	}

	if(m_player.chestOpen()) {
		for(size_t y = 0; y < INVENTORY_ROWS; y++) {
			for(size_t x = 0; x < INVENTORY_SIZE/INVENTORY_ROWS; x++) {
				MenuSlot m;
				m.index = x + y*INVENTORY_SIZE/INVENTORY_ROWS;
				m.ref = &m_world.getChest(m_player.getChestOpen().x, m_player.getChestOpen().y)[m.index];
				m.type = MenuSlot::Type::ITEM;
				m.loc = sf::Vector2i(sf::Vector2f(x, y + 5)*52.f*scale); // 52 = 48 + 4

				renderitems.push_back(m);
			}
		}
	}

	float xpos = ImGui::GetCursorPosX();

	for(size_t i = 0; i < renderitems.size(); i++) {

		ImGui::SetCursorPos(ImVec2(xpos + renderitems[i].loc.x, 1+renderitems[i].loc.y));

		// highlight selected item
		if(m_selected != i)
			ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(.35, .35, .65, 1));
		else ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(.65, .65, .85, 1));

		islot& slot = *renderitems[i].ref;

		if(ImGui::BeginChild(("##hotbaritem" + std::to_string(i)).c_str(), ImVec2(48*scale, 48*scale), true)) {
			
			if(Items::getItemData(slot.item).category == Items::Category::BLOCK) {
				m_inventoryrenderer.setTexture(m_assets.getTexture(Assets::texture_blocks));
			} else {
				m_inventoryrenderer.setTexture(m_assets.getTexture(Assets::texture_items));
			}
			
			bool hover = ImGui::IsWindowHovered();

			// render image
			sf::IntRect loc = m_assets.getAnimation(Items::getItemData(slot.item).anim)[0];
			m_inventoryrenderer.setTextureRect(loc);
			if(slot.item != Items::NONE) {
				ImGui::Image(m_inventoryrenderer, ImVec2(32*scale, 32*scale));

				// count
				if(slot.count > 1) {
					std::string str = std::to_string(slot.count);
					ImVec2 len = ImGui::CalcTextSize(str.c_str());
					ImGui::SetCursorPos(ImVec2(38*scale-len.x, 40*scale-len.y));
					len.x += 2;
					if(ImGui::BeginChild("##countw", len, false)) {
						ImGui::SetCursorPos(ImVec2(2,0));
						ImGui::Text(str.c_str());
						if(!hover && ImGui::IsWindowHovered())
							hover = true;
					} ImGui::EndChild();
				}
			}

			// init swap state
			if(m_swaping == nullptr) {

				// holding
				if(hover && (sf::Mouse::isButtonPressed(sf::Mouse::Left))) {
						
					m_selected = i;
					if(renderitems[i].main) {
						m_inventory.select(i);
					} else {
						m_inventory.select(INVENTORY_SIZE);
					}
						
					m_swaping = &slot;
					m_swapingt = (int)renderitems[i].type;
					m_swaptimer = 0;
				}
			} else if(m_swaping != nullptr) { // relesed mouse button, preform swap
				if(hover && !(sf::Mouse::isButtonPressed(sf::Mouse::Left))) {
					
					if(renderitems[i].type == MenuSlot::Type::ITEM) {
						if(m_swapingt == (int)MenuSlot::Type::ITEM)
							m_inventory.multswap(slot, *m_swaping);
						else if(m_swapingt == (int)MenuSlot::Type::ARMOR) {
							if((Armor::isArmor(m_swaping->item) && Armor::getArmorData(m_swaping->item).category == Armor::Category(renderitems[i].index)) || slot.item == Items::NONE)
								m_inventory.multswap(slot, *m_swaping);
						} else if(m_swapingt == (int)MenuSlot::Type::ACCESSORY) {
							if((Armor::isArmor(m_swaping->item) && Armor::getArmorData(m_swaping->item).category == Armor::Category::ACCESSORIES) || slot.item == Items::NONE)
								m_inventory.multswap(slot, *m_swaping);
						}
					} else if(renderitems[i].type == MenuSlot::Type::ARMOR) {
						if(Armor::isArmor(m_swaping->item) && Armor::getArmorData(m_swaping->item).category == Armor::Category(renderitems[i].index))
							m_inventory.multswap(slot, *m_swaping);
					} else if(renderitems[i].type == MenuSlot::Type::ACCESSORY) {
						if(Armor::isArmor(m_swaping->item) && Armor::getArmorData(m_swaping->item).category == Armor::Category::ACCESSORIES)
							m_inventory.multswap(slot, *m_swaping);
					}
					m_swaping = nullptr;
					m_swapingt = 0;
				}
			}


			// hover check
			if(hover) {
					
				// hog mouse
				m_mousea = false;

				// tooltip for info
				if(slot.item != Items::NONE && m_swaping == nullptr) {
					ImGui::BeginTooltip();
					ImGui::Text(Items::getItemData(slot.item).name);
					ImGui::Text(Items::getItemData(slot.item).data);
					ImGui::Separator();
					if(Tools::isTool(slot.item)) {
						auto& tool = Tools::getToolData(slot.item);
						if(tool.category == Tools::Category::SWORD || tool.category == Tools::Category::SPECIAL) {
							ImGui::Text("Damage: %u", tool.damage);
						} else if(tool.category == Tools::Category::PICKAXE) {
							ImGui::Text("Level: %u", tool.level);
						}

						ImGui::Text("Speed: %.2f", tool.speed);
						ImGui::Separator();
					} else if(Armor::isArmor(slot.item)) {
						auto& armor = Armor::getArmorData(slot.item);
						ImGui::Text("Defence: %.2f", armor.defence);
						ImGui::Separator();
					}
					if(m_inventory[i].count != 1) {
						ImGui::Text("Amount: %u", slot.count);
					}

					if(Armor::isArmor(slot.item) && Armor::getArmorData(slot.item).category == Armor::Category::ACCESSORIES) {
						ImGui::Separator();
						ImGui::TextColored(ImVec4(.55, .55, 1, 1), "Accessory");
					} else if(Armor::isArmor(slot.item)) {
						ImGui::Separator();
						ImGui::TextColored(ImVec4(.95, .35, .35, 1), "Armor");
					} else if(Tools::isTool(slot.item)) {
						ImGui::Separator();
						ImGui::TextColored(ImVec4(.36, .95, .35, 1), "Tool");
					}
					ImGui::EndTooltip();
				}
			}
		}
		ImGui::EndChild();
		ImGui::PopStyleColor();

	}


	// crafting menu
	if(m_inventory_a) {
		
		ImGui::SetCursorPos(ImVec2(2, (m_player.chestOpen() ? 2 : 1)*245*scale));
		if(ImGui::BeginChild("##crafting", ImVec2(405*scale+48*scale-2, 112*scale), true, ImGuiWindowFlags_NoBackground)) {

			ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(.35, .35, .65, 1));

			// set spacing between tiles to 4 pixels
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4,4));
			ImGui::SetCursorPosX(xpos-2);

			if(ImGui::IsWindowHovered(ImGuiHoveredFlags_RectOnly)) {
				m_mousea = false;
			}

			auto recipes = m_craft.list();
			for (size_t i = 0; i < recipes.size(); i++) {
				if(ImGui::BeginChild(("##craftitem" + std::to_string(i)).c_str(), ImVec2(48*scale, 48*scale), true)) {

					// render image
					if(Items::getItemData(recipes[i].item).category == Items::Category::BLOCK) {
						m_inventoryrenderer.setTexture(m_assets.getTexture(Assets::texture_blocks));
					} else {
						m_inventoryrenderer.setTexture(m_assets.getTexture(Assets::texture_items));
					}

					sf::IntRect loc = m_assets.getAnimation(Items::getItemData(recipes[i].item).anim)[0];
					m_inventoryrenderer.setTextureRect(loc);
					ImGui::Image(m_inventoryrenderer, ImVec2(32*scale, 32*scale));

					// hover check
					if(ImGui::IsWindowHovered()) {
						m_mousea = false;

						if(m_swaping == nullptr) {

							ImGui::BeginTooltip();
							ImGui::Text("%s: %u", Items::getItemData(recipes[i].item).name, recipes[i].count);
							ImGui::Text(Items::getItemData(recipes[i].item).data);
							ImGui::Separator();
							if(Tools::isTool(recipes[i].item)) {
								auto& tool = Tools::getToolData(recipes[i].item);
								if(tool.category == Tools::Category::SWORD || tool.category == Tools::Category::SPECIAL) {
									ImGui::Text("Damage: %u", tool.damage);
								} else if(tool.category == Tools::Category::PICKAXE) {
									ImGui::Text("Level: %u", tool.level);
								}

								ImGui::Text("Speed: %.2f", tool.speed);
								ImGui::Separator();
							} else if(Armor::isArmor(recipes[i].item)) {
								auto& armor = Armor::getArmorData(recipes[i].item);
								ImGui::Text("Defence: %.2f", armor.defence);
								ImGui::Separator();
							}

							ImGui::Text("Ingredients:");
							for(auto& item : recipes[i].req) {
								ImGui::Text("%s: %u", Items::getItemData(item.item).name, item.count);
							}
							ImGui::EndTooltip();

							// craft check
							if(sf::Mouse::isButtonPressed(sf::Mouse::Left) && !m_crafted) {
								
								Craft::Code c = Craft::Code::OK;
								while(c == Craft::Code::OK || c == Craft::Code::FULLINV) {
									c = m_craft.craft(recipes[i]);

									if(c == Craft::Code::FULLINV) {
										m_drops.addDrop(m_player.getPos(), {recipes[i].item, recipes[i].count});
									}

									if(!sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
										break;
									}
								}
								m_crafted = true;
							}
						}
					}


				} ImGui::EndChild();

				// new line every 8 items
				if((i+1) % 8 != 0) {
					ImGui::SameLine();
				} else {
					ImGui::SetCursorPosX(xpos-2);
				}
			}
			
			ImGui::PopStyleColor();
			ImGui::PopStyleVar();

		} ImGui::EndChild();

		if(!(sf::Mouse::isButtonPressed(sf::Mouse::Left)) && m_crafted) {
			m_crafted = false;
		}

	}

	// stats bar
	if(m_player.healTimer() > 0) {
		ImGui::Text("Can heal in %1.f seconds...", m_player.healTimer());
	}

	// trash slot
	if(m_inventory_a) {
		ImGui::SetCursorPosX(xpos +452*scale);
		ImGui::SetCursorPosY(1+3*52.f*scale);
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(.95, .35, .45, 1));
		if(ImGui::BeginChild("##trash", ImVec2(48*scale, 48*scale), true)) {
			if(m_swaping != nullptr) {
				if(ImGui::IsWindowHovered() && !(sf::Mouse::isButtonPressed(sf::Mouse::Left))) {
					m_mousea = false;
					*m_swaping = {Items::NONE, 0};
					m_swaping = nullptr;
				}
			}
			else if(ImGui::IsWindowHovered()) {
				m_mousea = false;
				if(m_swaping == nullptr) {
					ImGui::BeginTooltip();
					ImGui::Text("Trash");
					ImGui::EndTooltip();
				}
			}
		ImGui::PopStyleColor();
		} ImGui::EndChild();
	}

	if(m_swaping != nullptr) { // swap tooltip
		if(!(sf::Mouse::isButtonPressed(sf::Mouse::Left))) {
			m_swaping = nullptr;
		} else {

			m_tooltip_item = m_swaping->item;

			ImGui::BeginTooltip();
			ImGui::Text("Swap with %s", Items::getItemData(m_tooltip_item).name);
			ImGui::EndTooltip();

			if(m_tooltip_item != Items::NONE) {
				// render sprite
				if(Items::getItemData(m_tooltip_item).category == Items::Category::BLOCK)
					m_swapitem.setTexture(m_assets.getTexture(Assets::texture_blocks));
				else m_swapitem.setTexture(m_assets.getTexture(Assets::texture_items));

				m_swapitem.setTextureRect(m_assets.getAnimation(Items::getItemData(m_tooltip_item).anim)[0]);
				m_swapitem.setPosition(sf::Vector2f(m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window))));
			}

			// hog mouse
			m_mousea = false;
		}
	}

	if(m_swaptimer > 0 && !(sf::Mouse::isButtonPressed(sf::Mouse::Left))) {
		m_swaptimer = 0;
	}

	// health bar
	ImGui::PushFont(m_fontsmall);
	float hpbar_length = 300.f*scale;
	ImGui::SetCursorPos(ImVec2(m_window.getSize().x-(hpbar_length+100*scale), 0));
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 1));
	ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(.85, .15, .25, 1));
	std::string str = std::to_string((int)m_player.getHealth()) + "/" + std::to_string(PLAYER_MAX_HEALTH);
	ImGui::ProgressBar(m_player.getHealth()/PLAYER_MAX_HEALTH, ImVec2(hpbar_length, 0), str.c_str()); 
	ImGui::PopStyleColor();

	// air bar
	if(m_player.air() < 100) {
		ImGui::SetCursorPosX(m_window.getSize().x-(hpbar_length+100*scale));
		ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(.15, .25, .85, 1));
		str = std::to_string((int)m_player.air()) + "/100";
		ImGui::ProgressBar(m_player.air()/100, ImVec2(hpbar_length, 0), str.c_str());
		ImGui::PopStyleColor(2);
	}


	// check if player is alive
	if(!m_player.alive()) {
		setState(MenuState::DEAD);
	}

	// minimap
	auto& evector = m_enemies.vector();
	for(size_t i = 0; i < evector.size(); i++) {
		if(evector[i]->isBoss()) {
			m_world.addToMinimap({evector[i]->position(), MapObj::BOSS});
		} else {
			m_world.addToMinimap({evector[i]->position(), MapObj::ENEMY});
		}
	}

	// toggle minimap large mode
	bool mapbutton = sf::Keyboard::isKeyPressed(sf::Keyboard::M);
	if(mapbutton && !m_mapopen && !m_mapholding) {
		m_mapopen = true;
		m_mapholding = true;
	} else if(!mapbutton) {
		m_mapholding = false;
	}

	if(m_signediting) {
		m_mapopen = false;
	}

	if(!m_mapopen) {
		ImGui::SetCursorPos(ImVec2((m_window.getSize().x-MINIMAP_SIZE*MINIMAP_SCALE*scale) - 100*scale, 120*scale));
		sf::Sprite& min = m_world.getMinimap(m_player.getPos(), false, m_settings.getFlag(Settings::FOGOFWAR));
		min.scale(scale, scale);
		ImGui::Image(min, sf::Color::White, sf::Color::White);
	} else {
		setState(MenuState::MAP);
		m_inventory_a = false;
	}

	// boss bar
	auto& enemy_vector = m_enemies.vector();
	bool boss = false;
	Boss* boss_ptr = nullptr;
	for(size_t i = 0; i < enemy_vector.size(); i++) {
		if(enemy_vector[i]->isBoss()) {
			boss = true;
			boss_ptr = (Boss*)enemy_vector[i];
			break;
		}
	}

	if(boss) {
		float bossbar_length = m_window.getSize().x - 200;
		ImGui::SetCursorPos(ImVec2(50*scale, m_window.getSize().y - 140*scale));
		ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(.25, .15, .85, 1));
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 1));
		std::string str = std::to_string((int)boss_ptr->health()) + "/" + std::to_string((int)boss_ptr->maxHealth()) + " HP";
		ImGui::ProgressBar(boss_ptr->health()/boss_ptr->maxHealth(), ImVec2(bossbar_length, 60), str.c_str()); 
		ImGui::PopStyleColor(2);
	}
	ImGui::PopFont();

	// sign menu text
	static sf::Vector2f sign_pos;
	
	if(!m_signediting) {
		sf::Vector2f pos = m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window))/(float)TILE_SIZE;
		if(m_world.tile(pos.x, pos.y) == Blocks::SIGN && m_mousea) {

			ImGui::BeginTooltip();
			std::string& text = m_world.getSign(pos.x, pos.y);
			if(text != "") ImGui::Text("%s", text.c_str());
			else ImGui::Text("Right click to edit...");
			if(sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
				m_mousea = false;

				// allow edits
				sign_pos = pos;
				m_signediting = true;
			}
			ImGui::EndTooltip();
		}
	}

	// edit sign
	if(m_signediting) {
		if(ImGui::Begin("Edit Sign", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
			ImGui::SetWindowPos(ImVec2(m_window.getSize().x/2 - ImGui::GetWindowWidth()/2, m_window.getSize().y/2 - ImGui::GetWindowHeight()/2), ImGuiCond_Always);
			ImGui::InputTextMultiline("##edit_sign", &m_world.getSign(sign_pos.x, sign_pos.y), ImVec2(300*scale, 100*scale));
			if(ImGui::Button("Close")) {
				m_signediting = false;
			}
		}
		m_mousea = false;
		ImGui::End();
	}

	// tutorial
	if(m_tutorial)
		tutorial();

	ImGui::End();

	if(!m_mousea_old) {
		if(!sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
			m_mousea_old = true;
		} 
	} else {
		m_mousea_old = m_mousea;
	}
}