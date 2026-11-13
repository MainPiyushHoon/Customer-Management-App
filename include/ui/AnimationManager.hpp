#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace App::UI {

    class ViewTransitionManager {
    public:
        ViewTransitionManager() = default;

        // Triggers a new alpha cross-fade transition with a given duration (default 150ms)
        void triggerTransition(float duration = 0.15f);

        // Advances the transition timer by dt seconds
        void update(float dt);

        // Current transition alpha [0.0f, 1.0f]
        [[nodiscard]] float getAlpha() const { return m_alpha; }

        // Whether the transition has completed
        [[nodiscard]] bool isFinished() const { return m_isFinished; }

        // Resets transition state back to fully opaque at rest
        void reset();

    private:
        float m_duration = 0.15f;
        float m_elapsed = 0.15f;
        float m_alpha = 1.0f;
        bool m_isFinished = true;
    };

    struct ToastNotification {
        std::string message;
        uint32_t bgColor = 0xFF00FFA6;
        float duration = 3.0f;
        float elapsed = 0.0f;
        float alpha = 0.0f;
        float yOffset = -50.0f;
    };

    class ToastNotificationManager {
    public:
        ToastNotificationManager() = default;

        // Enqueues a new Neo-Brutalist floating toast notification
        void showToast(const std::string& message, uint32_t bgColor = 0xFF00FFA6, float duration = 3.0f);

        // Updates elapsed time, smooth slide-in, and fade-out animations for active toasts
        void update(float dt);

        // Renders active toasts in the top-right viewport corner as stark Neo-Brutalist banners
        void render();

        [[nodiscard]] bool hasActiveToasts() const { return !m_toasts.empty(); }
        [[nodiscard]] size_t getActiveToastCount() const { return m_toasts.size(); }
        [[nodiscard]] const std::vector<ToastNotification>& getToasts() const { return m_toasts; }
        void clear() { m_toasts.clear(); }

    private:
        std::vector<ToastNotification> m_toasts;
    };

} // namespace App::UI
