// init_database.cpp - Initialize database with default admin user
#include "../src/database/DatabaseManager.h"
#include <iostream>

int main() {
    PacketAnalyzer2026::DatabaseManager db;
    
    std::cout << "🗄️ Initializing PacketAnalyzer2026 database..." << std::endl;
    
    if (!db.initialize("packet_analyzer.db")) {
        std::cerr << "❌ Failed to initialize database" << std::endl;
        return 1;
    }
    
    // Create default admin user
    if (db.createUser("admin", "admin123", "admin", "admin@company.com")) {
        std::cout << "✅ Created default admin user (admin/admin123)" << std::endl;
    } else {
        std::cout << "ℹ️ Admin user already exists or creation failed" << std::endl;
    }
    
    // Create sample analyst user
    if (db.createUser("analyst", "analyst123", "analyst", "analyst@company.com")) {
        std::cout << "✅ Created analyst user (analyst/analyst123)" << std::endl;
    } else {
        std::cout << "ℹ️ Analyst user already exists or creation failed" << std::endl;
    }
    
    // Create sample viewer user
    if (db.createUser("viewer", "viewer123", "viewer", "viewer@company.com")) {
        std::cout << "✅ Created viewer user (viewer/viewer123)" << std::endl;
    } else {
        std::cout << "ℹ️ Viewer user already exists or creation failed" << std::endl;
    }
    
    std::cout << "🎯 Database initialization complete!" << std::endl;
    std::cout << "\n📋 Available users:" << std::endl;
    std::cout << "   👑 admin/admin123 (full access)" << std::endl;
    std::cout << "   🔍 analyst/analyst123 (analysis access)" << std::endl;
    std::cout << "   👁️ viewer/viewer123 (read-only access)" << std::endl;
    
    return 0;
}