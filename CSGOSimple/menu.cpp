#include "Menu.hpp"
#define NOMINMAX
#include <Windows.h>
#include <chrono>

#include "valve_sdk/csgostructs.hpp"
#include "helpers/input.hpp"
#include "options.hpp"
#include "ui.hpp"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui_internal.h"
#include "imgui/impl/imgui_impl_dx9.h"
#include "imgui/impl/imgui_impl_win32.h"


// =========================================================
// 
// These are the tabs on the sidebar
// 
// =========================================================
static char* sidebar_tabs[] = {
    "Visuals",
    "Misc",
    "Test",
    "Test2"
};

constexpr static float get_sidebar_item_width() { return 150.0f; }
constexpr static float get_sidebar_item_height() { return  50.0f; }

namespace ImGuiEx
{
    inline bool ColorEdit4(const char* label, Color* v, bool show_alpha = true)
    {
        auto clr = ImVec4{
            v->r() / 255.0f,
            v->g() / 255.0f,
            v->b() / 255.0f,
            v->a() / 255.0f
        };

        if(ImGui::ColorEdit4(label, &clr.x, show_alpha)) {
            v->SetColor(clr.x, clr.y, clr.z, clr.w);
            return true;
        }
        return false;
    }
    inline bool ColorEdit3(const char* label, Color* v)
    {
        return ColorEdit4(label, v, false);
    }
}

template<size_t N>
void render_tabs(char* (&names)[N], int& activetab, float w, float h, bool sameline)
{
    bool values[N] = { false };

    values[activetab] = true;

    for(auto i = 0; i < N; ++i) {
        if(ImGui::ToggleButton(names[i], &values[i], ImVec2{ w, h })) {
            activetab = i;
        }
        if(sameline && i < N - 1)
            ImGui::SameLine();
    }
}

ImVec2 get_sidebar_size()
{
    constexpr float padding = 10.0f;
    constexpr auto size_w = padding * 2.0f + get_sidebar_item_width();
    constexpr auto size_h = padding * 2.0f + (sizeof(sidebar_tabs) / sizeof(char*)) * get_sidebar_item_height();

    return ImVec2{ size_w, ImMax(325.0f, size_h) };
}

int get_fps()
{
    using namespace std::chrono;
    static int count = 0;
    static auto last = high_resolution_clock::now();
    auto now = high_resolution_clock::now();
    static int fps = 0;

    count++;

    if(duration_cast<milliseconds>(now - last).count() > 1000) {
        fps = count;
        count = 0;
        last = now;
    }

    return fps;
}

void RenderEspTab()
{
    static char* esp_tab_names[] = { "Players", "Other" };
    static int   active_esp_tab = 0;

    bool placeholder_true = true;

    auto& style = ImGui::GetStyle();
    float group_w = ImGui::GetCurrentWindow()->Size.x - style.WindowPadding.x * 2;
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    {
        render_tabs(esp_tab_names, active_esp_tab, group_w / _countof(esp_tab_names), 25.0f, true);
    }
    ImGui::PopStyleVar();
    ImGui::BeginGroupBox("##body_content");
    {
        if(active_esp_tab == 0) {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ style.WindowPadding.x, style.ItemSpacing.y });
            ImGui::Columns(3, nullptr, false);
            ImGui::SetColumnOffset(1, group_w / 3.0f);
            ImGui::SetColumnOffset(2, 2 * group_w / 3.0f);
            ImGui::SetColumnOffset(3, group_w);

            ImGui::Checkbox("Enable ESP", &g_Options.esp_enabled);
            //ImGui::Checkbox("Enemy Only", &g_Options.esp_enemies_only);
            ImGui::Checkbox("Boxes", &g_Options.esp_player_boxes);
            ImGui::Checkbox("Names", &g_Options.esp_player_names);
            ImGui::Checkbox("Health", &g_Options.esp_player_health);
            ImGui::Checkbox("Armour", &g_Options.esp_player_armour);
            ImGui::Checkbox("Weapon", &g_Options.esp_player_weapons);
            ImGui::Checkbox("Snaplines", &g_Options.esp_player_snaplines);
			ImGui::Checkbox("Glow", &g_Options.glow_enabled);
			//ImGui::Checkbox("Glow Enemy Only", &g_Options.glow_enemies_only);
            ImGui::PopItemWidth();

            ImGui::Columns(1, nullptr, false);
            ImGui::PopStyleVar();
        } else if(active_esp_tab == 1) {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ style.WindowPadding.x, style.ItemSpacing.y });
            ImGui::Columns(3, nullptr, false);
            ImGui::SetColumnOffset(1, group_w / 3.0f);
            ImGui::SetColumnOffset(2, 2 * group_w / 3.0f);
            ImGui::SetColumnOffset(3, group_w);

			ImGui::Checkbox("Crosshair", &g_Options.esp_crosshair);
			ImGui::Checkbox("Dropped Weapons", &g_Options.esp_dropped_weapons);
			ImGui::Checkbox("Defuse Kit", &g_Options.esp_defuse_kit);
			ImGui::Checkbox("Planted C4", &g_Options.esp_planted_c4);
			ImGui::Checkbox("Item Esp", &g_Options.esp_items);
			ImGui::Checkbox("Third Person", &g_Options.misc_thirdperson);
			if (g_Options.misc_thirdperson)
				ImGui::SliderFloat("Distance", &g_Options.misc_thirdperson_dist, 0.f, 150.f);

			ImGui::NextColumn();

			ImGui::PushItemWidth(100);
			ImGui::SliderInt("viewmodel_fov:", &g_Options.viewmodel_fov, 68, 120);
			ImGui::Text("Postprocessing:");
			ImGui::SliderFloat("Red", &g_Options.mat_ambient_light_r, 0, 1);
			ImGui::SliderFloat("Green", &g_Options.mat_ambient_light_g, 0, 1);
			ImGui::SliderFloat("Blue", &g_Options.mat_ambient_light_b, 0, 1);
			ImGui::Checkbox("No Hands", &g_Options.misc_no_hands);
			//ImGuiEx::ColorEdit3("Allies Visible", &g_Options.color_esp_ally_visible);
			///mGuiEx::ColorEdit3("Enemies Visible", &g_Options.color_esp_enemy_visible);
		//	ImGuiEx::ColorEdit3("Allies Occluded", &g_Options.color_esp_ally_occluded);
			//ImGuiEx::ColorEdit3("Enemies Occluded", &g_Options.color_esp_enemy_occluded);
			//ImGuiEx::ColorEdit3("Crosshair", &g_Options.color_esp_crosshair);
			//ImGuiEx::ColorEdit3("Dropped Weapons", &g_Options.color_esp_weapons);
			//ImGuiEx::ColorEdit3("Defuse Kit", &g_Options.color_esp_defuse);
			//ImGuiEx::ColorEdit3("Planted C4", &g_Options.color_esp_c4);
			//ImGuiEx::ColorEdit3("Item Esp", &g_Options.color_esp_item);
			
           // ImGui::Checkbox("Team check", &g_Options.glow_enemies_only);
            //ImGui::Checkbox("Players", &g_Options.glow_players);
           // ImGui::Checkbox("Chickens", &g_Options.glow_chickens);
           // ImGui::Checkbox("C4 Carrier", &g_Options.glow_c4_carrier);
           // ImGui::Checkbox("Planted C4", &g_Options.glow_planted_c4);
           // ImGui::Checkbox("Defuse Kits", &g_Options.glow_defuse_kits);
           // ImGui::Checkbox("Weapons", &g_Options.glow_weapons);

            ImGui::NextColumn();

            //ImGui::PushItemWidth(100);
            //ImGuiEx::ColorEdit3("Ally", &g_Options.color_glow_ally);
            //I//mGuiEx::ColorEdit3("Enemy", &g_Options.color_glow_enemy);
            //ImGuiEx::ColorEdit3("Chickens", &g_Options.color_glow_chickens);
            //ImGuiEx::ColorEdit3("C4 Carrier", &g_Options.color_glow_c4_carrier);
            //ImGuiEx::ColorEdit3("Planted C4", &g_Options.color_glow_planted_c4);
            //ImGuiEx::ColorEdit3("Defuse Kits", &g_Options.color_glow_defuse);
          //  ImGuiEx::ColorEdit3("Weapons", &g_Options.color_glow_weapons);
            ImGui::PopItemWidth();

            ImGui::NextColumn();

            ImGui::Columns(1, nullptr, false);
            ImGui::PopStyleVar();
        }
    }
    ImGui::EndGroupBox();
}

void RenderMiscTab()
{
    bool placeholder_true = true;

    auto& style = ImGui::GetStyle();
    float group_w = ImGui::GetCurrentWindow()->Size.x - style.WindowPadding.x * 2;

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    ImGui::ToggleButton("MISC", &placeholder_true, ImVec2{ group_w, 25.0f });
    ImGui::PopStyleVar();

    ImGui::BeginGroupBox("##body_content");
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ style.WindowPadding.x, style.ItemSpacing.y });
        ImGui::Columns(3, nullptr, false);
        ImGui::SetColumnOffset(1, group_w / 3.0f);
        ImGui::SetColumnOffset(2, 2 * group_w / 3.0f);
        ImGui::SetColumnOffset(3, group_w);

        ImGui::Checkbox("Bunnyhop", &g_Options.misc_bhop);
		ImGui::Checkbox("Edge Jump", &g_Options.misc_edgejump);
		ImGui::Checkbox("Backtrack", &g_Options.misc_backtrack);
		ImGui::Checkbox("Infinity Duck", &g_Options.misc_noduckcooldown);
		ImGui::Checkbox("ClanTag", &g_Options.misc_clantagchanger);
		ImGui::Checkbox("Reveal Ranks", &g_Options.misc_showranks);
		ImGui::Checkbox("Watermark##hc", &g_Options.misc_watermark);
        //ImGui::PushItemWidth(-1.0f);
        //ImGui::PopItemWidth();

        ImGui::Columns(1, nullptr, false);
        ImGui::PopStyleVar();
    }
    ImGui::EndGroupBox();
}

void RenderEmptyTab()
{
    auto& style = ImGui::GetStyle();
    float group_w = ImGui::GetCurrentWindow()->Size.x - style.WindowPadding.x * 2;

    bool placeholder_true = true;

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0,0));
    ImGui::ToggleButton("EMPTY", &placeholder_true, ImVec2{ group_w, 25.0f });
    ImGui::PopStyleVar();

    ImGui::BeginGroupBox("##body_content");
    {
        auto message = "There's nothing here yet. Add something you want!";

        auto pos = ImGui::GetCurrentWindow()->Pos;
        auto wsize = ImGui::GetCurrentWindow()->Size;

        pos = pos + wsize / 2.0f;

        ImGui::RenderText(pos - ImGui::CalcTextSize(message) / 2.0f, message);
    }
    ImGui::EndGroupBox();
}

void Menu::Initialize()
{
	CreateStyle();

    _visible = true;
}

void Menu::Shutdown()
{
    ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void Menu::OnDeviceLost()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
}

void Menu::OnDeviceReset()
{
    ImGui_ImplDX9_CreateDeviceObjects();
}

void Menu::Render()
{
	ImGui::GetIO().MouseDrawCursor = _visible;

    if(!_visible)
        return;

    const auto sidebar_size = get_sidebar_size();
    static int active_sidebar_tab = 0;

    //ImGui::PushStyle(_style);

    ImGui::SetNextWindowPos(ImVec2{ 0, 0 }, ImGuiSetCond_Once);
    ImGui::SetNextWindowSize(ImVec2{ 1000, 0 }, ImGuiSetCond_Once);


	if (ImGui::Begin("CSGOSimple",
		&_visible,
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar)) {

		//auto& style = ImGui::GetStyle();
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0,0));
        {
            ImGui::BeginGroupBox("##sidebar", sidebar_size);
            {
				//ImGui::GetCurrentWindow()->Flags &= ~ImGuiWindowFlags_ShowBorders;

                render_tabs(sidebar_tabs, active_sidebar_tab, get_sidebar_item_width(), get_sidebar_item_height(), false);
            }
            ImGui::EndGroupBox();
        }
        ImGui::PopStyleVar();
        ImGui::SameLine();

        // Make the body the same vertical size as the sidebar
        // except for the width, which we will set to auto
        auto size = ImVec2{ 0.0f, sidebar_size.y };

		ImGui::BeginGroupBox("##body", size);
        if(active_sidebar_tab == 0) {
            RenderEspTab();
        } else if(active_sidebar_tab == 1) {
			RenderMiscTab();
        } else if(active_sidebar_tab == 2) {
			RenderEmptyTab();
        } else if(active_sidebar_tab == 3) {
			RenderEmptyTab();
        }
        ImGui::EndGroupBox();

        ImGui::TextColored(ImVec4{ 255.0f, 255.0f, 255.0f, 1.0f }, "CSGOSimple" );
        ImGui::SameLine(ImGui::GetWindowWidth() - 150 - ImGui::GetStyle().WindowPadding.x);

        ImGui::End();
    }
}

void Menu::Toggle()
{
    _visible = !_visible;
}

void Menu::CreateStyle()
{
	ImGui::StyleColorsDark();
	ImGui::SetColorEditOptions(ImGuiColorEditFlags_HEX);
	_style.Alpha = 0.f;
	_style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
	_style.WindowMinSize = ImVec2(200, 200);
	_style.FramePadding = ImVec2(4, 2);
	_style.ItemSpacing = ImVec2(6, 3);
	_style.ItemInnerSpacing = ImVec2(6, 4);
	_style.Alpha = 1.f;
	_style.WindowRounding = 0.f;
	_style.FrameRounding = 0.f;
	_style.IndentSpacing = 6.0f;
	_style.ItemInnerSpacing = ImVec2(3, 4);
	_style.ColumnsMinSpacing = 50.0f;
	_style.GrabMinSize = 14.0f;
	_style.GrabRounding = 16.0f;
	_style.ScrollbarSize = 12.0f;
	_style.ScrollbarRounding = 16.0f;
	_style.AntiAliasedLines = true;
	//_style.AntiAliasedShapes = true;

	_style.Colors[ImGuiCol_Text] = ImVec4(0.87f, 0.85f, 0.92f, 1.00f);
	_style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.87f, 0.85f, 0.92f, 0.58f);
	_style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.12f, 0.16f, 0.71f);
	_style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.27f, 0.20f, 0.39f, 0.00f);
	_style.Colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.05f, 0.10f, 0.90f);
	_style.Colors[ImGuiCol_Border] = ImVec4(0.87f, 0.85f, 0.92f, 0.30f);
	_style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	_style.Colors[ImGuiCol_FrameBg] = ImVec4(0.27f, 0.20f, 0.39f, 1.00f);
	_style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.34f, 0.19f, 0.63f, 0.68f);
	_style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.46f, 0.27f, 0.80f, 1.00f);
	_style.Colors[ImGuiCol_TitleBg] = ImVec4(0.34f, 0.19f, 0.63f, 0.45f);
	_style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.34f, 0.19f, 0.63f, 0.35f);
	_style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.34f, 0.19f, 0.63f, 0.78f);
	_style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.27f, 0.20f, 0.39f, 0.57f);
	_style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.27f, 0.20f, 0.39f, 1.00f);
	_style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.19f, 0.63f, 0.31f);
	_style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.34f, 0.19f, 0.63f, 0.78f);
	_style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.34f, 0.19f, 0.63f, 1.00f);
	//_style.Colors[ImGuiCol_ComboBg] = ImVec4(0.27f, 0.20f, 0.39f, 1.00f);
	_style.Colors[ImGuiCol_CheckMark] = ImVec4(0.34f, 0.19f, 0.63f, 0.80f);
	_style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.19f, 0.63f, 0.24f);
	_style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.34f, 0.19f, 0.63f, 1.00f);
	_style.Colors[ImGuiCol_Button] = ImVec4(0.34f, 0.19f, 0.63f, 0.44f);
	_style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.34f, 0.19f, 0.63f, 0.86f);
	_style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.34f, 0.19f, 0.63f, 1.00f);
	_style.Colors[ImGuiCol_Header] = ImVec4(0.34f, 0.19f, 0.63f, 0.76f);
	_style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.34f, 0.19f, 0.63f, 0.86f);
	_style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.34f, 0.19f, 0.63f, 1.00f);
	_style.Colors[ImGuiCol_Column] = ImVec4(0.87f, 0.85f, 0.92f, 0.32f);
	_style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.87f, 0.85f, 0.92f, 0.78f);
	_style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.87f, 0.85f, 0.92f, 1.00f);
	_style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.34f, 0.19f, 0.63f, 0.20f);
	_style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.34f, 0.19f, 0.63f, 0.78f);
	_style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.34f, 0.19f, 0.63f, 1.00f);
	//_style.Colors[ImGuiCol_CloseButton] = ImVec4(0.87f, 0.85f, 0.92f, 0.16f);
	//_style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.87f, 0.85f, 0.92f, 0.39f);
	//_style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.87f, 0.85f, 0.92f, 1.00f);
	_style.Colors[ImGuiCol_PlotLines] = ImVec4(0.87f, 0.85f, 0.92f, 0.63f);
	_style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.34f, 0.19f, 0.63f, 1.00f);
	_style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.87f, 0.85f, 0.92f, 0.63f);
	_style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.34f, 0.19f, 0.63f, 1.00f);
	_style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.34f, 0.19f, 0.63f, 0.43f);
	_style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
	ImGui::GetStyle() = _style;
}

