#pragma once

#include <string>

namespace App::Core {

    struct AppConfig {
        static constexpr const char* AppName = "BRUTAL // OPS - Customer & Order Manager";
        static constexpr const char* Version = "1.0.0";
        
        static constexpr int DefaultWindowWidth = 1280;
        static constexpr int DefaultWindowHeight = 800;

        // Auto-save interval in seconds
        static constexpr int DefaultAutoSaveIntervalSeconds = 60;

        // Storage paths
        static constexpr const char* CustomersJsonPath = "data/customers.json";
        static constexpr const char* OrdersJsonPath = "data/orders.json";
        static constexpr const char* ExcelCsvExportPath = "data/records_export.csv";
    };

} // namespace App::Core
