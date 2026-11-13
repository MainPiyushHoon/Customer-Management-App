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
