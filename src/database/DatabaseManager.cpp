#include "DatabaseManager.h"
#include <QtSql/QSqlDriver>
#include <QCryptographicHash>
#include <QRandomGenerator>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>

DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager instance;
    return instance;
}

bool DatabaseManager::initialize(const QString& dbPath)
{
    if (m_initialized) {
        return true;
    }

    // Create database directory if it doesn't exist
    QDir dbDir = QFileInfo(dbPath).absoluteDir();
    if (!dbDir.exists()) {
        dbDir.mkpath(".");
    }

    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName(dbPath);

    if (!m_database.open()) {
        emit databaseError("Failed to open database: " + m_database.lastError().text());
        return false;
    }

    if (!createTables()) {
        emit databaseError("Failed to create database tables");
        return false;
    }

    m_initialized = true;
    qDebug() << "Database initialized successfully:" << dbPath;
    return true;
}

bool DatabaseManager::isConnected() const
{
    return m_database.isOpen() && m_initialized;
}

bool DatabaseManager::createTables()
{
    QSqlQuery query(m_database);
    
    // Execute the schema file
    QString schemaPath = ":/database/schema.sql";
    QFile schemaFile(schemaPath);
    
    if (!schemaFile.open(QIODevice::ReadOnly)) {
        // Fallback: create tables manually
        QStringList createStatements = {
            R"(CREATE TABLE IF NOT EXISTS users (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                username VARCHAR(50) UNIQUE NOT NULL,
                password_hash VARCHAR(255) NOT NULL,
                salt VARCHAR(32) NOT NULL,
                role VARCHAR(20) NOT NULL DEFAULT 'viewer',
                email VARCHAR(100),
                created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                last_login TIMESTAMP,
                is_active BOOLEAN DEFAULT 1
            ))",
            
            R"(CREATE TABLE IF NOT EXISTS capture_sessions (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                user_id INTEGER NOT NULL,
                session_name VARCHAR(100),
                interface_name VARCHAR(50) NOT NULL,
                filter_expression TEXT,
                start_time TIMESTAMP NOT NULL,
                end_time TIMESTAMP,
                total_packets INTEGER DEFAULT 0,
                total_bytes INTEGER DEFAULT 0,
                status VARCHAR(20) DEFAULT 'active',
                file_path VARCHAR(500),
                notes TEXT,
                FOREIGN KEY (user_id) REFERENCES users(id)
            ))",
            
            R"(CREATE TABLE IF NOT EXISTS packet_metadata (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                session_id INTEGER NOT NULL,
                packet_number INTEGER NOT NULL,
                timestamp_ns INTEGER NOT NULL,
                size_bytes INTEGER NOT NULL,
                protocol VARCHAR(20) NOT NULL,
                source_ip VARCHAR(45),
                dest_ip VARCHAR(45),
                source_port INTEGER,
                dest_port INTEGER,
                flags VARCHAR(20),
                is_encrypted BOOLEAN DEFAULT 0,
                application VARCHAR(50),
                FOREIGN KEY (session_id) REFERENCES capture_sessions(id)
            ))",
            
            R"(CREATE TABLE IF NOT EXISTS user_preferences (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                user_id INTEGER NOT NULL,
                preference_key VARCHAR(50) NOT NULL,
                preference_value TEXT NOT NULL,
                updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                FOREIGN KEY (user_id) REFERENCES users(id),
                UNIQUE(user_id, preference_key)
            ))",
            
            R"(CREATE TABLE IF NOT EXISTS filter_presets (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                user_id INTEGER NOT NULL,
                name VARCHAR(100) NOT NULL,
                filter_expression TEXT NOT NULL,
                description TEXT,
                is_public BOOLEAN DEFAULT 0,
                usage_count INTEGER DEFAULT 0,
                created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                FOREIGN KEY (user_id) REFERENCES users(id)
            ))"
        };
        
        for (const QString& statement : createStatements) {
            if (!query.exec(statement)) {
                qDebug() << "Failed to create table:" << query.lastError().text();
                return false;
            }
        }
        
        // Create default admin user
        createUser("admin", "admin123", "admin");
        
        // Create default filter presets
        QSqlQuery userQuery(m_database);
        userQuery.prepare("SELECT id FROM users WHERE username = ?");
        userQuery.addBindValue("admin");
        if (userQuery.exec() && userQuery.next()) {
            int adminId = userQuery.value(0).toInt();
            saveFilterPreset(adminId, "HTTP Traffic", "tcp port 80", "Capture HTTP web traffic");
            saveFilterPreset(adminId, "HTTPS Traffic", "tcp port 443", "Capture HTTPS encrypted web traffic");
            saveFilterPreset(adminId, "DNS Queries", "udp port 53", "Capture DNS name resolution");
        }
    }
    
    return true;
}

bool DatabaseManager::createUser(const QString& username, const QString& password, const QString& role)
{
    QString salt = generateSalt();
    QString hashedPassword = hashPassword(password, salt);
    
    QSqlQuery query(m_database);
    query.prepare("INSERT INTO users (username, password_hash, salt, role) VALUES (?, ?, ?, ?)");
    query.addBindValue(username);
    query.addBindValue(hashedPassword);
    query.addBindValue(salt);
    query.addBindValue(role);
    
    if (!query.exec()) {
        qDebug() << "Failed to create user:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool DatabaseManager::authenticateUser(const QString& username, const QString& password)
{
    QSqlQuery query(m_database);
    query.prepare("SELECT id, password_hash, salt FROM users WHERE username = ? AND is_active = 1");
    query.addBindValue(username);
    
    if (!query.exec() || !query.next()) {
        return false;
    }
    
    int userId = query.value(0).toInt();
    QString storedHash = query.value(1).toString();
    QString salt = query.value(2).toString();
    
    QString hashedPassword = hashPassword(password, salt);
    
    if (hashedPassword == storedHash) {
        // Update last login
        QSqlQuery updateQuery(m_database);
        updateQuery.prepare("UPDATE users SET last_login = CURRENT_TIMESTAMP WHERE id = ?");
        updateQuery.addBindValue(userId);
        updateQuery.exec();
        
        emit userAuthenticated(userId, username);
        return true;
    }
    
    return false;
}

int DatabaseManager::createCaptureSession(int userId, const QString& sessionName, const QString& interface)
{
    QSqlQuery query(m_database);
    query.prepare("INSERT INTO capture_sessions (user_id, session_name, interface_name, start_time) VALUES (?, ?, ?, CURRENT_TIMESTAMP)");
    query.addBindValue(userId);
    query.addBindValue(sessionName);
    query.addBindValue(interface);
    
    if (!query.exec()) {
        qDebug() << "Failed to create capture session:" << query.lastError().text();
        return -1;
    }
    
    return query.lastInsertId().toInt();
}

bool DatabaseManager::insertPacketMetadata(int sessionId, const QJsonObject& packetData)
{
    QSqlQuery query(m_database);
    query.prepare(R"(INSERT INTO packet_metadata 
        (session_id, packet_number, timestamp_ns, size_bytes, protocol, source_ip, dest_ip, source_port, dest_port, application) 
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?))");
    
    query.addBindValue(sessionId);
    query.addBindValue(packetData["number"].toInt());
    query.addBindValue(QDateTime::currentMSecsSinceEpoch() * 1000000); // Convert to nanoseconds
    query.addBindValue(packetData["length"].toString().toInt());
    query.addBindValue(packetData["protocol"].toString());
    query.addBindValue(packetData["source"].toString());
    query.addBindValue(packetData["dest"].toString());
    query.addBindValue(packetData["source_port"].toInt(0));
    query.addBindValue(packetData["dest_port"].toInt(0));
    query.addBindValue(packetData["info"].toString());
    
    return query.exec();
}

bool DatabaseManager::setUserPreference(int userId, const QString& key, const QVariant& value)
{
    QSqlQuery query(m_database);
    query.prepare("INSERT OR REPLACE INTO user_preferences (user_id, preference_key, preference_value, updated_at) VALUES (?, ?, ?, CURRENT_TIMESTAMP)");
    query.addBindValue(userId);
    query.addBindValue(key);
    query.addBindValue(value.toString());
    
    return query.exec();
}

QVariant DatabaseManager::getUserPreference(int userId, const QString& key, const QVariant& defaultValue)
{
    QSqlQuery query(m_database);
    query.prepare("SELECT preference_value FROM user_preferences WHERE user_id = ? AND preference_key = ?");
    query.addBindValue(userId);
    query.addBindValue(key);
    
    if (query.exec() && query.next()) {
        return query.value(0);
    }
    
    return defaultValue;
}

bool DatabaseManager::saveFilterPreset(int userId, const QString& name, const QString& expression, const QString& description)
{
    QSqlQuery query(m_database);
    query.prepare("INSERT INTO filter_presets (user_id, name, filter_expression, description, is_public) VALUES (?, ?, ?, ?, 1)");
    query.addBindValue(userId);
    query.addBindValue(name);
    query.addBindValue(expression);
    query.addBindValue(description);
    
    return query.exec();
}

QString DatabaseManager::hashPassword(const QString& password, const QString& salt)
{
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData((password + salt).toUtf8());
    return hash.result().toHex();
}

QString DatabaseManager::generateSalt()
{
    QByteArray salt;
    for (int i = 0; i < 16; ++i) {
        salt.append(static_cast<char>(QRandomGenerator::global()->bounded(256)));
    }
    return salt.toHex();
}