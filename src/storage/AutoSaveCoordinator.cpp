// src/storage/AutoSaveCoordinator.cpp
#include "storage/AutoSaveCoordinator.hpp"
#include <iomanip>
#include <sstream>
#include <ctime>

namespace App::Storage {

    AutoSaveCoordinator::AutoSaveCoordinator(int intervalSeconds)
        : m_intervalSeconds(intervalSeconds), m_timer(static_cast<float>(intervalSeconds)) {}

    void AutoSaveCoordinator::update(float deltaTime, const std::function<void()>& onSaveCallback) {
        m_timer -= deltaTime;
        if (m_timer <= 0.0f) {
            forceSave(onSaveCallback);
            m_timer = static_cast<float>(m_intervalSeconds);
        }
    }

    void AutoSaveCoordinator::forceSave(const std::function<void()>& onSaveCallback) {
        m_isSaving = true;
        if (onSaveCallback) {
            onSaveCallback();
        }
        m_timer = static_cast<float>(m_intervalSeconds);

        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::tm tmNow{};
#if defined(_WIN32)
        localtime_s(&tmNow, &now);
#else
        localtime_r(&now, &tmNow);
#endif
        std::ostringstream ss;
        ss << std::put_time(&tmNow, "%H:%M:%S");
        m_lastSaveTimestamp = ss.str();
        m_isSaving = false;
    }

    int AutoSaveCoordinator::getSecondsRemaining() const {
        return (m_timer > 0.0f) ? static_cast<int>(m_timer) : 0;
    }

    const std::string& AutoSaveCoordinator::getLastSaveTimestamp() const {
        return m_lastSaveTimestamp;
    }

} // namespace App::Storage
