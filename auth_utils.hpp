#pragma once
#include <string>
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <jwt-cpp/jwt.h>
#include <ctime>

class AuthUtils {
public:
    static bool verifyPassword(const std::string& inputPassword, const std::string& storedHash) {
        // Hash the input password with SHA-256
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256_CTX sha256;
        SHA256_Init(&sha256);
        SHA256_Update(&sha256, inputPassword.c_str(), inputPassword.length());
        SHA256_Final(hash, &sha256);
        
        std::string hashedInput = bytesToHex(hash, SHA256_DIGEST_LENGTH);
        return hashedInput == storedHash;
    }

    static std::string generateToken(int userId, const std::string& username) {
        auto token = jwt::create()
            .set_issuer("crm_system")
            .set_type("JWS")
            .set_issued_at(std::chrono::system_clock::now())
            .set_expires_at(std::chrono::system_clock::now() + std::chrono::hours(24))
            .set_payload_claim("user_id", jwt::claim(std::to_string(userId)))
            .set_payload_claim("username", jwt::claim(username))
            .sign(jwt::algorithm::hs256{"your_secret_key"});
        
        return token;
    }

    static std::string hashPassword(const std::string& password) {
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256_CTX sha256;
        SHA256_Init(&sha256);
        SHA256_Update(&sha256, password.c_str(), password.length());
        SHA256_Final(hash, &sha256);
        
        return bytesToHex(hash, SHA256_DIGEST_LENGTH);
    }

private:
    static std::string bytesToHex(unsigned char* bytes, unsigned int length) {
        std::stringstream ss;
        for(unsigned int i = 0; i < length; i++) {
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)bytes[i];
        }
        return ss.str();
    }
};