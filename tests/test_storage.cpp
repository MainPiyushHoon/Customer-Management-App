// tests/test_storage.cpp
#include "TestRunner.hpp"
#include "storage/JsonStorage.hpp"
#include "storage/ExcelStorage.hpp"
#include "storage/AutoSaveCoordinator.hpp"
#include <filesystem>
#include <fstream>

int main() {
    std::cout << "Running Storage Layer Tests...\n";
    std::filesystem::create_directories("test_data");

    App::Storage::JsonStorage jsonStorage;
    App::Storage::ExcelStorage excelStorage;

    // Create sample data
    std::vector<App::Models::Customer> customers;
    App::Models::Customer c(101, "Jane Smith", "+1 987-6543");
    c.setCreatedAt("2026-11-13 10:00");
    c.addMeasurement({"Chest", "36", "in"});
    c.addMeasurement({"Waist", "28", "in"});
    c.addOrderId(501);
    customers.push_back(c);

    std::vector<App::Models::Order> orders;
    App::Models::Order o(501, 101, "Jane Smith", "2026-11-13 12:00");
    o.addItem({"Blazer", 1, 180.0});
    orders.push_back(o);

    // 1. Test JSON save and load
    bool jsonSavedCust = jsonStorage.saveCustomers(customers, "test_data/test_customers.json");
    TestRunner::assert_true(jsonSavedCust, "Customers saved to JSON");
    TestRunner::assert_true(!std::filesystem::exists("test_data/test_customers.json.tmp"), "No leftover .tmp file after customer atomic save");

    std::vector<App::Models::Customer> loadedCust;
    bool jsonLoadedCust = jsonStorage.loadCustomers(loadedCust, "test_data/test_customers.json");
    TestRunner::assert_true(jsonLoadedCust && loadedCust.size() == 1, "Customers loaded from JSON");
    TestRunner::assert_true(loadedCust[0].getCreatedAt() == "2026-11-13 10:00", "Customer createdAt restored from JSON");
    TestRunner::assert_true(loadedCust[0].getMeasurements().size() == 2, "Customer measurements restored");

    bool jsonSavedOrd = jsonStorage.saveOrders(orders, "test_data/test_orders.json");
    TestRunner::assert_true(jsonSavedOrd, "Orders saved to JSON");
    TestRunner::assert_true(!std::filesystem::exists("test_data/test_orders.json.tmp"), "No leftover .tmp file after order atomic save");

    std::vector<App::Models::Order> loadedOrd;
    bool jsonLoadedOrd = jsonStorage.loadOrders(loadedOrd, "test_data/test_orders.json");
    TestRunner::assert_true(jsonLoadedOrd && loadedOrd.size() == 1, "Orders loaded from JSON");
    TestRunner::assert_near(loadedOrd[0].getTotalAmount(), 180.0, "Loaded order total matches");

    // 2. Test Combined Excel/CSV export
    bool excelExported = excelStorage.exportToSpreadsheet(customers, orders, "test_data/test_export.csv");
    TestRunner::assert_true(excelExported, "Combined spreadsheet CSV exported successfully");
    TestRunner::assert_true(std::filesystem::file_size("test_data/test_export.csv") > 0, "Spreadsheet file is not empty");
    TestRunner::assert_true(!std::filesystem::exists("test_data/test_export.csv.tmp"), "No leftover .tmp file after spreadsheet export");

    {
        std::ifstream f("test_data/test_export.csv");
        std::string content((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
        TestRunner::assert_true(content.find("Created At") != std::string::npos, "Spreadsheet customer header contains 'Created At'");
        TestRunner::assert_true(content.find("2026-11-13 10:00") != std::string::npos, "Spreadsheet data row contains customer Created At value");
        TestRunner::assert_true(content.find("=== CUSTOMERS ===") != std::string::npos && content.find("=== ORDERS ===") != std::string::npos, "Combined spreadsheet contains both section banners");
    }

    // 3. Test Individual Sheet Exporters
    bool custSheetExported = excelStorage.exportCustomersSheet(customers, "test_data/test_customers_export.csv");
    TestRunner::assert_true(custSheetExported, "Customers individual sheet exported successfully");
    TestRunner::assert_true(!std::filesystem::exists("test_data/test_customers_export.csv.tmp"), "No leftover .tmp file after customers sheet export");
    {
        std::ifstream f("test_data/test_customers_export.csv");
        std::string content((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
        TestRunner::assert_true(content.find("Customer ID,Name,Phone Number,Created At") != std::string::npos, "Customers sheet has correct header including Created At");
        TestRunner::assert_true(content.find("Jane Smith") != std::string::npos && content.find("2026-11-13 10:00") != std::string::npos, "Customers sheet contains customer row data");
        TestRunner::assert_true(content.find("=== ORDERS ===") == std::string::npos && content.find("Order ID,Customer ID") == std::string::npos, "Customers sheet does not emit empty orders section banner or headers");
    }

    bool ordSheetExported = excelStorage.exportOrdersSheet(orders, "test_data/test_orders_export.csv");
    TestRunner::assert_true(ordSheetExported, "Orders individual sheet exported successfully");
    TestRunner::assert_true(!std::filesystem::exists("test_data/test_orders_export.csv.tmp"), "No leftover .tmp file after orders sheet export");
    {
        std::ifstream f("test_data/test_orders_export.csv");
        std::string content((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
        TestRunner::assert_true(content.find("Order ID,Customer ID,Customer Name,Date") != std::string::npos, "Orders sheet has correct header");
        TestRunner::assert_true(content.find("Blazer") != std::string::npos, "Orders sheet contains order item data");
        TestRunner::assert_true(content.find("=== CUSTOMERS ===") == std::string::npos && content.find("Measurements Summary") == std::string::npos, "Orders sheet does not emit empty customers section banner or headers");
    }

    // Test RFC 4180 \r escape in CSV
    std::vector<App::Models::Customer> rfcCust;
    App::Models::Customer cRfc(202, "Bob\rBuilder", "+1 000-0000");
    cRfc.setCreatedAt("2026-11-13 11:00");
    rfcCust.push_back(cRfc);
    excelStorage.exportCustomersSheet(rfcCust, "test_data/test_rfc_export.csv");
    {
        std::ifstream f("test_data/test_rfc_export.csv");
        std::string content((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
        TestRunner::assert_true(content.find("\"Bob\rBuilder\"") != std::string::npos, "CSV escapes text containing \\r in double quotes per RFC 4180");
    }

    // 4. Test AutoSaveCoordinator
    App::Storage::AutoSaveCoordinator autoSave(60);
    TestRunner::assert_true(autoSave.getSecondsRemaining() == 60, "AutoSave timer initializes to 60s");
    bool callbackTriggered = false;
    autoSave.update(30.0f, [&]() { callbackTriggered = true; });
    TestRunner::assert_true(!callbackTriggered && autoSave.getSecondsRemaining() == 30, "Timer decrements to 30s");
    autoSave.update(31.0f, [&]() { callbackTriggered = true; });
    TestRunner::assert_true(callbackTriggered, "AutoSave callback triggers after 60s");
    TestRunner::assert_true(autoSave.getLastSaveTimestamp() != "Not saved yet", "Last save timestamp recorded");

    return TestRunner::summary();
}
