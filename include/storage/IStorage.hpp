#pragma once

#include "models/Customer.hpp"
#include "models/Order.hpp"
#include <vector>
#include <string>

namespace App::Storage {

    class IStorage {
    public:
        virtual ~IStorage() = default;

        virtual bool saveCustomers(const std::vector<Models::Customer>& customers, const std::string& destination) = 0;
        virtual bool loadCustomers(std::vector<Models::Customer>& customers, const std::string& source) = 0;

        virtual bool saveOrders(const std::vector<Models::Order>& orders, const std::string& destination) = 0;
        virtual bool loadOrders(std::vector<Models::Order>& orders, const std::string& source) = 0;
    };

} // namespace App::Storage
