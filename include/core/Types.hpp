#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <chrono>

namespace App::Core {

    using CustomerId = uint64_t;
    using OrderId = uint64_t;

    enum class OrderStatus {
        Pending,
        InProgress,
        Completed,
        Delivered,
        Cancelled
    };

    inline std::string OrderStatusToString(OrderStatus status) {
        switch (status) {
            case OrderStatus::Pending:    return "Pending";
            case OrderStatus::InProgress: return "In Progress";
            case OrderStatus::Completed:  return "Completed";
            case OrderStatus::Delivered:  return "Delivered";
            case OrderStatus::Cancelled:  return "Cancelled";
            default:                      return "Unknown";
        }
    }

    inline OrderStatus StringToOrderStatus(const std::string& str) {
        if (str == "In Progress") return OrderStatus::InProgress;
        if (str == "Completed")   return OrderStatus::Completed;
        if (str == "Delivered")   return OrderStatus::Delivered;
        if (str == "Cancelled")   return OrderStatus::Cancelled;
        return OrderStatus::Pending;
    }

} // namespace App::Core
