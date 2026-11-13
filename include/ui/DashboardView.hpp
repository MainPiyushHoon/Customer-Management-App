#pragma once

#include "services/DashboardStats.hpp"
#include "storage/AutoSaveCoordinator.hpp"
#include "ui/ThemeManager.hpp"
#include <functional>

namespace App::UI {

    class DashboardView {
    public:
        DashboardView() = default;

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
