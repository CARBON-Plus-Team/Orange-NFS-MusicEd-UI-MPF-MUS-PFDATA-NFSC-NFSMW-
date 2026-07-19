#include "MainWindow.h"
#include <QApplication>
#include <QDir>
#include <QMessageBox>
#include <QTranslator>
#include <QLocale>
#include <QIcon>
#include <QStyleFactory>
#include <QSettings>
#include <QStandardPaths>
#include <QDebug>
#include <QScreen>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QSysInfo>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

static QFile* g_logFile = nullptr;
static QTextStream* g_logStream = nullptr;
static QString g_logSessionId;

QString generateSessionId()
{
    return QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss") + "_" +
           QString::number(QCoreApplication::applicationPid());
}

QString logLevelToString(QtMsgType type)
{
    switch (type) {
    case QtDebugMsg:    return "DEBUG";
    case QtInfoMsg:     return "INFO";
    case QtWarningMsg:  return "WARN";
    case QtCriticalMsg: return "ERROR";
    case QtFatalMsg:    return "FATAL";
    default:            return "UNKNOWN";
    }
}

void logMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    QString level = logLevelToString(type);
    QString sessionId = g_logSessionId;
    QString source = context.file ? QFileInfo(context.file).fileName() + ":" + QString::number(context.line) : "system";

    QString logLine = QString("[%1] [%2] [%3] [%4] %5\n")
                          .arg(timestamp, level, sessionId, source, msg);

    if (g_logStream) {
        (*g_logStream) << logLine;
        g_logStream->flush();
    }

    fprintf(stderr, "%s", logLine.toUtf8().constData());
}

void initFileLogger()
{
    g_logSessionId = generateSessionId();

    QString logDir = QApplication::applicationDirPath() + "/Logs";
    QDir dir;
    if (!dir.exists(logDir)) {
        dir.mkpath(logDir);
    }

    QString logFileName = logDir + "/app_" + g_logSessionId + ".log";
    g_logFile = new QFile(logFileName);
    if (g_logFile->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
        g_logStream = new QTextStream(g_logFile);
        qInstallMessageHandler(logMessageHandler);

        qDebug().noquote() << "============================================================";
        qDebug().noquote() << "  Orange NFS MusicEd UI - Application Log";
        qDebug().noquote() << "============================================================";
        qDebug().noquote() << QString("Session ID : %1").arg(g_logSessionId);
        qDebug().noquote() << QString("Log File   : %1").arg(logFileName);
        qDebug().noquote() << QString("App Dir    : %1").arg(QApplication::applicationDirPath());
        qDebug().noquote() << QString("OS         : %1").arg(QSysInfo::prettyProductName());
        qDebug().noquote() << QString("OS Version : %1").arg(QSysInfo::productVersion());
        qDebug().noquote() << QString("CPU Arch   : %1").arg(QSysInfo::currentCpuArchitecture());
        qDebug().noquote() << QString("Qt Version : %1").arg(qVersion());
        qDebug().noquote() << "============================================================";
        qDebug() << "Logger initialized successfully";
    } else {
        delete g_logFile;
        g_logFile = nullptr;
    }
}

int main(int argc, char *argv[])
{

#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif

    QApplication app(argc, argv);

    initFileLogger();

    QString appIconPath = QApplication::applicationDirPath() + "/app.ico";
    if (QFile::exists(appIconPath)) {
        app.setWindowIcon(QIcon(appIconPath));
    } else {

        appIconPath = QCoreApplication::applicationDirPath() + "/../app.ico";
        if (QFile::exists(appIconPath)) {
            app.setWindowIcon(QIcon(appIconPath));
        }
    }

    app.setApplicationName("Orange NFS MusicEd UI By:Shynian");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("Orange NFSMusicEd");
    app.setOrganizationDomain("nfsmusiced.com");

    app.setStyle(QStyleFactory::create("Fusion"));

    QString appDataLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString appDataDir = appDataLocation + "/NFSMusicEd";
    QDir appDataDirObj(appDataDir);

    if (!appDataDirObj.exists()) {
        if (!appDataDirObj.mkpath(".")) {
            QString docsDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/NFSMusicEd";
            QDir docsDirObj(docsDir);
            if (!docsDirObj.exists() && !docsDirObj.mkpath(".")) {
                qCritical() << "Failed to create app data directory:" << appDataDir;
                QMessageBox::critical(nullptr, "Fatal Error",
                    QString("Failed to create application data directory.\n\nAppDataLocation: %1\nDocsLocation: %2")
                        .arg(appDataLocation, docsDir));
                return -1;
            }
            appDataDir = docsDir;
        }
    }

    QString tempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/NFSMusicEd_temp";
    QDir tempDirObj(tempDir);
    if (!tempDirObj.exists()) {
        if (!tempDirObj.mkpath(".")) {
            QString fallbackTemp = QDir::tempPath() + "/NFSMusicEd_temp";
            QDir fallbackObj(fallbackTemp);
            if (!fallbackObj.exists() && !fallbackObj.mkpath(".")) {
                qCritical() << "Failed to create temp directory:" << tempDir;
                qCritical() << "Also failed fallback temp:" << fallbackTemp;
                QMessageBox::critical(nullptr, "Fatal Error",
                                      "Failed to create temporary directory. Application cannot start.\n\n"
                                      "Tried paths:\n" + tempDir + "\n" + fallbackTemp);
                return -1;
            }
            tempDir = fallbackTemp;
        }
    }

    QString asfDir = tempDir + "/asf";
    QString wavDir = tempDir + "/wav";
    QString musTempDir = tempDir + "/MUS_Temp";
    QString txtDir = tempDir + "/txt";

    QDir(asfDir).mkpath(".");
    QDir(wavDir).mkpath(".");
    QDir(musTempDir).mkpath(".");
    QDir(txtDir).mkpath(".");

    Q_UNUSED(appDataDir);

    bool isAnotherInstanceRunning = false;

#ifdef Q_OS_WIN
    QString appKey = "NFSMusicEdAppInstance";
    HANDLE hMutex = CreateMutexA(NULL, TRUE, appKey.toLocal8Bit().constData());
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        isAnotherInstanceRunning = true;
        CloseHandle(hMutex);
    }
#endif

    if (isAnotherInstanceRunning) {
        QMessageBox::warning(nullptr, QObject::tr("Application Already Running"),
                             QObject::tr("NFS MusicEd is already running. Please check your system tray or task manager."));
        return 0;
    }

    try {

        MainWindow window;

        QSize defaultSize(1000, 700);
        window.resize(defaultSize);

        QScreen *primaryScreen = app.primaryScreen();
        if (primaryScreen) {
            QRect screenGeometry = primaryScreen->availableGeometry();
            int x = (screenGeometry.width() - window.width()) / 2;
            int y = (screenGeometry.height() - window.height()) / 2;
            window.move(x, y);
        }

        window.show();
        window.raise();
        window.activateWindow();

        qDebug() << "Application started successfully";
        qDebug() << "Application directory:" << QApplication::applicationDirPath();
        qDebug() << "Temp directory:" << tempDir;
        qDebug() << "App data directory:" << appDataDir;

        int result = app.exec();

#ifdef Q_OS_WIN
        if (hMutex) {
            CloseHandle(hMutex);
        }
#endif

        qDebug() << "Application exited with code:" << result;
        return result;

    } catch (const std::exception& e) {
        qCritical() << "Fatal error:" << e.what();
        QMessageBox::critical(nullptr, "Fatal Error",
                              QString("A fatal error occurred: %1").arg(e.what()));
        return -1;
    } catch (...) {
        qCritical() << "Unknown fatal error";
        QMessageBox::critical(nullptr, "Fatal Error",
                              "An unknown fatal error occurred.");
        return -1;
    }
}