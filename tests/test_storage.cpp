// tests/test_storage.cpp
#include "TestRunner.hpp"
#include "storage/JsonStorage.hpp"
#include "storage/ExcelStorage.hpp"
#include "storage/AutoSaveCoordinator.hpp"
#include <filesystem>

int main() {
    std::cout << "Running Storage Layer Tests...\n";
    std::filesystem::create_directories("test_data");

    App::Storage::JsonStorage jsonStorage;
    App::Storage::ExcelStorage excelStorage;

    // Create sample data
    std::vector<App::Models::Customer> customers;
    App::Models::Customer c(101, "Jane Smith", "+1 987-6543");
    c.addMeasurement({"Chest", "36", "in"});
    c.addMeasurement({"Waist", "28", "in"});
    c.addOrderId(501);
    customers.push_back(c);

    std::vector<App::Models::Order> orders;
    App::Models::Order o(501, 101, "Jane Smith", "2026-11-13 12:00");
    o.addItem({"Blazer", 1, 180.0});
    orders.push_back(o);

    // Test JSON save and load
    bool jsonSavedCust = jsonStorage.saveCustomers(customers, "test_data/test_customers.json");
    TestRunner::assert_true(jsonSavedCust, "Customers saved to JSON");

    std::vector<App::Models::Customer> loadedCust;
    bool jsonLoadedCust = jsonStorage.loadCustomers(loadedCust, "test_data/test_customers.json");
    TestRunner::assert_true(jsonLoadedCust && loadedCust.size() == 1, "Customers loaded from JSON");
    TestRunner::assert_true(loadedCust[0].getMeasurements().size() == 2, "Customer measurements restored");

    bool jsonSavedOrd = jsonStorage.saveOrders(orders, "test_data/test_orders.json");
    TestRunner::assert_true(jsonSavedOrd, "Orders saved to JSON");

    std::vector<App::Models::Order> loadedOrd;
    bool jsonLoadedOrd = jsonStorage.loadOrders(loadedOrd, "test_data/test_orders.json");
    TestRunner::assert_true(jsonLoadedOrd && loadedOrd.size() == 1, "Orders loaded from JSON");
    TestRunner::assert_true(loadedOrd[0].getTotalAmount() == 180.0, "Loaded order total matches");

    // Test Excel/CSV export
    bool excelExported = excelStorage.exportToSpreadsheet(customers, orders, "test_data/test_export.csv");
    TestRunner::assert_true(excelExported, "Spreadsheet CSV exported successfully");
    TestRunner::assert_true(std::filesystem::file_size("test_data/test_export.csv") > 0, "Spreadsheet file is not empty");

    // Test AutoSaveCoordinator
    App::Storage::AutoSaveCoordinator autoSave(60);
    TestRunner::assert_true(autoSave.getSecondsRemaining() == 60, "AutoSave timer initializes to 60s");
    bool callbackTriggered = false;
    autoSave.update(30.0f, [&]() { callbackTriggered = true; });
    TestRunner::assert_true(!callbackTriggered && autoSave.getSecondsRemaining() == 30, "Timer decrements to 30s");
    autoSave.update(31.0f, [&]() { callbackTriggered = true; });
    TestRunner::assert_true(callbackTriggered, "AutoSave callback triggers after 60s");

    return TestRunner::summary();
}
