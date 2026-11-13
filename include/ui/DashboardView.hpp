#pragma once

#include "services/DashboardStats.hpp"
#include "services/OrderManager.hpp"
#include "storage/AutoSaveCoordinator.hpp"
#include "ui/ThemeManager.hpp"
#include <functional>

namespace App::UI {

    class DashboardView {
    public:
        DashboardView() = default;

        // End-user focused dashboard render with Live Recent Orders feed
        void render(
            const Services::DashboardMetrics& metrics,
            const Services::OrderManager& orderMgr,
            const std::function<void()>& onAddNewCustomerClick = nullptr,
            const std::function<void()>& onAddNewOrderClick = nullptr,
            const std::function<void(ViewTab)>& onNavigateTab = nullptr,
            const std::function<void(Core::OrderId)>& onViewOrderClick = nullptr,
            const std::function<void()>& onManualSaveClick = nullptr
        );

        // Backward compatibility overload for existing call sites during migration
        void render(
            const Services::DashboardMetrics& metrics,
            Storage::AutoSaveCoordinator& autoSave,
            ThemeManager& themeMgr,
            const std::function<void()>& onManualSaveClick,
            const std::function<void(ViewTab)>& onNavigateTab = nullptr,
            const std::function<void()>& onAddNewCustomerClick = nullptr,
            const std::function<void()>& onAddNewOrderClick = nullptr
        );
    };

} // namespace App::UI
