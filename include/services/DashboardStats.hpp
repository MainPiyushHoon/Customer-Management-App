#pragma once

#include "services/CustomerManager.hpp"
#include "services/OrderManager.hpp"
#include <cstddef>

namespace App::Services {

    struct DashboardMetrics {
        size_t totalCustomers = 0;
        size_t activeOrders = 0;
        size_t completedOrders = 0;
        double totalRevenue = 0.0;
        double pendingRevenue = 0.0;
    };

    class DashboardStats {
    public:
        static DashboardMetrics compute(
            const CustomerManager& customerMgr,
            const OrderManager& orderMgr
        );
    };

} // namespace App::Services
