#pragma once

#include "core/Types.hpp"
#include "models/Measurement.hpp"
#include <string>
#include <vector>
#include <algorithm>

namespace App::Models {

    class Customer {
    public:
        Customer() = default;
        Customer(Core::CustomerId id, std::string name, std::string phone)
            : m_id(id), m_name(std::move(name)), m_phone(std::move(phone)) {}

        // Identity
        [[nodiscard]] Core::CustomerId getId() const { return m_id; }
        void setId(Core::CustomerId id) { m_id = id; }

        // Basic Information
        [[nodiscard]] const std::string& getName() const { return m_name; }
        void setName(std::string name) { m_name = std::move(name); }

        [[nodiscard]] const std::string& getPhone() const { return m_phone; }
        void setPhone(std::string phone) { m_phone = std::move(phone); }

        // Dynamic Measurements (Editable)
        [[nodiscard]] const std::vector<Measurement>& getMeasurements() const { return m_measurements; }
        [[nodiscard]] std::vector<Measurement>& getMeasurements() { return m_measurements; }
        
        void addMeasurement(const Measurement& m) { m_measurements.push_back(m); }
        void removeMeasurement(size_t index) {
            if (index < m_measurements.size()) {
                m_measurements.erase(m_measurements.begin() + index);
            }
        }
        void setMeasurements(std::vector<Measurement> measurements) {
            m_measurements = std::move(measurements);
        }

        // Associated Orders
        [[nodiscard]] const std::vector<Core::OrderId>& getOrderIds() const { return m_orderIds; }
        void addOrderId(Core::OrderId orderId) { m_orderIds.push_back(orderId); }
        [[nodiscard]] bool removeOrderId(Core::OrderId id) {
            return std::erase(m_orderIds, id) > 0;
        }

        // Timestamps
        [[nodiscard]] const std::string& getCreatedAt() const { return m_createdAt; }
        void setCreatedAt(std::string timestamp) { m_createdAt = std::move(timestamp); }

    private:
        Core::CustomerId m_id = 0;
        std::string m_name;
        std::string m_phone;
        std::vector<Measurement> m_measurements;
        std::vector<Core::OrderId> m_orderIds;
        std::string m_createdAt;
    };

} // namespace App::Models
