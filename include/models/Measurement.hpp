#pragma once

#include <string>

namespace App::Models {

    class Measurement {
    public:
        Measurement() = default;
        Measurement(std::string key, std::string value, std::string unit = "")
            : m_key(std::move(key)), m_value(std::move(value)), m_unit(std::move(unit)) {}

        // Getters
        [[nodiscard]] const std::string& getKey() const { return m_key; }
        [[nodiscard]] const std::string& getValue() const { return m_value; }
        [[nodiscard]] const std::string& getUnit() const { return m_unit; }

        // Setters for dynamic editing
        void setKey(std::string key) { m_key = std::move(key); }
        void setValue(std::string value) { m_value = std::move(value); }
        void setUnit(std::string unit) { m_unit = std::move(unit); }

    private:
        std::string m_key;
        std::string m_value;
        std::string m_unit;
    };

} // namespace App::Models
