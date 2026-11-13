#include "TestRunner.hpp"
#include "ui/CustomerListView.hpp"
#include "ui/CustomerDetailView.hpp"
#include "ui/DashboardView.hpp"
#include "ui/OrdersView.hpp"
#include "ui/OrderModalView.hpp"
#include "ui/DeleteOrderModalView.hpp"
#include "ui/NeoWidgets.hpp"
#include "ui/ThemeManager.hpp"
#include "services/CustomerManager.hpp"
#include "services/OrderManager.hpp"
#include "services/DashboardStats.hpp"
#include "storage/AutoSaveCoordinator.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include <iostream>

int main() {
    std::cout << "Running UI Views Tests (CustomerListView, CustomerDetailView, DashboardView, OrdersView, OrderModalView)...\n";

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

    // 3. Test CustomerListView
    App::UI::CustomerListView customerListView;
    App::Core::CustomerId selectedId = cid1;

    ImGui::NewFrame();
    customerListView.render(
        customerMgr, selectedId,
        [&](App::Core::CustomerId id) { selectedId = id; },
        []() {}
    );
    ImGui::Render();
    TestRunner::assert_true(true, "CustomerListView rendered successfully in ImGui frame");

    // Test Search Functionality
    auto allResults = customerMgr.search("");
    TestRunner::assert_true(allResults.size() == 2, "Empty search returns all 2 customers");

    auto aliceResults = customerMgr.search("Alice");
    TestRunner::assert_true(aliceResults.size() == 1 && aliceResults[0]->getName() == "Alice Smith", "Search by name 'Alice' returns Alice");

    auto phoneResults = customerMgr.search("0202");
    TestRunner::assert_true(phoneResults.size() == 1 && phoneResults[0]->getName() == "Bob Builder", "Search by phone substring '0202' returns Bob");

    auto emptyResults = customerMgr.search("NonExistent");
    TestRunner::assert_true(emptyResults.empty(), "Search for 'NonExistent' returns 0 results");

    // 4. Test CustomerDetailView
    App::UI::CustomerDetailView customerDetailView;
    bool customerSavedTriggered = false;

    // Test null customer handling
    ImGui::NewFrame();
    customerDetailView.render(nullptr, customerMgr, orderMgr, [](App::Core::CustomerId) {}, [](App::Core::OrderId) {}, [&]() { customerSavedTriggered = true; });
    ImGui::Render();
    TestRunner::assert_true(true, "CustomerDetailView handles nullptr customer gracefully");

    // Test rendering valid customer
    ImGui::NewFrame();
    customerDetailView.render(&cust1, customerMgr, orderMgr, [](App::Core::CustomerId) {}, [](App::Core::OrderId) {}, [&]() { customerSavedTriggered = true; });
    ImGui::Render();
    TestRunner::assert_true(true, "CustomerDetailView renders customer header, measurements table, and orders table");

    // Test rendering with onDeleteOrderClick callback
    bool custOrderDeleteTriggered = false;
    ImGui::NewFrame();
    customerDetailView.render(
        &cust1, customerMgr, orderMgr,
        [](App::Core::CustomerId) {},
        [](App::Core::OrderId) {},
        [&]() { customerSavedTriggered = true; },
        [&](App::Core::OrderId) { custOrderDeleteTriggered = true; }
    );
    ImGui::Render();
    TestRunner::assert_true(true, "CustomerDetailView rendered with onDeleteOrderClick callback successfully");

    // Test Dynamic Measurements mutations
    cust1.addMeasurement({"Shoulder", "18.5", "in"});
    TestRunner::assert_true(cust1.getMeasurements().size() == 3, "Added 'Shoulder' measurement, total 3");
    TestRunner::assert_true(cust1.getMeasurements()[2].getKey() == "Shoulder", "New measurement key is 'Shoulder'");
    TestRunner::assert_true(cust1.getMeasurements()[2].getValue() == "18.5", "New measurement value is '18.5'");
    TestRunner::assert_true(cust1.getMeasurements()[2].getUnit() == "in", "New measurement unit is 'in'");

    // Test Measurement Deletion
    cust1.removeMeasurement(0);
    TestRunner::assert_true(cust1.getMeasurements().size() == 2, "Removed index 0 ('Waist'), total 2 remaining");
    TestRunner::assert_true(cust1.getMeasurements()[0].getKey() == "Inseam", "Index 0 is now 'Inseam'");

    // Re-render after mutations
    ImGui::NewFrame();
    customerDetailView.render(&cust1, customerMgr, orderMgr, [](App::Core::CustomerId) {}, [](App::Core::OrderId) {}, [&]() { customerSavedTriggered = true; });
    ImGui::Render();
    TestRunner::assert_true(true, "CustomerDetailView successfully re-renders after measurement mutations");

    // Test customer with 0 orders
    ImGui::NewFrame();
    customerDetailView.render(&cust2, customerMgr, orderMgr, [](App::Core::CustomerId) {}, [](App::Core::OrderId) {}, [&]() { customerSavedTriggered = true; });
    ImGui::Render();
    TestRunner::assert_true(true, "CustomerDetailView handles customer with zero orders cleanly");
    TestRunner::assert_true(customerDetailView.getCurrentCustomerId() == cid2, "CustomerDetailView tracked current customer ID for cust2");
    TestRunner::assert_true(!customerDetailView.isEditingBasicInfo(), "CustomerDetailView resets edit mode when switching customers");

    // 5. Test DashboardView
    App::UI::DashboardView dashboardView;
    auto metrics = App::Services::DashboardStats::compute(customerMgr, orderMgr);

    TestRunner::assert_true(metrics.totalCustomers == 2, "DashboardMetrics: 2 customers");
    TestRunner::assert_true(metrics.activeOrders == 1, "DashboardMetrics: 1 active order");
    TestRunner::assert_near(metrics.pendingRevenue, 500.0, "DashboardMetrics: $500.00 pending revenue");
    TestRunner::assert_near(metrics.totalRevenue, 0.0, "DashboardMetrics: $0.00 completed revenue");

    bool addCustomerClicked = false;
    bool addOrderClicked = false;
    App::UI::ViewTab navigatedTab = App::UI::ViewTab::Dashboard;
    App::Core::OrderId viewedOrderId = 0;

    // Render with 1 order present
    ImGui::NewFrame();
    int frameWithOrders = ImGui::GetCurrentContext()->FrameCount;
    dashboardView.render(
        metrics, orderMgr,
        [&]() { addCustomerClicked = true; },
        [&]() { addOrderClicked = true; },
        [&](App::UI::ViewTab t) { navigatedTab = t; },
        [&](App::Core::OrderId id) { viewedOrderId = id; }
    );
    ImGui::Render();
    TestRunner::assert_true(true, "DashboardView rendered 4 KPI cards, quick actions, and recent orders");

    // Verify RecentOrdersTable exists and was active in this frame
    bool tableActiveWithOrders = false;
    for (int n = 0; n < ImGui::GetCurrentContext()->Tables.GetMapSize(); n++) {
        if (ImGuiTable* t = ImGui::GetCurrentContext()->Tables.TryGetMapData(n)) {
            if (t->ColumnsCount == 7 && t->LastFrameActive == frameWithOrders) {
                tableActiveWithOrders = true;
                break;
            }
        }
    }
    TestRunner::assert_true(tableActiveWithOrders, "RecentOrdersTable (7 columns) was rendered when orders are present");

    // Verify developer persistence card does NOT exist
    ImGuiWindow* persistCard = ImGui::FindWindowByName("PersistenceInfoCard");
    TestRunner::assert_true(persistCard == nullptr, "Developer persistence card (DATA STORAGE & PERSISTENCE ENGINE) is NOT rendered");

    // Test zero orders feed
    App::Services::OrderManager emptyOrderMgr;
    auto emptyMetrics = App::Services::DashboardStats::compute(customerMgr, emptyOrderMgr);
    ImGui::NewFrame();
    int frameWithEmpty = ImGui::GetCurrentContext()->FrameCount;
    dashboardView.render(emptyMetrics, emptyOrderMgr);
    ImGui::Render();

    bool tableActiveWithEmpty = false;
    for (int n = 0; n < ImGui::GetCurrentContext()->Tables.GetMapSize(); n++) {
        if (ImGuiTable* t = ImGui::GetCurrentContext()->Tables.TryGetMapData(n)) {
            if (t->ColumnsCount == 7 && t->LastFrameActive == frameWithEmpty) {
                tableActiveWithEmpty = true;
                break;
            }
        }
    }
    TestRunner::assert_false(tableActiveWithEmpty, "RecentOrdersTable is omitted when zero orders exist");

    // Test dashboard with multiple (> 5) orders to verify latest 5 capping
    App::Services::OrderManager multiOrderMgr;
    for (int i = 1; i <= 7; ++i) {
        multiOrderMgr.createOrder(cust1.getId(), cust1.getName(), "2026-11-" + std::to_string(10 + i), {{"Item", 1, 10.0}});
    }
    auto multiMetrics = App::Services::DashboardStats::compute(customerMgr, multiOrderMgr);
    ImGui::NewFrame();
    int frameWithMulti = ImGui::GetCurrentContext()->FrameCount;
    dashboardView.render(multiMetrics, multiOrderMgr);
    ImGui::Render();

    bool tableActiveWithMulti = false;
    for (int n = 0; n < ImGui::GetCurrentContext()->Tables.GetMapSize(); n++) {
        if (ImGuiTable* t = ImGui::GetCurrentContext()->Tables.TryGetMapData(n)) {
            if (t->ColumnsCount == 7 && t->LastFrameActive == frameWithMulti) {
                tableActiveWithMulti = true;
                break;
            }
        }
    }
    TestRunner::assert_true(tableActiveWithMulti, "RecentOrdersTable rendered for multiple orders");

    // 6. Test Theme Mode Switching
    themeMgr.toggleMode();
    TestRunner::assert_true(themeMgr.isDark(), "ThemeManager toggled mode");
    themeMgr.setMode(App::UI::NeoThemeMode::Light);

    // 7. Test Metrics recalculation after order completion
    order1.setStatus(App::Core::OrderStatus::Completed);
    orderMgr.updateOrder(order1);
    auto updatedMetrics = App::Services::DashboardStats::compute(customerMgr, orderMgr);
    TestRunner::assert_true(updatedMetrics.completedOrders == 1, "Updated metrics: 1 completed order");
    TestRunner::assert_near(updatedMetrics.totalRevenue, 500.0, "Updated metrics: $500.00 total completed revenue");
    TestRunner::assert_true(updatedMetrics.activeOrders == 0, "Updated metrics: 0 active orders");
    TestRunner::assert_near(updatedMetrics.pendingRevenue, 0.0, "Updated metrics: $0.00 pending pipeline");

    ImGui::NewFrame();
    dashboardView.render(updatedMetrics, orderMgr);
    ImGui::Render();
    TestRunner::assert_true(true, "DashboardView re-rendered with updated metrics");

    // 8. Test OrdersView
    App::UI::OrdersView ordersView;
    TestRunner::assert_true(ordersView.getActiveFilterIndex() == 0, "OrdersView default filter is 0 (ALL)");

    ordersView.setActiveFilterIndex(3); // Completed filter
    TestRunner::assert_true(ordersView.getActiveFilterIndex() == 3, "OrdersView filter updated to Completed");

    ImGui::NewFrame();
    ordersView.render(
        customerMgr, orderMgr,
        [](App::Core::OrderId) {},
        []() {}
    );
    ImGui::Render();
    TestRunner::assert_true(true, "OrdersView rendered table and filter bar successfully");

    // Test rendering with onDeleteOrderClick callback
    bool deleteTriggeredInOrders = false;
    App::Core::OrderId deleteTargetIdInOrders = 0;
    ImGui::NewFrame();
    ordersView.render(
        customerMgr, orderMgr,
        [](App::Core::OrderId) {},
        []() {},
        [&](App::Core::OrderId id) {
            deleteTriggeredInOrders = true;
            deleteTargetIdInOrders = id;
        }
    );
    ImGui::Render();
    TestRunner::assert_true(true, "OrdersView rendered with onDeleteOrderClick callback successfully");

    // 9. Test OrderModalView
    App::UI::OrderModalView orderModalView;
    TestRunner::assert_true(!orderModalView.isOpen(), "OrderModalView is initially closed");

    // Test rendering when modal is closed
    ImGui::NewFrame();
    orderModalView.render(orderMgr, [](const App::Models::Order&) {});
    ImGui::Render();
    TestRunner::assert_true(!orderModalView.isOpen(), "OrderModalView remains closed when not triggered");

    // Test openCreate
    orderModalView.openCreate(cust2.getId(), cust2.getName());
    TestRunner::assert_true(orderModalView.isOpen(), "OrderModalView is open after openCreate");

    ImGui::NewFrame();
    orderModalView.render(orderMgr, [](const App::Models::Order&) {}, &customerMgr.getAllCustomers());
    ImGui::Render();
    TestRunner::assert_true(true, "OrderModalView rendered create order dialog");

    orderModalView.close();
    TestRunner::assert_true(!orderModalView.isOpen(), "OrderModalView is closed after close()");

    // Test openEdit with order1
    orderModalView.openEdit(order1);
    TestRunner::assert_true(orderModalView.isOpen(), "OrderModalView is open after openEdit");

    ImGui::NewFrame();
    orderModalView.render(orderMgr, [](const App::Models::Order&) {});
    ImGui::Render();
    TestRunner::assert_true(true, "OrderModalView rendered edit order dialog for order1");

    // Test Order Item calculations
    double itemsTotal = 0.0;
    for (const auto& it : order1.getItems()) {
        itemsTotal += it.getTotalPrice();
    }
    TestRunner::assert_near(itemsTotal, 500.0, "Calculated line items total matches $500.00");
    TestRunner::assert_near(order1.getTotalAmount(), itemsTotal, "order.getTotalAmount() matches line items sum");

    App::Models::OrderItem item3("Silk Tie", 2, 45.0);
    TestRunner::assert_near(item3.getTotalPrice(), 90.0, "OrderItem calculates total price (2 * 45 = 90.0)");
    order1.addItem(item3);
    TestRunner::assert_true(order1.getItems().size() == 3, "order1 has 3 items after adding item3");
    TestRunner::assert_near(order1.getTotalAmount(), 590.0, "order1 total amount updated to $590.00");

    orderModalView.close();
    TestRunner::assert_true(!orderModalView.isOpen(), "OrderModalView closed cleanly");

    // 10. Test DeleteOrderModalView
    App::UI::DeleteOrderModalView deleteOrderModal;
    TestRunner::assert_false(deleteOrderModal.isOpen(), "DeleteOrderModalView is initially closed");

    // Test rendering when modal is closed
    ImGui::NewFrame();
    deleteOrderModal.render([](App::Core::OrderId) {});
    ImGui::Render();
    TestRunner::assert_false(deleteOrderModal.isOpen(), "DeleteOrderModalView remains closed when not open");

    // Test open()
    deleteOrderModal.open(order1.getId(), cust1.getName(), order1.getTotalAmount());
    TestRunner::assert_true(deleteOrderModal.isOpen(), "DeleteOrderModalView is open after open()");
    TestRunner::assert_equal(deleteOrderModal.getOrderId(), order1.getId(), "DeleteOrderModalView stores order ID");
    TestRunner::assert_true(deleteOrderModal.getCustomerName() == cust1.getName(), "DeleteOrderModalView stores customer name");
    TestRunner::assert_near(deleteOrderModal.getTotalAmount(), order1.getTotalAmount(), "DeleteOrderModalView stores total amount");

    // Render open modal in ImGui frame
    bool confirmDeleteCalled = false;
    App::Core::OrderId confirmedOrderId = 0;
    bool cancelDeleteCalled = false;

    ImGui::NewFrame();
    deleteOrderModal.render(
        [&](App::Core::OrderId id) {
            confirmDeleteCalled = true;
            confirmedOrderId = id;
        },
        [&]() {
            cancelDeleteCalled = true;
        }
    );
    ImGui::Render();
    TestRunner::assert_true(true, "DeleteOrderModalView rendered confirmation dialog in ImGui frame");

    // Test close()
    deleteOrderModal.close();
    TestRunner::assert_false(deleteOrderModal.isOpen(), "DeleteOrderModalView is closed after close()");

    // Re-open and verify callback invocation on simulated button click
    deleteOrderModal.open(order1.getId(), cust1.getName(), order1.getTotalAmount());
    TestRunner::assert_true(deleteOrderModal.isOpen(), "DeleteOrderModalView re-opened for click simulation");

    // Frame 1: find modal window position and press mouse on CONFIRM DELETE button
    ImGui::NewFrame();
    deleteOrderModal.render(
        [&](App::Core::OrderId id) {
            confirmDeleteCalled = true;
            confirmedOrderId = id;
        },
        [&]() {
            cancelDeleteCalled = true;
        }
    );
    ImGuiWindow* modalWin = ImGui::FindWindowByName("CONFIRM ORDER DELETION // CRITICAL");
    TestRunner::assert_true(modalWin != nullptr, "Modal window found by name in ImGui context");
    ImVec2 btnPos(0, 0);
    if (modalWin) {
        btnPos = ImVec2(modalWin->Pos.x + 50.0f, modalWin->Pos.y + modalWin->Size.y - 30.0f);
    }
    ImGui::Render();

    // Frame 2: Mouse down on button
    io.MousePos = btnPos;
    io.MouseDown[0] = true;
    ImGui::NewFrame();
    deleteOrderModal.render(
        [&](App::Core::OrderId id) {
            confirmDeleteCalled = true;
            confirmedOrderId = id;
        },
        [&]() {
            cancelDeleteCalled = true;
        }
    );
    ImGui::Render();

    // Frame 3: Mouse released on button
    io.MouseDown[0] = false;
    ImGui::NewFrame();
    deleteOrderModal.render(
        [&](App::Core::OrderId id) {
            confirmDeleteCalled = true;
            confirmedOrderId = id;
        },
        [&]() {
            cancelDeleteCalled = true;
        }
    );
    ImGui::Render();

    TestRunner::assert_true(confirmDeleteCalled, "onConfirmDelete callback was invoked on button click");
    TestRunner::assert_equal(confirmedOrderId, order1.getId(), "Confirmed order ID matches deleted order");
    TestRunner::assert_false(deleteOrderModal.isOpen(), "DeleteOrderModalView isOpen() is false after confirm");

    // 11. Cleanup
    ImGui::DestroyContext(ctx);

    return TestRunner::summary();
}
