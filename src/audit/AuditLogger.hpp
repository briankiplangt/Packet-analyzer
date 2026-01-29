// AuditLogger.hpp - Complete security audit trail
#pragma once

#include <fstream>
#include <string>
#include <chrono>
#include <mutex>
#include <iostream>
#include <iomanip>
#include <sstream>

namespace PacketAnalyzer2026::Audit {

class AuditLogger {
private:
    std::ofstream logFile_;
    std::mutex logMutex_;
    std::string logFilePath_;

    std::string getCurrentTimestamp() const {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }

    void writeLogEntry(const std::string& level, const std::string& category, 
                      const std::string& action, const std::string& details) {
        std::lock_guard<std::mutex> lock(logMutex_);
        
        std::string timestamp = getCurrentTimestamp();
        std::string logEntry = timestamp + " [" + level + "] [" + category + "] " + action;
        
        if (!details.empty()) {
            logEntry += " - " + details;
        }
        
        // Write to file
        if (logFile_.is_open()) {
            logFile_ << logEntry << std::endl;
            logFile_.flush();
        }
        
        // Also output to console for debugging
        std::cout << "📝 AUDIT: " << logEntry << std::endl;
    }

public:
    AuditLogger(const std::string& logPath = "packet_analyzer_audit.log") 
        : logFilePath_(logPath) {
        
        logFile_.open(logFilePath_, std::ios::app);
        if (!logFile_.is_open()) {
            std::cerr << "❌ Failed to open audit log file: " << logFilePath_ << std::endl;
        } else {
            std::cout << "📝 Audit logging initialized: " << logFilePath_ << std::endl;
            writeLogEntry("INFO", "SYSTEM", "AUDIT_START", "Packet Analyzer audit logging started");
        }
    }

    ~AuditLogger() {
        if (logFile_.is_open()) {
            writeLogEntry("INFO", "SYSTEM", "AUDIT_STOP", "Packet Analyzer audit logging stopped");
            logFile_.close();
        }
    }

    void logCaptureStart(const std::string& interface, const std::string& filter) {
        std::string details = "Interface: " + interface;
        if (!filter.empty()) {
            details += ", Filter: " + filter;
        }
        writeLogEntry("INFO", "CAPTURE", "START", details);
    }

    void logCaptureStop(const std::string& interface, size_t packetCount) {
        std::string details = "Interface: " + interface + ", Packets: " + std::to_string(packetCount);
        writeLogEntry("INFO", "CAPTURE", "STOP", details);
    }

    void logPrivilegeDrop(bool success) {
        if (success) {
            writeLogEntry("INFO", "SECURITY", "PRIVILEGE_DROP", "Successfully dropped elevated privileges");
        } else {
            writeLogEntry("WARNING", "SECURITY", "PRIVILEGE_DROP_FAILED", "Failed to drop elevated privileges");
        }
    }

    void logSecurityViolation(const std::string& violation, const std::string& details) {
        writeLogEntry("WARNING", "SECURITY", "VIOLATION", violation + " - " + details);
    }

    void logSystemError(const std::string& error, const std::string& component) {
        writeLogEntry("ERROR", "SYSTEM", "ERROR", "Component: " + component + ", Error: " + error);
    }

    void logUserAction(const std::string& user, const std::string& action, const std::string& details) {
        writeLogEntry("INFO", "USER", action, "User: " + user + ", Details: " + details);
    }

    void logDatabaseOperation(const std::string& operation, bool success, const std::string& details) {
        std::string level = success ? "INFO" : "ERROR";
        std::string status = success ? "SUCCESS" : "FAILED";
        writeLogEntry(level, "DATABASE", operation + "_" + status, details);
    }

    void logCircuitBreakerEvent(const std::string& component, const std::string& state, const std::string& reason) {
        writeLogEntry("WARNING", "RESILIENCE", "CIRCUIT_BREAKER", 
                     "Component: " + component + ", State: " + state + ", Reason: " + reason);
    }

    void logPerformanceMetric(const std::string& metric, const std::string& value) {
        writeLogEntry("INFO", "PERFORMANCE", "METRIC", metric + ": " + value);
    }

    void logConfigurationChange(const std::string& setting, const std::string& oldValue, const std::string& newValue) {
        writeLogEntry("INFO", "CONFIG", "CHANGE", 
                     "Setting: " + setting + ", Old: " + oldValue + ", New: " + newValue);
    }

    void logExportOperation(const std::string& format, const std::string& filename, size_t packetCount) {
        std::string details = "Format: " + format + ", File: " + filename + ", Packets: " + std::to_string(packetCount);
        writeLogEntry("INFO", "EXPORT", "PCAP_EXPORT", details);
    }

    void logFilterApplication(const std::string& filter, size_t matchedPackets) {
        std::string details = "Filter: " + filter + ", Matched: " + std::to_string(matchedPackets);
        writeLogEntry("INFO", "FILTER", "APPLY", details);
    }

    // Get log file path for external access
    std::string getLogFilePath() const {
        return logFilePath_;
    }

    // Check if logging is working
    bool isLoggingActive() const {
        return logFile_.is_open();
    }
};

} // namespace PacketAnalyzer2026::Audit