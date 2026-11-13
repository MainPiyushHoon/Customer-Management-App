// src/storage/JsonStorage.cpp
#include "storage/JsonStorage.hpp"
#if __has_include(<nlohmann/json.hpp>)
#include <nlohmann/json.hpp>
#elif __has_include("nlohmann/json.hpp")
#include "nlohmann/json.hpp"
#elif __has_include(<json.hpp>)
#include <json.hpp>
#elif __has_include("json.hpp")
#include "json.hpp"
#endif
#include <fstream>
#include <iostream>
#include <filesystem>

using json = nlohmann::json;

namespace App::Storage {

    bool JsonStorage::saveCustomers(const std::vector<Models::Customer>& customers, const std::string& destination) {
        std::string tmpPath = destination + ".tmp";
        try {
            json j = json::array();
            for (const auto& c : customers) {
                json custJson;
                custJson["id"] = c.getId();
                custJson["name"] = c.getName();
                custJson["phone"] = c.getPhone();
                custJson["createdAt"] = c.getCreatedAt();
                custJson["orderIds"] = c.getOrderIds();

                json measArray = json::array();
                for (const auto& m : c.getMeasurements()) {
                    measArray.push_back({
                        {"key", m.getKey()},
                        {"value", m.getValue()},
                        {"unit", m.getUnit()}
                    });
                }
                custJson["measurements"] = measArray;
                j.push_back(custJson);
            }

            {
                std::ofstream file(tmpPath);
                if (!file.is_open()) return false;
                file << j.dump(2);
                file.flush();
                if (!file.good()) {
                    file.close();
                    std::error_code ec;
                    std::filesystem::remove(tmpPath, ec);
                    return false;
                }
            }

            std::error_code ec;
            if (std::filesystem::exists(destination, ec)) {
                std::filesystem::remove(destination, ec);
            }
            std::filesystem::rename(tmpPath, destination, ec);
            if (ec) {
                std::filesystem::remove(tmpPath, ec);
                return false;
            }
            return true;
        } catch (const std::exception& e) {
            std::cerr << "JsonStorage error saving customers: " << e.what() << "\n";
            std::error_code ec;
            std::filesystem::remove(tmpPath, ec);
            return false;
        }
    }

    bool JsonStorage::loadCustomers(std::vector<Models::Customer>& customers, const std::string& source) {
        try {
            std::ifstream file(source);
            if (!file.is_open()) return false;

            json j;
            file >> j;
            customers.clear();

            for (const auto& item : j) {
                Core::CustomerId id = item.value("id", 0ULL);
                std::string name = item.value("name", "");
                std::string phone = item.value("phone", "");
                Models::Customer c(id, name, phone);
                c.setCreatedAt(item.value("createdAt", ""));

                if (item.contains("orderIds") && item["orderIds"].is_array()) {
                    for (auto& ordId : item["orderIds"]) {
                        c.addOrderId(ordId.get<Core::OrderId>());
                    }
                }

                if (item.contains("measurements") && item["measurements"].is_array()) {
                    for (const auto& mJson : item["measurements"]) {
                        c.addMeasurement({
                            mJson.value("key", ""),
                            mJson.value("value", ""),
                            mJson.value("unit", "")
                        });
                    }
                }
                customers.push_back(std::move(c));
            }
            return true;
        } catch (const std::exception& e) {
            std::cerr << "JsonStorage error loading customers: " << e.what() << "\n";
            return false;
        }
    }

    bool JsonStorage::saveOrders(const std::vector<Models::Order>& orders, const std::string& destination) {
        std::string tmpPath = destination + ".tmp";
        try {
            json j = json::array();
            for (const auto& o : orders) {
                json orderJson;
                orderJson["id"] = o.getId();
                orderJson["customerId"] = o.getCustomerId();
                orderJson["customerName"] = o.getCustomerName();
                orderJson["orderDate"] = o.getOrderDate();
                orderJson["status"] = Core::OrderStatusToString(o.getStatus());
                orderJson["notes"] = o.getNotes();

                json itemsArray = json::array();
                for (const auto& it : o.getItems()) {
                    itemsArray.push_back({
                        {"itemName", it.getItemName()},
                        {"quantity", it.getQuantity()},
                        {"unitPrice", it.getUnitPrice()}
                    });
                }
                orderJson["items"] = itemsArray;
                j.push_back(orderJson);
            }

            {
                std::ofstream file(tmpPath);
                if (!file.is_open()) return false;
                file << j.dump(2);
                file.flush();
                if (!file.good()) {
                    file.close();
                    std::error_code ec;
                    std::filesystem::remove(tmpPath, ec);
                    return false;
                }
            }

            std::error_code ec;
            if (std::filesystem::exists(destination, ec)) {
                std::filesystem::remove(destination, ec);
            }
            std::filesystem::rename(tmpPath, destination, ec);
            if (ec) {
                std::filesystem::remove(tmpPath, ec);
                return false;
            }
            return true;
        } catch (const std::exception& e) {
            std::cerr << "JsonStorage error saving orders: " << e.what() << "\n";
            std::error_code ec;
            std::filesystem::remove(tmpPath, ec);
            return false;
        }
    }

    bool JsonStorage::loadOrders(std::vector<Models::Order>& orders, const std::string& source) {
        try {
            std::ifstream file(source);
            if (!file.is_open()) return false;

            json j;
            file >> j;
            orders.clear();

            for (const auto& item : j) {
                Core::OrderId id = item.value("id", 0ULL);
                Core::CustomerId customerId = item.value("customerId", 0ULL);
                std::string customerName = item.value("customerName", "");
                std::string orderDate = item.value("orderDate", "");
                Models::Order o(id, customerId, customerName, orderDate);

                o.setStatus(Core::StringToOrderStatus(item.value("status", "Pending")));
                o.setNotes(item.value("notes", ""));

                if (item.contains("items") && item["items"].is_array()) {
                    for (const auto& itJson : item["items"]) {
                        o.addItem({
                            itJson.value("itemName", ""),
                            itJson.value("quantity", 1),
                            itJson.value("unitPrice", 0.0)
                        });
                    }
                }
                orders.push_back(std::move(o));
            }
            return true;
        } catch (const std::exception& e) {
            std::cerr << "JsonStorage error loading orders: " << e.what() << "\n";
            return false;
        }
    }

} // namespace App::Storage
