#pragma once

#include "models/Order.hpp"
#include <vector>
#include <optional>
#include <string>

namespace App::Services {

    class OrderManager {
    public:
        OrderManager() = default;

        // CRUD Operations
        Models::Order& createOrder(
            Core::CustomerId customerId,
            const std::string& customerName,
            const std::string& orderDate,
            const std::vector<Models::OrderItem>& items,
            Core::OrderStatus status = Core::OrderStatus::Pending,
            const std::string& notes = ""
        );

        bool updateOrder(const Models::Order& order);
        bool deleteOrder(Core::OrderId id);

        [[nodiscard]] const std::vector<Models::Order>& getAllOrders() const { return m_orders; }
        [[nodiscard]] std::vector<Models::Order>& getAllOrders() { return m_orders; }

        [[nodiscard]] std::optional<std::reference_wrapper<const Models::Order>> findById(Core::OrderId id) const;
        [[nodiscard]] std::optional<std::reference_wrapper<Models::Order>> findByIdMut(Core::OrderId id);

        // Querying orders for a particular customer
        [[nodiscard]] std::vector<const Models::Order*> getOrdersForCustomer(Core::CustomerId customerId) const;

        // Status Filtering
        [[nodiscard]] std::vector<const Models::Order*> filterByStatus(Core::OrderStatus status) const;

        // ID Generation
        Core::OrderId generateNextId();

    private:
        std::vector<Models::Order> m_orders;
        Core::OrderId m_nextId = 5001;
    };

} // namespace App::Services
