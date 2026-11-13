#pragma once

#include "ui/ThemeManager.hpp"
#include <string>

namespace App::UI::NeoWidgets {

    // Draws a solid rectangular card with a hard 4px offset drop shadow
    void BeginBrutalistCard(const char* strId, float width = 0.0f, float height = 0.0f, uint32_t bgColor = 0);
    void EndBrutalistCard();

    // Custom Neo-Brutalist button with thick border, offset shadow, and click-press offset
    bool BrutalistButton(const char* label, float width = 0.0f, float height = 0.0f, uint32_t accentColor = 0);

    // Pill badge for order status, customer badges, counters
    void BrutalistBadge(const char* text, uint32_t bgColor, uint32_t textColor = 0xFF000000);

    // Metric Summary Card (e.g. for dashboard)
    void StatCard(const char* label, const std::string& value, uint32_t accentColor, const char* icon = nullptr);

} // namespace App::UI::NeoWidgets
