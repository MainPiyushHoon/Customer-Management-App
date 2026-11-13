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
    TestRunner::assert_true(!c1.getCreatedAt().empty(), "Customer createdAt initialized on creation");
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
    TestRunner::assert_near(o1.getTotalAmount(), 200.0, "Order total is 200.0");

    auto customerOrders = orderMgr.getOrdersForCustomer(c1.getId());
    TestRunner::assert_true(customerOrders.size() == 1, "Customer has 1 order");

    auto inProgressOrders = orderMgr.filterByStatus(App::Core::OrderStatus::InProgress);
    TestRunner::assert_true(inProgressOrders.size() == 1, "1 order in progress");

    // Dashboard Stats
    auto metrics = App::Services::DashboardStats::compute(customerMgr, orderMgr);
    TestRunner::assert_true(metrics.totalCustomers == 1, "Dashboard shows 1 total customer");
    TestRunner::assert_true(metrics.activeOrders == 1, "Dashboard shows 1 active order");
    TestRunner::assert_near(metrics.pendingRevenue, 200.0, "Dashboard shows 200 pending revenue");

    // Deletion via std::erase_if
    bool ordDeleted = orderMgr.deleteOrder(o1.getId());
    TestRunner::assert_true(ordDeleted, "Order deleted via std::erase_if");
    TestRunner::assert_true(orderMgr.getAllOrders().empty(), "Orders list is empty after deletion");

    bool custDeleted = customerMgr.deleteCustomer(c1.getId());
    TestRunner::assert_true(custDeleted, "Customer deleted via std::erase_if");
    TestRunner::assert_true(customerMgr.getAllCustomers().empty(), "Customers list is empty after deletion");

    return TestRunner::summary();
}
