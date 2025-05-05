#pragma once
#include <boost/beast/http.hpp>
#include <string>
#include <sstream>
#include <ctime>
#include <mysql_connection.h>
#include <mysql_driver.h>
#include <cppconn/exception.h>
#include <cppconn/statement.h>

class APIHandler {
public:
    boost::beast::http::response<boost::beast::http::string_body> 
    handleRequest(boost::beast::http::request<boost::beast::http::string_body>&& req) {
        boost::beast::http::response<boost::beast::http::string_body> res{
            boost::beast::http::status::ok, req.version()};
        
        res.set(boost::beast::http::field::server, "CRM Server");
        res.set(boost::beast::http::field::content_type, "application/json");
        res.set(boost::beast::http::field::access_control_allow_origin, "*");
        
        std::string target = std::string(req.target());
        
        try {
            if (req.method() == boost::beast::http::verb::get) {
                if (target == "/" || target == "/index.html") {
                    res.set(boost::beast::http::field::content_type, "text/html");
                    res.body() = R"(
                        <!DOCTYPE html>
                        <html>
                        <head>
                            <title>CRM System</title>
                            <style>
                                body { font-family: Arial, sans-serif; margin: 40px; }
                                h1 { color: #2c3e50; }
                                p { color: #34495e; }
                            </style>
                        </head>
                        <body>
                            <h1>Welcome to CRM System</h1>
                            <p>Server is running successfully!</p>
                            <p>Status: Connected to Database</p>
                        </body>
                        </html>
                    )";
                } else if (target == "/api/status") {
                    std::stringstream ss;
                    ss << "{\"status\": \"running\", \"version\": \"1.0.0\", \"timestamp\": " 
                       << std::time(nullptr) << ", \"database\": \"connected\"}";
                    res.body() = ss.str();
                } else {
                    res.result(boost::beast::http::status::not_found);
                    res.body() = R"({"error": "Route not found"})";
                }
            }
        } catch (const sql::SQLException& e) {
            res.result(boost::beast::http::status::internal_server_error);
            res.body() = "{\"error\": \"Database error\"}";
        } catch (const std::exception& e) {
            res.result(boost::beast::http::status::internal_server_error);
            res.body() = "{\"error\": \"Internal server error\"}";
        }
        
        res.prepare_payload();
        return res;
    }
};

// Add database connection member
private:
    sql::mysql::MySQL_Driver *driver;
    std::unique_ptr<sql::Connection> conn;

public:
    APIHandler() {
        // Initialize database connection
        try {
            driver = sql::mysql::get_mysql_driver_instance();
            conn.reset(driver->connect("tcp://127.0.0.1:3306", "root", "root"));
            conn->setSchema("crm_system");
        } catch (const sql::SQLException& e) {
            std::cerr << "Database connection failed: " << e.what() << std::endl;
        }
    }