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
        const std::function<void()>& onManualSaveClick,
        const std::function<void(ViewTab)>& onNavigateTab,
        const std::function<void()>& onAddNewCustomerClick,
        const std::function<void()>& onAddNewOrderClick
    ) {
        ImGui::BeginChild("DashboardContentScroll", ImVec2(0, 0), false, ImGuiWindowFlags_AlwaysVerticalScrollbar);

        // Section Title
        ImGui::TextUnformatted("SYSTEM OVERVIEW // LIVE METRICS & OPERATIONS");
        ImGui::Spacing();

        // 4 Large KPI Stat Cards
        float availW = ImGui::GetContentRegionAvail().x;
        float cardW = (availW - 36.0f) / 4.0f;

        ImGui::Columns(4, "KpiColumns", false);
        ImGui::SetColumnWidth(0, cardW + 12.0f);
        ImGui::SetColumnWidth(1, cardW + 12.0f);
        ImGui::SetColumnWidth(2, cardW + 12.0f);
        ImGui::SetColumnWidth(3, cardW + 12.0f);

        NeoWidgets::StatCard("TOTAL CUSTOMERS", std::to_string(metrics.totalCustomers), 0xFF00E6FF, "// CLIENTS");
        ImGui::NextColumn();

        NeoWidgets::StatCard("ACTIVE ORDERS", std::to_string(metrics.activeOrders), 0xFFA76EFF, "// WORK ORDERS");
        ImGui::NextColumn();

        char revBuf[64];
        std::snprintf(revBuf, sizeof(revBuf), "$%.2f", metrics.totalRevenue);
        NeoWidgets::StatCard("COMPLETED REVENUE", revBuf, 0xFF9DFF00, "// COMPLETED");
        ImGui::NextColumn();

        char pendBuf[64];
        std::snprintf(pendBuf, sizeof(pendBuf), "$%.2f", metrics.pendingRevenue);
        NeoWidgets::StatCard("PENDING PIPELINE", pendBuf, 0xFFFFF000, "// IN PIPELINE");
        ImGui::Columns(1);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Quick Navigation & Actions Section
        ImGui::TextUnformatted("QUICK ACTIONS & NAVIGATION:");
        ImGui::Spacing();

        if (NeoWidgets::BrutalistButton(">> GO TO CUSTOMER MANAGEMENT", 280.0f, 42.0f, 0xFF00E6FF)) {
            if (onNavigateTab) {
                onNavigateTab(ViewTab::Customers);
            }
        }
        ImGui::SameLine();
        if (NeoWidgets::BrutalistButton(">> GO TO ORDER MANAGEMENT", 280.0f, 42.0f, 0xFFA76EFF)) {
            if (onNavigateTab) {
                onNavigateTab(ViewTab::Orders);
            }
        }

        ImGui::Spacing();

        if (NeoWidgets::BrutalistButton("+ NEW CUSTOMER", 200.0f, 36.0f, 0xFF9DFF00)) {
            if (onAddNewCustomerClick) {
                onAddNewCustomerClick();
            }
        }
        ImGui::SameLine();
        if (NeoWidgets::BrutalistButton("+ PLACE NEW ORDER", 200.0f, 36.0f, 0xFFFFF000)) {
            if (onAddNewOrderClick) {
                onAddNewOrderClick();
            }
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // System Persistence Info Card
        NeoWidgets::BeginBrutalistCard("PersistenceInfoCard", 0.0f, 130.0f, 0);
        ImGui::TextUnformatted("DATA STORAGE & PERSISTENCE ENGINE");
        ImGui::Text("Last Synchronized to Disk: %s", autoSave.getLastSaveTimestamp().c_str());
        ImGui::Text("Auto-Save Frequency: Every 60 seconds (Live Countdown: %ds remaining)", autoSave.getSecondsRemaining());
        ImGui::TextUnformatted("Dual Output Targets: data/customers.json  |  data/orders.json  |  data/records_export.csv");

        if (NeoWidgets::BrutalistButton("FORCE SAVE NOW (JSON & EXCEL)", 260.0f, 30.0f, 0xFF9DFF00)) {
            if (onManualSaveClick) {
                onManualSaveClick();
            }
        }
        NeoWidgets::EndBrutalistCard();

        ImGui::EndChild();
    }

} // namespace App::UI
