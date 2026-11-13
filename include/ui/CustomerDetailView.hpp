#pragma once

#include "models/Customer.hpp"
#include "services/CustomerManager.hpp"
#include "services/OrderManager.hpp"
#include <functional>

namespace App::UI {

    class CustomerDetailView {
    public:
        CustomerDetailView() = default;

        void render(
            Models::Customer* customer,
            Services::CustomerManager& customerMgr,
            Services::OrderManager& orderMgr,
            const std::function<void(Core::CustomerId)>& onNewOrderClick,
            const std::function<void(Core::OrderId)>& onEditOrderClick
        );

        [[nodiscard]] bool isEditingBasicInfo() const { return m_isEditingBasicInfo; }
        [[nodiscard]] Core::CustomerId getCurrentCustomerId() const { return m_currentCustomerId; }

    private:
        char m_editName[128] = "";
        char m_editPhone[64] = "";
        char m_newMeasKey[64] = "";
        char m_newMeasVal[64] = "";
        char m_newMeasUnit[32] = "";
        bool m_isEditingBasicInfo = false;
        Core::CustomerId m_currentCustomerId = 0;
    };

} // namespace App::UI
