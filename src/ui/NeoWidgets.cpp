#include "ui/NeoWidgets.hpp"
#include "imgui.h"

namespace App::UI::NeoWidgets {

    void BeginBrutalistCard(const char* strId, float width, float height, uint32_t bgColor) {
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImVec2 size = ImVec2(width > 0 ? width : ImGui::GetContentRegionAvail().x, height > 0 ? height : 100.0f);

        // Draw hard drop shadow offset (+4, +4)
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        drawList->AddRectFilled(
            ImVec2(p.x + 4.0f, p.y + 4.0f),
            ImVec2(p.x + size.x + 4.0f, p.y + size.y + 4.0f),
            0xFF000000, 0.0f
        );

        ImGui::PushStyleColor(ImGuiCol_ChildBg, bgColor ? bgColor : ImGui::GetColorU32(ImGuiCol_ChildBg));
        ImGui::BeginChild(strId, size, ImGuiChildFlags_Borders, ImGuiWindowFlags_None);
    }

    void EndBrutalistCard() {
        ImGui::EndChild();
        ImGui::PopStyleColor();
    }

    bool BrutalistButton(const char* label, float width, float height, uint32_t accentColor) {
        if (accentColor != 0) {
            ImGui::PushStyleColor(ImGuiCol_Button, accentColor);
        }

        ImVec2 p = ImGui::GetCursorScreenPos();
        ImVec2 size = ImVec2(width > 0 ? width : ImGui::CalcTextSize(label).x + 24.0f, height > 0 ? height : 36.0f);

        // Draw shadow under button
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        drawList->AddRectFilled(
            ImVec2(p.x + 3.0f, p.y + 3.0f),
            ImVec2(p.x + size.x + 3.0f, p.y + size.y + 3.0f),
            0xFF000000, 0.0f
        );

        bool clicked = ImGui::Button(label, size);

        if (accentColor != 0) {
            ImGui::PopStyleColor();
        }
        return clicked;
    }

    void BrutalistBadge(const char* text, uint32_t bgColor, uint32_t textColor) {
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 textSize = ImGui::CalcTextSize(text);
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImVec2 padding(8.0f, 4.0f);
        ImVec2 boxSize(textSize.x + padding.x * 2, textSize.y + padding.y * 2);

        // Drop shadow
        drawList->AddRectFilled(ImVec2(p.x + 2, p.y + 2), ImVec2(p.x + boxSize.x + 2, p.y + boxSize.y + 2), 0xFF000000);
        // Badge body
        drawList->AddRectFilled(p, ImVec2(p.x + boxSize.x, p.y + boxSize.y), bgColor);
        // Border
        drawList->AddRect(p, ImVec2(p.x + boxSize.x, p.y + boxSize.y), 0xFF000000, 0.0f, 0, 2.0f);
        // Text
        drawList->AddText(ImVec2(p.x + padding.x, p.y + padding.y), textColor, text);

        ImGui::Dummy(boxSize);
    }

    void StatCard(const char* label, const std::string& value, uint32_t accentColor, const char* icon) {
        BeginBrutalistCard(label, 0.0f, 90.0f, accentColor);
        ImGui::TextDisabled("%s", icon ? icon : "// KPI");
        ImGui::TextUnformatted(label);
        ImGui::SetWindowFontScale(1.4f);
        ImGui::TextColored(ImVec4(0, 0, 0, 1), "%s", value.c_str());
        ImGui::SetWindowFontScale(1.0f);
        EndBrutalistCard();
    }

} // namespace App::UI::NeoWidgets
