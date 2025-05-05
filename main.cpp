#include "server_config.hpp"
#include "api_handler.hpp"
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <mysql_connection.h>
#include <mysql_driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <iostream>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

int main() {
    try {
        // Initialize MySQL driver
        sql::mysql::MySQL_Driver *driver = sql::mysql::get_mysql_driver_instance();
        
        // Configure server
        ServerConfig::configure();
        
        // Test database connection
        std::unique_ptr<sql::Connection> conn(driver->connect(
            "tcp://" + ServerConfig::getDbHost() + ":" + std::to_string(ServerConfig::getDbPort()),
            ServerConfig::getDbUser(),
            ServerConfig::getDbPassword()
        ));
        conn->setSchema(ServerConfig::getDbName());
        
        std::cout << "Database connection successful" << std::endl;
        
        // Set up the IO context and acceptor
        net::io_context ioc{ServerConfig::getThreadCount()};
        tcp::acceptor acceptor{ioc, {net::ip::make_address("0.0.0.0"), ServerConfig::getServerPort()}};
        
        std::cout << "Server running on port " << ServerConfig::getServerPort() << std::endl;
        
        // Accept and handle connections
        while(true) {
            tcp::socket socket{ioc};
            acceptor.accept(socket);
            
            // Handle the connection
            APIHandler handler;
            beast::flat_buffer buffer;
            http::request<http::string_body> req;
            http::read(socket, buffer, req);
            
            auto res = handler.handleRequest(std::move(req));
            http::write(socket, res);
        }
    } catch(const sql::SQLException& e) {
        std::cerr << "SQL Error: " << e.what() << std::endl;
        return 1;
    } catch(const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}