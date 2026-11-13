// src/storage/ExcelStorage.cpp
#include "storage/ExcelStorage.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>

namespace App::Storage {

    static std::string escapeCsv(const std::string& text) {
        bool needsQuotes = text.find(',') != std::string::npos || 
                           text.find('"') != std::string::npos || 
                           text.find('\n') != std::string::npos ||
                           text.find('\r') != std::string::npos;
        if (!needsQuotes) return text;
        std::string result = "\"";
        for (char c : text) {
            if (c == '"') result += "\"\"";
            else result += c;
        }
        result += "\"";
        return result;
    }

    static void writeCustomersCsv(std::ostream& file, const std::vector<Models::Customer>& customers) {
        file << "Customer ID,Name,Phone Number,Created At,Measurements Summary,Total Orders Placed\n";
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
                 << escapeCsv(c.getCreatedAt()) << ","
                 << escapeCsv(measStream.str()) << ","
                 << c.getOrderIds().size() << "\n";
        }
    }

    static void writeOrdersCsv(std::ostream& file, const std::vector<Models::Order>& orders) {
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
    }

    bool ExcelStorage::exportToSpreadsheet(
        const std::vector<Models::Customer>& customers,
        const std::vector<Models::Order>& orders,
        const std::string& destinationPath
    ) {
        std::string tmpPath = destinationPath + ".tmp";
        try {
            {
                std::ofstream file(tmpPath);
                if (!file.is_open()) return false;

                // Section 1: Customers
                file << "=== CUSTOMERS ===\n";
                writeCustomersCsv(file, customers);

                // Section 2: Orders
                file << "\n=== ORDERS ===\n";
                writeOrdersCsv(file, orders);

                file.flush();
                if (!file.good()) {
                    file.close();
                    std::error_code ec;
                    std::filesystem::remove(tmpPath, ec);
                    return false;
                }
            }

            std::error_code ec;
            if (std::filesystem::exists(destinationPath, ec)) {
                std::filesystem::remove(destinationPath, ec);
            }
            std::filesystem::rename(tmpPath, destinationPath, ec);
            if (ec) {
                std::filesystem::remove(tmpPath, ec);
                return false;
            }
            return true;
        } catch (const std::exception& e) {
            std::error_code ec;
            std::filesystem::remove(tmpPath, ec);
            return false;
        }
    }

    bool ExcelStorage::exportCustomersSheet(const std::vector<Models::Customer>& customers, const std::string& filePath) {
        std::string tmpPath = filePath + ".tmp";
        try {
            {
                std::ofstream file(tmpPath);
                if (!file.is_open()) return false;

                writeCustomersCsv(file, customers);

                file.flush();
                if (!file.good()) {
                    file.close();
                    std::error_code ec;
                    std::filesystem::remove(tmpPath, ec);
                    return false;
                }
            }

            std::error_code ec;
            if (std::filesystem::exists(filePath, ec)) {
                std::filesystem::remove(filePath, ec);
            }
            std::filesystem::rename(tmpPath, filePath, ec);
            if (ec) {
                std::filesystem::remove(tmpPath, ec);
                return false;
            }
            return true;
        } catch (const std::exception& e) {
            std::error_code ec;
            std::filesystem::remove(tmpPath, ec);
            return false;
        }
    }

    bool ExcelStorage::exportOrdersSheet(const std::vector<Models::Order>& orders, const std::string& filePath) {
        std::string tmpPath = filePath + ".tmp";
        try {
            {
                std::ofstream file(tmpPath);
                if (!file.is_open()) return false;

                writeOrdersCsv(file, orders);

                file.flush();
                if (!file.good()) {
                    file.close();
                    std::error_code ec;
                    std::filesystem::remove(tmpPath, ec);
                    return false;
                }
            }

            std::error_code ec;
            if (std::filesystem::exists(filePath, ec)) {
                std::filesystem::remove(filePath, ec);
            }
            std::filesystem::rename(tmpPath, filePath, ec);
            if (ec) {
                std::filesystem::remove(tmpPath, ec);
                return false;
            }
            return true;
        } catch (const std::exception& e) {
            std::error_code ec;
            std::filesystem::remove(tmpPath, ec);
            return false;
        }
    }

} // namespace App::Storage
