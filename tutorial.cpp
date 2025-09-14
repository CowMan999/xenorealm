#include "menu.hpp"

void Menu::tutorial() {
	ImGui::Begin("Tutorial", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	ImGui::SetWindowSize(ImVec2(320, 150));
	ImGui::SetWindowPos(ImVec2(m_window.getSize().x-ImGui::GetWindowWidth(), m_window.getSize().y-(ImGui::GetWindowHeight()*2)));
	
	switch (m_tutorial_step)
	{
	case 0:
		ImGui::TextWrapped("Welcome to Xenorealm! This is a tutorial to help you get started. Press the next button to continue.");
		break;
	
	case 1:
		ImGui::TextWrapped("Use WASD/Arrow keys to move and jump.");
		break;

	case 2:
		ImGui::TextWrapped("Try heading to the right untill you find a tree.");
		break;

	case 3:
		ImGui::TextWrapped("Congrats, you made it! Using the hotbar on the top left, click on the axe. By using the left mouse button, break the tree.");
		break;

	case 4:
		ImGui::TextWrapped("Left click can be used for other items as well. Swords, to attack enemies. Pixaxes, to break solid blocks. Axes, to break blocks that you can go through. Consumables, to summon bosses or heal. Blocks, to place them.");
		break;

	case 5:
		ImGui::TextWrapped("Try some more controls: M - Map, Tab - Full inventory, Esc - Pause");
		break;

	case 6:
		ImGui::TextWrapped("Head further to the right, until you find a chest. Open the chest with the right mouse button! Collect what is inside by dragging it into your inventory.");\
		break;
	
	case 7:
		ImGui::TextWrapped("Using the crafting menu under your inventory, try crafting stone bricks by clicking on them.");
		break;

	case 8:
		ImGui::TextWrapped("There is armor available in the chest, try equipping it via the armor slots to the right of your inventory. Further to the right are the accessory slots.");
		break;

	case 9:
		ImGui::TextWrapped("Use the bricks you crafted to create a nice bridge over the gap to the right.");
		break;

	case 10:
		ImGui::TextWrapped("Looks like you learn quick! It is advised that you create a new world to begin, as progress in the tutorial world will NOT be saved. That's all, have fun!");
		break;

	default:
		break;
	}

	if (ImGui::Button("Next")) {
		if(m_tutorial_step < 10)
			m_tutorial_step++;
	} ImGui::SameLine(); if (ImGui::Button("Back")) {
		if(m_tutorial_step > 0)
			m_tutorial_step--;
	}

	ImGui::End();
}