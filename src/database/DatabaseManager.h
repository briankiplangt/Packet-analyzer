#pragma once

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QObject>
#include <QVariant>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonDocument>

class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    static DatabaseManager& instance();
    
    bool initialize(const QString& dbPath = "packet_analyzer.db");
    bool isConnected() const;
    
    // User Management
    bool createUser(const QString& username, const QString& password, const QString& role = "viewer");
    bool authenticateUser(const QString& username, const QString& password);
    QJsonObject getUserInfo(const QString& username);
    
    // Capture Sessions
    int createCaptureSession(int userId, const QString& sessionName, const QString& interface);
    bool updateCaptureSession(int sessionId, const QJsonObject& updates);
    bool endCaptureSession(int sessionId, int totalPackets, qint64 totalBytes);
    QJsonArray getCaptureSessionHistory(int userId);
    
    // Packet Metadata
    bool insertPacketMetadata(int sessionId, const QJsonObject& packetData);
    QJsonArray getPacketMetadata(int sessionId, int limit = 1000, int offset = 0);
    
    // Statistics
    QJsonObject getProtocolStatistics(int sessionId);
    QJsonObject getSessionStatistics(int sessionId);
    
    // Preferences
    bool setUserPreference(int userId, const QString& key, const QVariant& value);
    QVariant getUserPreference(int userId, const QString& key, const QVariant& defaultValue = QVariant());
    
    // Filter Presets
    bool saveFilterPreset(int userId, const QString& name, const QString& expression, const QString& description = "");
    QJsonArray getFilterPresets(int userId, bool includePublic = true);
    
    // Audit Logging
    bool logAuditEvent(int userId, const QString& action, const QString& resource = "", const QJsonObject& details = QJsonObject());
    
    // Performance Metrics
    bool recordPerformanceMetric(const QString& metricName, double value, int sessionId = -1);
    QJsonArray getPerformanceMetrics(const QString& metricName, const QDateTime& since);

private:
    DatabaseManager() = default;
    ~DatabaseManager() = default;
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;
    
    bool createTables();
    bool executeSqlFile(const QString& filePath);
    QString hashPassword(const QString& password, const QString& salt);
    QString generateSalt();
    
    QSqlDatabase m_database;
    bool m_initialized = false;

signals:
    void databaseError(const QString& error);
    void userAuthenticated(int userId, const QString& username);
};