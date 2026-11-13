// src/main.cpp
#include "core/Config.hpp"
#include "services/CustomerManager.hpp"
#include "services/OrderManager.hpp"
#include "services/DashboardStats.hpp"
#include "storage/JsonStorage.hpp"
#include "storage/ExcelStorage.hpp"
#include "storage/AutoSaveCoordinator.hpp"
#include "ui/ThemeManager.hpp"
#include "ui/AnimationManager.hpp"
#include "ui/DashboardView.hpp"
#include "ui/CustomerListView.hpp"
#include "ui/CustomerDetailView.hpp"
#include "ui/OrdersView.hpp"
#include "ui/OrderModalView.hpp"
#include "ui/DeleteOrderModalView.hpp"
#include "ui/NeoWidgets.hpp"

#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>
#include <filesystem>
#include <algorithm>

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

    // Load Bold Industrial Brutalist Fonts (Arial 18px body, Arial Bold 24px headers)
    ImFont* bodyFont = nullptr;
    ImFont* headerFont = nullptr;

    if (std::filesystem::exists("C:\\Windows\\Fonts\\arial.ttf")) {
        bodyFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\arial.ttf", 18.0f);
    }
    if (std::filesystem::exists("C:\\Windows\\Fonts\\arialbd.ttf")) {
        headerFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\arialbd.ttf", 24.0f);
    }
    if (!bodyFont) {
        bodyFont = io.Fonts->AddFontDefault();
    }
    if (!headerFont) {
        headerFont = bodyFont;
    }

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

    // Seed sample data if empty
    if (customerMgr.getAllCustomers().empty()) {
        auto& c1 = customerMgr.createCustomer("Marcus Sterling", "+1 202-555-0143");
        c1.setCreatedAt("2026-11-13 10:00");
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
        c2.setCreatedAt("2026-11-12 09:30");
        c2.addMeasurement({"Bust", "36", "in"});
        c2.addMeasurement({"Waist", "28", "in"});
        c2.addMeasurement({"Hips", "38", "in"});
        auto& o2 = orderMgr.createOrder(
            c2.getId(), c2.getName(), "2026-11-12 11:15",
            { {"Cashmere Overcoat", 1, 820.0} },
            App::Core::OrderStatus::Completed, "Delivered via courier"
        );
        c2.addOrderId(o2.getId());

        performSave();
    }

    // UI Modules
    App::UI::ThemeManager themeMgr;
    themeMgr.setBodyFont(bodyFont);
    themeMgr.setHeaderFont(headerFont);
    themeMgr.applyStyle();

    App::UI::ViewTransitionManager viewTransition;
    App::UI::ToastNotificationManager toastMgr;
    App::UI::DashboardView dashboardView;
    App::UI::CustomerListView customerListView;
    App::UI::CustomerDetailView customerDetailView;
    App::UI::OrdersView ordersView;
    App::UI::OrderModalView orderModalView;
    App::UI::DeleteOrderModalView deleteOrderModal;

    App::UI::ViewTab currentTab = App::UI::ViewTab::Dashboard;
    App::Core::CustomerId selectedCustomerId = customerMgr.getAllCustomers().empty() ? 0 : customerMgr.getAllCustomers().front().getId();
    bool showAddCustomerDialog = false;
    char newCustName[128] = "";
    char newCustPhone[64] = "";

    auto openDeleteModal = [&](App::Core::OrderId id) {
        auto optOrd = orderMgr.findById(id);
        if (optOrd) {
            deleteOrderModal.open(id, optOrd->get().getCustomerName(), optOrd->get().getTotalAmount());
        }
    };

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

        float dt = ImGui::GetIO().DeltaTime;
        viewTransition.update(dt);
        toastMgr.update(dt);

        // Fullscreen host window
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGuiWindowFlags hostFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;

        ImGui::Begin("NeoBrutalistWorkspace", nullptr, hostFlags);

        // 1. TOP NEO-BRUTALIST NAVBAR
        ImGui::BeginChild("TopNavbar", ImVec2(0, 56.0f), ImGuiChildFlags_Borders, ImGuiWindowFlags_NoScrollbar);

        // App Brand Header
        if (themeMgr.getHeaderFont()) ImGui::PushFont(themeMgr.getHeaderFont());
        ImGui::TextUnformatted("BRUTAL // OPS");
        if (themeMgr.getHeaderFont()) ImGui::PopFont();

        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 24.0f);

        // Navigation Tab Buttons
        bool isDash = (currentTab == App::UI::ViewTab::Dashboard);
        bool isCust = (currentTab == App::UI::ViewTab::Customers);
        bool isOrd = (currentTab == App::UI::ViewTab::Orders);

        if (App::UI::NeoWidgets::BrutalistButton("01 // DASHBOARD", 180.0f, 38.0f, isDash ? 0xFF00E6FF : 0)) {
            if (currentTab != App::UI::ViewTab::Dashboard) {
                currentTab = App::UI::ViewTab::Dashboard;
                viewTransition.triggerTransition(0.15f);
            }
        }
        ImGui::SameLine();
        if (App::UI::NeoWidgets::BrutalistButton("02 // CUSTOMERS", 180.0f, 38.0f, isCust ? 0xFF00E6FF : 0)) {
            if (currentTab != App::UI::ViewTab::Customers) {
                currentTab = App::UI::ViewTab::Customers;
                viewTransition.triggerTransition(0.15f);
            }
        }
        ImGui::SameLine();
        if (App::UI::NeoWidgets::BrutalistButton("03 // ORDERS", 160.0f, 38.0f, isOrd ? 0xFF00E6FF : 0)) {
            if (currentTab != App::UI::ViewTab::Orders) {
                currentTab = App::UI::ViewTab::Orders;
                viewTransition.triggerTransition(0.15f);
            }
        }

        // Right side of Navbar: Theme Toggle, Save Button, Auto-save Badge
        float rightControlsWidth = 460.0f;
        float availX = ImGui::GetWindowWidth();
        if (availX > rightControlsWidth + 600.0f) {
            ImGui::SameLine();
            ImGui::SetCursorPosX(availX - rightControlsWidth - 16.0f);
        } else {
            ImGui::SameLine();
        }

        // Theme Toggle
        std::string themeLabel = themeMgr.isDark() ? "LIGHT MODE" : "DARK MODE";
        if (App::UI::NeoWidgets::BrutalistButton(themeLabel.c_str(), 110.0f, 32.0f, 0xFFFFF000)) {
            themeMgr.toggleMode();
        }

        ImGui::SameLine();
        // Manual Save Button
        if (App::UI::NeoWidgets::BrutalistButton("SAVE NOW", 110.0f, 32.0f, 0xFF00FFA6)) {
            autoSave.forceSave(performSave);
            toastMgr.showToast("DATA SYNCHRONIZED TO DISK", 0xFF00E6FF);
        }

        ImGui::SameLine();
        // Auto-save countdown pill badge
        char autoSavePill[64];
        std::snprintf(autoSavePill, sizeof(autoSavePill), "AUTOSAVE: %ds", autoSave.getSecondsRemaining());
        App::UI::NeoWidgets::BrutalistBadge(autoSavePill, 0xFF00E6FF);

        ImGui::EndChild();

        ImGui::Spacing();

        // 2. ACTIVE VIEW CONTENT (BASED ON NAVBAR SELECTION)
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, viewTransition.getAlpha());

        if (currentTab == App::UI::ViewTab::Dashboard) {
            auto metrics = App::Services::DashboardStats::compute(customerMgr, orderMgr);
            dashboardView.render(
                metrics,
                orderMgr,
                [&]() { showAddCustomerDialog = true; },
                [&]() { orderModalView.openCreate(0, ""); },
                [&](App::UI::ViewTab targetTab) {
                    if (currentTab != targetTab) {
                        currentTab = targetTab;
                        viewTransition.triggerTransition(0.15f);
                    }
                },
                [&](App::Core::OrderId id) {
                    auto optOrd = orderMgr.findById(id);
                    if (optOrd) {
                        orderModalView.openEdit(optOrd->get());
                    }
                },
                [&]() {
                    autoSave.forceSave(performSave);
                    toastMgr.showToast("DATA SYNCHRONIZED TO DISK", 0xFF00E6FF);
                }
            );
        } else if (currentTab == App::UI::ViewTab::Customers) {
            float contentW = ImGui::GetContentRegionAvail().x;
            float leftColW = 420.0f;
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
                },
                [&]() {
                    // Instant save callback when measurements or customer info are saved
                    performSave();
                    toastMgr.showToast("MEASUREMENTS SAVED ✓", 0xFF00FFA6);
                },
                openDeleteModal
            );

            ImGui::Columns(1);
        } else if (currentTab == App::UI::ViewTab::Orders) {
            ordersView.render(
                customerMgr,
                orderMgr,
                [&](App::Core::OrderId oId) {
                    auto oOpt = orderMgr.findById(oId);
                    if (oOpt) orderModalView.openEdit(oOpt->get());
                },
                [&]() {
                    orderModalView.openCreate(0, "");
                },
                openDeleteModal
            );
        }

        ImGui::PopStyleVar();

        // Add Customer Modal Dialog
        if (showAddCustomerDialog) {
            ImGui::OpenPopup("AddCustomerModal");
        }
        if (ImGui::BeginPopupModal("AddCustomerModal", &showAddCustomerDialog, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::TextUnformatted("ADD NEW CUSTOMER");
            ImGui::InputText("Full Name", newCustName, sizeof(newCustName));
            ImGui::InputText("Phone Number", newCustPhone, sizeof(newCustPhone));

            if (App::UI::NeoWidgets::BrutalistButton("CREATE", 120.0f, 32.0f, 0xFF00FFA6)) {
                if (newCustName[0] != '\0') {
                    auto& created = customerMgr.createCustomer(newCustName, newCustPhone);
                    selectedCustomerId = created.getId();
                    newCustName[0] = '\0';
                    newCustPhone[0] = '\0';
                    showAddCustomerDialog = false;
                    performSave();
                    toastMgr.showToast("NEW CUSTOMER CREATED", 0xFF00E6FF);
                }
            }
            ImGui::SameLine();
            if (App::UI::NeoWidgets::BrutalistButton("CANCEL", 90.0f, 32.0f)) {
                showAddCustomerDialog = false;
            }
            ImGui::EndPopup();
        }

        // Order Modal
        orderModalView.render(
            orderMgr,
            [&](const App::Models::Order& savedOrd) {
                auto custOpt = customerMgr.findByIdMut(savedOrd.getCustomerId());
                bool isNewOrder = true;
                if (custOpt) {
                    auto& cust = custOpt->get();
                    const auto& orderIds = cust.getOrderIds();
                    if (std::find(orderIds.begin(), orderIds.end(), savedOrd.getId()) == orderIds.end()) {
                        cust.addOrderId(savedOrd.getId());
                        isNewOrder = true;
                    } else {
                        isNewOrder = false;
                    }
                }
                performSave();
                if (isNewOrder) {
                    toastMgr.showToast("NEW ORDER #" + std::to_string(savedOrd.getId()) + " CREATED", 0xFF00FFA6);
                } else {
                    toastMgr.showToast("ORDER #" + std::to_string(savedOrd.getId()) + " UPDATED", 0xFF00E6FF);
                }
            },
            &customerMgr.getAllCustomers()
        );

        // Delete Order Confirmation Modal
        deleteOrderModal.render([&](App::Core::OrderId id) {
            auto optOrd = orderMgr.findById(id);
            if (optOrd) {
                auto optCust = customerMgr.findByIdMut(optOrd->get().getCustomerId());
                if (optCust) {
                    static_cast<void>(optCust->get().removeOrderId(id));
                }
            }
            orderMgr.deleteOrder(id);
            performSave();
            toastMgr.showToast("ORDER #" + std::to_string(id) + " DELETED", 0xFFBCAEFF);
        });

        ImGui::End();

        // 3. RENDER ACTIVE TOAST NOTIFICATIONS OVERLAY
        toastMgr.render();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        if (themeMgr.isDark()) {
            glClearColor(0.08f, 0.07f, 0.06f, 1.0f);
        } else {
            glClearColor(0.96f, 0.98f, 0.98f, 1.0f);
        }
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
