#pragma once
#include "database.hpp"
#include <jwt-cpp/jwt.h>
#include <bcrypt/BCrypt.hpp>

class AuthHandler {
public:
    static std::string handleLogin(const std::string& username, const std::string& password) {
        auto& db = Database::getInstance();
        auto conn = db.getConnection();
        
        try {
            std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
                "SELECT id, password_hash FROM users WHERE username = ?"
            ));
            stmt->setString(1, username);
            
            std::unique_ptr<sql::ResultSet> result(stmt->executeQuery());
            
            if (result->next()) {
                std::string stored_hash = result->getString("password_hash");
                if (BCrypt::validatePassword(password, stored_hash)) {
                    return generateToken(result->getInt("id"), username);
                }
            }
        } catch (sql::SQLException& e) {
            throw std::runtime_error("Database error during login");
        }
        
        throw std::runtime_error("Invalid credentials");
    }

private:
    static std::string generateToken(int user_id, const std::string& username) {
        auto token = jwt::create()
            .set_issuer("crm_system")
            .set_type("JWS")
            .set_payload_claim("user_id", jwt::claim(std::to_string(user_id)))
            .set_payload_claim("username", jwt::claim(username))
            .set_issued_at(std::chrono::system_clock::now())
            .set_expires_at(std::chrono::system_clock::now() + std::chrono::hours(24))
            .sign(jwt::algorithm::hs256{"your_secret_key"});
        
        return token;
    }
};