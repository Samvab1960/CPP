#pragma once
#include "database.hpp"
#include <nlohmann/json.hpp>

class AnalyticsHandler {
public:
    static json getDashboardStats(int userId) {
        auto& db = Database::getInstance();
        auto conn = db.getConnection();

        json stats;
        stats["customer_trends"] = getCustomerTrends(conn.get(), userId);
        stats["contact_distribution"] = getContactDistribution(conn.get(), userId);
        stats["recent_activities"] = getRecentActivities(conn.get(), userId);
        stats["performance_metrics"] = getPerformanceMetrics(conn.get(), userId);

        return stats;
    }

private:
    static json getCustomerTrends(sql::Connection* conn, int userId) {
        std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
            "SELECT DATE(created_at) as date, COUNT(*) as count "
            "FROM customers WHERE created_by = ? "
            "GROUP BY DATE(created_at) "
            "ORDER BY date DESC LIMIT 30"
        ));
        stmt->setInt(1, userId);
        
        std::unique_ptr<sql::ResultSet> result(stmt->executeQuery());
        json trends = json::array();
        while (result->next()) {
            trends.push_back({
                {"date", result->getString("date")},
                {"count", result->getInt("count")}
            });
        }
        return trends;
    }

    static json getContactDistribution(sql::Connection* conn, int userId) {
        std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
            "SELECT c.type, COUNT(*) as count "
            "FROM contacts c "
            "JOIN customers cu ON c.customer_id = cu.id "
            "WHERE cu.created_by = ? "
            "GROUP BY c.type"
        ));
        stmt->setInt(1, userId);
        
        std::unique_ptr<sql::ResultSet> result(stmt->executeQuery());
        json distribution;
        while (result->next()) {
            distribution[result->getString("type")] = result->getInt("count");
        }
        return distribution;
    }

    static json getRecentActivities(sql::Connection* conn, int userId) {
        std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
            "SELECT activity_type, description, created_at "
            "FROM user_activity_log "
            "WHERE user_id = ? "
            "ORDER BY created_at DESC LIMIT 10"
        ));
        stmt->setInt(1, userId);
        
        std::unique_ptr<sql::ResultSet> result(stmt->executeQuery());
        json activities = json::array();
        while (result->next()) {
            activities.push_back({
                {"type", result->getString("activity_type")},
                {"description", result->getString("description")},
                {"timestamp", result->getString("created_at")}
            });
        }
        return activities;
    }

    static json getPerformanceMetrics(sql::Connection* conn, int userId) {
        // Calculate various performance metrics
        json metrics;
        
        // Customer growth rate
        metrics["customer_growth"] = calculateCustomerGrowth(conn, userId);
        
        // Contact engagement rate
        metrics["contact_engagement"] = calculateContactEngagement(conn, userId);
        
        // Activity score
        metrics["activity_score"] = calculateActivityScore(conn, userId);
        
        return metrics;
    }

    // Additional helper methods for calculating metrics...
};