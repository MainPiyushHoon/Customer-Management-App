#include "TestRunner.hpp"
#include "ui/ThemeManager.hpp"

int main() {
    std::cout << "Running ThemeManager Tests...\n";
    App::UI::ThemeManager theme;
    TestRunner::assert_true(!theme.isDark(), "Default mode is Light");
    TestRunner::assert_true(theme.getMode() == App::UI::NeoThemeMode::Light, "Mode is NeoThemeMode::Light");
    TestRunner::assert_true(theme.getPalette().accentYellow != 0, "Yellow accent is defined");
    auto lightBg = theme.getPalette().background;

    theme.toggleMode();
    TestRunner::assert_true(theme.isDark(), "Mode toggled to Dark");
    TestRunner::assert_true(theme.getMode() == App::UI::NeoThemeMode::Dark, "Mode is NeoThemeMode::Dark");
    TestRunner::assert_true(theme.getPalette().background != lightBg, "Dark background is different from Light background");

    theme.setMode(App::UI::NeoThemeMode::Light);
    TestRunner::assert_true(!theme.isDark(), "Mode set to Light");
    TestRunner::assert_true(theme.getPalette().background == lightBg, "Background restored to lightBg");

    return TestRunner::summary();
}
