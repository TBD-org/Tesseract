#include "PanelHierarchy.h"

#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "GameObject.h"

#include "imgui.h"

#include "Leaks.h"


PanelHierarchy::PanelHierarchy() : Panel("Hierarchy", true) {}

void PanelHierarchy::Update()
{
	ImGui::SetNextWindowDockID(App->editor->dock_left_id, ImGuiCond_FirstUseEver);
	if (ImGui::Begin(name, &enabled))
	{
		GameObject* root = App->scene->root;
		if (selected_object == nullptr) 
		{
			selected_object = root;
		}
		if (root != nullptr)
		{
			UpdateHierarchyNode(root);
		}
	}
	ImGui::End();
}

void PanelHierarchy::UpdateHierarchyNode(GameObject* game_object)
{
	static std::string selected = "-1";
	char label[160];
	sprintf_s(label, "%s##%p", game_object->name.c_str(), game_object);
	if (ImGui::Selectable(label, selected == game_object->name.c_str()))
	{
		selected_object = game_object;
		selected = game_object->name.c_str();
	}
	std::vector<GameObject*> children = game_object->GetChildren();
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;
	if (children.empty()) flags |= ImGuiTreeNodeFlags_Leaf;
	if (ImGui::TreeNodeEx(label, flags))
	{
		for (GameObject* child : children)
		{
			UpdateHierarchyNode(child);
		}
		ImGui::TreePop();
	}
}