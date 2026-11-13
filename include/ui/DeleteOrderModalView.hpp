#pragma once

#include "core/Types.hpp"
#include <string>
#include <functional>

namespace App::UI {

    class DeleteOrderModalView {
    public:
        DeleteOrderModalView() = default;

        void open(Core::OrderId orderId, const std::string& customerName, double totalAmount);
        void close();

        [[nodiscard]] bool isOpen() const { return m_isOpen; }
        [[nodiscard]] Core::OrderId getOrderId() const { return m_orderId; }
        [[nodiscard]] const std::string& getCustomerName() const { return m_customerName; }
        [[nodiscard]] double getTotalAmount() const { return m_totalAmount; }

        void render(
            const std::function<void(Core::OrderId)>& onConfirmDelete,
            const std::function<void()>& onCancel = nullptr
        );

    private:
        bool m_isOpen = false;
        Core::OrderId m_orderId = 0;
        std::string m_customerName;
        double m_totalAmount = 0.0;
    };

} // namespace App::UI
