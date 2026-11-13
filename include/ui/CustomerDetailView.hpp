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

    private:
        char m_editName[128] = "";
        char m_editPhone[64] = "";
        char m_newMeasKey[64] = "";
        char m_newMeasVal[64] = "";
        char m_newMeasUnit[32] = "";
        bool m_isEditingBasicInfo = false;
    };

} // namespace App::UI
