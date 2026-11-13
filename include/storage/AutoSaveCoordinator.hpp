#pragma once

#include "storage/JsonStorage.hpp"
#include "storage/ExcelStorage.hpp"
#include <chrono>
#include <functional>
#include <string>

namespace App::Storage {

    class AutoSaveCoordinator {
    public:
        AutoSaveCoordinator(int intervalSeconds = 60);

        // Called on each frame/tick to check if 60 seconds have passed
        void update(
            float deltaTime,
            const std::function<void()>& onSaveCallback
        );

        // Triggers immediate save
        void forceSave(const std::function<void()>& onSaveCallback);

        // Getters for UI display
        [[nodiscard]] int getSecondsRemaining() const;
        [[nodiscard]] const std::string& getLastSaveTimestamp() const;
        [[nodiscard]] bool isSaving() const { return m_isSaving; }

    private:
        int m_intervalSeconds = 60;
        float m_timer = 0.0f;
        bool m_isSaving = false;
        std::string m_lastSaveTimestamp = "Not saved yet";
    };

} // namespace App::Storage
