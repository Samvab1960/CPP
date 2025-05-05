#include "auth_utils.hpp"

if (target == "/login" && req.method() == boost::beast::http::verb::post) {
    try {
        auto request_body = json::parse(req.body());
        std::string username = request_body["username"];
        std::string password = request_body["password"];
        
        auto prep_stmt = conn->prepareStatement("SELECT id, password_hash FROM users WHERE username = ?");
        prep_stmt->setString(1, username);
        auto rs = prep_stmt->executeQuery();
        
        if (rs->next() && AuthUtils::verifyPassword(password, rs->getString("password_hash"))) {
            std::string token = AuthUtils::generateToken(rs->getInt("id"), username);
            res.body() = "{\"status\": \"success\", \"token\": \"" + token + "\"}";
        } else {
            res.result(boost::beast::http::status::unauthorized);
            res.body() = "{\"error\": \"Invalid username or password\"}";
        }
    } catch (const std::exception& e) {
        res.result(boost::beast::http::status::bad_request);
        res.body() = "{\"error\": \"Invalid request format\"}";
    }
}