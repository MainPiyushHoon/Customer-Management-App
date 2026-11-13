# Customer & Order Management Desktop App Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Build a native C++20 desktop application featuring Neo-Brutalist visual design, customer management with dynamic editable measurements, order management with line items, and dual JSON + Excel persistence with 60-second background auto-save.

**Architecture:** Strictly modular Object-Oriented design separating data models (`Models::*`), business domain services (`Services::*`), dual file storage & auto-save coordination (`Storage::*`), and Dear ImGui + GLFW/OpenGL3 Neo-Brutalist UI views (`UI::*`).

**Tech Stack:** C++20, MinGW-w64 / MSYS2 GCC 14.2.0, CMake, Dear ImGui (Docking/Tables API), GLFW, OpenGL3, nlohmann/json, custom CSV/Excel XML serializer.

## Global Constraints
- Language Standard: C++20 (`-std=c++20`).
- Compiler: GCC 14.2.0 (UCRT64) on Windows.
- Geometry & Styling: Neo-Brutalist design (0 corner rounding, 2.5-3px solid borders, 4px solid drop shadows, dual Light/Dark palette).
- Customer Fields: Unique ID, Name, Phone Number, Dynamic Key-Value Measurements (editable on the fly), Order IDs.
- Order Fields: Unique ID, Customer ID & Name, Date, Detailed Line Items (Item Name, Qty, Unit Price), Total Price, Status (Pending, In Progress, Completed, Delivered, Cancelled), Notes.
- Persistence: JSON (`data/customers.json`, `data/orders.json`) and Excel-compatible format (`data/records_export.csv` / `.xlsx`).
- Auto-save: Triggers automatically every 60 seconds with live countdown indicator, saves on manual button click, and flushes on window close.

---

### Task 1: Core Models Implementation & Unit Tests

**Files:**
- Create: `src/models/Measurement.cpp`
- Create: `src/models/OrderItem.cpp`
- Create: `src/models/Customer.cpp`
- Create: `src/models/Order.cpp`
- Create: `tests/test_models.cpp`
- Create: `tests/TestRunner.hpp`

**Interfaces:**
- Consumes: `include/core/Types.hpp`, `include/models/Measurement.hpp`, `include/models/OrderItem.hpp`, `include/models/Customer.hpp`, `include/models/Order.hpp`
- Produces: Compiled `Models::Measurement`, `Models::OrderItem`, `Models::Customer`, and `Models::Order` with full getter, setter, accumulation, and mutation capabilities.

- [ ] **Step 1: Create lightweight test framework header and write failing test for models**

```cpp
// tests/TestRunner.hpp
#pragma once
#include <iostream>
#include <string>
#include <vector>

namespace TestRunner {
    inline int passed = 0;
    inline int failed = 0;

    inline void assert_true(bool condition, const std::string& message) {
        if (condition) {
            std::cout << "  [PASS] " << message << "\n";
            passed++;
        } else {
            std::cerr << "  [FAIL] " << message << "\n";
            failed++;
        }
    }

    inline int summary() {
        std::cout << "\n=====================================\n";
        std::cout << "TEST SUMMARY: " << passed << " passed, " << failed << " failed.\n";
        std::cout << "=====================================\n";
        return failed == 0 ? 0 : 1;
    }
}
```

```cpp
// tests/test_models.cpp
#include "TestRunner.hpp"
#include "models/Measurement.hpp"
#include "models/OrderItem.hpp"
#include "models/Customer.hpp"
#include "models/Order.hpp"

int main() {
    std::cout << "Running Model Tests...\n";

    // Test Measurement
    App::Models::Measurement m("Chest", "38.5", "in");
    TestRunner::assert_true(m.getKey() == "Chest", "Measurement key is Chest");
    TestRunner::assert_true(m.getValue() == "38.5", "Measurement value is 38.5");
    TestRunner::assert_true(m.getUnit() == "in", "Measurement unit is in");
    m.setValue("40");
    TestRunner::assert_true(m.getValue() == "40", "Measurement value updated to 40");

    // Test OrderItem
    App::Models::OrderItem item("Custom Suit", 2, 150.0);
    TestRunner::assert_true(item.getItemName() == "Custom Suit", "OrderItem name matches");
    TestRunner::assert_true(item.getQuantity() == 2, "OrderItem quantity matches");
    TestRunner::assert_true(item.getUnitPrice() == 150.0, "OrderItem price matches");
    TestRunner::assert_true(item.getTotalPrice() == 300.0, "OrderItem total price is 300");

    // Test Customer
    App::Models::Customer cust(1001, "Alice Doe", "+1 555-0199");
    TestRunner::assert_true(cust.getId() == 1001, "Customer ID is 1001");
    TestRunner::assert_true(cust.getName() == "Alice Doe", "Customer name matches");
    cust.addMeasurement(m);
    TestRunner::assert_true(cust.getMeasurements().size() == 1, "Customer has 1 measurement");
    cust.addOrderId(5001);
    TestRunner::assert_true(cust.getOrderIds().size() == 1, "Customer has 1 order ID");
    cust.removeOrderId(5001);
    TestRunner::assert_true(cust.getOrderIds().empty(), "Customer order ID removed");

    // Test Order
    App::Models::Order order(5001, 1001, "Alice Doe", "2026-11-13 19:00");
    order.addItem(item);
    App::Models::OrderItem item2("Tie", 1, 25.0);
    order.addItem(item2);
    TestRunner::assert_true(order.getItems().size() == 2, "Order has 2 items");
    TestRunner::assert_true(order.getTotalAmount() == 325.0, "Order total amount is 325");
    TestRunner::assert_true(order.getStatus() == App::Core::OrderStatus::Pending, "Default status is Pending");
    order.setStatus(App::Core::OrderStatus::InProgress);
    TestRunner::assert_true(order.getStatus() == App::Core::OrderStatus::InProgress, "Status updated to InProgress");

    return TestRunner::summary();
}
```

- [ ] **Step 2: Run test to verify compilation and execution**

Run:
```powershell
g++ -std=c++20 -Iinclude tests/test_models.cpp -o build_test_models.exe
./build_test_models.exe
```
Expected: Compiles and outputs test summary.

- [ ] **Step 3: Implement model source files**

```cpp
// src/models/Measurement.cpp
#include "models/Measurement.hpp"

namespace App::Models {
    // Header implementation is inline; translation unit reserved for custom validators
}
```

```cpp
// src/models/OrderItem.cpp
#include "models/OrderItem.hpp"

namespace App::Models {
    // Inline implementation in header
}
```

```cpp
// src/models/Customer.cpp
#include "models/Customer.hpp"

namespace App::Models {
    // Customer implementation
}
```

```cpp
// src/models/Order.cpp
#include "models/Order.hpp"

namespace App::Models {
    // Order implementation
}
```

- [ ] **Step 4: Re-run test with model sources included**

Run:
```powershell
g++ -std=c++20 -Iinclude src/models/*.cpp tests/test_models.cpp -o build_test_models.exe
./build_test_models.exe
```
Expected: `TEST SUMMARY: 14 passed, 0 failed.`

- [ ] **Step 5: Commit**

```bash
git add src/models/ tests/
git commit -m "feat: implement and verify core data models and unit tests"
```

---

### Task 2: Service Layer Implementation & Unit Tests

**Files:**
- Create: `src/services/CustomerManager.cpp`
- Create: `src/services/OrderManager.cpp`
- Create: `src/services/DashboardStats.cpp`
- Create: `tests/test_services.cpp`

**Interfaces:**
- Consumes: `include/services/CustomerManager.hpp`, `include/services/OrderManager.hpp`, `include/services/DashboardStats.hpp`, `include/models/*.hpp`
- Produces: Fully functional CRUD services, search querying by Name/Phone, filtering orders by status, and KPI stats calculations.

- [ ] **Step 1: Write failing unit test for services**

```cpp
// tests/test_services.cpp
#include "TestRunner.hpp"
#include "services/CustomerManager.hpp"
#include "services/OrderManager.hpp"
#include "services/DashboardStats.hpp"

int main() {
    std::cout << "Running Service Layer Tests...\n";

    App::Services::CustomerManager customerMgr;
    auto& c1 = customerMgr.createCustomer("Bob Builder", "+1 123-4567");
    TestRunner::assert_true(c1.getId() == 1001, "First customer assigned ID 1001");
    TestRunner::assert_true(customerMgr.getAllCustomers().size() == 1, "Total customers is 1");

    auto found = customerMgr.findById(1001);
    TestRunner::assert_true(found.has_value() && found->get().getName() == "Bob Builder", "Found customer by ID");

    auto searchResults = customerMgr.search("Bob");
    TestRunner::assert_true(searchResults.size() == 1, "Search by name returns 1 result");

    auto searchPhone = customerMgr.search("123");
    TestRunner::assert_true(searchPhone.size() == 1, "Search by phone substring returns 1 result");

    // Orders Service
    App::Services::OrderManager orderMgr;
    std::vector<App::Models::OrderItem> items = {
        {"Dress Shirt", 1, 60.0},
        {"Trousers", 2, 70.0}
    };
    auto& o1 = orderMgr.createOrder(c1.getId(), c1.getName(), "2026-11-13 10:00", items, App::Core::OrderStatus::InProgress);
    TestRunner::assert_true(o1.getId() == 5001, "First order assigned ID 5001");
    TestRunner::assert_true(o1.getTotalAmount() == 200.0, "Order total is 200.0");

    auto customerOrders = orderMgr.getOrdersForCustomer(c1.getId());
    TestRunner::assert_true(customerOrders.size() == 1, "Customer has 1 order");

    auto inProgressOrders = orderMgr.filterByStatus(App::Core::OrderStatus::InProgress);
    TestRunner::assert_true(inProgressOrders.size() == 1, "1 order in progress");

    // Dashboard Stats
    auto metrics = App::Services::DashboardStats::compute(customerMgr, orderMgr);
    TestRunner::assert_true(metrics.totalCustomers == 1, "Dashboard shows 1 total customer");
    TestRunner::assert_true(metrics.activeOrders == 1, "Dashboard shows 1 active order");
    TestRunner::assert_true(metrics.pendingRevenue == 200.0, "Dashboard shows 200 pending revenue");

    return TestRunner::summary();
}
```

- [ ] **Step 2: Run test to verify it fails before implementation**

Run:
```powershell
g++ -std=c++20 -Iinclude tests/test_services.cpp -o build_test_services.exe
```
Expected: FAIL with undefined references to `CustomerManager::createCustomer`, `OrderManager::createOrder`, etc.

- [ ] **Step 3: Implement CustomerManager, OrderManager, and DashboardStats**

```cpp
// src/services/CustomerManager.cpp
#include "services/CustomerManager.hpp"
#include <algorithm>

namespace App::Services {

    Models::Customer& CustomerManager::createCustomer(const std::string& name, const std::string& phone) {
        Core::CustomerId id = generateNextId();
        m_customers.emplace_back(id, name, phone);
        return m_customers.back();
    }

    bool CustomerManager::updateCustomer(const Models::Customer& customer) {
        for (auto& c : m_customers) {
            if (c.getId() == customer.getId()) {
                c = customer;
                return true;
            }
        }
        return false;
    }

    bool CustomerManager::deleteCustomer(Core::CustomerId id) {
        auto it = std::remove_if(m_customers.begin(), m_customers.end(),
            [id](const Models::Customer& c) { return c.getId() == id; });
        if (it != m_customers.end()) {
            m_customers.erase(it, m_customers.end());
            return true;
        }
        return false;
    }

    std::optional<std::reference_wrapper<const Models::Customer>> CustomerManager::findById(Core::CustomerId id) const {
        for (const auto& c : m_customers) {
            if (c.getId() == id) return std::cref(c);
        }
        return std::nullopt;
    }

    std::optional<std::reference_wrapper<Models::Customer>> CustomerManager::findByIdMut(Core::CustomerId id) {
        for (auto& c : m_customers) {
            if (c.getId() == id) return std::ref(c);
        }
        return std::nullopt;
    }

    std::vector<const Models::Customer*> CustomerManager::search(const std::string& query) const {
        std::vector<const Models::Customer*> results;
        if (query.empty()) {
            for (const auto& c : m_customers) results.push_back(&c);
            return results;
        }

        std::string lowerQuery = query;
        std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);

        for (const auto& c : m_customers) {
            std::string lowerName = c.getName();
            std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
            if (lowerName.find(lowerQuery) != std::string::npos || c.getPhone().find(lowerQuery) != std::string::npos) {
                results.push_back(&c);
            }
        }
        return results;
    }

    Core::CustomerId CustomerManager::generateNextId() {
        return m_nextId++;
    }

} // namespace App::Services
```

```cpp
// src/services/OrderManager.cpp
#include "services/OrderManager.hpp"
#include <algorithm>

namespace App::Services {

    Models::Order& OrderManager::createOrder(
        Core::CustomerId customerId,
        const std::string& customerName,
        const std::string& orderDate,
        const std::vector<Models::OrderItem>& items,
        Core::OrderStatus status,
        const std::string& notes
    ) {
        Core::OrderId id = generateNextId();
        auto& order = m_orders.emplace_back(id, customerId, customerName, orderDate);
        order.setItems(items);
        order.setStatus(status);
        order.setNotes(notes);
        return order;
    }

    bool OrderManager::updateOrder(const Models::Order& order) {
        for (auto& o : m_orders) {
            if (o.getId() == order.getId()) {
                o = order;
                return true;
            }
        }
        return false;
    }

    bool OrderManager::deleteOrder(Core::OrderId id) {
        auto it = std::remove_if(m_orders.begin(), m_orders.end(),
            [id](const Models::Order& o) { return o.getId() == id; });
        if (it != m_orders.end()) {
            m_orders.erase(it, m_orders.end());
            return true;
        }
        return false;
    }

    std::optional<std::reference_wrapper<const Models::Order>> OrderManager::findById(Core::OrderId id) const {
        for (const auto& o : m_orders) {
            if (o.getId() == id) return std::cref(o);
        }
        return std::nullopt;
    }

    std::optional<std::reference_wrapper<Models::Order>> OrderManager::findByIdMut(Core::OrderId id) {
        for (auto& o : m_orders) {
            if (o.getId() == id) return std::ref(o);
        }
        return std::nullopt;
    }

    std::vector<const Models::Order*> OrderManager::getOrdersForCustomer(Core::CustomerId customerId) const {
        std::vector<const Models::Order*> results;
        for (const auto& o : m_orders) {
            if (o.getCustomerId() == customerId) {
                results.push_back(&o);
            }
        }
        return results;
    }

    std::vector<const Models::Order*> OrderManager::filterByStatus(Core::OrderStatus status) const {
        std::vector<const Models::Order*> results;
        for (const auto& o : m_orders) {
            if (o.getStatus() == status) {
                results.push_back(&o);
            }
        }
        return results;
    }

    Core::OrderId OrderManager::generateNextId() {
        return m_nextId++;
    }

} // namespace App::Services
```

```cpp
// src/services/DashboardStats.cpp
#include "services/DashboardStats.hpp"

namespace App::Services {

    DashboardMetrics DashboardStats::compute(
        const CustomerManager& customerMgr,
        const OrderManager& orderMgr
    ) {
        DashboardMetrics metrics;
        metrics.totalCustomers = customerMgr.getAllCustomers().size();

        for (const auto& o : orderMgr.getAllOrders()) {
            if (o.getStatus() == Core::OrderStatus::Completed || o.getStatus() == Core::OrderStatus::Delivered) {
                metrics.completedOrders++;
                metrics.totalRevenue += o.getTotalAmount();
            } else if (o.getStatus() != Core::OrderStatus::Cancelled) {
                metrics.activeOrders++;
                metrics.pendingRevenue += o.getTotalAmount();
            }
        }
        return metrics;
    }

} // namespace App::Services
```

- [ ] **Step 4: Run test to verify it passes**

Run:
```powershell
g++ -std=c++20 -Iinclude src/services/*.cpp tests/test_services.cpp -o build_test_services.exe
./build_test_services.exe
```
Expected: `TEST SUMMARY: 10 passed, 0 failed.`

- [ ] **Step 5: Commit**

```bash
git add src/services/ tests/test_services.cpp
git commit -m "feat: implement CustomerManager, OrderManager, and DashboardStats services"
```

---

### Task 3: Storage Layer Implementation (JSON & Excel) & Auto-Save Coordinator

**Files:**
- Create: `third_party/nlohmann/json.hpp` (Single-header JSON library)
- Create: `src/storage/JsonStorage.cpp`
- Create: `src/storage/ExcelStorage.cpp`
- Create: `src/storage/AutoSaveCoordinator.cpp`
- Create: `tests/test_storage.cpp`

**Interfaces:**
- Consumes: `include/storage/*.hpp`, `third_party/nlohmann/json.hpp`, `include/models/*.hpp`
- Produces: Dual JSON and Excel spreadsheet persistence, atomic write-out, 60s periodic trigger, and safe deserialization.

- [ ] **Step 1: Download nlohmann/json single header into third_party**

Download single-header `json.hpp` via Invoke-WebRequest.
Run:
```powershell
New-Item -ItemType Directory -Force -Path "third_party/nlohmann" | Out-Null
Invoke-WebRequest -Uri "https://github.com/nlohmann/json/releases/download/v3.11.3/json.hpp" -OutFile "third_party/nlohmann/json.hpp"
```
Verify: `Test-Path third_party/nlohmann/json.hpp` is True.

- [ ] **Step 2: Write failing unit test for storage**

```cpp
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
```

- [ ] **Step 3: Run test to verify it fails**

Run:
```powershell
g++ -std=c++20 -Iinclude -Ithird_party tests/test_storage.cpp -o build_test_storage.exe
```
Expected: FAIL with undefined reference errors.

- [ ] **Step 4: Implement JsonStorage, ExcelStorage, and AutoSaveCoordinator**

```cpp
// src/storage/JsonStorage.cpp
#include "storage/JsonStorage.hpp"
#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>

using json = nlohmann::json;

namespace App::Storage {

    bool JsonStorage::saveCustomers(const std::vector<Models::Customer>& customers, const std::string& destination) {
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

            std::ofstream file(destination);
            if (!file.is_open()) return false;
            file << j.dump(2);
            return true;
        } catch (const std::exception& e) {
            std::cerr << "JsonStorage error saving customers: " << e.what() << "\n";
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

            std::ofstream file(destination);
            if (!file.is_open()) return false;
            file << j.dump(2);
            return true;
        } catch (const std::exception& e) {
            std::cerr << "JsonStorage error saving orders: " << e.what() << "\n";
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
```

```cpp
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
```

```cpp
// src/storage/AutoSaveCoordinator.cpp
#include "storage/AutoSaveCoordinator.hpp"
#include <iomanip>
#include <sstream>
#include <ctime>

namespace App::Storage {

    AutoSaveCoordinator::AutoSaveCoordinator(int intervalSeconds)
        : m_intervalSeconds(intervalSeconds), m_timer(static_cast<float>(intervalSeconds)) {}

    void AutoSaveCoordinator::update(float deltaTime, const std::function<void()>& onSaveCallback) {
        m_timer -= deltaTime;
        if (m_timer <= 0.0f) {
            forceSave(onSaveCallback);
            m_timer = static_cast<float>(m_intervalSeconds);
        }
    }

    void AutoSaveCoordinator::forceSave(const std::function<void()>& onSaveCallback) {
        m_isSaving = true;
        if (onSaveCallback) {
            onSaveCallback();
        }
        m_timer = static_cast<float>(m_intervalSeconds);

        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::tm tmNow{};
#if defined(_WIN32)
        localtime_s(&tmNow, &now);
#else
        localtime_r(&now, &tmNow);
#endif
        std::ostringstream ss;
        ss << std::put_time(&tmNow, "%H:%M:%S");
        m_lastSaveTimestamp = ss.str();
        m_isSaving = false;
    }

    int AutoSaveCoordinator::getSecondsRemaining() const {
        return (m_timer > 0.0f) ? static_cast<int>(m_timer) : 0;
    }

    const std::string& AutoSaveCoordinator::getLastSaveTimestamp() const {
        return m_lastSaveTimestamp;
    }

} // namespace App::Storage
```

- [ ] **Step 5: Run tests to verify all tests pass**

Run:
```powershell
g++ -std=c++20 -Iinclude -Ithird_party/nlohmann src/storage/*.cpp tests/test_storage.cpp -o build_test_storage.exe
./build_test_storage.exe
```
Expected: `TEST SUMMARY: 10 passed, 0 failed.`

- [ ] **Step 6: Commit**

```bash
git add third_party/nlohmann/ src/storage/ tests/test_storage.cpp
git commit -m "feat: implement JSON, Excel/CSV storage and 60-second AutoSaveCoordinator"
```

---

### Task 4: Third-Party Dependencies (GLFW & Dear ImGui) and CMake Build Integration

**Files:**
- Create / Fetch: `third_party/imgui/` (Dear ImGui source & headers)
- Modify: `CMakeLists.txt`
- Test: Verify CMake configuration and GLFW linking on Windows MinGW/UCRT64

**Interfaces:**
- Consumes: GLFW & Dear ImGui v1.91+
- Produces: Fully configured CMake build producing native `BrutalOps.exe` executable with OpenGL3 & GLFW backends.

- [ ] **Step 1: Download or clone Dear ImGui into `third_party/imgui`**

Clone official Dear ImGui repository tag v1.91.5 (or latest stable) and copy needed backend files:
Run:
```powershell
git clone --depth 1 --branch v1.91.5 https://github.com/ocornut/imgui.git third_party/imgui
```
Verify: `third_party/imgui/imgui.h` and `third_party/imgui/backends/imgui_impl_glfw.h` exist.

- [ ] **Step 2: Configure CMake to build GLFW automatically via FetchContent**

Update `CMakeLists.txt` to include `FetchContent` for GLFW if not found locally, plus build all ImGui source files.

```cmake
cmake_minimum_required(VERSION 3.20)
project(BrutalOps VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable FetchContent for GLFW
include(FetchContent)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)
FetchContent_MakeAvailable(glfw)

# Directories
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party)
set(IMGUI_DIR ${THIRD_PARTY_DIR}/imgui)

set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

file(GLOB_RECURSE APP_SOURCES "${SRC_DIR}/*.cpp")

add_executable(BrutalOps
    ${APP_SOURCES}
    ${IMGUI_SOURCES}
)

target_include_directories(BrutalOps PUBLIC
    ${INCLUDE_DIR}
    ${THIRD_PARTY_DIR}
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

target_link_libraries(BrutalOps PRIVATE
    glfw
    opengl32
    gdi32
    imm32
    dwmapi
    comdlg32
)
```

- [ ] **Step 3: Test CMake configuration**

Run:
```powershell
cmake -B build -G "MinGW Makefiles"
```
Expected: Configuration completes with `Build files have been written to: .../build`.

- [ ] **Step 4: Commit**

```bash
git add CMakeLists.txt third_party/imgui/
git commit -m "build: configure GLFW FetchContent and Dear ImGui into CMake"
```

---

### Task 5: Neo-Brutalist Theme System & Custom Brutalist Widgets

**Files:**
- Create: `src/ui/ThemeManager.cpp`
- Create: `src/ui/NeoWidgets.cpp`
- Test: `tests/test_theme.cpp`

**Interfaces:**
- Consumes: `include/ui/ThemeManager.hpp`, `include/ui/NeoWidgets.hpp`, `imgui.h`
- Produces: Dual Light & Dark Neo-Brutalist palettes, 0-rounding, 3px solid borders, hard 4px offset shadow buttons and cards.

- [ ] **Step 1: Write test for ThemeManager palettes and toggles**

```cpp
// tests/test_theme.cpp
#include "TestRunner.hpp"
#include "ui/ThemeManager.hpp"

int main() {
    std::cout << "Running ThemeManager Tests...\n";
    App::UI::ThemeManager theme;
    TestRunner::assert_true(!theme.isDark(), "Default mode is Light");
    TestRunner::assert_true(theme.getPalette().accentYellow != 0, "Yellow accent is defined");

    theme.toggleMode();
    TestRunner::assert_true(theme.isDark(), "Mode toggled to Dark");

    theme.setMode(App::UI::NeoThemeMode::Light);
    TestRunner::assert_true(!theme.isDark(), "Mode set to Light");

    return TestRunner::summary();
}
```

- [ ] **Step 2: Run test to verify it fails**

Run:
```powershell
g++ -std=c++20 -Iinclude tests/test_theme.cpp -o build_test_theme.exe
```
Expected: FAIL with undefined references.

- [ ] **Step 3: Implement ThemeManager.cpp and NeoWidgets.cpp**

```cpp
// src/ui/ThemeManager.cpp
#include "ui/ThemeManager.hpp"
#include "imgui.h"

namespace App::UI {

    ThemeManager::ThemeManager() {
        // Light Neo-Brutalism Palette
        m_lightPalette = {
            .background       = 0xFFF8FDFD, // Cream/Off-white
            .surfaceCard      = 0xFFFFFFFF, // Pure White
            .border           = 0xFF000000, // Solid Black
            .shadow           = 0xFF000000, // Solid Black
            .textPrimary      = 0xFF000000, // Pitch Black
            .textSecondary    = 0xFF555555, // Charcoal
            .accentYellow     = 0xFF00E6FF, // Electric Yellow (#FFE600 ABGR: FF00E6FF)
            .accentPink       = 0xFFA76EFF, // Hot Pink (#FF6EA7)
            .accentCyan       = 0xFFFFF000, // Cyan (#00F0FF)
            .accentMint       = 0xFF9DFF00, // Mint (#00FF9D)
            .inputBackground  = 0xFFF0F0F0
        };

        // Cyber Dark Neo-Brutalism Palette
        m_darkPalette = {
            .background       = 0xFF141210, // Deep Charcoal
            .surfaceCard      = 0xFF1E1918, // Card Surface
            .border           = 0xFFE6E0E0, // Crisp White/Grey Border
            .shadow           = 0xFF000000, // Pitch Black Shadow
            .textPrimary      = 0xFFF7F5F5, // Crisp Off-White
            .textSecondary    = 0xFFAAAAAA, // Muted Grey
            .accentYellow     = 0xFF00E6FF, // Electric Yellow
            .accentPink       = 0xFF7A00FF, // Cyber Magenta (#FF007A)
            .accentCyan       = 0xFFFFE500, // Neon Cyan
            .accentMint       = 0xFF85FF00, // Neon Green (#00FF85)
            .inputBackground  = 0xFF2A2826
        };
    }

    void ThemeManager::setMode(NeoThemeMode mode) {
        m_currentMode = mode;
        applyStyle();
    }

    void ThemeManager::toggleMode() {
        setMode(m_currentMode == NeoThemeMode::Light ? NeoThemeMode::Dark : NeoThemeMode::Light);
    }

    const NeoPalette& ThemeManager::getPalette() const {
        return (m_currentMode == NeoThemeMode::Dark) ? m_darkPalette : m_lightPalette;
    }

    void ThemeManager::applyStyle() {
        ImGuiStyle& style = ImGui::GetStyle();

        // 1. Brutalist Geometry: 0 Rounding
        style.WindowRounding    = 0.0f;
        style.ChildRounding     = 0.0f;
        style.FrameRounding     = 0.0f;
        style.PopupRounding     = 0.0f;
        style.ScrollbarRounding = 0.0f;
        style.GrabRounding      = 0.0f;
        style.TabRounding       = 0.0f;

        // 2. Thick, crisp borders (2.5f to 3.0f)
        style.WindowBorderSize  = 3.0f;
        style.ChildBorderSize   = 2.5f;
        style.PopupBorderSize   = 3.0f;
        style.FrameBorderSize   = 2.0f;
        style.TabBorderSize     = 2.0f;

        // 3. Generous padding
        style.WindowPadding     = ImVec2(16.0f, 16.0f);
        style.FramePadding      = ImVec2(12.0f, 8.0f);
        style.ItemSpacing       = ImVec2(12.0f, 10.0f);
        style.ItemInnerSpacing  = ImVec2(8.0f, 6.0f);

        // 4. Color Assignment
        const auto& p = getPalette();
        ImVec4* colors = style.Colors;

        colors[ImGuiCol_WindowBg]          = ImGui::ColorConvertU32ToFloat4(p.background);
        colors[ImGuiCol_ChildBg]           = ImGui::ColorConvertU32ToFloat4(p.surfaceCard);
        colors[ImGuiCol_PopupBg]           = ImGui::ColorConvertU32ToFloat4(p.surfaceCard);
        colors[ImGuiCol_Border]            = ImGui::ColorConvertU32ToFloat4(p.border);
        colors[ImGuiCol_BorderShadow]      = ImGui::ColorConvertU32ToFloat4(p.shadow);
        colors[ImGuiCol_Text]              = ImGui::ColorConvertU32ToFloat4(p.textPrimary);
        colors[ImGuiCol_TextDisabled]      = ImGui::ColorConvertU32ToFloat4(p.textSecondary);
        colors[ImGuiCol_FrameBg]           = ImGui::ColorConvertU32ToFloat4(p.inputBackground);
        colors[ImGuiCol_FrameBgHovered]    = ImGui::ColorConvertU32ToFloat4(p.accentYellow);
        colors[ImGuiCol_Button]            = ImGui::ColorConvertU32ToFloat4(p.accentYellow);
        colors[ImGuiCol_ButtonHovered]     = ImGui::ColorConvertU32ToFloat4(p.accentMint);
        colors[ImGuiCol_ButtonActive]      = ImGui::ColorConvertU32ToFloat4(p.accentPink);
        colors[ImGuiCol_Header]            = ImGui::ColorConvertU32ToFloat4(p.accentYellow);
        colors[ImGuiCol_HeaderHovered]     = ImGui::ColorConvertU32ToFloat4(p.accentCyan);
        colors[ImGuiCol_HeaderActive]      = ImGui::ColorConvertU32ToFloat4(p.accentPink);
    }

} // namespace App::UI
```

```cpp
// src/ui/NeoWidgets.cpp
#include "ui/NeoWidgets.hpp"
#include "imgui.h"

namespace App::UI::NeoWidgets {

    void BeginBrutalistCard(const char* strId, float width, float height, uint32_t bgColor) {
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImVec2 size = ImVec2(width > 0 ? width : ImGui::GetContentRegionAvail().x, height > 0 ? height : 100.0f);

        // Draw hard drop shadow offset (+4, +4)
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        drawList->AddRectFilled(
            ImVec2(p.x + 4.0f, p.y + 4.0f),
            ImVec2(p.x + size.x + 4.0f, p.y + size.y + 4.0f),
            0xFF000000, 0.0f
        );

        ImGui::PushStyleColor(ImGuiCol_ChildBg, bgColor ? bgColor : ImGui::GetColorU32(ImGuiCol_ChildBg));
        ImGui::BeginChild(strId, size, true, ImGuiWindowFlags_None);
    }

    void EndBrutalistCard() {
        ImGui::EndChild();
        ImGui::PopStyleColor();
    }

    bool BrutalistButton(const char* label, float width, float height, uint32_t accentColor) {
        if (accentColor != 0) {
            ImGui::PushStyleColor(ImGuiCol_Button, accentColor);
        }

        ImVec2 p = ImGui::GetCursorScreenPos();
        ImVec2 size = ImVec2(width > 0 ? width : ImGui::CalcTextSize(label).x + 24.0f, height > 0 ? height : 36.0f);

        // Draw shadow under button
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        drawList->AddRectFilled(
            ImVec2(p.x + 3.0f, p.y + 3.0f),
            ImVec2(p.x + size.x + 3.0f, p.y + size.y + 3.0f),
            0xFF000000, 0.0f
        );

        bool clicked = ImGui::Button(label, size);

        if (accentColor != 0) {
            ImGui::PopStyleColor();
        }
        return clicked;
    }

    void BrutalistBadge(const char* text, uint32_t bgColor, uint32_t textColor) {
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 textSize = ImGui::CalcTextSize(text);
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImVec2 padding(8.0f, 4.0f);
        ImVec2 boxSize(textSize.x + padding.x * 2, textSize.y + padding.y * 2);

        // Drop shadow
        drawList->AddRectFilled(ImVec2(p.x + 2, p.y + 2), ImVec2(p.x + boxSize.x + 2, p.y + boxSize.y + 2), 0xFF000000);
        // Badge body
        drawList->AddRectFilled(p, ImVec2(p.x + boxSize.x, p.y + boxSize.y), bgColor);
        // Border
        drawList->AddRect(p, ImVec2(p.x + boxSize.x, p.y + boxSize.y), 0xFF000000, 0.0f, 0, 2.0f);
        // Text
        drawList->AddText(ImVec2(p.x + padding.x, p.y + padding.y), textColor, text);

        ImGui::Dummy(boxSize);
    }

    void StatCard(const char* label, const std::string& value, uint32_t accentColor, const char* icon) {
        BeginBrutalistCard(label, 0.0f, 90.0f, accentColor);
        ImGui::TextDisabled("%s", icon ? icon : "// KPI");
        ImGui::TextUnformatted(label);
        ImGui::SetWindowFontScale(1.4f);
        ImGui::TextColored(ImVec4(0, 0, 0, 1), "%s", value.c_str());
        ImGui::SetWindowFontScale(1.0f);
        EndBrutalistCard();
    }

} // namespace App::UI::NeoWidgets
```

- [ ] **Step 4: Run unit test to verify ThemeManager passes**

Run:
```powershell
g++ -std=c++20 -Iinclude -Ithird_party/imgui src/ui/ThemeManager.cpp third_party/imgui/imgui.cpp third_party/imgui/imgui_draw.cpp third_party/imgui/imgui_tables.cpp third_party/imgui/imgui_widgets.cpp tests/test_theme.cpp -o build_test_theme.exe
./build_test_theme.exe
```
Expected: `TEST SUMMARY: 4 passed, 0 failed.`

- [ ] **Step 5: Commit**

```bash
git add src/ui/ThemeManager.cpp src/ui/NeoWidgets.cpp tests/test_theme.cpp
git commit -m "feat: implement Neo-Brutalist theme engine and custom brutalist widgets"
```

---

### Task 6: Customer Management Views (CustomerListView & CustomerDetailView with Dynamic Measurements)

**Files:**
- Create: `src/ui/CustomerListView.cpp`
- Create: `src/ui/CustomerDetailView.cpp`

**Interfaces:**
- Consumes: `CustomerManager`, `OrderManager`, `NeoWidgets`, `Customer`
- Produces: Searchable customer table, add customer modal, dynamic measurement editor (add, edit, delete measurement rows), and customer orders overview.

- [ ] **Step 1: Implement CustomerListView.cpp**

```cpp
// src/ui/CustomerListView.cpp
#include "ui/CustomerListView.hpp"
#include "ui/NeoWidgets.hpp"
#include "imgui.h"

namespace App::UI {

    void CustomerListView::render(
        Services::CustomerManager& customerMgr,
        Core::CustomerId selectedCustomerId,
        const std::function<void(Core::CustomerId)>& onSelectCustomer,
        const std::function<void()>& onAddNewCustomerClick
    ) {
        ImGui::TextUnformatted("SEARCH CUSTOMERS:");
        ImGui::InputTextWithHint("##SearchCust", "Search by name or phone...", m_searchQuery, IM_ARRAYSIZE(m_searchQuery));

        ImGui::SameLine();
        if (NeoWidgets::BrutalistButton("+ NEW CUSTOMER", 150.0f, 34.0f, 0xFF00E6FF)) {
            onAddNewCustomerClick();
        }

        ImGui::Separator();

        auto filtered = customerMgr.search(m_searchQuery);

        if (ImGui::BeginTable("CustomersTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
            ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 60.0f);
            ImGui::TableSetupColumn("Customer Name", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Phone", ImGuiTableColumnFlags_WidthFixed, 130.0f);
            ImGui::TableSetupColumn("Orders", ImGuiTableColumnFlags_WidthFixed, 70.0f);
            ImGui::TableHeadersRow();

            for (const auto* c : filtered) {
                ImGui::TableNextRow();
                bool isSelected = (c->getId() == selectedCustomerId);

                ImGui::TableSetColumnIndex(0);
                std::string idStr = "#" + std::to_string(c->getId());
                if (ImGui::Selectable(idStr.c_str(), isSelected, ImGuiSelectableFlags_SpanAllColumns)) {
                    onSelectCustomer(c->getId());
                }

                ImGui::TableSetColumnIndex(1);
                ImGui::TextUnformatted(c->getName().c_str());

                ImGui::TableSetColumnIndex(2);
                ImGui::TextUnformatted(c->getPhone().c_str());

                ImGui::TableSetColumnIndex(3);
                ImGui::Text("%zu", c->getOrderIds().size());
            }
            ImGui::EndTable();
        }
    }

} // namespace App::UI
```

- [ ] **Step 2: Implement CustomerDetailView.cpp (Editable Measurements Table)**

```cpp
// src/ui/CustomerDetailView.cpp
#include "ui/CustomerDetailView.hpp"
#include "ui/NeoWidgets.hpp"
#include "imgui.h"
#include <cstring>

namespace App::UI {

    void CustomerDetailView::render(
        Models::Customer* customer,
        Services::CustomerManager& customerMgr,
        Services::OrderManager& orderMgr,
        const std::function<void(Core::CustomerId)>& onNewOrderClick,
        const std::function<void(Core::OrderId)>& onEditOrderClick
    ) {
        if (!customer) {
            ImGui::TextDisabled("Select a customer from the left list to view details and measurements.");
            return;
        }

        // Customer Header Card
        NeoWidgets::BeginBrutalistCard("CustHeader", 0.0f, 110.0f, 0xFFFFFFFF);
        if (!m_isEditingBasicInfo) {
            ImGui::TextColored(ImVec4(0, 0, 0, 1), "CUSTOMER #%llu: %s", customer->getId(), customer->getName().c_str());
            ImGui::Text("PHONE: %s", customer->getPhone().c_str());
            ImGui::Text("ORDERS PLACED: %zu", customer->getOrderIds().size());

            if (NeoWidgets::BrutalistButton("EDIT INFO", 110.0f, 28.0f, 0xFFFFF000)) {
                m_isEditingBasicInfo = true;
                std::strncpy(m_editName, customer->getName().c_str(), sizeof(m_editName));
                std::strncpy(m_editPhone, customer->getPhone().c_str(), sizeof(m_editPhone));
            }
        } else {
            ImGui::InputText("Name", m_editName, sizeof(m_editName));
            ImGui::InputText("Phone", m_editPhone, sizeof(m_editPhone));
            if (NeoWidgets::BrutalistButton("SAVE", 80.0f, 26.0f, 0xFF9DFF00)) {
                customer->setName(m_editName);
                customer->setPhone(m_editPhone);
                customerMgr.updateCustomer(*customer);
                m_isEditingBasicInfo = false;
            }
            ImGui::SameLine();
            if (NeoWidgets::BrutalistButton("CANCEL", 80.0f, 26.0f)) {
                m_isEditingBasicInfo = false;
            }
        }
        NeoWidgets::EndBrutalistCard();

        ImGui::Spacing();

        // Editable Measurements Section
        ImGui::TextColored(ImVec4(0, 0, 0, 1), "MEASUREMENTS (DYNAMIC & EDITABLE)");
        if (ImGui::BeginTable("MeasTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
            ImGui::TableSetupColumn("Measurement Type", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed, 100.0f);
            ImGui::TableSetupColumn("Unit", ImGuiTableColumnFlags_WidthFixed, 60.0f);
            ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed, 80.0f);
            ImGui::TableHeadersRow();

            int removeIndex = -1;
            auto& measurements = customer->getMeasurements();

            for (size_t i = 0; i < measurements.size(); ++i) {
                ImGui::TableNextRow();
                ImGui::PushID(static_cast<int>(i));

                ImGui::TableSetColumnIndex(0);
                char keyBuf[64];
                std::strncpy(keyBuf, measurements[i].getKey().c_str(), sizeof(keyBuf));
                if (ImGui::InputText("##k", keyBuf, sizeof(keyBuf))) {
                    measurements[i].setKey(keyBuf);
                }

                ImGui::TableSetColumnIndex(1);
                char valBuf[64];
                std::strncpy(valBuf, measurements[i].getValue().c_str(), sizeof(valBuf));
                if (ImGui::InputText("##v", valBuf, sizeof(valBuf))) {
                    measurements[i].setValue(valBuf);
                }

                ImGui::TableSetColumnIndex(2);
                char unitBuf[32];
                std::strncpy(unitBuf, measurements[i].getUnit().c_str(), sizeof(unitBuf));
                if (ImGui::InputText("##u", unitBuf, sizeof(unitBuf))) {
                    measurements[i].setUnit(unitBuf);
                }

                ImGui::TableSetColumnIndex(3);
                if (ImGui::Button("DEL")) {
                    removeIndex = static_cast<int>(i);
                }

                ImGui::PopID();
            }

            if (removeIndex >= 0) {
                customer->removeMeasurement(static_cast<size_t>(removeIndex));
            }

            ImGui::EndTable();
        }

        // Add new measurement row
        ImGui::InputTextWithHint("##newK", "e.g., Inseam", m_newMeasKey, sizeof(m_newMeasKey));
        ImGui::SameLine();
        ImGui::InputTextWithHint("##newV", "e.g., 32", m_newMeasVal, sizeof(m_newMeasVal));
        ImGui::SameLine();
        ImGui::InputTextWithHint("##newU", "in", m_newMeasUnit, sizeof(m_newMeasUnit));
        ImGui::SameLine();
        if (NeoWidgets::BrutalistButton("+ ADD ROW", 100.0f, 30.0f, 0xFF00E6FF)) {
            if (m_newMeasKey[0] != '\0') {
                customer->addMeasurement({m_newMeasKey, m_newMeasVal, m_newMeasUnit});
                m_newMeasKey[0] = '\0';
                m_newMeasVal[0] = '\0';
                m_newMeasUnit[0] = '\0';
            }
        }

        ImGui::Spacing();
        ImGui::Separator();

        // Customer's Orders Section
        ImGui::TextColored(ImVec4(0, 0, 0, 1), "ORDERS PLACED BY THIS CUSTOMER");
        ImGui::SameLine();
        if (NeoWidgets::BrutalistButton("+ PLACE NEW ORDER", 170.0f, 28.0f, 0xFF9DFF00)) {
            onNewOrderClick(customer->getId());
        }

        auto orders = orderMgr.getOrdersForCustomer(customer->getId());
        if (orders.empty()) {
            ImGui::TextDisabled("No orders recorded for this customer.");
        } else {
            if (ImGui::BeginTable("CustOrdersTable", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                ImGui::TableSetupColumn("Order ID", ImGuiTableColumnFlags_WidthFixed, 80.0f);
                ImGui::TableSetupColumn("Date", ImGuiTableColumnFlags_WidthFixed, 140.0f);
                ImGui::TableSetupColumn("Total Amount", ImGuiTableColumnFlags_WidthFixed, 100.0f);
                ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthFixed, 110.0f);
                ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed, 70.0f);
                ImGui::TableHeadersRow();

                for (const auto* ord : orders) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("#%llu", ord->getId());

                    ImGui::TableSetColumnIndex(1);
                    ImGui::TextUnformatted(ord->getOrderDate().c_str());

                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("$%.2f", ord->getTotalAmount());

                    ImGui::TableSetColumnIndex(3);
                    ImGui::TextUnformatted(Core::OrderStatusToString(ord->getStatus()).c_str());

                    ImGui::TableSetColumnIndex(4);
                    ImGui::PushID(static_cast<int>(ord->getId()));
                    if (ImGui::Button("VIEW")) {
                        onEditOrderClick(ord->getId());
                    }
                    ImGui::PopID();
                }
                ImGui::EndTable();
            }
        }
    }

} // namespace App::UI
```

- [ ] **Step 3: Test compilation of Customer views**

Run:
```powershell
g++ -std=c++20 -c -Iinclude -Ithird_party/imgui src/ui/CustomerListView.cpp src/ui/CustomerDetailView.cpp
```
Expected: Compiles with 0 errors. Clean up temporary `.o` files.

- [ ] **Step 4: Commit**

```bash
git add src/ui/CustomerListView.cpp src/ui/CustomerDetailView.cpp
git commit -m "feat: implement CustomerListView and CustomerDetailView with dynamic measurements editor"
```

---

### Task 7: Order Management & Dashboard Header Views

**Files:**
- Create: `src/ui/OrderModalView.cpp`
- Create: `src/ui/DashboardView.cpp`

**Interfaces:**
- Consumes: `include/ui/OrderModalView.hpp`, `include/ui/DashboardView.hpp`, `AutoSaveCoordinator`, `OrderManager`, `ThemeManager`
- Produces: Modal dialog to create/edit orders with item line entries, and top dashboard header with live metrics and auto-save countdown pill.

- [ ] **Step 1: Implement OrderModalView.cpp**

```cpp
// src/ui/OrderModalView.cpp
#include "ui/OrderModalView.hpp"
#include "ui/NeoWidgets.hpp"
#include "imgui.h"
#include <ctime>
#include <iomanip>
#include <sstream>

namespace App::UI {

    void OrderModalView::openCreate(Core::CustomerId customerId, const std::string& customerName) {
        m_isOpen = true;
        m_isEditMode = false;
        m_orderId = 0;
        m_customerId = customerId;
        m_customerName = customerName;
        m_lineItems.clear();
        m_notes[0] = '\0';
        m_statusIndex = 0;

        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::tm tmNow{};
#if defined(_WIN32)
        localtime_s(&tmNow, &now);
#else
        localtime_r(&now, &tmNow);
#endif
        std::ostringstream ss;
        ss << std::put_time(&tmNow, "%Y-%m-%d %H:%M");
        std::strncpy(m_orderDate, ss.str().c_str(), sizeof(m_orderDate));
    }

    void OrderModalView::openEdit(const Models::Order& order) {
        m_isOpen = true;
        m_isEditMode = true;
        m_orderId = order.getId();
        m_customerId = order.getCustomerId();
        m_customerName = order.getCustomerName();
        m_lineItems = order.getItems();
        std::strncpy(m_orderDate, order.getOrderDate().c_str(), sizeof(m_orderDate));
        std::strncpy(m_notes, order.getNotes().c_str(), sizeof(m_notes));
        m_statusIndex = static_cast<int>(order.getStatus());
    }

    void OrderModalView::close() {
        m_isOpen = false;
    }

    void OrderModalView::render(
        Services::OrderManager& orderMgr,
        const std::function<void(const Models::Order&)>& onOrderSaved
    ) {
        if (!m_isOpen) return;

        ImGui::OpenPopup("OrderDialog");
        ImGui::SetNextWindowSize(ImVec2(600, 520), ImGuiCond_Appearing);

        if (ImGui::BeginPopupModal("OrderDialog", &m_isOpen, ImGuiWindowFlags_NoResize)) {
            ImGui::TextColored(ImVec4(0, 0, 0, 1), m_isEditMode ? "EDIT ORDER #%llu" : "CREATE NEW ORDER", m_orderId);
            ImGui::Text("Customer: %s (ID: #%llu)", m_customerName.c_str(), m_customerId);
            ImGui::InputText("Order Date", m_orderDate, sizeof(m_orderDate));

            const char* statuses[] = { "Pending", "In Progress", "Completed", "Delivered", "Cancelled" };
            ImGui::Combo("Order Status", &m_statusIndex, statuses, IM_ARRAYSIZE(statuses));

            ImGui::Separator();
            ImGui::TextUnformatted("ORDERED ITEMS:");

            if (ImGui::BeginTable("OrderItemsTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                ImGui::TableSetupColumn("Item Name", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Qty", ImGuiTableColumnFlags_WidthFixed, 50.0f);
                ImGui::TableSetupColumn("Price", ImGuiTableColumnFlags_WidthFixed, 80.0f);
                ImGui::TableSetupColumn("Total", ImGuiTableColumnFlags_WidthFixed, 80.0f);
                ImGui::TableHeadersRow();

                int deleteItemIdx = -1;
                for (size_t i = 0; i < m_lineItems.size(); ++i) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::TextUnformatted(m_lineItems[i].getItemName().c_str());

                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%d", m_lineItems[i].getQuantity());

                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("$%.2f", m_lineItems[i].getUnitPrice());

                    ImGui::TableSetColumnIndex(3);
                    ImGui::Text("$%.2f", m_lineItems[i].getTotalPrice());
                }
                ImGui::EndTable();
            }

            // New Line Item Row
            ImGui::InputTextWithHint("##ItemName", "Item description", m_newItemName, sizeof(m_newItemName));
            ImGui::SameLine();
            ImGui::SetNextItemWidth(60);
            ImGui::InputInt("##ItemQty", &m_newItemQty);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(80);
            ImGui::InputDouble("##ItemPrice", &m_newItemPrice, 0.0, 0.0, "%.2f");
            ImGui::SameLine();
            if (NeoWidgets::BrutalistButton("+ ADD ITEM", 100.0f, 28.0f, 0xFF00E6FF)) {
                if (m_newItemName[0] != '\0' && m_newItemQty > 0) {
                    m_lineItems.emplace_back(m_newItemName, m_newItemQty, m_newItemPrice);
                    m_newItemName[0] = '\0';
                    m_newItemQty = 1;
                    m_newItemPrice = 0.0;
                }
            }

            double grandTotal = 0.0;
            for (const auto& item : m_lineItems) grandTotal += item.getTotalPrice();
            ImGui::TextColored(ImVec4(0, 0, 0, 1), "ORDER GRAND TOTAL: $%.2f", grandTotal);

            ImGui::InputTextMultiline("Notes / Instructions", m_notes, sizeof(m_notes), ImVec2(-1, 50));

            ImGui::Spacing();
            if (NeoWidgets::BrutalistButton("SAVE ORDER", 140.0f, 36.0f, 0xFF9DFF00)) {
                if (m_isEditMode) {
                    auto opt = orderMgr.findByIdMut(m_orderId);
                    if (opt) {
                        auto& ord = opt->get();
                        ord.setOrderDate(m_orderDate);
                        ord.setStatus(static_cast<Core::OrderStatus>(m_statusIndex));
                        ord.setNotes(m_notes);
                        ord.setItems(m_lineItems);
                        onOrderSaved(ord);
                    }
                } else {
                    auto& newOrd = orderMgr.createOrder(
                        m_customerId, m_customerName, m_orderDate, m_lineItems,
                        static_cast<Core::OrderStatus>(m_statusIndex), m_notes
                    );
                    onOrderSaved(newOrd);
                }
                close();
            }

            ImGui::SameLine();
            if (NeoWidgets::BrutalistButton("CANCEL", 100.0f, 36.0f)) {
                close();
            }

            ImGui::EndPopup();
        }
    }

} // namespace App::UI
```

- [ ] **Step 2: Implement DashboardView.cpp**

```cpp
// src/ui/DashboardView.cpp
#include "ui/DashboardView.hpp"
#include "ui/NeoWidgets.hpp"
#include "imgui.h"

namespace App::UI {

    void DashboardView::render(
        const Services::DashboardMetrics& metrics,
        Storage::AutoSaveCoordinator& autoSave,
        ThemeManager& themeMgr,
        const std::function<void()>& onManualSaveClick
    ) {
        // Top Brutalist Brand Bar
        ImGui::TextColored(ImVec4(0, 0, 0, 1), "BRUTAL // OPS — CUSTOMER & ORDER MANAGEMENT");
        ImGui::SameLine();

        // Theme Toggle Button
        std::string themeLabel = themeMgr.isDark() ? "LIGHT MODE" : "DARK MODE";
        if (NeoWidgets::BrutalistButton(themeLabel.c_str(), 110.0f, 26.0f, 0xFFFFF000)) {
            themeMgr.toggleMode();
        }

        ImGui::SameLine();
        // Manual Save Button
        if (NeoWidgets::BrutalistButton("SAVE NOW (JSON/EXCEL)", 190.0f, 26.0f, 0xFF9DFF00)) {
            onManualSaveClick();
        }

        ImGui::SameLine();
        // Live Auto-Save Countdown Pill
        char autoSavePill[64];
        snprintf(autoSavePill, sizeof(autoSavePill), "AUTOSAVE IN %ds", autoSave.getSecondsRemaining());
        NeoWidgets::BrutalistBadge(autoSavePill, 0xFF00E6FF);

        ImGui::Spacing();

        // 4 KPI Stat Cards
        float availW = ImGui::GetContentRegionAvail().x;
        float cardW = (availW - 36.0f) / 4.0f;

        ImGui::Columns(4, "KpiColumns", false);
        ImGui::SetColumnWidth(0, cardW + 12.0f);
        ImGui::SetColumnWidth(1, cardW + 12.0f);
        ImGui::SetColumnWidth(2, cardW + 12.0f);
        ImGui::SetColumnWidth(3, cardW + 12.0f);

        NeoWidgets::StatCard("TOTAL CUSTOMERS", std::to_string(metrics.totalCustomers), 0xFF00E6FF, "// CUSTOMERS");
        ImGui::NextColumn();

        NeoWidgets::StatCard("ACTIVE ORDERS", std::to_string(metrics.activeOrders), 0xFFA76EFF, "// ORDERS");
        ImGui::NextColumn();

        char revBuf[64];
        snprintf(revBuf, sizeof(revBuf), "$%.2f", metrics.totalRevenue);
        NeoWidgets::StatCard("COMPLETED REVENUE", revBuf, 0xFF9DFF00, "// REVENUE");
        ImGui::NextColumn();

        char pendBuf[64];
        snprintf(pendBuf, sizeof(pendBuf), "$%.2f", metrics.pendingRevenue);
        NeoWidgets::StatCard("PENDING PIPELINE", pendBuf, 0xFFFFF000, "// PIPELINE");
        ImGui::Columns(1);

        ImGui::Spacing();
        ImGui::Separator();
    }

} // namespace App::UI
```

- [ ] **Step 3: Test compilation of UI modules**

Run:
```powershell
g++ -std=c++20 -c -Iinclude -Ithird_party/imgui src/ui/OrderModalView.cpp src/ui/DashboardView.cpp
```
Expected: Compiles with 0 errors.

- [ ] **Step 4: Commit**

```bash
git add src/ui/OrderModalView.cpp src/ui/DashboardView.cpp
git commit -m "feat: implement OrderModalView and DashboardView header components"
```

---

### Task 8: Application Entry Point, GLFW + ImGui Integration, Auto-Save Wiring & End-to-End Verification

**Files:**
- Create: `src/main.cpp`
- Modify: `CMakeLists.txt`
- Build & Run: Compile standalone binary `build/BrutalOps.exe`

**Interfaces:**
- Consumes: All `Services::*`, `Storage::*`, `UI::*`, GLFW window event loop.
- Produces: Complete, standalone, responsive desktop GUI application with instant Light/Dark switching, auto-saving every 60s to JSON & Excel, and on-close persistence.

- [ ] **Step 1: Implement src/main.cpp**

```cpp
// src/main.cpp
#include "core/Config.hpp"
#include "services/CustomerManager.hpp"
#include "services/OrderManager.hpp"
#include "services/DashboardStats.hpp"
#include "storage/JsonStorage.hpp"
#include "storage/ExcelStorage.hpp"
#include "storage/AutoSaveCoordinator.hpp"
#include "ui/ThemeManager.hpp"
#include "ui/DashboardView.hpp"
#include "ui/CustomerListView.hpp"
#include "ui/CustomerDetailView.hpp"
#include "ui/OrderModalView.hpp"
#include "ui/NeoWidgets.hpp"

#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>
#include <filesystem>

int main() {
    std::filesystem::create_directories("data");

    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    // GL 3.0 + GLSL 130
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(
        App::Core::AppConfig::DefaultWindowWidth,
        App::Core::AppConfig::DefaultWindowHeight,
        App::Core::AppConfig::AppName,
        nullptr, nullptr
    );
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Services & Storage
    App::Services::CustomerManager customerMgr;
    App::Services::OrderManager orderMgr;
    App::Storage::JsonStorage jsonStorage;
    App::Storage::ExcelStorage excelStorage;
    App::Storage::AutoSaveCoordinator autoSave(App::Core::AppConfig::DefaultAutoSaveIntervalSeconds);

    // Initial Load from JSON
    jsonStorage.loadCustomers(customerMgr.getAllCustomers(), App::Core::AppConfig::CustomersJsonPath);
    jsonStorage.loadOrders(orderMgr.getAllOrders(), App::Core::AppConfig::OrdersJsonPath);

    // Seed sample data if empty
    if (customerMgr.getAllCustomers().empty()) {
        auto& c1 = customerMgr.createCustomer("Marcus Sterling", "+1 202-555-0143");
        c1.addMeasurement({"Chest", "42", "in"});
        c1.addMeasurement({"Waist", "34", "in"});
        c1.addMeasurement({"Shoulder", "18.5", "in"});
        c1.addMeasurement({"Inseam", "32", "in"});

        auto& o1 = orderMgr.createOrder(
            c1.getId(), c1.getName(), "2026-11-13 14:30",
            { {"Bespoke Wool Suit", 1, 650.0}, {"Silk Necktie", 2, 45.0} },
            App::Core::OrderStatus::InProgress, "Hand-stitched lapels"
        );
        c1.addOrderId(o1.getId());

        auto& c2 = customerMgr.createCustomer("Elena Rostova", "+1 312-555-0188");
        c2.addMeasurement({"Bust", "36", "in"});
        c2.addMeasurement({"Waist", "28", "in"});
        c2.addMeasurement({"Hips", "38", "in"});
        auto& o2 = orderMgr.createOrder(
            c2.getId(), c2.getName(), "2026-11-12 11:15",
            { {"Cashmere Overcoat", 1, 820.0} },
            App::Core::OrderStatus::Completed, "Delivered via courier"
        );
        c2.addOrderId(o2.getId());
    }

    // Save Lambda
    auto performSave = [&]() {
        jsonStorage.saveCustomers(customerMgr.getAllCustomers(), App::Core::AppConfig::CustomersJsonPath);
        jsonStorage.saveOrders(orderMgr.getAllOrders(), App::Core::AppConfig::OrdersJsonPath);
        excelStorage.exportToSpreadsheet(
            customerMgr.getAllCustomers(),
            orderMgr.getAllOrders(),
            App::Core::AppConfig::ExcelCsvExportPath
        );
    };

    // UI Modules
    App::UI::ThemeManager themeMgr;
    themeMgr.applyStyle();

    App::UI::DashboardView dashboardView;
    App::UI::CustomerListView customerListView;
    App::UI::CustomerDetailView customerDetailView;
    App::UI::OrderModalView orderModalView;

    App::Core::CustomerId selectedCustomerId = customerMgr.getAllCustomers().empty() ? 0 : customerMgr.getAllCustomers().front().getId();
    bool showAddCustomerDialog = false;
    char newCustName[128] = "";
    char newCustPhone[64] = "";

    float lastTime = static_cast<float>(glfwGetTime());

    // Main Loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        float currentTime = static_cast<float>(glfwGetTime());
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // Auto-save update
        autoSave.update(deltaTime, performSave);

        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Fullscreen dockable background window
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGuiWindowFlags hostFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;

        ImGui::Begin("NeoBrutalistWorkspace", nullptr, hostFlags);

        // Dashboard Metrics Header
        auto metrics = App::Services::DashboardStats::compute(customerMgr, orderMgr);
        dashboardView.render(metrics, autoSave, themeMgr, [&]() {
            autoSave.forceSave(performSave);
        });

        // Main Layout Columns (Left: Customer List, Right: Details & Measurements)
        float contentW = ImGui::GetContentRegionAvail().x;
        float leftColW = 380.0f;
        float rightColW = contentW - leftColW - 16.0f;

        ImGui::Columns(2, "MainColumns", false);
        ImGui::SetColumnWidth(0, leftColW);
        ImGui::SetColumnWidth(1, rightColW);

        // Left Column: Customer List
        customerListView.render(
            customerMgr,
            selectedCustomerId,
            [&](App::Core::CustomerId id) { selectedCustomerId = id; },
            [&]() { showAddCustomerDialog = true; }
        );

        ImGui::NextColumn();

        // Right Column: Customer Details & Dynamic Measurements
        auto custOpt = customerMgr.findByIdMut(selectedCustomerId);
        App::Models::Customer* activeCustomer = custOpt ? &custOpt->get() : nullptr;

        customerDetailView.render(
            activeCustomer,
            customerMgr,
            orderMgr,
            [&](App::Core::CustomerId cId) {
                if (activeCustomer) orderModalView.openCreate(cId, activeCustomer->getName());
            },
            [&](App::Core::OrderId oId) {
                auto oOpt = orderMgr.findById(oId);
                if (oOpt) orderModalView.openEdit(oOpt->get());
            }
        );

        ImGui::Columns(1);

        // Add Customer Modal Dialog
        if (showAddCustomerDialog) {
            ImGui::OpenPopup("AddCustomerModal");
        }
        if (ImGui::BeginPopupModal("AddCustomerModal", &showAddCustomerDialog, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::TextColored(ImVec4(0, 0, 0, 1), "ADD NEW CUSTOMER");
            ImGui::InputText("Full Name", newCustName, sizeof(newCustName));
            ImGui::InputText("Phone Number", newCustPhone, sizeof(newCustPhone));

            if (App::UI::NeoWidgets::BrutalistButton("CREATE", 120.0f, 32.0f, 0xFF9DFF00)) {
                if (newCustName[0] != '\0') {
                    auto& created = customerMgr.createCustomer(newCustName, newCustPhone);
                    selectedCustomerId = created.getId();
                    newCustName[0] = '\0';
                    newCustPhone[0] = '\0';
                    showAddCustomerDialog = false;
                    performSave();
                }
            }
            ImGui::SameLine();
            if (App::UI::NeoWidgets::BrutalistButton("CANCEL", 90.0f, 32.0f)) {
                showAddCustomerDialog = false;
            }
            ImGui::EndPopup();
        }

        // Order Modal
        orderModalView.render(orderMgr, [&](const App::Models::Order& savedOrd) {
            if (activeCustomer) {
                activeCustomer->addOrderId(savedOrd.getId());
            }
            performSave();
        });

        ImGui::End();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.96f, 0.98f, 0.98f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Window Closing: Guaranteed Persistence Flush to JSON and Excel
    std::cout << "Flushing data to JSON and Excel before application exit...\n";
    performSave();

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
```

- [ ] **Step 2: Build the full desktop application with CMake**

Run:
```powershell
cmake -B build -G "MinGW Makefiles"
cmake --build build --config Release
```
Expected: Compiles cleanly into `build/BrutalOps.exe`.

- [ ] **Step 3: Run end-to-end launch verification**

Run:
```powershell
./build/BrutalOps.exe
```
Verify:
1. Standalone desktop window opens with high-contrast Neo-Brutalist styling.
2. 4 KPI stat cards display live metrics.
3. Customer list and search function properly.
4. Dynamic measurements table allows adding, modifying, and deleting measurement entries.
5. Create New Order modal calculates line item totals dynamically.
6. Auto-save countdown pill ticks from 60s and writes to `data/customers.json`, `data/orders.json`, and `data/records_export.csv`.
7. Closing window triggers final flush to disk.

- [ ] **Step 4: Final Git Commit**

```bash
git add src/ CMakeLists.txt docs/
git commit -m "feat: complete end-to-end Neo-Brutalist C++ Customer & Order Management desktop application"
```
