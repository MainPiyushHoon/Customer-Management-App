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
            ImGui::TextUnformatted("ADD NEW CUSTOMER");
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
