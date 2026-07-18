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

#ifdef Q_OS_WIN
#include <windows.h>
#endif

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

    QDir parentDir(appDataLocation);
    QFileInfo parentDirInfo(parentDir.absolutePath());

    if (!appDataDirObj.exists()) {
        bool success = appDataDirObj.mkpath(".");

        if (!success) {

            QString docsDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/NFSMusicEd";

            QDir docsDirObj(docsDir);
            if (!docsDirObj.exists()) {
                bool docsSuccess = docsDirObj.mkpath(".");

                if (docsSuccess) {

                    appDataDir = docsDir;
                } else {
                    qCritical() << "Failed to create application data directory:" << appDataDir;
                    qCritical() << "Also failed to create in Documents directory:" << docsDir;
                    QMessageBox::critical(nullptr, "Fatal Error",
                                          "Failed to create application data directory. Application cannot start." + 
                                          QString("\n\nDetails:\nAppDataLocation: %1\nDocsLocation: %2").arg(appDataLocation, docsDir));
                    return -1;
                }
            }
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


        int result = app.exec();

#ifdef Q_OS_WIN
        if (hMutex) {
            CloseHandle(hMutex);
        }
#endif

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

