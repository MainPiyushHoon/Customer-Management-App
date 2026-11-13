#include "ui/AnimationManager.hpp"
#include "imgui.h"
#include <algorithm>

namespace App::UI {

    // ==========================================
    // ViewTransitionManager
    // ==========================================

    void ViewTransitionManager::triggerTransition(float duration) {
        m_duration = (duration > 0.0f) ? duration : 0.15f;
        m_elapsed = 0.0f;
        m_alpha = 0.0f;
        m_isFinished = false;
    }

    void ViewTransitionManager::update(float dt) {
        if (m_isFinished) {
            return;
        }

        if (dt < 0.0f) {
            dt = 0.0f;
        }

        m_elapsed += dt;
        if (m_elapsed >= m_duration) {
            m_elapsed = m_duration;
            m_alpha = 1.0f;
            m_isFinished = true;
        } else {
            m_alpha = std::clamp(m_elapsed / m_duration, 0.0f, 1.0f);
        }
    }

    void ViewTransitionManager::reset() {
        m_elapsed = m_duration;
        m_alpha = 1.0f;
        m_isFinished = true;
    }

    // ==========================================
    // ToastNotificationManager
    // ==========================================

    void ToastNotificationManager::showToast(const std::string& message, uint32_t bgColor, float duration) {
        if (m_toasts.size() >= 5) {
            m_toasts.erase(m_toasts.begin());
        }

        ToastNotification toast;
        toast.message = message;
        toast.bgColor = bgColor;
        toast.duration = std::max(duration, 1.0f);
        toast.elapsed = 0.0f;
        toast.alpha = 0.0f;
        toast.yOffset = -50.0f;
        m_toasts.push_back(std::move(toast));
    }

    void ToastNotificationManager::update(float dt) {
        if (dt < 0.0f) {
            dt = 0.0f;
        }

        constexpr float slideInTime = 0.25f;
        constexpr float fadeOutTime = 0.50f;

        for (auto& toast : m_toasts) {
            toast.elapsed += dt;

            // 1. Slide-in from top: yOffset eases from -50.0f to 0.0f, alpha eases from 0.0f to 1.0f
            if (toast.elapsed < slideInTime) {
                float inProgress = std::clamp(toast.elapsed / slideInTime, 0.0f, 1.0f);
                float ease = inProgress * (2.0f - inProgress); // ease-out quad
                toast.yOffset = -50.0f * (1.0f - ease);
                toast.alpha = inProgress;
            } else {
                toast.yOffset = 0.0f;
                toast.alpha = 1.0f;
            }

            // 2. Alpha fade-out during the last 0.5s of duration
            float fadeStart = std::max(0.0f, toast.duration - fadeOutTime);
            if (toast.elapsed >= fadeStart) {
                float remaining = toast.duration - toast.elapsed;
                toast.alpha = std::clamp(remaining / fadeOutTime, 0.0f, 1.0f);
            }
        }

        // Remove expired toasts
        std::erase_if(m_toasts, [](const ToastNotification& toast) {
            return toast.elapsed >= toast.duration;
        });
    }

    void ToastNotificationManager::render() {
        if (!ImGui::GetCurrentContext() || m_toasts.empty()) {
            return;
        }

        ImDrawList* drawList = ImGui::GetForegroundDrawList();
        if (!drawList) {
            return;
        }

        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 viewportPos = viewport ? viewport->Pos : ImVec2(0.0f, 0.0f);
        ImVec2 viewportSize = viewport ? viewport->Size : ImGui::GetIO().DisplaySize;

        const float rightMargin = 24.0f;
        const float startY = 24.0f;
        float currentStackY = 0.0f;

        auto applyAlpha = [](uint32_t col, float alpha) -> uint32_t {
            uint32_t a = static_cast<uint32_t>(std::clamp(alpha, 0.0f, 1.0f) * 255.0f);
            return (col & 0x00FFFFFF) | (a << 24);
        };

        for (const auto& toast : m_toasts) {
            if (toast.alpha <= 0.001f) {
                continue;
            }

            ImVec2 textSize = ImGui::CalcTextSize(toast.message.c_str());
            float boxWidth = std::max(240.0f, textSize.x + 36.0f);
            float boxHeight = std::max(44.0f, textSize.y + 20.0f);

            float x = viewportPos.x + viewportSize.x - rightMargin - boxWidth;
            float y = viewportPos.y + startY + toast.yOffset + currentStackY;

            ImVec2 minPos(x, y);
            ImVec2 maxPos(x + boxWidth, y + boxHeight);

            // 1. Hard 4px drop shadow (0xFF000000 with alpha)
            uint32_t shadowCol = applyAlpha(0xFF000000, toast.alpha * 0.9f);
            drawList->AddRectFilled(
                ImVec2(minPos.x + 4.0f, minPos.y + 4.0f),
                ImVec2(maxPos.x + 4.0f, maxPos.y + 4.0f),
                shadowCol, 0.0f
            );

            // 2. Banner background: electric pastel background, 0 rounding
            uint32_t bgCol = applyAlpha(toast.bgColor, toast.alpha);
            drawList->AddRectFilled(minPos, maxPos, bgCol, 0.0f);

            // 3. Thick solid 2.5px border
            uint32_t borderCol = applyAlpha(0xFF000000, toast.alpha);
            drawList->AddRect(minPos, maxPos, borderCol, 0.0f, 0, 2.5f);

            // 4. Bold black text
            uint32_t textCol = applyAlpha(0xFF000000, toast.alpha);
            ImVec2 textPos(
                minPos.x + (boxWidth - textSize.x) * 0.5f,
                minPos.y + (boxHeight - textSize.y) * 0.5f
            );
            drawList->AddText(textPos, textCol, toast.message.c_str());

            currentStackY += boxHeight + 12.0f;
        }
    }

} // namespace App::UI
