#pragma once

#include <QObject>
#include <QAbstractListModel>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include "../core/PacketCaptureEngine.h"
#include "../database/DatabaseManager.h"

class PacketAnalyzerModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isCapturing READ isCapturing NOTIFY isCapturingChanged)
    Q_PROPERTY(int packetCount READ packetCount NOTIFY packetCountChanged)
    Q_PROPERTY(double bandwidthMbps READ bandwidthMbps NOTIFY bandwidthMbpsChanged)
    Q_PROPERTY(double cpuUsage READ cpuUsage NOTIFY cpuUsageChanged)
    Q_PROPERTY(QString currentInterface READ currentInterface NOTIFY currentInterfaceChanged)
    Q_PROPERTY(QString currentFilter READ currentFilter NOTIFY currentFilterChanged)
    Q_PROPERTY(QJsonArray availableInterfaces READ availableInterfaces NOTIFY availableInterfacesChanged)
    Q_PROPERTY(QJsonArray packets READ packets NOTIFY packetsChanged)
    Q_PROPERTY(QJsonObject protocolStatistics READ protocolStatistics NOTIFY protocolStatisticsChanged)
    Q_PROPERTY(bool isAuthenticated READ isAuthenticated NOTIFY isAuthenticatedChanged)
    Q_PROPERTY(QString currentUser READ currentUser NOTIFY currentUserChanged)
    Q_PROPERTY(int currentSessionId READ currentSessionId NOTIFY currentSessionIdChanged)

public:
    explicit PacketAnalyzerModel(QObject* parent = nullptr);
    ~PacketAnalyzerModel();

    // Authentication
    Q_INVOKABLE bool authenticateUser(const QString& username, const QString& password);
    Q_INVOKABLE void logout();
    Q_INVOKABLE bool createUser(const QString& username, const QString& password, const QString& role = "viewer");

    // Capture control
    Q_INVOKABLE bool startCapture(const QString& sessionName = "", const QString& filter = "");
    Q_INVOKABLE void stopCapture();
    Q_INVOKABLE bool setInterface(const QString& interfaceName);
    Q_INVOKABLE bool setFilter(const QString& filter);

    // Data export
    Q_INVOKABLE bool exportToPcap(const QString& filePath);
    Q_INVOKABLE bool exportToJson(const QString& filePath);
    Q_INVOKABLE bool exportToCSV(const QString& filePath);

    // Session management
    Q_INVOKABLE QJsonArray getCaptureHistory();
    Q_INVOKABLE bool loadSession(int sessionId);

    // Filter presets
    Q_INVOKABLE bool saveFilterPreset(const QString& name, const QString& expression, const QString& description = "");
    Q_INVOKABLE QJsonArray getFilterPresets();
    Q_INVOKABLE bool deleteFilterPreset(int presetId);

    // User preferences
    Q_INVOKABLE bool setPreference(const QString& key, const QVariant& value);
    Q_INVOKABLE QVariant getPreference(const QString& key, const QVariant& defaultValue = QVariant());

    // Statistics and analysis
    Q_INVOKABLE QJsonObject getDetailedStatistics();
    Q_INVOKABLE QJsonObject getNetworkTopology();
    Q_INVOKABLE QJsonArray getTopTalkers(int limit = 10);
    Q_INVOKABLE QJsonArray getProtocolDistribution();

    // Property getters
    bool isCapturing() const { return m_isCapturing; }
    int packetCount() const { return m_packetCount; }
    double bandwidthMbps() const { return m_bandwidthMbps; }
    double cpuUsage() const { return m_cpuUsage; }
    QString currentInterface() const { return m_currentInterface; }
    QString currentFilter() const { return m_currentFilter; }
    QJsonArray availableInterfaces() const { return m_availableInterfaces; }
    QJsonArray packets() const { return m_packets; }
    QJsonObject protocolStatistics() const { return m_protocolStatistics; }
    bool isAuthenticated() const { return m_isAuthenticated; }
    QString currentUser() const { return m_currentUser; }
    int currentSessionId() const { return m_currentSessionId; }

private slots:
    void onPacketCaptured(const QJsonObject& packet);
    void onStatisticsUpdated(int totalPackets, qint64 totalBytes, double bandwidth, double cpuUsage);
    void onCaptureStarted(const QString& interface);
    void onCaptureStopped();
    void onCaptureError(const QString& error);
    void updateCpuUsage();
    void refreshInterfaces();

private:
    void initializeDatabase();
    void updatePacketsList();
    void savePacketToDatabase(const PacketInfo& packet);
    QJsonObject packetInfoToJson(const PacketInfo& packet);
    double getCurrentCpuUsage();
    void logUserAction(const QString& action, const QString& details = "");

    // Core components
    PacketCaptureEngine* m_captureEngine;
    DatabaseManager* m_database;

    // State properties
    bool m_isCapturing;
    int m_packetCount;
    double m_bandwidthMbps;
    double m_cpuUsage;
    QString m_currentInterface;
    QString m_currentFilter;
    QJsonArray m_availableInterfaces;
    QJsonArray m_packets;
    QJsonObject m_protocolStatistics;
    bool m_isAuthenticated;
    QString m_currentUser;
    int m_currentUserId;
    int m_currentSessionId;

    // Timers
    QTimer* m_cpuTimer;
    QTimer* m_interfaceRefreshTimer;

    // Packet storage
    QList<QJsonObject> m_recentPackets;
    static const int MAX_DISPLAYED_PACKETS = 1000;

signals:
    void isCapturingChanged();
    void packetCountChanged();
    void bandwidthMbpsChanged();
    void cpuUsageChanged();
    void currentInterfaceChanged();
    void currentFilterChanged();
    void availableInterfacesChanged();
    void packetsChanged();
    void protocolStatisticsChanged();
    void isAuthenticatedChanged();
    void currentUserChanged();
    void currentSessionIdChanged();
    
    // User notifications
    void captureStarted(const QString& interface);
    void captureStopped();
    void captureError(const QString& error);
    void userAuthenticated(const QString& username);
    void userLoggedOut();
    void databaseError(const QString& error);
    void exportCompleted(const QString& filePath);
    void exportFailed(const QString& error);
};