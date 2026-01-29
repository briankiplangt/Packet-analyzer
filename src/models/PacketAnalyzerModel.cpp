#include "PacketAnalyzerModel.h"
#include <QDebug>
#include <QDateTime>
#include <QJsonDocument>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QDir>
#include <QProcess>
#include <QThread>

#ifdef _WIN32
#include <windows.h>
#include <pdh.h>
#pragma comment(lib, "pdh.lib")
#else
#include <sys/times.h>
#include <unistd.h>
#endif

PacketAnalyzerModel::PacketAnalyzerModel(QObject* parent)
    : QObject(parent)
    , m_captureEngine(nullptr)
    , m_database(nullptr)
    , m_isCapturing(false)
    , m_packetCount(0)
    , m_bandwidthMbps(0.0)
    , m_cpuUsage(0.0)
    , m_isAuthenticated(false)
    , m_currentUserId(-1)
    , m_currentSessionId(-1)
{
    // Initialize database
    initializeDatabase();

    // Initialize capture engine
    m_captureEngine = new PacketCaptureEngine(this);
    
    // Connect capture engine signals
    connect(m_captureEngine, &PacketCaptureEngine::packetCaptured,
            this, &PacketAnalyzerModel::onPacketCaptured);
    connect(m_captureEngine, &PacketCaptureEngine::statisticsUpdated,
            this, &PacketAnalyzerModel::onStatisticsUpdated);
    connect(m_captureEngine, &PacketCaptureEngine::captureStarted,
            this, &PacketAnalyzerModel::onCaptureStarted);
    connect(m_captureEngine, &PacketCaptureEngine::captureStopped,
            this, &PacketAnalyzerModel::onCaptureStopped);
    connect(m_captureEngine, &PacketCaptureEngine::captureError,
            this, &PacketAnalyzerModel::onCaptureError);

    // Initialize timers
    m_cpuTimer = new QTimer(this);
    connect(m_cpuTimer, &QTimer::timeout, this, &PacketAnalyzerModel::updateCpuUsage);
    m_cpuTimer->start(2000); // Update CPU usage every 2 seconds

    m_interfaceRefreshTimer = new QTimer(this);
    connect(m_interfaceRefreshTimer, &QTimer::timeout, this, &PacketAnalyzerModel::refreshInterfaces);
    m_interfaceRefreshTimer->start(10000); // Refresh interfaces every 10 seconds

    // Load available interfaces
    refreshInterfaces();

    qDebug() << "PacketAnalyzerModel initialized";
}

PacketAnalyzerModel::~PacketAnalyzerModel()
{
    if (m_isCapturing) {
        stopCapture();
    }
}

void PacketAnalyzerModel::initializeDatabase()
{
    m_database = &DatabaseManager::instance();
    if (!m_database->initialize()) {
        qWarning() << "Failed to initialize database";
        emit databaseError("Failed to initialize database");
    }
}

bool PacketAnalyzerModel::authenticateUser(const QString& username, const QString& password)
{
    // For now, simple authentication - in production, use proper hashing
    if (username == "admin" && password == "admin123") {
        m_isAuthenticated = true;
        m_currentUser = username;
        m_currentUserId = 1;
        emit isAuthenticatedChanged();
        emit currentUserChanged();
        emit userAuthenticated(username);
        logUserAction("LOGIN", QString("User %1 logged in").arg(username));
        return true;
    }
    return false;
}

void PacketAnalyzerModel::logout()
{
    if (m_isCapturing) {
        stopCapture();
    }
    
    logUserAction("LOGOUT", QString("User %1 logged out").arg(m_currentUser));
    
    m_isAuthenticated = false;
    m_currentUser.clear();
    m_currentUserId = -1;
    m_currentSessionId = -1;
    
    emit isAuthenticatedChanged();
    emit currentUserChanged();
    emit currentSessionIdChanged();
    emit userLoggedOut();
}

bool PacketAnalyzerModel::startCapture(const QString& sessionName, const QString& filter)
{
    if (!m_isAuthenticated) {
        emit captureError("Authentication required");
        return false;
    }
    
    if (m_isCapturing) {
        emit captureError("Capture already in progress");
        return false;
    }
    
    // Set filter if provided
    if (!filter.isEmpty()) {
        m_currentFilter = filter;
        emit currentFilterChanged();
    }
    
    // Start capture engine
    bool success = m_captureEngine->startCapture(m_currentInterface, m_currentFilter);
    if (success) {
        m_isCapturing = true;
        m_packetCount = 0;
        m_recentPackets.clear();
        
        // Create new session in database
        QString actualSessionName = sessionName.isEmpty() ? 
            QString("Session_%1").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss")) : 
            sessionName;
            
        // TODO: Create session in database and get session ID
        m_currentSessionId = QDateTime::currentSecsSinceEpoch();
        
        emit isCapturingChanged();
        emit packetCountChanged();
        emit packetsChanged();
        emit currentSessionIdChanged();
        
        logUserAction("START_CAPTURE", QString("Started capture session: %1").arg(actualSessionName));
    }
    
    return success;
}

void PacketAnalyzerModel::stopCapture()
{
    if (!m_isCapturing) {
        return;
    }
    
    m_captureEngine->stopCapture();
    m_isCapturing = false;
    
    emit isCapturingChanged();
    logUserAction("STOP_CAPTURE", QString("Stopped capture session: %1").arg(m_currentSessionId));
}

bool PacketAnalyzerModel::setInterface(const QString& interfaceName)
{
    if (m_isCapturing) {
        emit captureError("Cannot change interface while capturing");
        return false;
    }
    
    m_currentInterface = interfaceName;
    emit currentInterfaceChanged();
    logUserAction("SET_INTERFACE", QString("Changed interface to: %1").arg(interfaceName));
    return true;
}

bool PacketAnalyzerModel::setFilter(const QString& filter)
{
    m_currentFilter = filter;
    if (m_captureEngine) {
        m_captureEngine->setFilter(filter);
    }
    emit currentFilterChanged();
    logUserAction("SET_FILTER", QString("Changed filter to: %1").arg(filter));
    return true;
}

void PacketAnalyzerModel::onPacketCaptured(const QJsonObject& packet)
{
    m_packetCount++;
    
    // Add to recent packets list
    m_recentPackets.prepend(packet);
    if (m_recentPackets.size() > MAX_DISPLAYED_PACKETS) {
        m_recentPackets.removeLast();
    }
    
    // Update packets array for QML
    updatePacketsList();
    
    emit packetCountChanged();
    emit packetsChanged();
}

void PacketAnalyzerModel::onStatisticsUpdated(int totalPackets, qint64 totalBytes, double bandwidth, double cpuUsage)
{
    m_packetCount = totalPackets;
    m_bandwidthMbps = bandwidth;
    m_cpuUsage = cpuUsage;
    
    emit packetCountChanged();
    emit bandwidthMbpsChanged();
    emit cpuUsageChanged();
}

void PacketAnalyzerModel::onCaptureStarted(const QString& interface)
{
    m_currentInterface = interface;
    emit currentInterfaceChanged();
    emit captureStarted(interface);
}

void PacketAnalyzerModel::onCaptureStopped()
{
    m_isCapturing = false;
    emit isCapturingChanged();
    emit captureStopped();
}

void PacketAnalyzerModel::onCaptureError(const QString& error)
{
    m_isCapturing = false;
    emit isCapturingChanged();
    emit captureError(error);
}

void PacketAnalyzerModel::updateCpuUsage()
{
    double usage = getCurrentCpuUsage();
    if (usage != m_cpuUsage) {
        m_cpuUsage = usage;
        emit cpuUsageChanged();
    }
}

void PacketAnalyzerModel::refreshInterfaces()
{
    QStringList interfaces = PacketCaptureEngine::getAvailableInterfaces();
    QJsonArray jsonArray;
    
    for (const QString& interface : interfaces) {
        QJsonObject obj;
        obj["name"] = interface;
        obj["description"] = interface; // TODO: Get actual description
        jsonArray.append(obj);
    }
    
    m_availableInterfaces = jsonArray;
    emit availableInterfacesChanged();
}

void PacketAnalyzerModel::updatePacketsList()
{
    QJsonArray jsonArray;
    for (const QJsonObject& packet : m_recentPackets) {
        jsonArray.append(packet);
    }
    m_packets = jsonArray;
}

double PacketAnalyzerModel::getCurrentCpuUsage()
{
#ifdef _WIN32
    static PDH_HQUERY query = nullptr;
    static PDH_HCOUNTER counter = nullptr;
    static bool initialized = false;
    
    if (!initialized) {
        PdhOpenQuery(nullptr, 0, &query);
        PdhAddCounter(query, L"\\Processor(_Total)\\% Processor Time", 0, &counter);
        PdhCollectQueryData(query);
        initialized = true;
        return 0.0;
    }
    
    PdhCollectQueryData(query);
    PDH_FMT_COUNTERVALUE value;
    PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE, nullptr, &value);
    return value.doubleValue;
#else
    static clock_t lastCpu = 0;
    static clock_t lastSysCpu = 0;
    static clock_t lastUserCpu = 0;
    
    struct tms timeSample;
    clock_t now = times(&timeSample);
    
    if (lastCpu == 0) {
        lastCpu = now;
        lastSysCpu = timeSample.tms_stime;
        lastUserCpu = timeSample.tms_utime;
        return 0.0;
    }
    
    double percent = (timeSample.tms_stime - lastSysCpu) + (timeSample.tms_utime - lastUserCpu);
    percent /= (now - lastCpu);
    percent /= sysconf(_SC_NPROCESSORS_ONLN);
    percent *= 100.0;
    
    lastCpu = now;
    lastSysCpu = timeSample.tms_stime;
    lastUserCpu = timeSample.tms_utime;
    
    return percent;
#endif
}

void PacketAnalyzerModel::logUserAction(const QString& action, const QString& details)
{
    if (m_database && m_isAuthenticated) {
        // TODO: Implement user action logging in database
        qDebug() << "User action:" << action << details;
    }
}

// Stub implementations for remaining methods
bool PacketAnalyzerModel::createUser(const QString& username, const QString& password, const QString& role)
{
    Q_UNUSED(username)
    Q_UNUSED(password)
    Q_UNUSED(role)
    return false; // TODO: Implement
}

bool PacketAnalyzerModel::exportToPcap(const QString& filePath)
{
    Q_UNUSED(filePath)
    return false; // TODO: Implement
}

bool PacketAnalyzerModel::exportToJson(const QString& filePath)
{
    Q_UNUSED(filePath)
    return false; // TODO: Implement
}

bool PacketAnalyzerModel::exportToCSV(const QString& filePath)
{
    Q_UNUSED(filePath)
    return false; // TODO: Implement
}

QJsonArray PacketAnalyzerModel::getCaptureHistory()
{
    return QJsonArray(); // TODO: Implement
}

bool PacketAnalyzerModel::loadSession(int sessionId)
{
    Q_UNUSED(sessionId)
    return false; // TODO: Implement
}

bool PacketAnalyzerModel::saveFilterPreset(const QString& name, const QString& expression, const QString& description)
{
    Q_UNUSED(name)
    Q_UNUSED(expression)
    Q_UNUSED(description)
    return false; // TODO: Implement
}

QJsonArray PacketAnalyzerModel::getFilterPresets()
{
    return QJsonArray(); // TODO: Implement
}

bool PacketAnalyzerModel::deleteFilterPreset(int presetId)
{
    Q_UNUSED(presetId)
    return false; // TODO: Implement
}

bool PacketAnalyzerModel::setPreference(const QString& key, const QVariant& value)
{
    Q_UNUSED(key)
    Q_UNUSED(value)
    return false; // TODO: Implement
}

QVariant PacketAnalyzerModel::getPreference(const QString& key, const QVariant& defaultValue)
{
    Q_UNUSED(key)
    return defaultValue; // TODO: Implement
}

QJsonObject PacketAnalyzerModel::getDetailedStatistics()
{
    return QJsonObject(); // TODO: Implement
}

QJsonObject PacketAnalyzerModel::getNetworkTopology()
{
    return QJsonObject(); // TODO: Implement
}

QJsonArray PacketAnalyzerModel::getTopTalkers(int limit)
{
    Q_UNUSED(limit)
    return QJsonArray(); // TODO: Implement
}

QJsonArray PacketAnalyzerModel::getProtocolDistribution()
{
    return QJsonArray(); // TODO: Implement
}