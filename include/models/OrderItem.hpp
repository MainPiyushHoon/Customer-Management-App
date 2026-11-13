#pragma once

#include <string>

namespace App::Models {

    class OrderItem {
    public:
        OrderItem() = default;
        OrderItem(std::string itemName, int quantity, double unitPrice)
            : m_itemName(std::move(itemName)), m_quantity(quantity), m_unitPrice(unitPrice) {}

        // Getters
        [[nodiscard]] const std::string& getItemName() const { return m_itemName; }
        [[nodiscard]] int getQuantity() const { return m_quantity; }
        [[nodiscard]] double getUnitPrice() const { return m_unitPrice; }
        [[nodiscard]] double getTotalPrice() const { return m_quantity * m_unitPrice; }

        // Setters
        void setItemName(std::string name) { m_itemName = std::move(name); }
        void setQuantity(int qty) { m_quantity = (qty > 0) ? qty : 1; }
        void setUnitPrice(double price) { m_unitPrice = (price >= 0.0) ? price : 0.0; }

    private:
        std::string m_itemName;
        int m_quantity = 1;
        double m_unitPrice = 0.0;
    };

} // namespace App::Models
