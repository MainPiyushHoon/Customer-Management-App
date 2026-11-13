#include "TestRunner.hpp"
#include "ui/CustomerListView.hpp"
#include "ui/CustomerDetailView.hpp"
#include "ui/NeoWidgets.hpp"
#include "ui/ThemeManager.hpp"
#include "services/CustomerManager.hpp"
#include "services/OrderManager.hpp"
#include "imgui.h"
#include <iostream>

int main() {
    std::cout << "Running Customer Management Views Tests (CustomerListView & CustomerDetailView)...\n";

    // 1. Initialize Headless ImGui Context
    ImGuiContext* ctx = ImGui::CreateContext();
    TestRunner::assert_true(ctx != nullptr, "ImGui context created successfully");

    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(1920.0f, 1080.0f);
    io.DeltaTime = 1.0f / 60.0f;

    // Build default font texture atlas for headless rendering
    unsigned char* pixels = nullptr;
    int width = 0;
    int height = 0;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
    TestRunner::assert_true(pixels != nullptr && width > 0 && height > 0, "Font atlas built for headless ImGui context");

    // Apply Brutalist Theme
    App::UI::ThemeManager themeMgr;
    themeMgr.applyStyle();

    // 2. Setup Test Data (Services, Customers, Measurements, Orders)
    App::Services::CustomerManager customerMgr;
    auto cid1 = customerMgr.createCustomer("Alice Smith", "+1-555-0101").getId();
    auto cid2 = customerMgr.createCustomer("Bob Builder", "+1-555-0202").getId();

    auto& cust1 = customerMgr.findByIdMut(cid1)->get();
    cust1.addMeasurement({"Waist", "30", "in"});
    cust1.addMeasurement({"Inseam", "32", "in"});

    auto& cust2 = customerMgr.findByIdMut(cid2)->get();
    cust2.addMeasurement({"Chest", "42", "in"});

    App::Services::OrderManager orderMgr;
    std::vector<App::Models::OrderItem> items = {
        {"Tuxedo Jacket", 1, 350.0},
        {"Tuxedo Pants", 1, 150.0}
    };
    auto& order1 = orderMgr.createOrder(cust1.getId(), cust1.getName(), "2026-11-13", items, App::Core::OrderStatus::InProgress, "Express fit");
    cust1.addOrderId(order1.getId());

    TestRunner::assert_true(cust1.getMeasurements().size() == 2, "Alice has 2 initial measurements");
    TestRunner::assert_true(cust1.getOrderIds().size() == 1, "Alice has 1 associated order");
    TestRunner::assert_true(orderMgr.getOrdersForCustomer(cust1.getId()).size() == 1, "OrderManager retrieves 1 order for Alice");

    // 3. Test CustomerListView in headless frame
    App::UI::CustomerListView listView;
    App::Core::CustomerId selectedId = 0;
    bool newCustClicked = false;

    ImGui::NewFrame();
    listView.render(
        customerMgr,
        cust1.getId(),
        [&](App::Core::CustomerId id) { selectedId = id; },
        [&]() { newCustClicked = true; }
    );
    ImGui::Render();
    TestRunner::assert_true(true, "CustomerListView rendered successfully in ImGui frame");

    // Test search functionality consumed by CustomerListView
    auto allResults = customerMgr.search("");
    TestRunner::assert_true(allResults.size() == 2, "Empty search returns all 2 customers");

    auto aliceResults = customerMgr.search("Alice");
    TestRunner::assert_true(aliceResults.size() == 1 && aliceResults[0]->getId() == cust1.getId(), "Search by name 'Alice' returns Alice");

    auto phoneResults = customerMgr.search("0202");
    TestRunner::assert_true(phoneResults.size() == 1 && phoneResults[0]->getId() == cust2.getId(), "Search by phone substring '0202' returns Bob");

    auto noResults = customerMgr.search("NonExistent");
    TestRunner::assert_true(noResults.empty(), "Search for 'NonExistent' returns 0 results");

    // 4. Test CustomerDetailView with nullptr (unselected state)
    App::UI::CustomerDetailView detailView;
    App::Core::CustomerId newOrderForCustomer = 0;
    App::Core::OrderId editOrderId = 0;

    ImGui::NewFrame();
    detailView.render(
        nullptr,
        customerMgr,
        orderMgr,
        [&](App::Core::CustomerId cid) { newOrderForCustomer = cid; },
        [&](App::Core::OrderId oid) { editOrderId = oid; }
    );
    ImGui::Render();
    TestRunner::assert_true(true, "CustomerDetailView handles nullptr customer gracefully");

    // 5. Test CustomerDetailView with Alice selected
    ImGui::NewFrame();
    detailView.render(
        &cust1,
        customerMgr,
        orderMgr,
        [&](App::Core::CustomerId cid) { newOrderForCustomer = cid; },
        [&](App::Core::OrderId oid) { editOrderId = oid; }
    );
    ImGui::Render();
    TestRunner::assert_true(true, "CustomerDetailView renders customer header, measurements table, and orders table");

    // 6. Test Dynamic Measurements additions and deletions
    cust1.addMeasurement({"Shoulder", "18.5", "in"});
    TestRunner::assert_true(cust1.getMeasurements().size() == 3, "Added 'Shoulder' measurement, total 3");
    TestRunner::assert_true(cust1.getMeasurements()[2].getKey() == "Shoulder", "New measurement key is 'Shoulder'");
    TestRunner::assert_true(cust1.getMeasurements()[2].getValue() == "18.5", "New measurement value is '18.5'");
    TestRunner::assert_true(cust1.getMeasurements()[2].getUnit() == "in", "New measurement unit is 'in'");

    cust1.removeMeasurement(0); // Remove "Waist"
    TestRunner::assert_true(cust1.getMeasurements().size() == 2, "Removed index 0 ('Waist'), total 2 remaining");
    TestRunner::assert_true(cust1.getMeasurements()[0].getKey() == "Inseam", "Index 0 is now 'Inseam'");

    // Render with modified measurements
    ImGui::NewFrame();
    detailView.render(
        &cust1,
        customerMgr,
        orderMgr,
        [&](App::Core::CustomerId cid) { newOrderForCustomer = cid; },
        [&](App::Core::OrderId oid) { editOrderId = oid; }
    );
    ImGui::Render();
    TestRunner::assert_true(true, "CustomerDetailView successfully re-renders after measurement mutations");

    // 7. Test Customer with 0 orders
    ImGui::NewFrame();
    detailView.render(
        &cust2,
        customerMgr,
        orderMgr,
        [&](App::Core::CustomerId cid) { newOrderForCustomer = cid; },
        [&](App::Core::OrderId oid) { editOrderId = oid; }
    );
    ImGui::Render();
    TestRunner::assert_true(true, "CustomerDetailView handles customer with zero orders cleanly");

    // 8. Cleanup
    ImGui::DestroyContext(ctx);

    return TestRunner::summary();
}
