#pragma once

#include "models/Customer.hpp"
#include "models/Order.hpp"
#include <vector>
#include <string>

namespace App::Storage {

    class ExcelStorage {
    public:
        ExcelStorage() = default;

        // Exports Customers and Orders into spreadsheet format (CSV and/or XLSX)
        bool exportToSpreadsheet(
            const std::vector<Models::Customer>& customers,
            const std::vector<Models::Order>& orders,
            const std::string& destinationPath
        );

        // Individual sheet exporters
        bool exportCustomersSheet(const std::vector<Models::Customer>& customers, const std::string& filePath);
        bool exportOrdersSheet(const std::vector<Models::Order>& orders, const std::string& filePath);
    };

} // namespace App::Storage
