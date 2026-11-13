#include "TestRunner.hpp"
#include "ui/ThemeManager.hpp"
#include "imgui.h"

int main() {
    std::cout << "Running ThemeManager Tests...\n";

    // Initialize ImGui context
    ImGuiContext* ctx = ImGui::CreateContext();
    TestRunner::assert_true(ctx != nullptr, "ImGui context created");

    App::UI::ThemeManager theme;
    TestRunner::assert_true(!theme.isDark(), "Default mode is Light");
    TestRunner::assert_true(theme.getMode() == App::UI::NeoThemeMode::Light, "Mode is NeoThemeMode::Light");
    TestRunner::assert_true(theme.getPalette().accentYellow != 0, "Yellow accent is defined");
    auto lightBg = theme.getPalette().background;

    // Apply style and verify Brutalist geometry metrics
    theme.applyStyle();
    TestRunner::assert_true(ImGui::GetStyle().WindowRounding == 0.0f, "WindowRounding is 0.0f");
    TestRunner::assert_true(ImGui::GetStyle().ChildRounding == 0.0f, "ChildRounding is 0.0f");
    TestRunner::assert_true(ImGui::GetStyle().FrameRounding == 0.0f, "FrameRounding is 0.0f");
    TestRunner::assert_true(ImGui::GetStyle().PopupRounding == 0.0f, "PopupRounding is 0.0f");
    TestRunner::assert_true(ImGui::GetStyle().WindowBorderSize >= 2.5f, "WindowBorderSize >= 2.5f");
    TestRunner::assert_true(ImGui::GetStyle().ChildBorderSize >= 2.5f, "ChildBorderSize >= 2.5f");

    theme.toggleMode();
    TestRunner::assert_true(theme.isDark(), "Mode toggled to Dark");
    TestRunner::assert_true(theme.getMode() == App::UI::NeoThemeMode::Dark, "Mode is NeoThemeMode::Dark");
    TestRunner::assert_true(theme.getPalette().background != lightBg, "Dark background is different from Light background");

    theme.setMode(App::UI::NeoThemeMode::Light);
    TestRunner::assert_true(!theme.isDark(), "Mode set to Light");
    TestRunner::assert_true(theme.getPalette().background == lightBg, "Background restored to lightBg");

    ImGui::DestroyContext(ctx);

    return TestRunner::summary();
}
