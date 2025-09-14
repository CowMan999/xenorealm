#include "menu.hpp"

#define CHECKBOX_FLAG(name, flag) if(ImGui::Checkbox(name, &m_settings.getFlag(flag))) changed = true

Menu::Menu(sf::RenderWindow& window, Assets& assets, Inventory& inventory, Craft& craft, Player& player, Drops& drops, Enemies& enemies, World& world, Settings& settings) : m_window(window), m_assets(assets), m_inventory(inventory), m_craft(craft), m_player(player), m_drops(drops), m_enemies(enemies), m_world(world), m_settings(settings) {

	// set up imgui
	ASSERT(ImGui::SFML::Init(m_window));
	ImGui::GetIO().IniFilename = nullptr;
	

	// state corresponds to menu
	m_state = MenuState::LOAD;
	
	m_window.setKeyRepeatEnabled(false);
	m_view = m_window.getView();

	m_swapitem.setOrigin(8,8);
	m_swapitem.setScale(2,2);

	style();

	m_size_scales = sf::Vector2f((float)m_window.getSize().x / (1920.f), (m_window.getSize().y) / (1080.f));

	ImGui::GetIO().FontGlobalScale = m_size_scales.x;
	ImGui::GetStyle().ScaleAllSizes(m_size_scales.x);
	m_view.setSize(sf::Vector2f(m_window.getSize()) / m_size_scales.x);

	m_cursortexture.loadFromFile("assets/spritesheets/cursor.png");
	m_cursorsprite.setTexture(m_cursortexture);
}

Menu::~Menu() {
	
	// clean up imgui
	ImGui::SFML::Shutdown();
}

bool Menu::loadFont() {
	ImGuiIO& io = ImGui::GetIO();

	// load font
	m_fonttiny = io.Fonts->AddFontFromFileTTF("assets/fonts/tiny.ttf", 24.f);
	m_fontsmall = io.Fonts->AddFontFromFileTTF("assets/fonts/small.ttf", 48.f);
	m_fontlarge = io.Fonts->AddFontFromFileTTF("assets/fonts/large.ttf", 72.f);

	bool suc = ImGui::SFML::UpdateFontTexture();

	m_fontloaded = true;

	return suc;
}


#define WINDOW_FLAGS ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus


// update camera
void Menu::camUpdate() {

	m_view.setCenter(m_player.getPos());
	sf::FloatRect viewrect(m_view.getCenter()-m_view.getSize()/2.f, m_view.getSize());

	// cap view to world bounds
	if(viewrect.getPosition().x < 0) m_view.move(-viewrect.getPosition().x, 0);
	if(viewrect.getPosition().y < 0) m_view.move(0, -viewrect.getPosition().y);
	if(viewrect.getPosition().x+viewrect.getSize().x > WORLD_WIDTH*TILE_SIZE) m_view.move(WORLD_WIDTH*TILE_SIZE-(viewrect.getPosition().x+viewrect.getSize().x), 0);
	if(viewrect.getPosition().y+viewrect.getSize().y > WORLD_HEIGHT*TILE_SIZE) m_view.move(0, WORLD_HEIGHT*TILE_SIZE-(viewrect.getPosition().y+viewrect.getSize().y));

	m_window.setView(m_view);
}

void Menu::update(sf::Time dt) {
	
	if(!m_fontloaded) {
		return;
	}

	float scale = m_size_scales.x;
	m_cursorsprite.setScale((m_window.getView().getSize().x/1920) * 2, (m_window.getView().getSize().y/1080) * 2);

	if(m_settings.getFlag(Settings::CUSTOMCURSOR) && !(ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)) {
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
	} else if(!m_settings.getFlag(Settings::CUSTOMCURSOR) && (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)) {
		ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouseCursorChange;
	}

	m_cursorsprite.setPosition(m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window)));

	// update imgui
	ImGui::SFML::Update(m_window, dt);

	// audio
	if(m_state != MenuState::LOAD && m_state != MenuState::PLAY && m_state != MenuState::MAP) {
		if(m_world.getAudio().playing() != Audio::Song::MENU) {
			m_world.getAudio().play(Audio::Song::MENU);
		}
	}

	static bool first = true;
	if(first) {
		first = false;
		m_world.updateBg(m_window);
		m_world.getAudio().setVolume(m_settings.volume());
	}

	// depending on state, render different menu
	switch (m_state)
	{
	case MenuState::LOAD:
	{
		if(m_settings.getFlag(Settings::CUSTOMCURSOR)){
			m_window.setMouseCursorGrabbed(true);
			m_window.setMouseCursorVisible(false);
		}else{
			m_window.setMouseCursorGrabbed(true);
			m_window.setMouseCursorVisible(true);
		}

		ImGui::Begin("##loadwindow", nullptr, WINDOW_FLAGS);
		ImGui::SetWindowPos(ImVec2(0, 0));
		ImGui::SetWindowSize(m_window.getSize());

		ImGui::PushFont(m_fontlarge);
		ImGui::SetCursorPosY((ImGui::GetWindowHeight()-ImGui::GetFontSize())*0.5f);

		centeredText("Loading...");
		ImGui::PopFont();
		ImGui::PushFont(m_fontsmall);
		centeredText(m_world.getStatus().c_str());
		ImGui::PopFont();
		ImGui::End();
		
		// check the state of the asset loader, if it's done, change state
		if (m_assets.getStatus() == Assets::status_done) {
			
			setState(MenuState::MAIN);			
			
			// cleanup
			m_assets.cleanup();

		} else if (m_assets.getStatus() == Assets::status_failed) {

			// cleanup
			m_assets.cleanup();

			// prepare an error to throw
			std::string error = "Failed to load asset: ";
			error += m_assets.getFail();

			// throw the error
			throw std::runtime_error(error);
		}
	}
	break;

	case MenuState::MAIN:
	{
		ImGui::Begin("##mainwindow", nullptr, WINDOW_FLAGS | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
		ImGui::SetWindowPos(ImVec2(0, 0));
		ImGui::SetWindowSize(m_window.getSize());

		float val = m_window.getSize().y / m_titlebgsprite.getLocalBounds().height;
		m_titlebgsprite.setScale(val, val);
		ImGui::SetCursorPos(ImVec2(0, 0));
		ImGui::Image(m_titlebgsprite, sf::Vector2f(m_window.getSize()));

		ImGui::PushFont(m_fontlarge);
		ImGui::SetCursorPosY((ImGui::GetWindowHeight()-ImGui::GetFontSize()*3-ImGui::GetStyle().ItemSpacing.y*2)*0.5f);

		centeredText(GAME_NAME);
		ImGui::PopFont();
		ImGui::PushFont(m_fontsmall);
		std::bitset<2> bits = centeredButtonRowE<2>({"Play", "Quit"}, ImVec2(300 * scale, ImGui::GetFontSize()*1.5f));
		std::bitset<2> bits2 = centeredButtonRowE<2>({"Settings", "Credits"}, ImVec2(300 * scale, ImGui::GetFontSize()*1.5f));
		centerByWidth(300 * scale);
		bool bit3 = ImGui::Button("Tutorial", ImVec2(300 * scale, ImGui::GetFontSize()*1.5f));
		if(bits[0]) setState(MenuState::SELECT);
		else if(bits[1]) m_window.close(); // close window, user has a life or something
		else if(bits2[0]) setState(MenuState::SETTINGS);
		else if(bits2[1]) setState(MenuState::CREDITS);

		else if(bit3) {
			// copy tutorial world
			if(!std::filesystem::exists("./tmp"))
				std::filesystem::create_directory("./tmp");

			if(std::filesystem::exists("./tmp/tutorial"))
				std::filesystem::remove_all("./tmp/tutorial");
				
			std::filesystem::copy("./assets/tutorial", "./tmp/tutorial");

			m_selectedworld.path = "./tmp/tutorial";
			m_selectedworld.name = "Tutorial";
			m_selectedworld.seed = "";
			m_selectedworld.abs = "";
			m_selectedworld.gametime = 0;
			m_selectedworld.hardmode = false;

			setState(MenuState::LOAD);
			m_createworld = true;
			m_tutorial = true;
			m_tutorial_step = 0;
		}
		ImGui::PopFont();

		ImGui::End();
	}
	break;
	
	case MenuState::SELECT: // world select
	{
		if(m_tutorial) {
			m_tutorial = false;
			m_tutorial_step = 0;
		}
		ImGui::Begin("##selectwindow", nullptr, WINDOW_FLAGS);
		ImGui::SetWindowPos(ImVec2(0, 0));
		ImGui::SetWindowSize(m_window.getSize());

		ImGui::PushFont(m_fontlarge);
		ImGui::SetCursorPosY(40*scale);
		centeredText("World Select");
		ImGui::SetCursorPos(ImVec2(100*scale, 200*scale));
		ImGui::PopFont();

		ImGui::PushFont(m_fonttiny);
		if(ImGui::BeginChild("##worldlist", m_window.getSize()-sf::Vector2u(200*scale, 250*scale), true)) {
			
			// list worlds
			size_t count = 0;
			for(size_t i = 0; i < m_worlds.size(); i++) {
				count++;
				auto& world = m_worlds[i];
				if(ImGui::TreeNode((world.name + "##" + std::to_string(count)).c_str())) {

					ImGui::Text("Seed: %s", world.seed.c_str());
					ImGui::Text("Playtime: %s", longToTimestamp(world.gametime).c_str());
					if(world.hardmode) {
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0.25, 0.25, 1));
						ImGui::Text("Ruthless Mode Enabled");
						ImGui::PopStyleColor();
					}
					
					if(ImGui::Button("Play")) {
						m_selectedworld = world;
						setState(MenuState::LOAD);
						m_createworld = true;
					}

					ImGui::SameLine();

					if(ImGui::Button("Delete")) {
						m_selectedworld = WorldMeta();
						bool done = std::filesystem::remove(world.path);
						if(done) {
							m_worlds.erase(m_worlds.begin() + i);
							i--;

							// iterate through the rest of the worlds and decrement their count
							size_t count2 = 0;
							for(auto& world2 : m_worlds) {
								std::filesystem::rename(world2.path, "data/worlds/world" + std::to_string(count2));
								world2.path = "data/worlds/world" + std::to_string(count2);
								world2.abs = std::filesystem::absolute(world2.path);
								count2++;
							}
						} else {
							throw std::runtime_error("Failed to delete world: " + world.path.string());
						}
					}

					ImGui::TreePop();
				}
			}

			// new world button
			ImGui::Separator();
			if(ImGui::Button("New World")) {
				setState(MenuState::NEW);
			}

			ImGui::SameLine();

			if(ImGui::Button("Cancel")) {
				setState(MenuState::MAIN);
			}

		} ImGui::EndChild();

		ImGui::PopFont();

		ImGui::End();
	}
	break;

	case MenuState::NEW:
	{
		ImGui::Begin("##newworldwindow", nullptr, WINDOW_FLAGS);
		ImGui::SetWindowPos(ImVec2(0, 0));
		ImGui::SetWindowSize(m_window.getSize());

		ImGui::PushFont(m_fontlarge);
		ImGui::SetCursorPosY(40*scale);
		centeredText("New World");
		ImGui::SetCursorPos(ImVec2(150*scale, 150*scale));
		ImGui::PopFont();

		ImGui::PushFont(m_fonttiny);
		if(ImGui::BeginChild("##newworld", m_window.getSize()-sf::Vector2u(300*scale, 200*scale), true)) {
			ImGui::InputTextWithHint("##newworldname", "World Name", &m_selectedworld.name);
			ImGui::InputTextWithHint("##newworldseed", "World Seed", &m_selectedworld.seed);
			ImGui::Checkbox("Ruthless Mode", &m_selectedworld.hardmode);
			ImGui::Indent(12);
			ImGui::Text("Warning, this mode is quite difficult!\nTry normal mode first!");
			ImGui::Unindent(12);
			ImGui::Separator();
			if(ImGui::Button("Create")) {
				if(m_selectedworld.name[0] == '\0') m_selectedworld.name = "Unnamed World";
				if(m_selectedworld.seed[0] == '\0') m_selectedworld.seed = std::to_string(Random::get(INT32_MIN, INT32_MAX));

				setState(MenuState::LOAD);
				m_createworld = true;
			}
			ImGui::SameLine();
			if(ImGui::Button("Cancel")) {

				setState(MenuState::SELECT);
			}
		} ImGui::EndChild();

		ImGui::PopFont();

		ImGui::End();
	}
	break;

	case MenuState::DEAD:
	case MenuState::PAUSE:
	{

		if(m_state == MenuState::PAUSE && m_settings.getFlag(Settings::INVISPAUSE) && m_settings.getFlag(Settings::DEBUG)) {
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) 
				setState(MenuState::SETTINGS);
			break;
		}

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 255));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(.1, .1, .1, 255));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(.2, .2, .2, 255));

		if(m_state == MenuState::DEAD) {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, .2, 255));
		}


		ImGui::Begin("##pausewindow", nullptr, WINDOW_FLAGS | ImGuiWindowFlags_NoBackground);
		ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Once);
		ImGui::SetWindowSize(m_window.getSize());

		ImGui::PushFont(m_fontlarge);
		ImGui::SetCursorPosY(40*scale);			
		if(m_state == MenuState::PAUSE) {
			centeredText("Paused");
		} else if(m_state == MenuState::DEAD) {
			centeredText("You were slain...");
		}
		ImGui::SetCursorPosY(200*scale);
		ImGui::PopFont();

		if(m_state == MenuState::DEAD) {
			ImGui::PopStyleColor();
		}

		ImGui::PushFont(m_fontsmall);
		std::bitset<3> bits = centeredButtonRowE<3>({"Save", (m_state == MenuState::PAUSE ? "Resume" : "Retry"), "Quit"}, ImVec2(250*scale, ImGui::GetFontSize()*2));

		if(bits[1]) {
			if(m_state == MenuState::DEAD) m_player.reset();
			setState(MenuState::PLAY);
			m_world.getAudio().revert();
		}
		else if(bits[0]) {
			if(m_state == MenuState::DEAD) m_player.reset();
			m_save = true;
		}
		else if(bits[2]) {
			if(m_state == MenuState::DEAD) m_player.reset();
			m_quit = true;
		}

		std::bitset<2> bits2 = centeredButtonRowE<2>({"Settings", "Crafting"}, ImVec2(250*scale, ImGui::GetFontSize()*2));
		
		if(bits2[0]) setState(MenuState::SETTINGS);
		else if(bits2[1]) setState(MenuState::CRAFTING);

		ImGui::PopFont();
	
		ImGui::End();

		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
	}
	break;

	case MenuState::MAP:
	{
		ImGui::Begin("##mapwindow", nullptr, ImGuiWindowFlags_NoBackground | WINDOW_FLAGS | ImGuiWindowFlags_NoDecoration);
		ImGui::SetWindowPos(ImVec2(0, 0));
		ImGui::SetWindowSize(m_window.getSize());

		bool mapbutton = (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::M)) && !m_signediting;
		if(mapbutton && m_mapopen && !m_mapholding) {
			m_mapopen = false;
			m_mapholding = true;
		} else if(!mapbutton) {
			m_mapholding = false;
		}

		// down to... float?
		float dtf = dt.asSeconds();

		if(m_map_pos_old.x != -1) {
			m_map_pos = m_map_pos_old + (m_map_drag - sf::Vector2f(sf::Mouse::getPosition(m_window))) / m_mapzoom;
		}

		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
			m_map_pos.y -= (float)MINIMAP_SPEED/m_mapzoom * dtf * scale;
		} if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
			m_map_pos.y += (float)MINIMAP_SPEED/m_mapzoom * dtf * scale;
		} if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
			m_map_pos.x -= (float)MINIMAP_SPEED/m_mapzoom * dtf * scale;
		} if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
			m_map_pos.x += (float)MINIMAP_SPEED/m_mapzoom * dtf * scale;
		} if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q)) {
			m_mapzoom += MINIMAP_ZSPEED * dtf;
		} if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E)) {
			m_mapzoom -= MINIMAP_ZSPEED * dtf;
		}

		if(m_mapzoom < 1) m_mapzoom = 1;
		else if(m_mapzoom > MINIMAP_MIN_ZOOM) m_mapzoom = MINIMAP_MIN_ZOOM;
		if(m_map_pos.x < (float)WORLD_WIDTH/(m_mapzoom*2)) m_map_pos.x = (float)WORLD_WIDTH/(m_mapzoom*2);
		else if(m_map_pos.x > (float)WORLD_WIDTH - (float)WORLD_WIDTH/(m_mapzoom*2)) m_map_pos.x = (float)WORLD_WIDTH - (float)WORLD_WIDTH/(m_mapzoom*2);
		if(m_map_pos.y < (float)WORLD_HEIGHT/(m_mapzoom*2)) m_map_pos.y = (float)WORLD_HEIGHT/(m_mapzoom*2);
		else if(m_map_pos.y > (float)WORLD_HEIGHT - (float)WORLD_HEIGHT/(m_mapzoom*2)) m_map_pos.y = (float)WORLD_HEIGHT - (float)WORLD_HEIGHT/(m_mapzoom*2);
		
		m_maprect.width = (float)WORLD_WIDTH / m_mapzoom;
		m_maprect.height = (float)WORLD_HEIGHT / m_mapzoom;
		m_maprect.left = m_map_pos.x - m_maprect.width/2;
		m_maprect.top = m_map_pos.y - m_maprect.height/2;

		ImGui::SetCursorPos(ImVec2(MINIMAP_FULLSCREEN_BORDER, MINIMAP_FULLSCREEN_BORDER*2 * scale));
		bool fog = m_settings.getFlag(Settings::FOGOFWAR);
		if(!fog && !m_settings.getFlag(Settings::DEBUG))
			fog = true;
		ImGui::Image(m_world.getMinimap(m_player.getPos(), true, fog, &m_maprect), sf::Color::White, sf::Color::White);

		if(m_mapopen == false) {
			setState(MenuState::PLAY);
			m_map_pos = sf::Vector2f(WORLD_WIDTH/2, WORLD_HEIGHT/2);
			m_mapzoom = 1;
			m_maprect = sf::IntRect(0, 0, WORLD_WIDTH, WORLD_HEIGHT);
		}		

		ImGui::End();
	}
	break;

	case MenuState::SETTINGS: // settings
	{
		ImGui::Begin("##settingwindow", nullptr, WINDOW_FLAGS);
		ImGui::SetWindowPos(ImVec2(0, 0));
		ImGui::SetWindowSize(m_window.getSize());

		ImGui::PushFont(m_fontlarge);
		ImGui::SetCursorPosY(40*scale);
		centeredText("Settings");
		ImGui::SetCursorPos(ImVec2(100*scale, 200*scale));
		ImGui::PopFont();
		ImGui::PushFont(m_fonttiny);

		if(ImGui::BeginChild("##settinglist", m_window.getSize()-sf::Vector2u(200*scale, 350*scale), true)) {
			
			bool changed = false;

			// window config
			if(ImGui::TreeNode("Window Configuration")) {
				ImGui::Text("Resolution: %i x %i", m_window.getSize().x, m_window.getSize().y);
				CHECKBOX_FLAG("Fullscreen", Settings::FULLSCREEN);
				CHECKBOX_FLAG("VSync", Settings::VSYNC);

				bool fpscap = m_settings.framerate() != 0;

				if(!m_settings.getFlag(Settings::VSYNC)) {
					if(ImGui::Checkbox("Framerate Cap Enabled", &fpscap)) {
						if(!fpscap)
							m_settings.framerate() = 0;
						else
							m_settings.framerate() = 60;
						changed = true;
					}

					if(fpscap) {
						if(ImGui::SliderInt("Framerate Cap", &m_settings.framerate(), MIN_FRAMERATE, 500))
							changed = true;
					}
				}
				ImGui::TreePop();
			}

			// audio config
			if(ImGui::TreeNode("Audio Configuration")) {
				if(ImGui::SliderInt("Volume", &m_settings.volume(), 0, 100))
					changed = true;

				ImGui::TreePop();
			}

			// visual config
			if(ImGui::TreeNode("Visual Configuration")) {
				CHECKBOX_FLAG("Custom Cursor", Settings::CUSTOMCURSOR);
				CHECKBOX_FLAG("Anti Aliasing", Settings::ANTIALIASING);
				ImGui::TreePop();
			}

			// other config
			if(ImGui::TreeNode("Advanced Configuration")) {
				ImGui::InputInt("Major OpenGL Version", &m_settings.major(), 1, 1);
				ImGui::InputInt("Minor OpenGL Version", &m_settings.minor(), 1, 1);
				if(ImGui::Button("Apply OpenGL Version")) changed = true;

				CHECKBOX_FLAG("Debug Mode", Settings::DEBUG);

				// debug flags
				if(m_settings.getFlag(Settings::DEBUG) && ImGui::TreeNode("Debug Configuration")) {
					CHECKBOX_FLAG("Game Information", Settings::GAMEDATA);
					CHECKBOX_FLAG("Gui Metrics", Settings::METRICS);
					CHECKBOX_FLAG("Style Editor", Settings::STYLEEDIT);
					CHECKBOX_FLAG("Super Speed", Settings::SPEED);
					CHECKBOX_FLAG("Invisible Pause Menu", Settings::INVISPAUSE);
					CHECKBOX_FLAG("Fog of War", Settings::FOGOFWAR);
					CHECKBOX_FLAG("Enemy Spawns", Settings::ENEMYSPAWNING);
					if(ImGui::Button("Day")) {
						m_world.settime(0, 1);
					}
					ImGui::SameLine();
					if(ImGui::Button("Night")) {
						m_world.settime(DAY_LENGTH/2, 0);
					}
					if(ImGui::Button("Morning")) {
						m_world.settime(DAY_LENGTH/4, 0);
					}
					ImGui::SameLine();
					if(ImGui::Button("Evening")) {
						m_world.settime(DAY_LENGTH/4, 1);
					}

					static int block;
					static int count;
					if(m_prevstate == MenuState::PAUSE || m_prevstate == MenuState::DEAD) {
						ImGui::InputInt("ItemID", &block, 1, 10);
						ImGui::InputInt("Count", &count, 1, 100);
						if(ImGui::Button("Append Item")) {
							m_inventory.append({(Items::Type)block, (unsigned int)count});
						}
					}

					ImGui::TreePop();
				}

				ImGui::TreePop();
			}

			if(changed) {
				m_settings.apply();
				m_world.updateBg(m_window);
				m_world.getAudio().setVolume(m_settings.volume());
				sf::Event event; event.type = sf::Event::Resized; event.size.width = m_window.getSize().x; event.size.height = m_window.getSize().y;
				handleEvent(event);
			}

			if(ImGui::Button("Back")) {
				m_settings.save();
				setState(m_prevstate);
			}

		} ImGui::EndChild();

		ImGui::PopFont();

		ImGui::End();
	}
	break;

	case MenuState::CRAFTING:
	{
		ImGui::Begin("##craftingwindow", nullptr, WINDOW_FLAGS);
		ImGui::SetWindowPos(ImVec2(0, 0));
		ImGui::SetWindowSize(m_window.getSize());

		ImGui::PushFont(m_fontlarge);
		ImGui::SetCursorPosY(40*scale);
		centeredText("Crafting Tree");
		ImGui::SetCursorPos(ImVec2(100*scale, 200*scale));
		ImGui::PopFont();
		ImGui::PushFont(m_fonttiny);
		size_t count = 0;

		static auto lambda = [&](const std::vector<Recipe>& list) {
			for(auto& recipe : list) {
				count++;
				if(ImGui::TreeNode((Items::getItemData(recipe.item).name + std::string("##") + std::to_string(count)).c_str())) {
					ImGui::Separator();
					ImGui::Indent();
					ImGui::Text(Items::getItemData(recipe.item).data);
					ImGui::Dummy(ImVec2(0, 10));
					for(auto& ingredient : recipe.req) {
						auto& data = Items::getItemData(ingredient.item);
						ImGui::Text("%s x%i", data.name, ingredient.count);
					}
					ImGui::Unindent();
					ImGui::Separator();
					ImGui::TreePop();
				}
			}
		};

		if(ImGui::BeginChild("##settinglist", m_window.getSize()-sf::Vector2u(200*scale, 350*scale), true)) {

			if(ImGui::TreeNode("Craftable Items")) {
				auto craftable = m_craft.list();
				lambda(craftable);
				ImGui::TreePop();
			}

			if(ImGui::TreeNode("Uncraftable Items")) {
				auto uncraftable = m_craft.imp();
				lambda(uncraftable);
				ImGui::TreePop();
			}

			if(ImGui::TreeNode("All Items")) {
				auto all = m_craft.all();
				lambda(all);
				ImGui::TreePop();
			}


			if(ImGui::Button("Back")) {
				setState(m_prevstate);
			}
		} ImGui::EndChild();
			
		ImGui::PopFont();

		ImGui::End();
	}
	break;

	case MenuState::CREDITS:
	{
		ImGui::Begin("##creditwindow", nullptr, WINDOW_FLAGS);
		ImGui::SetWindowPos(ImVec2(0, 0));
		ImGui::SetWindowSize(m_window.getSize());

		ImGui::PushFont(m_fontlarge);
		ImGui::SetCursorPosY(40*scale);
		centeredText("Credits");
		ImGui::SetCursorPos(ImVec2(150*scale, 150*scale));
		ImGui::PopFont();

		ImGui::PushFont(m_fontsmall);
		if(ImGui::BeginChild("##creditsbox", m_window.getSize()-sf::Vector2u(300*scale, 200*scale), true)) {
			ImGui::TextWrapped(R"""(This game was developed by Edward Mayen, alongside the contributions of others.

SFML Team, SFML Library.
Ocornut and Contributors, Dear ImGui Library.

All my friends and family who supported me throughout the development of this game.

Thank you for playing!)""");
		
		if(ImGui::Button("Back"))
			setState(m_prevstate);
		} ImGui::EndChild();


		ImGui::PopFont();

		ImGui::End();
	}
	break;

	case MenuState::PLAY: // game open
	ingameGui(scale);
	break;

	default:
		break;
	}

	// debug window
	if(m_settings.getFlag(Settings::DEBUG)) {
		
		if(!TRAILER_BUILD) {
			ImGui::Begin("Debug", nullptr, WINDOW_FLAGS | ImGuiWindowFlags_NoBackground);
			ImGui::PushStyleColor(ImGuiCol_Text, sf::Color(185, 185, 185, 255));
			ImGui::SetWindowPos(m_window.getSize()-sf::Vector2u(265*scale, 115*scale));
			ImGui::SetWindowSize(sf::Vector2f(265, 130)*scale);

			ImGui::Text("DEBUG MODE ON");
			ImGui::Text(GAME_DATA);
			ImGui::Text("OpenGL Version: %i.%i", m_window.getSettings().majorVersion, m_window.getSettings().minorVersion);
			ImGui::Text("FPS: %2.f", ImGui::GetIO().Framerate);

			if(ImGui::Button("sigtrap")) {
				__debugbreak();
			}

			ImGui::PopStyleColor();

			ImGui::End();
		}

		if(m_settings.getFlag(Settings::GAMEDATA)) {
			ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_Once);
			ImGui::Begin("Game Data", nullptr);

			ImGui::Text("Framerate: %2.f", 1.f/dt.asSeconds());
			ImGui::Text("PlayerPos: (%.2f, %.2f)", m_player.getPos().x, m_player.getPos().y);
			ImGui::Text("PlayerPosWorld: (%.2f, %.2f)", m_player.getPos().x/TILE_SIZE, m_player.getPos().y/TILE_SIZE);
			ImGui::Text("PlayerOrientation: (%.3f, %.3f)", m_player.direction().x, m_player.direction().y);
			ImGui::Text("EnemyCount: %llu", m_enemies.vector().size());
			ImGui::Text("SelectedSlot: (%i, %u, %llu)", (int)m_inventory.selectedS().item, m_inventory.selectedS().count, m_inventory.selected());
			ImGui::Text("ChestOpened: (%i, %i)", m_player.getChestOpen().x, m_player.getChestOpen().y);
			if(ImGui::Button("GuiPicker")) {
				ImGui::DebugStartItemPicker();
			}
			if(ImGui::Button("BlockPicker")) {
				m_block_picker = true;
			}

			if(m_block_picker && ImGui::Begin("Block Picker", &m_block_picker)) {
				ImGui::Text("Block Picker");
				sf::Vector2i m_pos = sf::Vector2i((sf::Vector2f(sf::Mouse::getPosition(m_window)) + m_window.getView().getCenter() - m_window.getView().getSize()/2.f)/(float)TILE_SIZE);
				ImGui::Text("MousePos: (%i, %i)", m_pos.x, m_pos.y);
				ImGui::Text("Block: %i", (int)m_world.tile(m_pos.x, m_pos.y));
				ImGui::Text("BlockBg: %i", (int)m_world.tilebg(m_pos.x, m_pos.y));
				ImGui::Text("BlockLightUpdate: %i", (int)m_world.brightnessUpdated(m_pos.x, m_pos.y));
			} ImGui::End();

			ImGui::End();
		}

		if(m_settings.getFlag(Settings::METRICS)) {
			ImGui::SetNextWindowPos(ImVec2(250, 50), ImGuiCond_Once);
			ImGui::ShowMetricsWindow();
		}

		if(m_settings.getFlag(Settings::STACKTOOL)) {
			ImGui::SetNextWindowPos(ImVec2(450, 50), ImGuiCond_Once);
			ImGui::ShowStackToolWindow();
		}

		if(m_settings.getFlag(Settings::STYLEEDIT)) {
			ImGui::SetNextWindowPos(ImVec2(650, 50), ImGuiCond_Once);
			ImGui::ShowStyleEditor();
		}

		if(m_settings.getFlag(Settings::SPEED)) {
			m_player.getBuffs().add(Buff::SPEED, 16);
			m_player.getBuffs().add(Buff::JUMP, 12);
		}
	}

	// render imgui
	ImGui::SFML::Render(m_window);

	// swapitem
	if(m_swaping != nullptr) {
		if(m_tooltip_item != Items::NONE) {
			m_swapitem.setOrigin(m_swapitem.getLocalBounds().getSize()/2.f);
			m_window.draw(m_swapitem);
		}
	}

	if(m_settings.getFlag(Settings::CUSTOMCURSOR)) {
		m_window.draw(m_cursorsprite);
	}
}

void Menu::handleEvent(sf::Event& e) {
	
	// pass event to imgui
	ImGui::SFML::ProcessEvent(e);

	// handle events
	switch(e.type) {
	case sf::Event::KeyPressed:
		if(m_signediting) break;
		if(e.key.code == sf::Keyboard::Escape) {
			if(m_state == MenuState::PLAY) setState(MenuState::PAUSE);
			else if(m_state == MenuState::PAUSE) setState(MenuState::PLAY);
		} else if(e.key.code == sf::Keyboard::Tab && m_state == MenuState::PLAY) {
			m_inventory_a = !m_inventory_a;

			if(!m_inventory_a) {
				m_player.getChestOpen() = sf::Vector2i(-1, -1);
			}
		}

		if(m_swaping == nullptr) {

			// swap hotbar pos
			if(!m_inventory_a && getState() == MenuState::PLAY) {
				for (size_t i = sf::Keyboard::Num1; i < sf::Keyboard::Num1+INVENTORY_SIZE/INVENTORY_ROWS; i++) {
					if(e.key.code == (sf::Keyboard::Key)i) {
						m_inventory.select(i-sf::Keyboard::Num1);
						m_selected = m_inventory.selected();
					}
				}
			}
			
		}

		break;

	case sf::Event::Resized:
		m_view.setSize(e.size.width / m_size_scales.x, e.size.height / m_size_scales.x);
		break;

	case sf::Event::MouseButtonPressed:
		if(getState() == MenuState::MAP) {
			if(e.mouseButton.button == sf::Mouse::Left) {
				m_map_drag = sf::Vector2f(e.mouseButton.x, e.mouseButton.y);
				m_map_pos_old = m_map_pos;
			}
		}
		break;

	case sf::Event::MouseButtonReleased:
		if(getState() == MenuState::MAP) {
			if(e.mouseButton.button == sf::Mouse::Left) {
				m_map_drag = sf::Vector2f(-1, -1);
				m_map_pos_old = sf::Vector2f(-1, -1);
			}
		}
		break;

	case sf::Event::MouseWheelScrolled:
		if(getState() == MenuState::MAP) {
			m_mapzoom += e.mouseWheelScroll.delta/2;
		} else if(getState() == MenuState::PLAY && m_mousea) {
			if(e.mouseWheelScroll.delta > 0) {
				m_inventory.select(m_inventory.selected()+1 > INVENTORY_SIZE/INVENTORY_ROWS-1 ? INVENTORY_SIZE/INVENTORY_ROWS-1 : m_inventory.selected()+1);
				m_selected = m_inventory.selected();
			} else if(e.mouseWheelScroll.delta < 0) {
				m_inventory.select(m_inventory.selected()-1 > INVENTORY_SIZE/INVENTORY_ROWS-1 ? 0 : m_inventory.selected()-1);
				m_selected = m_inventory.selected();
			}
		}
		break;

	default:
		break;
	}
}

bool Menu::initWorldSelect() {
	m_worlds.clear();
	
	// check if world folder exists
	if(!std::filesystem::exists("data/worlds")) {
		std::filesystem::create_directory("data/worlds");
	}

	// get world list
	std::vector<std::filesystem::path> worlds;
	for(auto& p : std::filesystem::directory_iterator("data/worlds")) {
		worlds.push_back(p.path());
	}

	// load world meta
	for(auto& world : worlds) {
		std::ifstream file(world);
		WorldMeta meta;

		if(file.is_open()) {
			meta.path = world;
			meta.abs = std::filesystem::absolute(world);

			char* ptr;

			size_t len;
			file >> len;
			file.get();
			ptr = new char[len+1] {0};
			file.read(ptr, len);
			meta.name = ptr;
			delete[] ptr;
			
			file >> len;
			file.get();
			ptr = new char[len+1] {0};
			file.read(ptr, len);
			meta.seed = ptr;
			delete[] ptr;

			file.read(reinterpret_cast<char*>(&meta.gametime), sizeof(size_t));
			file.read(reinterpret_cast<char*>(&meta.hardmode), sizeof(bool));
		} else {
			return false;
		}

		m_worlds.push_back(meta);
	}

	return true;
}

void Menu::appendWorld(const WorldMeta& world) {
	m_worlds.push_back(world);
}

void Menu::centeredText(const char* text) {
	ImGui::SetCursorPosX((ImGui::GetWindowSize().x-ImGui::CalcTextSize(text).x)/2);
	ImGui::Text(text);
}

void Menu::centerByWidth(const float width)
{
	ImGui::SetCursorPosX((ImGui::GetWindowWidth()-width)*0.5f);
}

void Menu::setState(MenuState state) {
	m_prevstate = m_state;
	m_state = state;
}

MenuState Menu::getState() {
	return m_state;
}

bool& Menu::mouseAvailable() {
	return m_mousea;
}

bool& Menu::createWorld() {
	return m_createworld;
}

WorldMeta& Menu::worldMetadata() {
	return m_selectedworld;
}

bool Menu::save() {
	if(m_save) {
		m_save = false;
		return true;
	} else {
		return false;
	}
}

bool Menu::quit() {
	if(m_quit) {
		m_quit = false;
		setState(MenuState::SELECT);
		return true;
	} else {
		return false;
	}
}

bool Menu::mapFullscreen() {
	return m_mapopen && !m_inventory_a;
}

std::string Menu::longToTimestamp(size_t timestamp) {
	std::string str;
	unsigned char seconds = timestamp % 60; timestamp /= 60;
	unsigned char minutes = timestamp % 60; timestamp /= 60;
	unsigned int hours = timestamp % 24;

	if(hours < 10) str += "0";
	str += std::to_string(hours) + ":";
	if(minutes < 10) str += "0";
	str += std::to_string(minutes) + ":";
	if(seconds < 10) str += "0";
	str += std::to_string(seconds);
	
	return str;
}

bool Menu::hogKeyboard() {
	return m_signediting;
}

bool Menu::loadTitleBg() {
	if(!m_titlebg.loadFromFile("assets/backgrounds/background_title.png")) {
		return false;
	}

	m_titlebgsprite.setTexture(m_titlebg);
	return true;
}