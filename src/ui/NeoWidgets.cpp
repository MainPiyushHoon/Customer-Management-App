// src/ui/NeoWidgets.cpp
#include "ui/NeoWidgets.hpp"
#include "ui/ThemeManager.hpp"
#include "imgui.h"
#include <algorithm>
#include <cstring>

namespace App::UI::NeoWidgets {

    ImVec2 CalculateButtonSize(const char* label, float width, float height) {
        const char* safeLabel = label ? label : "";
        ImVec2 textSize = ImGui::CalcTextSize(safeLabel, nullptr, true);
        float minWidth = textSize.x + 32.0f; // 16px horizontal padding each side
        float actualWidth = (width > 0.0f) ? std::max(width, minWidth) : minWidth;
        float actualHeight = (height > 0.0f) ? std::max(height, 36.0f) : 36.0f;
        return ImVec2(actualWidth, actualHeight);
    }

    void BeginBrutalistCard(const char* strId, float width, float height, uint32_t bgColor) {
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImVec2 size = ImVec2(width > 0 ? width : ImGui::GetContentRegionAvail().x, height > 0 ? height : 100.0f);

        // Draw hard drop shadow offset (+4.0f, +4.0f)
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
        const char* safeLabel = label ? label : "";
        ImVec2 size = CalculateButtonSize(safeLabel, width, height);
        ImVec2 p = ImGui::GetCursorScreenPos();

        // Unique ID for InvisibleButton to capture input & state
        const char* strId = (safeLabel[0] != '\0') ? safeLabel : "##btn";
        bool clicked = ImGui::InvisibleButton(strId, size);
        bool isActive = ImGui::IsItemActive();
        bool isHovered = ImGui::IsItemHovered();

        // Tactile Mechanical Displacement:
        // - Active (held down): button shifts (+3.0f, +3.0f), shadow collapses to 1.0f
        // - Hovered (not active): button at (0, 0), shadow expands to 5.0f (lift)
        // - At rest: button at (0, 0), shadow at 4.0f
        ImVec2 renderOffset = isActive ? ImVec2(3.0f, 3.0f) : ImVec2(0.0f, 0.0f);
        float shadowOffset = isActive ? 1.0f : (isHovered ? 5.0f : 4.0f);

        ImDrawList* drawList = ImGui::GetWindowDrawList();

        // 1. Drop shadow
        ImVec2 shadowPos = ImVec2(p.x + renderOffset.x + shadowOffset, p.y + renderOffset.y + shadowOffset);
        drawList->AddRectFilled(
            shadowPos,
            ImVec2(shadowPos.x + size.x, shadowPos.y + size.y),
            0xFF000000, 0.0f
        );

        // 2. Button Body
        uint32_t effectiveColor = (accentColor != 0) ? accentColor : ImGui::GetColorU32(isHovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
        ImVec2 btnMin = ImVec2(p.x + renderOffset.x, p.y + renderOffset.y);
        ImVec2 btnMax = ImVec2(btnMin.x + size.x, btnMin.y + size.y);

        drawList->AddRectFilled(btnMin, btnMax, effectiveColor, 0.0f);

        // 3. Thick Solid Border (2.5px)
        uint32_t borderColor = ImGui::GetColorU32(ImGuiCol_Border);
        drawList->AddRect(btnMin, btnMax, borderColor, 0.0f, 0, 2.5f);

        // 4. Centered Button Label
        float lum = ThemeManager::calculateLuminance(effectiveColor);
        uint32_t textColor = (lum > 0.5f) ? 0xFF000000 : 0xFFFFFFFF;

        const char* textEnd = safeLabel;
        const char* hashPos = strstr(safeLabel, "##");
        if (hashPos) {
            textEnd = hashPos;
        } else {
            textEnd = safeLabel + strlen(safeLabel);
        }

        ImVec2 textSize = ImGui::CalcTextSize(safeLabel, textEnd, false);
        ImVec2 textPos = ImVec2(
            btnMin.x + (size.x - textSize.x) * 0.5f,
            btnMin.y + (size.y - textSize.y) * 0.5f
        );
        drawList->AddText(textPos, textColor, safeLabel, textEnd);

        return clicked;
    }

    void BrutalistBadge(const char* text, uint32_t bgColor, uint32_t textColor) {
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        const char* safeText = text ? text : "";
        ImVec2 textSize = ImGui::CalcTextSize(safeText);
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImVec2 padding(10.0f, 6.0f);
        ImVec2 boxSize(textSize.x + padding.x * 2, textSize.y + padding.y * 2);

        // Drop shadow (standardized 4.0f offset)
        drawList->AddRectFilled(
            ImVec2(p.x + 4.0f, p.y + 4.0f),
            ImVec2(p.x + boxSize.x + 4.0f, p.y + boxSize.y + 4.0f),
            0xFF000000
        );

        // Badge body
        drawList->AddRectFilled(p, ImVec2(p.x + boxSize.x, p.y + boxSize.y), bgColor);

        // Border (thick 2.0f solid)
        drawList->AddRect(p, ImVec2(p.x + boxSize.x, p.y + boxSize.y), 0xFF000000, 0.0f, 0, 2.0f);

        // Anti-dark text safety: if black font is requested, ensure background is high luminance, else white
        uint32_t effectiveText = textColor;
        if (textColor == 0xFF000000) {
            float lum = ThemeManager::calculateLuminance(bgColor);
            if (lum < 0.5f) {
                effectiveText = 0xFFFFFFFF;
            }
        }

        // Text
        drawList->AddText(ImVec2(p.x + padding.x, p.y + padding.y), effectiveText, safeText);

        ImGui::Dummy(boxSize);
    }

    void StatCard(const char* label, const std::string& value, uint32_t accentColor, const char* icon) {
        float lineHeight = ImGui::GetTextLineHeightWithSpacing();
        float neededHeight = 24.0f + lineHeight * 3.0f + 16.0f;
        float cardHeight = std::max(115.0f, neededHeight);

        BeginBrutalistCard(label ? label : "##stat", 0.0f, cardHeight, accentColor);

        // Contrast Calculation: determine if background is light or dark
        uint32_t effectiveBg = accentColor ? accentColor : ImGui::GetColorU32(ImGuiCol_ChildBg);
        float lum = ThemeManager::calculateLuminance(effectiveBg);

        ImVec4 textCol = (lum > 0.5f) ? ImVec4(0, 0, 0, 1) : ImVec4(1, 1, 1, 1);
        ImVec4 subtextCol = (lum > 0.5f) ? ImVec4(0.20f, 0.20f, 0.20f, 1.0f) : ImVec4(0.85f, 0.85f, 0.85f, 1.0f);

        ImGui::TextColored(subtextCol, "%s", icon ? icon : "// KPI");
        ImGui::Spacing();
        ImGui::TextColored(subtextCol, "%s", label ? label : "");
        ImGui::Spacing();
        ImGui::TextColored(textCol, "%s", value.c_str());

        EndBrutalistCard();
    }

} // namespace App::UI::NeoWidgets
