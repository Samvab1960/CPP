#pragma once
#include "database.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class CustomerHandler {
public:
    static json getAllCustomers() {
        auto& db = Database::getInstance();
        auto conn = db.getConnection();
        
        json customers = json::array();
        
        try {
            std::unique_ptr<sql::Statement> stmt(conn->createStatement());
            std::unique_ptr<sql::ResultSet> result(
                stmt->executeQuery("SELECT * FROM customers ORDER BY created_at DESC")
            );
            
            while (result->next()) {
                json customer;
                customer["id"] = result->getInt("id");
                customer["first_name"] = result->getString("first_name");
                customer["last_name"] = result->getString("last_name");
                customer["email"] = result->getString("email");
                customer["phone"] = result->getString("phone");
                customer["company"] = result->getString("company");
                customers.push_back(customer);
            }
        } catch (sql::SQLException& e) {
            throw std::runtime_error("Database error while fetching customers");
        }
        
        return customers;
    }

    static json createCustomer(const json& customerData) {
        auto& db = Database::getInstance();
        auto conn = db.getConnection();
        
        try {
            std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
                "INSERT INTO customers (first_name, last_name, email, phone, company) "
                "VALUES (?, ?, ?, ?, ?)"
            ));
            
            stmt->setString(1, customerData["first_name"]);
            stmt->setString(2, customerData["last_name"]);
            stmt->setString(3, customerData["email"]);
            stmt->setString(4, customerData["phone"]);
            stmt->setString(5, customerData["company"]);
            
            stmt->execute();
            return {{"success", true}, {"message", "Customer created successfully"}};
        } catch (sql::SQLException& e) {
            throw std::runtime_error("Database error while creating customer");
        }
    }
};