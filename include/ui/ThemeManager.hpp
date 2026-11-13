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
        uint32_t accentLavender;
        uint32_t accentCoral;
        uint32_t inputBackground;
    };

    class ThemeManager {
    public:
        // High-Luminance Neo-Brutalist Accent Constants (> 0.70 luminance for crisp black typography)
        static constexpr uint32_t ElectricLemon = 0xFF00E6FF; // #FFE600 (lum: 0.828)
        static constexpr uint32_t PastelMint   = 0xFF00FFA6; // #A6FF00 (lum: 0.782)
        static constexpr uint32_t SkyIce       = 0xFFE5E7A0; // #A0E7E5 (lum: 0.822)
        static constexpr uint32_t SoftLavender = 0xFFFDB7D7; // #D7B7FD (lum: 0.786)
        static constexpr uint32_t SoftCoral    = 0xFFBCAEFF; // #FFAEBC (lum: 0.784)

        // Utility: Compute relative luminance (Rec. 601) of an ABGR (0xAABBGGRR) color
        [[nodiscard]] static float calculateLuminance(uint32_t color) {
            float r = (color & 0xFF) / 255.0f;
            float g = ((color >> 8) & 0xFF) / 255.0f;
            float b = ((color >> 16) & 0xFF) / 255.0f;
            return 0.299f * r + 0.587f * g + 0.114f * b;
        }

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
