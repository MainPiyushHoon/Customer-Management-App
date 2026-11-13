// src/ui/NeoWidgets.cpp
#include "ui/NeoWidgets.hpp"
#include "imgui.h"
#include <algorithm>

namespace App::UI::NeoWidgets {

    ImVec2 CalculateButtonSize(const char* label, float width, float height) {
        ImVec2 textSize = ImGui::CalcTextSize(label);
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
        int pushedColors = 0;
        uint32_t effectiveColor = (accentColor != 0) ? accentColor : ImGui::GetColorU32(ImGuiCol_Button);
        if (accentColor != 0) {
            ImGui::PushStyleColor(ImGuiCol_Button, accentColor);
            pushedColors++;
        }

        // Ensure high-contrast readable text based on button accent luminance
        float r = (effectiveColor & 0xFF) / 255.0f;
        float g = ((effectiveColor >> 8) & 0xFF) / 255.0f;
        float b = ((effectiveColor >> 16) & 0xFF) / 255.0f;
        float lum = 0.299f * r + 0.587f * g + 0.114f * b;
        ImGui::PushStyleColor(ImGuiCol_Text, lum > 0.5f ? ImVec4(0, 0, 0, 1) : ImVec4(1, 1, 1, 1));
        pushedColors++;

        ImVec2 p = ImGui::GetCursorScreenPos();
        ImVec2 size = CalculateButtonSize(label, width, height);

        // Draw shadow under button (standardized 4.0f offset)
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        drawList->AddRectFilled(
            ImVec2(p.x + 4.0f, p.y + 4.0f),
            ImVec2(p.x + size.x + 4.0f, p.y + size.y + 4.0f),
            0xFF000000, 0.0f
        );

        bool clicked = ImGui::Button(label, size);

        if (pushedColors > 0) {
            ImGui::PopStyleColor(pushedColors);
        }
        return clicked;
    }

    void BrutalistBadge(const char* text, uint32_t bgColor, uint32_t textColor) {
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 textSize = ImGui::CalcTextSize(text);
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
            float r = (bgColor & 0xFF) / 255.0f;
            float g = ((bgColor >> 8) & 0xFF) / 255.0f;
            float b = ((bgColor >> 16) & 0xFF) / 255.0f;
            float lum = 0.299f * r + 0.587f * g + 0.114f * b;
            if (lum < 0.5f) {
                effectiveText = 0xFFFFFFFF;
            }
        }

        // Text
        drawList->AddText(ImVec2(p.x + padding.x, p.y + padding.y), effectiveText, text);

        ImGui::Dummy(boxSize);
    }

    void StatCard(const char* label, const std::string& value, uint32_t accentColor, const char* icon) {
        float lineHeight = ImGui::GetTextLineHeightWithSpacing();
        float neededHeight = 24.0f + lineHeight * 3.0f + 16.0f;
        float cardHeight = std::max(115.0f, neededHeight);

        BeginBrutalistCard(label, 0.0f, cardHeight, accentColor);

        // Contrast Calculation: determine if background is light or dark
        uint32_t effectiveBg = accentColor ? accentColor : ImGui::GetColorU32(ImGuiCol_ChildBg);
        float r = (effectiveBg & 0xFF) / 255.0f;
        float g = ((effectiveBg >> 8) & 0xFF) / 255.0f;
        float b = ((effectiveBg >> 16) & 0xFF) / 255.0f;
        float lum = 0.299f * r + 0.587f * g + 0.114f * b;

        ImVec4 textCol = (lum > 0.5f) ? ImVec4(0, 0, 0, 1) : ImVec4(1, 1, 1, 1);
        ImVec4 subtextCol = (lum > 0.5f) ? ImVec4(0.20f, 0.20f, 0.20f, 1.0f) : ImVec4(0.85f, 0.85f, 0.85f, 1.0f);

        ImGui::TextColored(subtextCol, "%s", icon ? icon : "// KPI");
        ImGui::Spacing();
        ImGui::TextColored(subtextCol, "%s", label);
        ImGui::Spacing();
        ImGui::TextColored(textCol, "%s", value.c_str());

        EndBrutalistCard();
    }

} // namespace App::UI::NeoWidgets
