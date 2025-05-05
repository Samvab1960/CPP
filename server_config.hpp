#pragma once
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <string>
#include <thread>

class ServerConfig {
public:
    static void configure() {
        // Load configuration from file or environment variables
        loadConfig();
        
        // Configure SSL context
        configureSSL();
        
        // Configure database connection pool
        configureDatabase();
        
        // Configure rate limiting
        configureRateLimiting();
        
        // Configure logging
        configureLogging();
    }

    static const std::string& getDbHost() { return dbHost; }
    static int getDbPort() { return dbPort; }
    static const std::string& getDbName() { return dbName; }
    static const std::string& getDbUser() { return dbUser; }
    static const std::string& getDbPassword() { return dbPassword; }
    static int getServerPort() { return serverPort; }
    static int getThreadCount() { return threadCount; }

private:
    static std::string dbHost;
    static int dbPort;
    static std::string dbName;
    static std::string dbUser;
    static std::string dbPassword;
    static int serverPort;
    static int threadCount;

    static void loadConfig() {
        // Database configuration
        dbHost = "127.0.0.1";
        dbPort = 3306;
        dbName = "crm_system";
        dbUser = "root";
        dbPassword = "root";
        serverPort = 8080;
        threadCount = std::thread::hardware_concurrency();
        
        // Set server address to localhost
        serverAddress = "localhost";
        enableSSL = false;  // Set to false for regular HTTP
    }

    // Add new member variables
    // Add these with other static member initializations
    std::string ServerConfig::serverAddress;
    bool ServerConfig::enableSSL;

    static void configureDatabase() {
        try {
            // Test database connection
            sql::mysql::MySQL_Driver *driver = sql::mysql::get_mysql_driver_instance();
            std::unique_ptr<sql::Connection> testConn(driver->connect(
                "tcp://" + dbHost + ":" + std::to_string(dbPort),
                dbUser,
                dbPassword
            ));
            testConn->setSchema(dbName);
        } catch (sql::SQLException &e) {
            throw std::runtime_error("Database connection failed: " + std::string(e.what()));
        }
    }

    static void configureRateLimiting() {
        // Basic rate limiting
        maxRequestsPerMinute = 100;
        maxConnectionsPerIp = 10;
    }

    static void configureSSL() {
        // Configure SSL certificates and settings
    }

    static void configureDatabase() {
        // Configure database connection pool settings
    }

    static void configureRateLimiting() {
        // Configure rate limiting rules
    }

    static void configureLogging() {
        // Configure logging settings
    }
};

// Initialize static members
std::string ServerConfig::dbHost;
int ServerConfig::dbPort;
std::string ServerConfig::dbName;
std::string ServerConfig::dbUser;
std::string ServerConfig::dbPassword;
int ServerConfig::serverPort;
int ServerConfig::threadCount;