#pragma once
#include "database.hpp"
#include "auth_handler.hpp"
#include <nlohmann/json.hpp>

class UserHandler {
public:
    static json getProfile(int userId) {
        auto& db = Database::getInstance();
        auto conn = db.getConnection();

        std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
            "SELECT u.username, u.email, u.avatar_url, u.created_at, u.last_login, "
            "(SELECT COUNT(*) FROM customers WHERE created_by = u.id) as customer_count, "
            "(SELECT COUNT(*) FROM contacts c JOIN customers cu ON c.customer_id = cu.id "
            "WHERE cu.created_by = u.id) as contact_count "
            "FROM users u WHERE u.id = ?"
        ));
        stmt->setInt(1, userId);

        std::unique_ptr<sql::ResultSet> result(stmt->executeQuery());
        if (result->next()) {
            return {
                {"username", result->getString("username")},
                {"email", result->getString("email")},
                {"avatar_url", result->getString("avatar_url")},
                {"created_at", result->getString("created_at")},
                {"last_login", result->getString("last_login")},
                {"customer_count", result->getInt("customer_count")},
                {"contact_count", result->getInt("contact_count")}
            };
        }
        throw std::runtime_error("User not found");
    }

    static void updateProfile(int userId, const json& data) {
        auto& db = Database::getInstance();
        auto conn = db.getConnection();

        // Start transaction
        conn->setAutoCommit(false);
        try {
            // Update basic info
            std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
                "UPDATE users SET username = ?, email = ? WHERE id = ?"
            ));
            stmt->setString(1, data["username"]);
            stmt->setString(2, data["email"]);
            stmt->setInt(3, userId);
            stmt->execute();

            // Update password if provided
            if (data.contains("current_password") && data.contains("new_password")) {
                verifyAndUpdatePassword(conn.get(), userId, 
                    data["current_password"], data["new_password"]);
            }

            // Log activity
            logUserActivity(conn.get(), userId, "profile_update", 
                "User profile updated");

            conn->commit();
        } catch (const std::exception& e) {
            conn->rollback();
            throw;
        }
        conn->setAutoCommit(true);
    }

private:
    static void verifyAndUpdatePassword(sql::Connection* conn, int userId,
        const std::string& currentPassword, const std::string& newPassword) {
        std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
            "SELECT password_hash FROM users WHERE id = ?"
        ));
        stmt->setInt(1, userId);
        
        std::unique_ptr<sql::ResultSet> result(stmt->executeQuery());
        if (!result->next() || !BCrypt::checkPassword(currentPassword, result->getString("password_hash"))) {
            throw std::runtime_error("Current password is incorrect");
        }

        std::string newHash = BCrypt::generateHash(newPassword);
        std::unique_ptr<sql::PreparedStatement> updateStmt(conn->prepareStatement(
            "UPDATE users SET password_hash = ? WHERE id = ?"
        ));
        updateStmt->setString(1, newHash);
        updateStmt->setInt(2, userId);
        updateStmt->execute();
    }

    static void logUserActivity(sql::Connection* conn, int userId,
        const std::string& activityType, const std::string& description) {
        std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
            "INSERT INTO user_activity_log (user_id, activity_type, description) "
            "VALUES (?, ?, ?)"
        ));
        stmt->setInt(1, userId);
        stmt->setString(2, activityType);
        stmt->setString(3, description);
        stmt->execute();
    }
};