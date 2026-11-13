#pragma once

#include <cstdint>

struct ImFont;

namespace App::UI {

    enum class NeoThemeMode {
        Light,
        Dark
    };

    enum class ViewTab {
        Dashboard,
        Customers,
        Orders
    };

    struct NeoPalette {
        uint32_t background;
        uint32_t surfaceCard;
        uint32_t border;
        uint32_t shadow;
        uint32_t textPrimary;
        uint32_t textSecondary;
        uint32_t accentYellow;
        uint32_t accentPink;
        uint32_t accentCyan;
        uint32_t accentMint;
        uint32_t inputBackground;
    };

    class ThemeManager {
    public:
        ThemeManager();

        void setMode(NeoThemeMode mode);
        void toggleMode();

        [[nodiscard]] NeoThemeMode getMode() const { return m_currentMode; }
        [[nodiscard]] bool isDark() const { return m_currentMode == NeoThemeMode::Dark; }
        [[nodiscard]] const NeoPalette& getPalette() const;

        // Applies Dear ImGui style colors and geometric metrics
        void applyStyle();

        // Typography accessors
        void setBodyFont(ImFont* font) { m_bodyFont = font; }
        void setHeaderFont(ImFont* font) { m_headerFont = font; }
        [[nodiscard]] ImFont* getBodyFont() const { return m_bodyFont; }
        [[nodiscard]] ImFont* getHeaderFont() const { return m_headerFont; }

    private:
        NeoThemeMode m_currentMode = NeoThemeMode::Light;
        NeoPalette m_lightPalette;
        NeoPalette m_darkPalette;
        ImFont* m_bodyFont = nullptr;
        ImFont* m_headerFont = nullptr;
    };

} // namespace App::UI
