// src/ui/OrderModalView.cpp
#include "ui/OrderModalView.hpp"
#include "ui/NeoWidgets.hpp"
#include "imgui.h"
#include <ctime>
#include <iomanip>
#include <sstream>
#include <cstring>
#include <algorithm>

namespace App::UI {

    void OrderModalView::openCreate(Core::CustomerId customerId, const std::string& customerName) {
        m_isOpen = true;
        m_isEditMode = false;
        m_orderId = 0;
        m_customerId = customerId;
        m_customerName = customerName;
        m_lineItems.clear();
        m_notes[0] = '\0';
        m_statusIndex = 0;

        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::tm tmNow{};
#if defined(_WIN32)
        localtime_s(&tmNow, &now);
#else
        localtime_r(&now, &tmNow);
#endif
        std::ostringstream ss;
        ss << std::put_time(&tmNow, "%Y-%m-%d %H:%M");
        std::strncpy(m_orderDate, ss.str().c_str(), sizeof(m_orderDate) - 1);
        m_orderDate[sizeof(m_orderDate) - 1] = '\0';

        m_newItemName[0] = '\0';
        m_newItemQty = 1;
        m_newItemPrice = 0.0;
    }

    void OrderModalView::openEdit(const Models::Order& order) {
        m_isOpen = true;
        m_isEditMode = true;
        m_orderId = order.getId();
        m_customerId = order.getCustomerId();
        m_customerName = order.getCustomerName();
        m_lineItems = order.getItems();
        std::strncpy(m_orderDate, order.getOrderDate().c_str(), sizeof(m_orderDate) - 1);
        m_orderDate[sizeof(m_orderDate) - 1] = '\0';
        std::strncpy(m_notes, order.getNotes().c_str(), sizeof(m_notes) - 1);
        m_notes[sizeof(m_notes) - 1] = '\0';
        m_statusIndex = std::clamp(static_cast<int>(order.getStatus()), 0, 4);

        m_newItemName[0] = '\0';
        m_newItemQty = 1;
        m_newItemPrice = 0.0;
    }

    void OrderModalView::close() {
        m_isOpen = false;
    }

    void OrderModalView::render(
        Services::OrderManager& orderMgr,
        const std::function<void(const Models::Order&)>& onOrderSaved,
        const std::vector<Models::Customer>* allCustomers
    ) {
        if (!m_isOpen) return;

        ImGui::OpenPopup("OrderDialog");
        ImGui::SetNextWindowSize(ImVec2(660, 560), ImGuiCond_Appearing);

        if (ImGui::BeginPopupModal("OrderDialog", &m_isOpen, ImGuiWindowFlags_NoResize)) {
            if (m_isEditMode) {
                ImGui::Text("EDIT ORDER #%llu", static_cast<unsigned long long>(m_orderId));
            } else {
                ImGui::TextUnformatted("CREATE NEW ORDER");
            }

            if (m_customerId == 0 && allCustomers && !allCustomers->empty()) {
                std::string preview = m_customerName.empty() ? "-- Select Customer --" : (m_customerName + " (#" + std::to_string(m_customerId) + ")");
                if (ImGui::BeginCombo("Select Customer", preview.c_str())) {
                    for (const auto& cust : *allCustomers) {
                        bool isSelected = (cust.getId() == m_customerId);
                        std::string label = cust.getName() + " (#" + std::to_string(cust.getId()) + ")";
                        if (ImGui::Selectable(label.c_str(), isSelected)) {
                            m_customerId = cust.getId();
                            m_customerName = cust.getName();
                        }
                    }
                    ImGui::EndCombo();
                }
            } else {
                ImGui::Text("Customer: %s (ID: #%llu)", m_customerName.c_str(), static_cast<unsigned long long>(m_customerId));
            }

            ImGui::InputText("Order Date", m_orderDate, sizeof(m_orderDate));

            const char* statuses[] = { "Pending", "In Progress", "Completed", "Delivered", "Cancelled" };
            ImGui::Combo("Order Status", &m_statusIndex, statuses, IM_ARRAYSIZE(statuses));

            ImGui::Separator();
            ImGui::TextUnformatted("ORDERED ITEMS:");

            if (ImGui::BeginTable("OrderItemsTable", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY, ImVec2(0.0f, 150.0f))) {
                ImGui::TableSetupColumn("Item Name", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Qty", ImGuiTableColumnFlags_WidthFixed, 50.0f);
                ImGui::TableSetupColumn("Price", ImGuiTableColumnFlags_WidthFixed, 80.0f);
                ImGui::TableSetupColumn("Total", ImGuiTableColumnFlags_WidthFixed, 80.0f);
                ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed, 60.0f);
                NeoWidgets::TableHeadersRow();

                int deleteItemIdx = -1;
                for (size_t i = 0; i < m_lineItems.size(); ++i) {
                    ImGui::TableNextRow();
                    ImGui::PushID(static_cast<int>(i));

                    ImGui::TableSetColumnIndex(0);
                    ImGui::TextUnformatted(m_lineItems[i].getItemName().c_str());

                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%d", m_lineItems[i].getQuantity());

                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("$%.2f", m_lineItems[i].getUnitPrice());

                    ImGui::TableSetColumnIndex(3);
                    ImGui::Text("$%.2f", m_lineItems[i].getTotalPrice());

                    ImGui::TableSetColumnIndex(4);
                    if (ImGui::Button("DEL")) {
                        deleteItemIdx = static_cast<int>(i);
                    }

                    ImGui::PopID();
                }

                if (deleteItemIdx >= 0 && deleteItemIdx < static_cast<int>(m_lineItems.size())) {
                    m_lineItems.erase(m_lineItems.begin() + deleteItemIdx);
                }

                ImGui::EndTable();
            }

            // New Line Item Row
            ImGui::InputTextWithHint("##ItemName", "Item description", m_newItemName, sizeof(m_newItemName));
            ImGui::SameLine();
            ImGui::SetNextItemWidth(60);
            ImGui::InputInt("##ItemQty", &m_newItemQty);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(80);
            ImGui::InputDouble("##ItemPrice", &m_newItemPrice, 0.0, 0.0, "%.2f");
            ImGui::SameLine();
            if (NeoWidgets::BrutalistButton("+ ADD ITEM", 100.0f, 28.0f, 0xFF00E6FF)) {
                if (m_newItemName[0] != '\0' && m_newItemQty > 0 && m_newItemPrice >= 0.0) {
                    m_lineItems.emplace_back(m_newItemName, m_newItemQty, m_newItemPrice);
                    m_newItemName[0] = '\0';
                    m_newItemQty = 1;
                    m_newItemPrice = 0.0;
                }
            }

            double grandTotal = 0.0;
            for (const auto& item : m_lineItems) grandTotal += item.getTotalPrice();
            ImGui::Text("ORDER GRAND TOTAL: $%.2f", grandTotal);

            ImGui::InputTextMultiline("Notes / Instructions", m_notes, sizeof(m_notes), ImVec2(-1, 50));

            ImGui::Spacing();
            if (NeoWidgets::BrutalistButton("SAVE ORDER", 140.0f, 36.0f, 0xFF00FFA6)) {
                if (m_customerId != 0) {
                    if (m_isEditMode) {
                        auto opt = orderMgr.findByIdMut(m_orderId);
                        if (opt) {
                            auto& ord = opt->get();
                            ord.setOrderDate(m_orderDate);
                            ord.setStatus(static_cast<Core::OrderStatus>(m_statusIndex));
                            ord.setNotes(m_notes);
                            ord.setItems(m_lineItems);
                            if (onOrderSaved) {
                                onOrderSaved(ord);
                            }
                        }
                    } else {
                        auto& newOrd = orderMgr.createOrder(
                            m_customerId, m_customerName, m_orderDate, m_lineItems,
                            static_cast<Core::OrderStatus>(m_statusIndex), m_notes
                        );
                        if (onOrderSaved) {
                            onOrderSaved(newOrd);
                        }
                    }
                    close();
                }
            }

            ImGui::SameLine();
            if (NeoWidgets::BrutalistButton("CANCEL", 100.0f, 36.0f)) {
                close();
            }

            ImGui::EndPopup();
        }
    }

} // namespace App::UI
