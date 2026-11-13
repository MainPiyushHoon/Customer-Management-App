// src/ui/DashboardView.cpp
#include "ui/DashboardView.hpp"
#include "ui/NeoWidgets.hpp"
#include "imgui.h"
#include <cstdio>
#include <string>
#include <vector>
#include <sstream>

namespace App::UI {

    void DashboardView::render(
        const Services::DashboardMetrics& metrics,
        const Services::OrderManager& orderMgr,
        const std::function<void()>& onAddNewCustomerClick,
        const std::function<void()>& onAddNewOrderClick,
        const std::function<void(ViewTab)>& onNavigateTab,
        const std::function<void(Core::OrderId)>& onViewOrderClick,
        const std::function<void()>& /*onManualSaveClick*/
    ) {
        ImGui::BeginChild("DashboardContentScroll", ImVec2(0, 0), false, ImGuiWindowFlags_AlwaysVerticalScrollbar);

        // 1. Section Title
        ImGui::TextUnformatted("SYSTEM OVERVIEW // LIVE METRICS & OPERATIONS");
        ImGui::Spacing();

        // 2. 4 Large KPI Stat Cards
        float availW = ImGui::GetContentRegionAvail().x;
        float cardW = (availW - 36.0f) / 4.0f;

        ImGui::Columns(4, "KpiColumns", false);
        ImGui::SetColumnWidth(0, cardW + 12.0f);
        ImGui::SetColumnWidth(1, cardW + 12.0f);
        ImGui::SetColumnWidth(2, cardW + 12.0f);
        ImGui::SetColumnWidth(3, cardW + 12.0f);

        NeoWidgets::StatCard("TOTAL CUSTOMERS", std::to_string(metrics.totalCustomers), 0xFF00E6FF, "// CLIENTS");
        ImGui::NextColumn();

        NeoWidgets::StatCard("ACTIVE ORDERS", std::to_string(metrics.activeOrders), 0xFFFDB7D7, "// WORK ORDERS");
        ImGui::NextColumn();

        char revBuf[64];
        std::snprintf(revBuf, sizeof(revBuf), "$%.2f", metrics.totalRevenue);
        NeoWidgets::StatCard("COMPLETED REVENUE", revBuf, 0xFF00FFA6, "// COMPLETED");
        ImGui::NextColumn();

        char pendBuf[64];
        std::snprintf(pendBuf, sizeof(pendBuf), "$%.2f", metrics.pendingRevenue);
        NeoWidgets::StatCard("PENDING PIPELINE", pendBuf, 0xFFFFF000, "// IN PIPELINE");
        ImGui::Columns(1);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // 3. Operational Shortcuts // Quick Actions Section
        ImGui::TextUnformatted("OPERATIONAL SHORTCUTS // QUICK ACTIONS");
        ImGui::Spacing();

        if (NeoWidgets::BrutalistButton("+ NEW CUSTOMER", 220.0f, 40.0f, 0xFF00FFA6)) {
            if (onAddNewCustomerClick) {
                onAddNewCustomerClick();
            }
        }
        ImGui::SameLine();
        if (NeoWidgets::BrutalistButton("+ PLACE NEW ORDER", 220.0f, 40.0f, 0xFF00E6FF)) {
            if (onAddNewOrderClick) {
                onAddNewOrderClick();
            }
        }
        ImGui::SameLine();
        if (NeoWidgets::BrutalistButton(">> MANAGE CUSTOMERS", 240.0f, 40.0f, 0xFFFDB7D7)) {
            if (onNavigateTab) {
                onNavigateTab(ViewTab::Customers);
            }
        }
        ImGui::SameLine();
        if (NeoWidgets::BrutalistButton(">> VIEW ALL ORDERS", 240.0f, 40.0f, 0xFFFFF000)) {
            if (onNavigateTab) {
                onNavigateTab(ViewTab::Orders);
            }
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // 4. Recent Orders Feed (Latest 5 Orders)
        ImGui::TextUnformatted("RECENT ORDERS // LATEST ACTIVITY");
        ImGui::Spacing();

        const auto& allOrders = orderMgr.getAllOrders();
        if (allOrders.empty()) {
            ImGui::TextUnformatted("No orders placed yet. Click '+ PLACE NEW ORDER' to create one.");
        } else {
            // Collect the 5 most recent orders in reverse chronological order
            std::vector<const Models::Order*> recentOrders;
            recentOrders.reserve(5);
            for (auto it = allOrders.rbegin(); it != allOrders.rend() && recentOrders.size() < 5; ++it) {
                recentOrders.push_back(&(*it));
            }

            if (ImGui::BeginTable("RecentOrdersTable", 7, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable)) {
                ImGui::TableSetupColumn("Order ID", ImGuiTableColumnFlags_WidthFixed, 90.0f);
                ImGui::TableSetupColumn("Customer", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Date", ImGuiTableColumnFlags_WidthFixed, 140.0f);
                ImGui::TableSetupColumn("Items Summary", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Total Amount", ImGuiTableColumnFlags_WidthFixed, 120.0f);
                ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthFixed, 130.0f);
                ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed, 90.0f);
                NeoWidgets::TableHeadersRow();

                for (const auto* ord : recentOrders) {
                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("#%llu", static_cast<unsigned long long>(ord->getId()));

                    ImGui::TableSetColumnIndex(1);
                    ImGui::TextUnformatted(ord->getCustomerName().c_str());

                    ImGui::TableSetColumnIndex(2);
                    ImGui::TextUnformatted(ord->getOrderDate().c_str());

                    ImGui::TableSetColumnIndex(3);
                    std::ostringstream itemsSummary;
                    for (size_t i = 0; i < ord->getItems().size(); ++i) {
                        const auto& item = ord->getItems()[i];
                        itemsSummary << item.getItemName() << " (" << item.getQuantity() << "x)";
                        if (i + 1 < ord->getItems().size()) itemsSummary << ", ";
                    }
                    ImGui::TextUnformatted(itemsSummary.str().c_str());

                    ImGui::TableSetColumnIndex(4);
                    ImGui::Text("$%.2f", ord->getTotalAmount());

                    ImGui::TableSetColumnIndex(5);
                    uint32_t statusColor = 0xFF00E6FF; // Yellow (Pending)
                    switch (ord->getStatus()) {
                        case Core::OrderStatus::Completed:  statusColor = 0xFF00FFA6; break; // Pastel Mint
                        case Core::OrderStatus::Delivered:  statusColor = 0xFF00FF85; break; // Green
                        case Core::OrderStatus::InProgress: statusColor = 0xFF00D8F6; break; // Cyan
                        case Core::OrderStatus::Cancelled:  statusColor = 0xFFBCAEFF; break; // Soft Coral
                        default: break;
                    }
                    NeoWidgets::BrutalistBadge(Core::OrderStatusToString(ord->getStatus()).c_str(), statusColor);

                    ImGui::TableSetColumnIndex(6);
                    ImGui::PushID(static_cast<int>(ord->getId()));
                    if (NeoWidgets::BrutalistButton("VIEW", 70.0f, 26.0f, 0xFF00E6FF)) {
                        if (onViewOrderClick) {
                            onViewOrderClick(ord->getId());
                        }
                    }
                    ImGui::PopID();
                }

                ImGui::EndTable();
            }
        }

        ImGui::EndChild();
    }

    void DashboardView::render(
        const Services::DashboardMetrics& metrics,
        Storage::AutoSaveCoordinator& /*autoSave*/,
        ThemeManager& /*themeMgr*/,
        const std::function<void()>& onManualSaveClick,
        const std::function<void(ViewTab)>& onNavigateTab,
        const std::function<void()>& onAddNewCustomerClick,
        const std::function<void()>& onAddNewOrderClick
    ) {
        static const Services::OrderManager dummyOrderMgr;
        render(
            metrics,
            dummyOrderMgr,
            onAddNewCustomerClick,
            onAddNewOrderClick,
            onNavigateTab,
            nullptr,
            onManualSaveClick
        );
    }

} // namespace App::UI
