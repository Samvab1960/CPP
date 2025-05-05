#pragma once
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <memory>
#include <string>

class Database {
public:
    static Database& getInstance() {
        static Database instance;
        return instance;
    }

    sql::Connection* getConnection() {
        if (!connection_ || connection_->isClosed()) {
            connect();
        }
        return connection_;
    }

private:
    Database() {
        connect();
    }

    void connect() {
        try {
            sql::Driver* driver = get_driver_instance();
            connection_ = driver->connect("tcp://127.0.0.1:3306", "root", "root");
            connection_->setSchema("crm_system");
        } catch (sql::SQLException& e) {
            std::cerr << "SQL Error: " << e.what() << std::endl;
        }
    }

    sql::Connection* connection_;
};