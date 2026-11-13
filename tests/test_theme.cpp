#include "TestRunner.hpp"
#include "ui/ThemeManager.hpp"
#include "ui/NeoWidgets.hpp"
#include "imgui.h"

int main() {
    std::cout << "Running ThemeManager & NeoWidgets Tests...\n";

    // Initialize ImGui context
    ImGuiContext* ctx = ImGui::CreateContext();
    TestRunner::assert_true(ctx != nullptr, "ImGui context created");

    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(1920.0f, 1080.0f);
    io.DeltaTime = 1.0f / 60.0f;
    unsigned char* tex_pixels = nullptr;
    int tex_w = 0, tex_h = 0;
    io.Fonts->GetTexDataAsRGBA32(&tex_pixels, &tex_w, &tex_h);
    ImGui::NewFrame();

    App::UI::ThemeManager theme;
    TestRunner::assert_true(!theme.isDark(), "Default mode is Light");
    TestRunner::assert_true(theme.getMode() == App::UI::NeoThemeMode::Light, "Mode is NeoThemeMode::Light");
    TestRunner::assert_true(theme.getPalette().accentYellow != 0, "Yellow accent is defined");
    TestRunner::assert_true(((theme.getPalette().accentYellow & 0xFF) > 200), "Yellow R component is vibrant");

    // Verify Light Mode high-luminance palette (> 0.70 luminance for crisp black typography)
    const auto& lightP = theme.getPalette();
    TestRunner::assert_true(App::UI::ThemeManager::calculateLuminance(lightP.accentYellow) > 0.70f, "Light accentYellow lum > 0.70");
    TestRunner::assert_true(App::UI::ThemeManager::calculateLuminance(lightP.accentMint) > 0.70f, "Light accentMint lum > 0.70");
    TestRunner::assert_true(App::UI::ThemeManager::calculateLuminance(lightP.accentCyan) > 0.70f, "Light accentCyan lum > 0.70");
    TestRunner::assert_true(App::UI::ThemeManager::calculateLuminance(lightP.accentPink) > 0.70f, "Light accentPink lum > 0.70");
    TestRunner::assert_true(App::UI::ThemeManager::calculateLuminance(lightP.accentLavender) > 0.70f, "Light accentLavender lum > 0.70");
    TestRunner::assert_true(App::UI::ThemeManager::calculateLuminance(lightP.accentCoral) > 0.70f, "Light accentCoral lum > 0.70");

    auto lightBg = lightP.background;

    // Apply style and verify Brutalist geometry metrics
    theme.applyStyle();
    TestRunner::assert_true(ImGui::GetStyle().WindowRounding == 0.0f, "WindowRounding is 0.0f");
    TestRunner::assert_true(ImGui::GetStyle().ChildRounding == 0.0f, "ChildRounding is 0.0f");
    TestRunner::assert_true(ImGui::GetStyle().FrameRounding == 0.0f, "FrameRounding is 0.0f");
    TestRunner::assert_true(ImGui::GetStyle().PopupRounding == 0.0f, "PopupRounding is 0.0f");
    TestRunner::assert_true(ImGui::GetStyle().WindowBorderSize >= 2.5f, "WindowBorderSize >= 2.5f");
    TestRunner::assert_true(ImGui::GetStyle().ChildBorderSize >= 2.5f, "ChildBorderSize >= 2.5f");

    // Toggle to Dark Mode
    theme.toggleMode();
    TestRunner::assert_true(theme.isDark(), "Mode toggled to Dark");
    TestRunner::assert_true(theme.getMode() == App::UI::NeoThemeMode::Dark, "Mode is NeoThemeMode::Dark");
    TestRunner::assert_true(theme.getPalette().background != lightBg, "Dark background is different from Light background");
    TestRunner::assert_true(theme.getPalette().accentPink != 0xFF7A00FF, "Dark magenta removed from dark mode");
    TestRunner::assert_true(theme.getPalette().surfaceCard == 0xFF17181A, "Dark card surface is #1A1817");

    // Verify Dark Mode high-luminance palette (> 0.70 luminance for crisp black typography)
    const auto& darkP = theme.getPalette();
    TestRunner::assert_true(App::UI::ThemeManager::calculateLuminance(darkP.accentYellow) > 0.70f, "Dark accentYellow lum > 0.70");
    TestRunner::assert_true(App::UI::ThemeManager::calculateLuminance(darkP.accentMint) > 0.70f, "Dark accentMint lum > 0.70");
    TestRunner::assert_true(App::UI::ThemeManager::calculateLuminance(darkP.accentCyan) > 0.70f, "Dark accentCyan lum > 0.70");
    TestRunner::assert_true(App::UI::ThemeManager::calculateLuminance(darkP.accentPink) > 0.70f, "Dark accentPink lum > 0.70");
    TestRunner::assert_true(App::UI::ThemeManager::calculateLuminance(darkP.accentLavender) > 0.70f, "Dark accentLavender lum > 0.70");
    TestRunner::assert_true(App::UI::ThemeManager::calculateLuminance(darkP.accentCoral) > 0.70f, "Dark accentCoral lum > 0.70");

    // Revert to Light Mode
    theme.setMode(App::UI::NeoThemeMode::Light);
    TestRunner::assert_true(!theme.isDark(), "Mode set to Light");
    TestRunner::assert_true(theme.getPalette().background == lightBg, "Background restored to lightBg");

    // Anti-Overflow Dynamic Button Sizing Tests (NeoWidgets::CalculateButtonSize)
    const char* longLabel = ">> GO TO CUSTOMER MANAGEMENT";
    ImVec2 textSize = ImGui::CalcTextSize(longLabel);
    float expectedMinWidth = textSize.x + 32.0f;

    // Test 1: Requested width is smaller than text (overflow prevention)
    ImVec2 smallSize = App::UI::NeoWidgets::CalculateButtonSize(longLabel, 50.0f, 20.0f);
    TestRunner::assert_true(smallSize.x >= expectedMinWidth, "Button width expanded beyond requested 50px to fit long label");
    TestRunner::assert_true(smallSize.y >= 36.0f, "Button height clamped to minimum 36px");

    // Test 2: Requested width is 0 (auto-size)
    ImVec2 autoSize = App::UI::NeoWidgets::CalculateButtonSize(longLabel, 0.0f, 0.0f);
    TestRunner::assert_true(autoSize.x >= expectedMinWidth, "Auto-size width includes text + 32px padding");
    TestRunner::assert_true(autoSize.y == 36.0f, "Auto-size default height is 36px");

    // Test 3: Requested width is comfortably large (honors custom width)
    ImVec2 largeSize = App::UI::NeoWidgets::CalculateButtonSize("OK", 400.0f, 50.0f);
    TestRunner::assert_true(largeSize.x == 400.0f, "Honors requested large width 400px");
    TestRunner::assert_true(largeSize.y == 50.0f, "Honors requested large height 50px");

    ImGui::DestroyContext(ctx);

    return TestRunner::summary();
}
