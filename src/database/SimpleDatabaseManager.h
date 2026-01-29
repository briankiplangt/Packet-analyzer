// SimpleDatabaseManager.h - SECURE Simplified database layer without external dependencies
#pragma once

#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <chrono>
#include <map>
#include <cstdint>
#include <fstream>
#include <mutex>

namespace PacketAnalyzer2026 {

struct User {
    int id;
    std::string username;
    std::string role; // admin, analyst, viewer
    std::string email;
    std::string passwordHash; // ✅ SECURITY: Store hashed password
    std::string salt;          // ✅ SECURITY: Store salt
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point lastLogin;
    bool isActive;
    int failedLoginAttempts;
};

struct CaptureSession {
    int id;
    int userId;
    std::string sessionName;
    std::string interfaceName;
    std::string filterExpression;
    std::chrono::system_clock::time_point startTime;
    std::optional<std::chrono::system_clock::time_point> endTime;
    uint64_t totalPackets;
    uint64_t totalBytes;
    std::string status; // active, completed, error, stopped
    std::optional<std::string> filePath;
    std::string notes;
};

struct PacketMetadata {
    uint64_t id;
    int sessionId;
    uint64_t packetNumber;
    uint64_t timestampNs;
    uint32_t sizeBytes;
    std::string protocol;
    std::string sourceIp;
    std::string destIp;
    uint16_t sourcePort;
    uint16_t destPort;
    std::string flags;
    bool isEncrypted;
    std::string application;
};

// ✅ SECURITY: Constants for security policies
constexpr int MAX_FAILED_ATTEMPTS = 5;
constexpr size_t MAX_PACKETS_IN_MEMORY = 100000;
constexpr size_t PACKET_CLEANUP_BATCH = 10000;

// SECURE file-based database for demo purposes
class SimpleDatabaseManager {
public:
    SimpleDatabaseManager();
    ~SimpleDatabaseManager();
    
    // Database lifecycle
    bool initialize(const std::string& dbPath = "packet_analyzer_data");
    void close();
    
    // User management
    std::optional<User> authenticateUser(const std::string& username, const std::string& password);
    bool createUser(const std::string& username, const std::string& password, 
                   const std::string& role, const std::string& email = "");
    bool updateLastLogin(int userId);
    bool incrementFailedLogin(const std::string& username);
    bool resetFailedLogin(const std::string& username);
    
    // Audit logging
    bool logAuditEvent(int userId, const std::string& action, const std::string& resource = "",
                      const std::string& details = "", const std::string& ipAddress = "",
                      bool success = true);
    
    // Capture session management
    int createCaptureSession(int userId, const std::string& sessionName,
                           const std::string& interfaceName, const std::string& filter = "");
    bool updateCaptureSession(int sessionId, uint64_t totalPackets, uint64_t totalBytes,
                            const std::string& status = "active");
    bool endCaptureSession(int sessionId, const std::string& filePath = "");
    
    // Packet metadata (simplified)
    bool insertPacketMetadata(const PacketMetadata& packet);
    
private:
    std::string dbPath_;
    std::vector<User> users_;
    std::vector<CaptureSession> sessions_;
    std::vector<PacketMetadata> packets_;
    int nextUserId_;
    int nextSessionId_;
    uint64_t nextPacketId_;
    
    // ✅ SECURITY: Thread safety
    mutable std::mutex dbMutex_;
    
    // ✅ SECURITY: Input validation methods
    bool isValidPath(const std::string& path);
    bool isValidUsername(const std::string& username);
    bool isStrongPassword(const std::string& password);
    bool isValidRole(const std::string& role);
    bool isValidEmail(const std::string& email);
    bool isValidSessionName(const std::string& name);
    bool isValidInterfaceName(const std::string& name);
    
    // ✅ SECURITY: Input sanitization
    std::string sanitizeInput(const std::string& input);
    std::string sanitizeLogInput(const std::string& input);
    
    // ✅ SECURITY: Secure password handling
    std::string generateSecureSalt();
    std::string hashPasswordSecure(const std::string& password, const std::string& salt);
    bool verifyPassword(const std::string& password, const std::string& hash, const std::string& salt);
    
    // ✅ SECURITY: Safe file operations
    void writeString(std::ofstream& file, const std::string& str);
    std::string readString(std::ifstream& file);
    
    // Helper methods (kept for compatibility)
    std::string hashPassword(const std::string& password, const std::string& salt);
    std::string generateSalt();
    bool saveToFile();
    bool loadFromFile();
    std::string timePointToString(const std::chrono::system_clock::time_point& tp);
    std::chrono::system_clock::time_point stringToTimePoint(const std::string& str);
};

} // namespace PacketAnalyzer2026