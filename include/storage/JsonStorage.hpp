#pragma once

#include "storage/IStorage.hpp"

namespace App::Storage {

    class JsonStorage : public IStorage {
    public:
        JsonStorage() = default;
        ~JsonStorage() override = default;

        bool saveCustomers(const std::vector<Models::Customer>& customers, const std::string& destination) override;
        bool loadCustomers(std::vector<Models::Customer>& customers, const std::string& source) override;

        bool saveOrders(const std::vector<Models::Order>& orders, const std::string& destination) override;
        bool loadOrders(std::vector<Models::Order>& orders, const std::string& source) override;
    };

} // namespace App::Storage
