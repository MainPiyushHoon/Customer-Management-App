// src/ui/DashboardView.cpp
#include "ui/DashboardView.hpp"
#include "ui/NeoWidgets.hpp"
#include "imgui.h"
#include <cstdio>
#include <string>

namespace App::UI {

    void DashboardView::render(
        const Services::DashboardMetrics& metrics,
        Storage::AutoSaveCoordinator& autoSave,
        ThemeManager& themeMgr,
        const std::function<void()>& onManualSaveClick
    ) {
        // Top Brutalist Brand Bar
        ImGui::TextUnformatted("BRUTAL // OPS — CUSTOMER & ORDER MANAGEMENT");
        ImGui::SameLine();

        // Theme Toggle Button
        std::string themeLabel = themeMgr.isDark() ? "LIGHT MODE" : "DARK MODE";
        if (NeoWidgets::BrutalistButton(themeLabel.c_str(), 110.0f, 26.0f, 0xFFFFF000)) {
            themeMgr.toggleMode();
        }

        ImGui::SameLine();
        // Manual Save Button
        if (NeoWidgets::BrutalistButton("SAVE NOW (JSON/EXCEL)", 190.0f, 26.0f, 0xFF9DFF00)) {
            if (onManualSaveClick) {
                onManualSaveClick();
            }
        }

        ImGui::SameLine();
        // Live Auto-Save Countdown Pill
        char autoSavePill[64];
        std::snprintf(autoSavePill, sizeof(autoSavePill), "AUTOSAVE IN %ds", autoSave.getSecondsRemaining());
        NeoWidgets::BrutalistBadge(autoSavePill, 0xFF00E6FF);

        ImGui::Spacing();

        // 4 KPI Stat Cards
        float availW = ImGui::GetContentRegionAvail().x;
        float cardW = (availW - 36.0f) / 4.0f;

        ImGui::Columns(4, "KpiColumns", false);
        ImGui::SetColumnWidth(0, cardW + 12.0f);
        ImGui::SetColumnWidth(1, cardW + 12.0f);
        ImGui::SetColumnWidth(2, cardW + 12.0f);
        ImGui::SetColumnWidth(3, cardW + 12.0f);

        NeoWidgets::StatCard("TOTAL CUSTOMERS", std::to_string(metrics.totalCustomers), 0xFF00E6FF, "// CUSTOMERS");
        ImGui::NextColumn();

        NeoWidgets::StatCard("ACTIVE ORDERS", std::to_string(metrics.activeOrders), 0xFFA76EFF, "// ORDERS");
        ImGui::NextColumn();

        char revBuf[64];
        std::snprintf(revBuf, sizeof(revBuf), "$%.2f", metrics.totalRevenue);
        NeoWidgets::StatCard("COMPLETED REVENUE", revBuf, 0xFF9DFF00, "// REVENUE");
        ImGui::NextColumn();

        char pendBuf[64];
        std::snprintf(pendBuf, sizeof(pendBuf), "$%.2f", metrics.pendingRevenue);
        NeoWidgets::StatCard("PENDING PIPELINE", pendBuf, 0xFFFFF000, "// PIPELINE");
        ImGui::Columns(1);

        ImGui::Spacing();
        ImGui::Separator();
    }

} // namespace App::UI
