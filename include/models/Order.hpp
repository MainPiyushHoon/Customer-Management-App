#pragma once

#include "core/Types.hpp"
#include "models/OrderItem.hpp"
#include <string>
#include <vector>
#include <numeric>

namespace App::Models {

    class Order {
    public:
        Order() = default;
        Order(Core::OrderId id, Core::CustomerId customerId, std::string customerName, std::string orderDate)
            : m_id(id), m_customerId(customerId), m_customerName(std::move(customerName)), m_orderDate(std::move(orderDate)) {}

        // Identity & Customer linkage
        [[nodiscard]] Core::OrderId getId() const { return m_id; }
        void setId(Core::OrderId id) { m_id = id; }

        [[nodiscard]] Core::CustomerId getCustomerId() const { return m_customerId; }
        void setCustomerId(Core::CustomerId customerId) { m_customerId = customerId; }

        [[nodiscard]] const std::string& getCustomerName() const { return m_customerName; }
        void setCustomerName(std::string name) { m_customerName = std::move(name); }

        // Date
        [[nodiscard]] const std::string& getOrderDate() const { return m_orderDate; }
        void setOrderDate(std::string date) { m_orderDate = std::move(date); }

        // Line Items
        [[nodiscard]] const std::vector<OrderItem>& getItems() const { return m_items; }
        [[nodiscard]] std::vector<OrderItem>& getItems() { return m_items; }
        void addItem(const OrderItem& item) { m_items.push_back(item); }
        void removeItem(size_t index) {
            if (index < m_items.size()) {
                m_items.erase(m_items.begin() + index);
            }
        }
        void setItems(std::vector<OrderItem> items) { m_items = std::move(items); }

        // Financials
        [[nodiscard]] double getTotalAmount() const {
            return std::accumulate(m_items.begin(), m_items.end(), 0.0,
                [](double sum, const OrderItem& item) {
                    return sum + item.getTotalPrice();
                });
        }

        // Status & Notes
        [[nodiscard]] Core::OrderStatus getStatus() const { return m_status; }
        void setStatus(Core::OrderStatus status) { m_status = status; }

        [[nodiscard]] const std::string& getNotes() const { return m_notes; }
        void setNotes(std::string notes) { m_notes = std::move(notes); }

    private:
        Core::OrderId m_id = 0;
        Core::CustomerId m_customerId = 0;
        std::string m_customerName;
        std::string m_orderDate;
        std::vector<OrderItem> m_items;
        Core::OrderStatus m_status = Core::OrderStatus::Pending;
        std::string m_notes;
    };

} // namespace App::Models
