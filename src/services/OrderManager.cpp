// src/services/OrderManager.cpp
#include "services/OrderManager.hpp"
#include <algorithm>

namespace App::Services {

    Models::Order& OrderManager::createOrder(
        Core::CustomerId customerId,
        const std::string& customerName,
        const std::string& orderDate,
        const std::vector<Models::OrderItem>& items,
        Core::OrderStatus status,
        const std::string& notes
    ) {
        Core::OrderId id = generateNextId();
        auto& order = m_orders.emplace_back(id, customerId, customerName, orderDate);
        order.setItems(items);
        order.setStatus(status);
        order.setNotes(notes);
        return order;
    }

    bool OrderManager::updateOrder(const Models::Order& order) {
        for (auto& o : m_orders) {
            if (o.getId() == order.getId()) {
                o = order;
                return true;
            }
        }
        return false;
    }

    bool OrderManager::deleteOrder(Core::OrderId id) {
        auto it = std::remove_if(m_orders.begin(), m_orders.end(),
            [id](const Models::Order& o) { return o.getId() == id; });
        if (it != m_orders.end()) {
            m_orders.erase(it, m_orders.end());
            return true;
        }
        return false;
    }

    std::optional<std::reference_wrapper<const Models::Order>> OrderManager::findById(Core::OrderId id) const {
        for (const auto& o : m_orders) {
            if (o.getId() == id) return std::cref(o);
        }
        return std::nullopt;
    }

    std::optional<std::reference_wrapper<Models::Order>> OrderManager::findByIdMut(Core::OrderId id) {
        for (auto& o : m_orders) {
            if (o.getId() == id) return std::ref(o);
        }
        return std::nullopt;
    }

    std::vector<const Models::Order*> OrderManager::getOrdersForCustomer(Core::CustomerId customerId) const {
        std::vector<const Models::Order*> results;
        for (const auto& o : m_orders) {
            if (o.getCustomerId() == customerId) {
                results.push_back(&o);
            }
        }
        return results;
    }

    std::vector<const Models::Order*> OrderManager::filterByStatus(Core::OrderStatus status) const {
        std::vector<const Models::Order*> results;
        for (const auto& o : m_orders) {
            if (o.getStatus() == status) {
                results.push_back(&o);
            }
        }
        return results;
    }

    Core::OrderId OrderManager::generateNextId() {
        for (const auto& o : m_orders) {
            if (o.getId() >= m_nextId) {
                m_nextId = o.getId() + 1;
            }
        }
        return m_nextId++;
    }

} // namespace App::Services
