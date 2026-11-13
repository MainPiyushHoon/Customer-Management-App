#include "ui/ThemeManager.hpp"
#include "imgui.h"

namespace App::UI {

    ThemeManager::ThemeManager() {
        // Light Neo-Brutalism Palette
        m_lightPalette = {
            .background       = 0xFFF8FDFD, // Cream/Off-white
            .surfaceCard      = 0xFFFFFFFF, // Pure White
            .border           = 0xFF000000, // Solid Black
            .shadow           = 0xFF000000, // Solid Black
            .textPrimary      = 0xFF000000, // Pitch Black
            .textSecondary    = 0xFF555555, // Charcoal
            .accentYellow     = 0xFF00E6FF, // Electric Yellow (#FFE600 ABGR: FF00E6FF)
            .accentPink       = 0xFFA76EFF, // Hot Pink (#FF6EA7)
            .accentCyan       = 0xFFFFF000, // Cyan (#00F0FF)
            .accentMint       = 0xFF9DFF00, // Mint (#00FF9D)
            .inputBackground  = 0xFFF0F0F0
        };

        // Cyber Dark Neo-Brutalism Palette
        m_darkPalette = {
            .background       = 0xFF141210, // Deep Charcoal
            .surfaceCard      = 0xFF1E1918, // Card Surface
            .border           = 0xFFE6E0E0, // Crisp White/Grey Border
            .shadow           = 0xFF000000, // Pitch Black Shadow
            .textPrimary      = 0xFFF7F5F5, // Crisp Off-White
            .textSecondary    = 0xFFAAAAAA, // Muted Grey
            .accentYellow     = 0xFF00E6FF, // Electric Yellow
            .accentPink       = 0xFF7A00FF, // Cyber Magenta (#FF007A)
            .accentCyan       = 0xFFFFE500, // Neon Cyan
            .accentMint       = 0xFF85FF00, // Neon Green (#00FF85)
            .inputBackground  = 0xFF2A2826
        };
    }

    void ThemeManager::setMode(NeoThemeMode mode) {
        m_currentMode = mode;
        applyStyle();
    }

    void ThemeManager::toggleMode() {
        setMode(m_currentMode == NeoThemeMode::Light ? NeoThemeMode::Dark : NeoThemeMode::Light);
    }

    const NeoPalette& ThemeManager::getPalette() const {
        return (m_currentMode == NeoThemeMode::Dark) ? m_darkPalette : m_lightPalette;
    }

    void ThemeManager::applyStyle() {
        if (!ImGui::GetCurrentContext()) {
            return;
        }

        ImGuiStyle& style = ImGui::GetStyle();

        // 1. Brutalist Geometry: 0 Rounding
        style.WindowRounding    = 0.0f;
        style.ChildRounding     = 0.0f;
        style.FrameRounding     = 0.0f;
        style.PopupRounding     = 0.0f;
        style.ScrollbarRounding = 0.0f;
        style.GrabRounding      = 0.0f;
        style.TabRounding       = 0.0f;

        // 2. Thick, crisp borders (2.5f to 3.0f)
        style.WindowBorderSize  = 3.0f;
        style.ChildBorderSize   = 2.5f;
        style.PopupBorderSize   = 3.0f;
        style.FrameBorderSize   = 2.0f;
        style.TabBorderSize     = 2.0f;

        // 3. Generous padding
        style.WindowPadding     = ImVec2(16.0f, 16.0f);
        style.FramePadding      = ImVec2(12.0f, 8.0f);
        style.ItemSpacing       = ImVec2(12.0f, 10.0f);
        style.ItemInnerSpacing  = ImVec2(8.0f, 6.0f);

        // 4. Color Assignment
        const auto& p = getPalette();
        ImVec4* colors = style.Colors;

        colors[ImGuiCol_WindowBg]          = ImGui::ColorConvertU32ToFloat4(p.background);
        colors[ImGuiCol_ChildBg]           = ImGui::ColorConvertU32ToFloat4(p.surfaceCard);
        colors[ImGuiCol_PopupBg]           = ImGui::ColorConvertU32ToFloat4(p.surfaceCard);
        colors[ImGuiCol_Border]            = ImGui::ColorConvertU32ToFloat4(p.border);
        colors[ImGuiCol_BorderShadow]      = ImGui::ColorConvertU32ToFloat4(p.shadow);
        colors[ImGuiCol_Text]              = ImGui::ColorConvertU32ToFloat4(p.textPrimary);
        colors[ImGuiCol_TextDisabled]      = ImGui::ColorConvertU32ToFloat4(p.textSecondary);
        colors[ImGuiCol_FrameBg]           = ImGui::ColorConvertU32ToFloat4(p.inputBackground);
        colors[ImGuiCol_FrameBgHovered]    = ImGui::ColorConvertU32ToFloat4(p.accentYellow);
        colors[ImGuiCol_Button]            = ImGui::ColorConvertU32ToFloat4(p.accentYellow);
        colors[ImGuiCol_ButtonHovered]     = ImGui::ColorConvertU32ToFloat4(p.accentMint);
        colors[ImGuiCol_ButtonActive]      = ImGui::ColorConvertU32ToFloat4(p.accentPink);
        colors[ImGuiCol_Header]            = ImGui::ColorConvertU32ToFloat4(p.accentYellow);
        colors[ImGuiCol_HeaderHovered]     = ImGui::ColorConvertU32ToFloat4(p.accentCyan);
        colors[ImGuiCol_HeaderActive]      = ImGui::ColorConvertU32ToFloat4(p.accentPink);
    }

} // namespace App::UI
