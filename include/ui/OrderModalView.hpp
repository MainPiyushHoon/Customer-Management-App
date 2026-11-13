#pragma once

#include "models/Order.hpp"
#include "models/Customer.hpp"
#include "services/OrderManager.hpp"
#include <functional>
#include <vector>

namespace App::UI {

    class OrderModalView {
    public:
        OrderModalView() = default;

        void openCreate(Core::CustomerId customerId = 0, const std::string& customerName = "");
        void openEdit(const Models::Order& order);
        void close();

        void render(
            Services::OrderManager& orderMgr,
            const std::function<void(const Models::Order&)>& onOrderSaved,
            const std::vector<Models::Customer>* allCustomers = nullptr
        );

        [[nodiscard]] bool isOpen() const { return m_isOpen; }
        [[nodiscard]] Core::OrderId getOrderId() const { return m_orderId; }
        [[nodiscard]] const std::vector<Models::OrderItem>& getLineItems() const { return m_lineItems; }

    private:
        bool m_isOpen = false;
        bool m_isEditMode = false;
        Core::OrderId m_orderId = 0;
        Core::CustomerId m_customerId = 0;
        std::string m_customerName;
        char m_orderDate[64] = "";
        char m_notes[256] = "";
        int m_statusIndex = 0;

        std::vector<Models::OrderItem> m_lineItems;
        char m_newItemName[128] = "";
        int m_newItemQty = 1;
        double m_newItemPrice = 0.0;
    };

} // namespace App::UI
