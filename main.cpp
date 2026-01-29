#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QIcon>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>
#include <QFileInfo>

// Backend includes
#include "src/database/DatabaseManager.h"
#include "src/core/PacketCaptureEngine.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    
    // Set application properties
    app.setApplicationName("Packet Analyzer 2026");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("Enterprise Network Solutions");
    app.setApplicationDisplayName("Packet Analyzer 2026 - Enterprise Edition");
    
    qDebug() << "🚀 Starting Packet Analyzer 2026...";
    
    // Initialize database
    QString dbPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/packet_analyzer.db";
    QDir().mkpath(QFileInfo(dbPath).absolutePath());
    
    qDebug() << "📊 Database path:" << dbPath;
    
    DatabaseManager& dbManager = DatabaseManager::instance();
    if (!dbManager.initialize(dbPath)) {
        qWarning() << "⚠️ Database initialization failed, continuing without database features";
        // Don't exit, continue without database
    } else {
        qDebug() << "✅ Database initialized successfully";
    }
    
    // Create backend instances
    PacketCaptureEngine captureEngine;
    qDebug() << "✅ PacketCaptureEngine created";
    
    // Register QML types
    qmlRegisterSingletonType<DatabaseManager>("PacketAnalyzer", 1, 0, "DatabaseManager", 
        [](QQmlEngine*, QJSEngine*) -> QObject* {
            return &DatabaseManager::instance();
        });
    
    qmlRegisterType<PacketCaptureEngine>("PacketAnalyzer", 1, 0, "PacketCaptureEngine");
    qDebug() << "✅ QML types registered";
    
    // Create QML engine
    QQmlApplicationEngine engine;
    
    // Connect to QML warnings and errors
    QObject::connect(&engine, &QQmlApplicationEngine::warnings, [](const QList<QQmlError> &warnings) {
        for (const auto &warning : warnings) {
            qWarning() << "QML Warning:" << warning.toString();
        }
    });
    
    // Add import paths
    engine.addImportPath("qrc:/");
    engine.addImportPath(":/");
    
    // ✅ EXPOSE REAL BACKEND INSTANCES TO QML
    engine.rootContext()->setContextProperty("PacketCaptureEngine", &captureEngine);
    engine.rootContext()->setContextProperty("DatabaseManager", &DatabaseManager::instance());
    engine.rootContext()->setContextProperty("applicationVersion", app.applicationVersion());
    engine.rootContext()->setContextProperty("applicationName", app.applicationName());
    
    qDebug() << "✅ Context properties set";
    
    // Load main QML file
    const QUrl url(QStringLiteral("qrc:/complete_interface.qml"));
    qDebug() << "🎯 Loading FULL QML from:" << url;
    
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl) {
            qCritical() << "❌ Failed to create QML object for:" << objUrl;
            QCoreApplication::exit(-1);
        } else if (obj && url == objUrl) {
            qDebug() << "✅ QML object created successfully";
        }
    }, Qt::QueuedConnection);
    
    engine.load(url);
    
    // Check if QML loaded successfully
    if (engine.rootObjects().isEmpty()) {
        qCritical() << "❌ Failed to load QML interface - no root objects created";
        return -1;
    }
    
    qDebug() << "✅ QML interface loaded successfully";
    qDebug() << "🚀 Packet Analyzer 2026 started successfully";
    qDebug() << "📊 Database:" << dbPath;
    qDebug() << "🎯 Version:" << app.applicationVersion();
    
    return app.exec();
}