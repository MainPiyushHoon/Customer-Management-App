#pragma once

#include "services/DashboardStats.hpp"
#include "storage/AutoSaveCoordinator.hpp"
#include "ui/ThemeManager.hpp"

namespace App::UI {

    class DashboardView {
    public:
        DashboardView() = default;

        void render(
            const Services::DashboardMetrics& metrics,
            Storage::AutoSaveCoordinator& autoSave,
            ThemeManager& themeMgr,
            const std::function<void()>& onManualSaveClick
        );
    };

} // namespace App::UI
