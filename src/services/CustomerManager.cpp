// src/services/CustomerManager.cpp
#include "services/CustomerManager.hpp"
#include <algorithm>
#include <cctype>

namespace App::Services {

    Models::Customer& CustomerManager::createCustomer(const std::string& name, const std::string& phone) {
        Core::CustomerId id = generateNextId();
        m_customers.emplace_back(id, name, phone);
        return m_customers.back();
    }

    bool CustomerManager::updateCustomer(const Models::Customer& customer) {
        for (auto& c : m_customers) {
            if (c.getId() == customer.getId()) {
                c = customer;
                return true;
            }
        }
        return false;
    }

    bool CustomerManager::deleteCustomer(Core::CustomerId id) {
        auto it = std::remove_if(m_customers.begin(), m_customers.end(),
            [id](const Models::Customer& c) { return c.getId() == id; });
        if (it != m_customers.end()) {
            m_customers.erase(it, m_customers.end());
            return true;
        }
        return false;
    }

    std::optional<std::reference_wrapper<const Models::Customer>> CustomerManager::findById(Core::CustomerId id) const {
        for (const auto& c : m_customers) {
            if (c.getId() == id) return std::cref(c);
        }
        return std::nullopt;
    }

    std::optional<std::reference_wrapper<Models::Customer>> CustomerManager::findByIdMut(Core::CustomerId id) {
        for (auto& c : m_customers) {
            if (c.getId() == id) return std::ref(c);
        }
        return std::nullopt;
    }

    std::vector<const Models::Customer*> CustomerManager::search(const std::string& query) const {
        std::vector<const Models::Customer*> results;
        if (query.empty()) {
            for (const auto& c : m_customers) results.push_back(&c);
            return results;
        }

        std::string lowerQuery = query;
        std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(),
                       [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });

        for (const auto& c : m_customers) {
            std::string lowerName = c.getName();
            std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(),
                           [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
            if (lowerName.find(lowerQuery) != std::string::npos || c.getPhone().find(lowerQuery) != std::string::npos) {
                results.push_back(&c);
            }
        }
        return results;
    }

    Core::CustomerId CustomerManager::generateNextId() {
        return m_nextId++;
    }

} // namespace App::Services
