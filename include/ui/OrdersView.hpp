#pragma once

#include "models/Order.hpp"
#include "services/CustomerManager.hpp"
#include "services/OrderManager.hpp"
#include <functional>
#include <string>

namespace App::UI {

    class OrdersView {
    public:
        OrdersView() = default;

        void render(
            Services::CustomerManager& customerMgr,
            Services::OrderManager& orderMgr,
            const std::function<void(Core::OrderId)>& onEditOrderClick,
            const std::function<void()>& onNewOrderClick
        );

        [[nodiscard]] int getActiveFilterIndex() const { return m_activeFilterIndex; }
        void setActiveFilterIndex(int index) { m_activeFilterIndex = index; }

    private:
        int m_activeFilterIndex = 0; // 0: All, 1: Pending, 2: InProgress, 3: Completed, 4: Delivered, 5: Cancelled
        char m_searchQuery[128] = "";
    };

} // namespace App::UI
