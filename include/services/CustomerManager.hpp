#pragma once

#include "models/Customer.hpp"
#include <vector>
#include <optional>
#include <string>

namespace App::Services {

    class CustomerManager {
    public:
        CustomerManager() = default;

        // CRUD Operations
        Models::Customer& createCustomer(const std::string& name, const std::string& phone);
        bool updateCustomer(const Models::Customer& customer);
        bool deleteCustomer(Core::CustomerId id);

        [[nodiscard]] const std::vector<Models::Customer>& getAllCustomers() const { return m_customers; }
        [[nodiscard]] std::vector<Models::Customer>& getAllCustomers() { return m_customers; }
        
        [[nodiscard]] std::optional<std::reference_wrapper<const Models::Customer>> findById(Core::CustomerId id) const;
        [[nodiscard]] std::optional<std::reference_wrapper<Models::Customer>> findByIdMut(Core::CustomerId id);

        // Search & Filtering
        [[nodiscard]] std::vector<const Models::Customer*> search(const std::string& query) const;

        // ID Generation
        Core::CustomerId generateNextId();

    private:
        std::vector<Models::Customer> m_customers;
        Core::CustomerId m_nextId = 1001;
    };

} // namespace App::Services
