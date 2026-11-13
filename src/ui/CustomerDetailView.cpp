// src/ui/CustomerDetailView.cpp
#include "ui/CustomerDetailView.hpp"
#include "ui/NeoWidgets.hpp"
#include "imgui.h"
#include <cstring>

namespace App::UI {

    void CustomerDetailView::render(
        Models::Customer* customer,
        Services::CustomerManager& customerMgr,
        Services::OrderManager& orderMgr,
        const std::function<void(Core::CustomerId)>& onNewOrderClick,
        const std::function<void(Core::OrderId)>& onEditOrderClick,
        const std::function<void()>& onSaveCustomerData
    ) {
        if (!customer) {
            m_currentCustomerId = 0;
            m_isEditingBasicInfo = false;
            ImGui::TextDisabled("Select a customer from the left list to view details and measurements.");
            return;
        }

        if (customer->getId() != m_currentCustomerId) {
            m_currentCustomerId = customer->getId();
            m_isEditingBasicInfo = false;
            m_showSaveSuccessBadge = false;
        }

        // Update timer for save badge
        if (m_showSaveSuccessBadge) {
            m_saveSuccessTimer -= ImGui::GetIO().DeltaTime;
            if (m_saveSuccessTimer <= 0.0f) {
                m_showSaveSuccessBadge = false;
            }
        }

        // Scrollable container ensuring all elements are accessible
        ImGui::BeginChild("CustomerDetailScrollContainer", ImVec2(0, 0), false, ImGuiWindowFlags_AlwaysVerticalScrollbar);

        // Customer Header Card
        NeoWidgets::BeginBrutalistCard("CustHeader", 0.0f, 120.0f, 0);
        if (!m_isEditingBasicInfo) {
            ImGui::Text("CUSTOMER #%llu: %s", static_cast<unsigned long long>(customer->getId()), customer->getName().c_str());
            ImGui::Text("PHONE: %s", customer->getPhone().c_str());
            ImGui::Text("MEMBER SINCE: %s  |  ORDERS PLACED: %zu", customer->getCreatedAt().empty() ? "N/A" : customer->getCreatedAt().c_str(), customer->getOrderIds().size());

            if (NeoWidgets::BrutalistButton("EDIT CUSTOMER INFO", 180.0f, 30.0f, 0xFFFFF000)) {
                m_isEditingBasicInfo = true;
                std::strncpy(m_editName, customer->getName().c_str(), sizeof(m_editName) - 1);
                m_editName[sizeof(m_editName) - 1] = '\0';
                std::strncpy(m_editPhone, customer->getPhone().c_str(), sizeof(m_editPhone) - 1);
                m_editPhone[sizeof(m_editPhone) - 1] = '\0';
            }
        } else {
            ImGui::SetNextItemWidth(260.0f);
            ImGui::InputText("Name", m_editName, sizeof(m_editName));
            ImGui::SameLine();
            ImGui::SetNextItemWidth(200.0f);
            ImGui::InputText("Phone", m_editPhone, sizeof(m_editPhone));

            if (NeoWidgets::BrutalistButton("SAVE INFO", 110.0f, 30.0f, 0xFF9DFF00)) {
                customer->setName(m_editName);
                customer->setPhone(m_editPhone);
                customerMgr.updateCustomer(*customer);
                m_isEditingBasicInfo = false;
                if (onSaveCustomerData) {
                    onSaveCustomerData();
                }
            }
            ImGui::SameLine();
            if (NeoWidgets::BrutalistButton("CANCEL", 90.0f, 30.0f)) {
                m_isEditingBasicInfo = false;
            }
        }
        NeoWidgets::EndBrutalistCard();

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Editable Measurements Section
        ImGui::TextUnformatted("DYNAMIC MEASUREMENTS (EDITABLE ON THE FLY)");
        ImGui::SameLine();

        // Primary Prominent Save Button
        if (NeoWidgets::BrutalistButton("SAVE ALL MEASUREMENTS", 220.0f, 32.0f, 0xFF9DFF00)) {
            customerMgr.updateCustomer(*customer);
            if (onSaveCustomerData) {
                onSaveCustomerData();
            }
            m_showSaveSuccessBadge = true;
            m_saveSuccessTimer = 3.0f;
        }

        if (m_showSaveSuccessBadge) {
            ImGui::SameLine();
            NeoWidgets::BrutalistBadge("MEASUREMENTS SAVED ✓", 0xFF9DFF00, 0xFF000000);
        }

        ImGui::Spacing();

        if (ImGui::BeginTable("MeasTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable)) {
            ImGui::TableSetupColumn("Measurement Type / Label", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed, 130.0f);
            ImGui::TableSetupColumn("Unit", ImGuiTableColumnFlags_WidthFixed, 90.0f);
            ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed, 90.0f);
            ImGui::TableHeadersRow();

            int removeIndex = -1;
            auto& measurements = customer->getMeasurements();

            for (size_t i = 0; i < measurements.size(); ++i) {
                ImGui::TableNextRow();
                ImGui::PushID(static_cast<int>(i));

                ImGui::TableSetColumnIndex(0);
                char keyBuf[64];
                std::strncpy(keyBuf, measurements[i].getKey().c_str(), sizeof(keyBuf) - 1);
                keyBuf[sizeof(keyBuf) - 1] = '\0';
                ImGui::SetNextItemWidth(-1);
                if (ImGui::InputText("##k", keyBuf, sizeof(keyBuf))) {
                    measurements[i].setKey(keyBuf);
                }

                ImGui::TableSetColumnIndex(1);
                char valBuf[64];
                std::strncpy(valBuf, measurements[i].getValue().c_str(), sizeof(valBuf) - 1);
                valBuf[sizeof(valBuf) - 1] = '\0';
                ImGui::SetNextItemWidth(-1);
                if (ImGui::InputText("##v", valBuf, sizeof(valBuf))) {
                    measurements[i].setValue(valBuf);
                }

                ImGui::TableSetColumnIndex(2);
                char unitBuf[32];
                std::strncpy(unitBuf, measurements[i].getUnit().c_str(), sizeof(unitBuf) - 1);
                unitBuf[sizeof(unitBuf) - 1] = '\0';
                ImGui::SetNextItemWidth(-1);
                if (ImGui::InputText("##u", unitBuf, sizeof(unitBuf))) {
                    measurements[i].setUnit(unitBuf);
                }

                ImGui::TableSetColumnIndex(3);
                if (NeoWidgets::BrutalistButton("DEL", 70.0f, 26.0f, 0xFFA76EFF)) {
                    removeIndex = static_cast<int>(i);
                }

                ImGui::PopID();
            }

            if (removeIndex >= 0) {
                customer->removeMeasurement(static_cast<size_t>(removeIndex));
            }

            ImGui::EndTable();
        }

        // Add new measurement row input bar
        ImGui::Spacing();
        ImGui::TextUnformatted("ADD NEW MEASUREMENT ROW:");
        ImGui::SetNextItemWidth(180.0f);
        ImGui::InputTextWithHint("##newK", "e.g. Chest / Waist", m_newMeasKey, sizeof(m_newMeasKey));
        ImGui::SameLine();
        ImGui::SetNextItemWidth(120.0f);
        ImGui::InputTextWithHint("##newV", "e.g. 38.5", m_newMeasVal, sizeof(m_newMeasVal));
        ImGui::SameLine();
        ImGui::SetNextItemWidth(80.0f);
        ImGui::InputTextWithHint("##newU", "in / cm", m_newMeasUnit, sizeof(m_newMeasUnit));
        ImGui::SameLine();
        if (NeoWidgets::BrutalistButton("+ ADD ROW", 120.0f, 32.0f, 0xFF00E6FF)) {
            if (m_newMeasKey[0] != '\0') {
                customer->addMeasurement({m_newMeasKey, m_newMeasVal, m_newMeasUnit});
                m_newMeasKey[0] = '\0';
                m_newMeasVal[0] = '\0';
                m_newMeasUnit[0] = '\0';
            }
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Customer's Orders Section
        ImGui::TextUnformatted("ORDERS PLACED BY THIS CUSTOMER");
        ImGui::SameLine();
        if (NeoWidgets::BrutalistButton("+ PLACE NEW ORDER", 180.0f, 30.0f, 0xFF9DFF00)) {
            if (onNewOrderClick) {
                onNewOrderClick(customer->getId());
            }
        }

        auto orders = orderMgr.getOrdersForCustomer(customer->getId());
        if (orders.empty()) {
            ImGui::TextDisabled("No orders recorded for this customer yet.");
        } else {
            if (ImGui::BeginTable("CustOrdersTable", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable)) {
                ImGui::TableSetupColumn("Order ID", ImGuiTableColumnFlags_WidthFixed, 90.0f);
                ImGui::TableSetupColumn("Date", ImGuiTableColumnFlags_WidthFixed, 150.0f);
                ImGui::TableSetupColumn("Total Amount", ImGuiTableColumnFlags_WidthFixed, 120.0f);
                ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthFixed, 130.0f);
                ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed, 80.0f);
                ImGui::TableHeadersRow();

                for (const auto* ord : orders) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("#%llu", static_cast<unsigned long long>(ord->getId()));

                    ImGui::TableSetColumnIndex(1);
                    ImGui::TextUnformatted(ord->getOrderDate().c_str());

                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("$%.2f", ord->getTotalAmount());

                    ImGui::TableSetColumnIndex(3);
                    uint32_t statusColor = 0xFF00E6FF;
                    switch (ord->getStatus()) {
                        case Core::OrderStatus::Completed:  statusColor = 0xFF9DFF00; break;
                        case Core::OrderStatus::Delivered:  statusColor = 0xFF00FF85; break;
                        case Core::OrderStatus::InProgress: statusColor = 0xFF00D8F6; break;
                        case Core::OrderStatus::Cancelled:  statusColor = 0xFFA76EFF; break;
                        default: break;
                    }
                    NeoWidgets::BrutalistBadge(Core::OrderStatusToString(ord->getStatus()).c_str(), statusColor);

                    ImGui::TableSetColumnIndex(4);
                    ImGui::PushID(static_cast<int>(ord->getId()));
                    if (NeoWidgets::BrutalistButton("VIEW", 70.0f, 24.0f, 0xFF00E6FF)) {
                        if (onEditOrderClick) {
                            onEditOrderClick(ord->getId());
                        }
                    }
                    ImGui::PopID();
                }
                ImGui::EndTable();
            }
        }

        ImGui::Spacing();
        ImGui::EndChild();
    }

} // namespace App::UI
