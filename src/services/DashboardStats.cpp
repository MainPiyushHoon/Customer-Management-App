// src/services/DashboardStats.cpp
#include "services/DashboardStats.hpp"

namespace App::Services {

    DashboardMetrics DashboardStats::compute(
        const CustomerManager& customerMgr,
        const OrderManager& orderMgr
    ) {
        DashboardMetrics metrics;
        metrics.totalCustomers = customerMgr.getAllCustomers().size();

        for (const auto& o : orderMgr.getAllOrders()) {
            if (o.getStatus() == Core::OrderStatus::Completed || o.getStatus() == Core::OrderStatus::Delivered) {
                metrics.completedOrders++;
                metrics.totalRevenue += o.getTotalAmount();
            } else if (o.getStatus() != Core::OrderStatus::Cancelled) {
                metrics.activeOrders++;
                metrics.pendingRevenue += o.getTotalAmount();
            }
        }
        return metrics;
    }

} // namespace App::Services
