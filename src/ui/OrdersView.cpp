// src/ui/OrdersView.cpp
#include "ui/OrdersView.hpp"
#include "ui/NeoWidgets.hpp"
#include "imgui.h"
#include <algorithm>
#include <sstream>

namespace App::UI {

    void OrdersView::render(
        Services::CustomerManager& customerMgr,
        Services::OrderManager& orderMgr,
        const std::function<void(Core::OrderId)>& onEditOrderClick,
        const std::function<void()>& onNewOrderClick,
        const std::function<void(Core::OrderId)>& onDeleteOrderClick
    ) {
        ImGui::BeginChild("OrdersViewContainer", ImVec2(0, 0), false, ImGuiWindowFlags_AlwaysVerticalScrollbar);

        // Header Action Bar
        ImGui::TextUnformatted("ORDER MANAGEMENT // ALL RECORDS");
        ImGui::SameLine();
        if (NeoWidgets::BrutalistButton("+ PLACE NEW ORDER", 180.0f, 34.0f, 0xFF00FFA6)) {
            if (onNewOrderClick) {
                onNewOrderClick();
            }
        }

        ImGui::Spacing();

        // Search Bar & Filter Buttons Bar
        ImGui::TextUnformatted("SEARCH ORDERS:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(260.0f);
        ImGui::InputTextWithHint("##SearchOrders", "Search by customer or item...", m_searchQuery, sizeof(m_searchQuery));

        ImGui::SameLine();
        ImGui::TextUnformatted("FILTER STATUS:");
        ImGui::SameLine();

        const char* filterLabels[] = { "ALL", "PENDING", "IN PROGRESS", "COMPLETED", "DELIVERED", "CANCELLED" };
        for (int i = 0; i < 6; ++i) {
            bool isActive = (m_activeFilterIndex == i);
            uint32_t btnColor = isActive ? 0xFF00E6FF : 0; // Highlight active filter in electric yellow
            if (NeoWidgets::BrutalistButton(filterLabels[i], 0.0f, 28.0f, btnColor)) {
                m_activeFilterIndex = i;
            }
            ImGui::SameLine();
        }
        ImGui::NewLine();

        ImGui::Separator();
        ImGui::Spacing();

        // Filter orders by status
        std::vector<const Models::Order*> displayOrders;
        if (m_activeFilterIndex == 0) {
            for (const auto& o : orderMgr.getAllOrders()) {
                displayOrders.push_back(&o);
            }
        } else {
            Core::OrderStatus targetStatus = static_cast<Core::OrderStatus>(m_activeFilterIndex - 1);
            displayOrders = orderMgr.filterByStatus(targetStatus);
        }

        // Filter by search query if any
        if (m_searchQuery[0] != '\0') {
            std::string q = m_searchQuery;
            std::transform(q.begin(), q.end(), q.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

            std::vector<const Models::Order*> filtered;
            for (const auto* ord : displayOrders) {
                std::string custName = ord->getCustomerName();
                std::transform(custName.begin(), custName.end(), custName.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

                bool match = (custName.find(q) != std::string::npos);
                if (!match) {
                    for (const auto& item : ord->getItems()) {
                        std::string itName = item.getItemName();
                        std::transform(itName.begin(), itName.end(), itName.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
                        if (itName.find(q) != std::string::npos) {
                            match = true;
                            break;
                        }
                    }
                }
                if (match) {
                    filtered.push_back(ord);
                }
            }
            displayOrders = std::move(filtered);
        }

        if (displayOrders.empty()) {
            ImGui::TextDisabled("No orders match the current filter and search query.");
        } else {
            if (ImGui::BeginTable("AllOrdersTable", 7, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable)) {
                ImGui::TableSetupColumn("Order ID", ImGuiTableColumnFlags_WidthFixed, 90.0f);
                ImGui::TableSetupColumn("Customer", ImGuiTableColumnFlags_WidthFixed, 180.0f);
                ImGui::TableSetupColumn("Date", ImGuiTableColumnFlags_WidthFixed, 150.0f);
                ImGui::TableSetupColumn("Items Summary", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Total Amount", ImGuiTableColumnFlags_WidthFixed, 120.0f);
                ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthFixed, 130.0f);
                ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed, 170.0f);
                NeoWidgets::TableHeadersRow();

                for (const auto* ord : displayOrders) {
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
                    uint32_t statusColor = 0xFF00E6FF; // Yellow
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
                    if (NeoWidgets::BrutalistButton("EDIT", 70.0f, 26.0f, 0xFF00E6FF)) {
                        if (onEditOrderClick) {
                            onEditOrderClick(ord->getId());
                        }
                    }
                    ImGui::SameLine();
                    if (NeoWidgets::BrutalistButton("DELETE", 80.0f, 26.0f, 0xFFBCAEFF)) {
                        if (onDeleteOrderClick) {
                            onDeleteOrderClick(ord->getId());
                        }
                    }
                    ImGui::PopID();
                }

                ImGui::EndTable();
            }
        }

        ImGui::EndChild();
    }

} // namespace App::UI
