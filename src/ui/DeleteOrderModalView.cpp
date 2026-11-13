// src/ui/DeleteOrderModalView.cpp
#include "ui/DeleteOrderModalView.hpp"
#include "ui/NeoWidgets.hpp"
#include "imgui.h"

namespace App::UI {

    void DeleteOrderModalView::open(Core::OrderId orderId, const std::string& customerName, double totalAmount) {
        m_isOpen = true;
        m_orderId = orderId;
        m_customerName = customerName;
        m_totalAmount = totalAmount;
    }

    void DeleteOrderModalView::close() {
        m_isOpen = false;
    }

    void DeleteOrderModalView::render(
        const std::function<void(Core::OrderId)>& onConfirmDelete,
        const std::function<void()>& onCancel
    ) {
        if (!m_isOpen) return;

        ImGui::OpenPopup("CONFIRM ORDER DELETION // CRITICAL");

        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(520.0f, 260.0f), ImGuiCond_Appearing);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 3.0f);
        ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(0, 0, 0, 255));

        bool popupActive = ImGui::BeginPopupModal("CONFIRM ORDER DELETION // CRITICAL", &m_isOpen, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::PopStyleColor();
        ImGui::PopStyleVar(2);

        if (popupActive) {
            ImDrawList* bgDrawList = ImGui::GetBackgroundDrawList();
            if (bgDrawList) {
                ImVec2 winPos = ImGui::GetWindowPos();
                ImVec2 winSize = ImGui::GetWindowSize();
                bgDrawList->AddRectFilled(
                    ImVec2(winPos.x + 4.0f, winPos.y + 4.0f),
                    ImVec2(winPos.x + winSize.x + 4.0f, winPos.y + winSize.y + 4.0f),
                    0xFF000000, 0.0f
                );
            }

            ImGui::Spacing();
            ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "WARNING: You are about to permanently delete this order record.");
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::Text("Order ID: #%llu", static_cast<unsigned long long>(m_orderId));
            ImGui::Text("Customer: %s", m_customerName.c_str());
            ImGui::Text("Total Amount: $%.2f", m_totalAmount);
            ImGui::Spacing();
            ImGui::TextDisabled("This action cannot be undone.");

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            if (NeoWidgets::BrutalistButton("[CONFIRM DELETE]", 180.0f, 36.0f, 0xFFBCAEFF)) {
                if (onConfirmDelete) {
                    onConfirmDelete(m_orderId);
                }
                ImGui::CloseCurrentPopup();
                close();
            }

            ImGui::SameLine();
            ImGui::Spacing();
            ImGui::SameLine();

            if (NeoWidgets::BrutalistButton("[CANCEL]", 130.0f, 36.0f, 0xFFE5E7A0)) {
                if (onCancel) {
                    onCancel();
                }
                ImGui::CloseCurrentPopup();
                close();
            }

            ImGui::EndPopup();
        } else {
            if (!m_isOpen && onCancel) {
                onCancel();
            }
        }
    }

} // namespace App::UI
