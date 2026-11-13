// src/storage/ExcelStorage.cpp
#include "storage/ExcelStorage.hpp"
#include <fstream>
#include <sstream>

namespace App::Storage {

    static std::string escapeCsv(const std::string& text) {
        bool needsQuotes = text.find(',') != std::string::npos || 
                           text.find('"') != std::string::npos || 
                           text.find('\n') != std::string::npos;
        if (!needsQuotes) return text;
        std::string result = "\"";
        for (char c : text) {
            if (c == '"') result += "\"\"";
            else result += c;
        }
        result += "\"";
        return result;
    }

    bool ExcelStorage::exportToSpreadsheet(
        const std::vector<Models::Customer>& customers,
        const std::vector<Models::Order>& orders,
        const std::string& destinationPath
    ) {
        std::ofstream file(destinationPath);
        if (!file.is_open()) return false;

        // Section 1: Customers
        file << "=== CUSTOMERS ===\n";
        file << "Customer ID,Name,Phone Number,Measurements Summary,Total Orders Placed\n";
        for (const auto& c : customers) {
            std::ostringstream measStream;
            for (size_t i = 0; i < c.getMeasurements().size(); ++i) {
                const auto& m = c.getMeasurements()[i];
                measStream << m.getKey() << ": " << m.getValue();
                if (!m.getUnit().empty()) measStream << " " << m.getUnit();
                if (i + 1 < c.getMeasurements().size()) measStream << " | ";
            }

            file << c.getId() << ","
                 << escapeCsv(c.getName()) << ","
                 << escapeCsv(c.getPhone()) << ","
                 << escapeCsv(measStream.str()) << ","
                 << c.getOrderIds().size() << "\n";
        }

        file << "\n=== ORDERS ===\n";
        file << "Order ID,Customer ID,Customer Name,Date,Items Ordered,Total Amount,Status,Notes\n";
        for (const auto& o : orders) {
            std::ostringstream itemsStream;
            for (size_t i = 0; i < o.getItems().size(); ++i) {
                const auto& it = o.getItems()[i];
                itemsStream << it.getItemName() << " (" << it.getQuantity() << "x @ $" << it.getUnitPrice() << ")";
                if (i + 1 < o.getItems().size()) itemsStream << " ; ";
            }

            file << o.getId() << ","
                 << o.getCustomerId() << ","
                 << escapeCsv(o.getCustomerName()) << ","
                 << escapeCsv(o.getOrderDate()) << ","
                 << escapeCsv(itemsStream.str()) << ","
                 << o.getTotalAmount() << ","
                 << Core::OrderStatusToString(o.getStatus()) << ","
                 << escapeCsv(o.getNotes()) << "\n";
        }

        return true;
    }

    bool ExcelStorage::exportCustomersSheet(const std::vector<Models::Customer>& customers, const std::string& filePath) {
        std::vector<Models::Order> emptyOrders;
        return exportToSpreadsheet(customers, emptyOrders, filePath);
    }

    bool ExcelStorage::exportOrdersSheet(const std::vector<Models::Order>& orders, const std::string& filePath) {
        std::vector<Models::Customer> emptyCustomers;
        return exportToSpreadsheet(emptyCustomers, orders, filePath);
    }

} // namespace App::Storage
