#pragma once

#include "services/CustomerManager.hpp"
#include <functional>
#include <string>

namespace App::UI {

    class CustomerListView {
    public:
        CustomerListView() = default;

        void render(
            Services::CustomerManager& customerMgr,
            Core::CustomerId selectedCustomerId,
            const std::function<void(Core::CustomerId)>& onSelectCustomer,
            const std::function<void()>& onAddNewCustomerClick
        );

    private:
        char m_searchQuery[128] = "";
    };

} // namespace App::UI
