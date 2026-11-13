// src/ui/CustomerListView.cpp
#include "ui/CustomerListView.hpp"
#include "ui/NeoWidgets.hpp"
#include "imgui.h"

namespace App::UI {

    void CustomerListView::render(
        Services::CustomerManager& customerMgr,
        Core::CustomerId selectedCustomerId,
        const std::function<void(Core::CustomerId)>& onSelectCustomer,
        const std::function<void()>& onAddNewCustomerClick
    ) {
        ImGui::BeginChild("CustomerListScrollContainer", ImVec2(0, 0), false, ImGuiWindowFlags_None);

        ImGui::TextUnformatted("SEARCH CUSTOMERS:");
        ImGui::InputTextWithHint("##SearchCust", "Search by name or phone...", m_searchQuery, IM_ARRAYSIZE(m_searchQuery));

        ImGui::SameLine();
        if (NeoWidgets::BrutalistButton("+ NEW CUSTOMER", 160.0f, 32.0f, 0xFF00E6FF)) {
            if (onAddNewCustomerClick) {
                onAddNewCustomerClick();
            }
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        auto filtered = customerMgr.search(m_searchQuery);

        if (ImGui::BeginTable("CustomersTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_Resizable)) {
            ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 70.0f);
            ImGui::TableSetupColumn("Customer Name", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Phone", ImGuiTableColumnFlags_WidthFixed, 140.0f);
            ImGui::TableSetupColumn("Orders", ImGuiTableColumnFlags_WidthFixed, 80.0f);
            NeoWidgets::TableHeadersRow();

            for (const auto* c : filtered) {
                ImGui::TableNextRow();
                bool isSelected = (c->getId() == selectedCustomerId);

                ImGui::TableSetColumnIndex(0);
                std::string idStr = "#" + std::to_string(c->getId());
                if (ImGui::Selectable(idStr.c_str(), isSelected, ImGuiSelectableFlags_SpanAllColumns)) {
                    if (onSelectCustomer) {
                        onSelectCustomer(c->getId());
                    }
                }

                ImGui::TableSetColumnIndex(1);
                ImGui::TextUnformatted(c->getName().c_str());

                ImGui::TableSetColumnIndex(2);
                ImGui::TextUnformatted(c->getPhone().c_str());

                ImGui::TableSetColumnIndex(3);
                ImGui::Text("%zu", c->getOrderIds().size());
            }
            ImGui::EndTable();
        }

        ImGui::EndChild();
    }

} // namespace App::UI
