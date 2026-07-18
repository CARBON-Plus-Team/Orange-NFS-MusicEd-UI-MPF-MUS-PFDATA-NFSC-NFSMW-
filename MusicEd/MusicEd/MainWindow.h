#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QTranslator>
#include <QString>
#include <QProcess>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QTimer>
#include <QProgressDialog>

class QListWidget;
class QListWidgetItem;
class QPushButton;
class QLabel;
class QTextEdit;

class QComboBox;
class QSlider;
class QStackedWidget;
class QGroupBox;
class QVBoxLayout;
class QHBoxLayout;
class QEvent;
class QWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;
    void changeEvent(QEvent *event) override;

private slots:
    void onSelectMusFile();
    void onExtractMusicFiles();
    void onConvertAllASF();
    void onOneClickFullConvert();
    void onConvertSingleASFtoWAV();
    void onPlaySelected();
    void onStopPlayback();
    void onReplaceMusic();
    void onConvertToASF();
    void onConvertToMUS();
    void onBackToMain();
    void onHowToUse();

    void onExtractFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onExtractError(QProcess::ProcessError error);
    void onDecompileFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onDecompileError(QProcess::ProcessError error);
    void onGenerateEventFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onGenerateEventError(QProcess::ProcessError error);

    void onASFtoWAVConversionFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onWAVtoASFConversionFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onASFtoMUSConversionFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onConversionError(QProcess::ProcessError error);
    void onConversionOutputReady();
    void onConversionProgressCanceled();

    void onLanguageChanged(int index);
    void onAsfFileSelected(QListWidgetItem *item);
    void onWavFileSelected(QListWidgetItem *item);
    void onVolumeChanged(int volume);

    void onProgressSliderMoved(int position);
    void onMediaPositionChanged(qint64 position);
    void onMediaDurationChanged(qint64 duration);
    void onPlaybackStateChanged(QMediaPlayer::PlaybackState state);
    void updatePlaybackProgress();

private:
    void setupUI();
    void applyDarkOrangeTheme();
    void setupConnections();
    void setupMainPage();
    void setupFileManagementPage();
    void setupPlayerControls(QVBoxLayout *parentLayout);
    void initializeMediaComponents();
    void loadLanguage(const QString &languageCode);
    void checkTools();
    void retranslateUI();
    void saveMusFilePath(const QString &musPath);
    void loadLastMusFile();

    void autoDecompileMpf(const QString &mpfFilePath);
    void extractMusFile(const QString &mpfFilePath, const QString &musFilePath,
                        const QString &outputDir);

    bool convertASFtoWAV(const QString &asfFilePath, const QString &wavFilePath);
    bool convertWAVtoASF(const QString &wavFilePath, const QString &asfFilePath);
    bool runBatchConversion(const QString &batFileName);

    void convertNextASFtoWAV();
    void convertNextWAVtoASF();
    void moveFinalFiles();

    void packAllAsfToMus();

    bool copyResourceToTemp(const QString &resourcePath, const QString &fileName);
    void updateFileList();
    void clearTempFiles();
    void clearTempFilesExceptBats();
    QString findMatchingMpfFile(const QString &musFilePath);
    bool ensureAsfDirectoryExists();
    bool ensureWavDirectoryExists();
    bool ensureTxtDirectoryExists();
    QString getAsfDirectoryPath() const;

    void showConversionProgressDialog(const QString &title, int maxValue);
    void updateConversionProgress(int value, const QString &text = QString());
    void hideConversionProgressDialog();

    void updateAsfCountLabel(int count);
    void updateWavCountLabel(int count);
    void updateStatusLabel(const QString &message, bool isError = false);

    QString formatTime(qint64 ms) const;
    void appendLog(const QString &message, bool isError = false);


    void runGenerateEventOnDecompFiles(); 
    void processNextGenerateEventFile();
    QString identifierForNumber(const QString &number);

    QStackedWidget *stackedWidget;
    QWidget *mainPage;
    QWidget *fileManagementPage;
    QPushButton *selectMusButton;
    QLabel *selectedFileLabel;
    QPushButton *extractButton;
    QPushButton *oneClickConvertButton;
    QPushButton *singleConvertButton;
    QLabel *languageLabel;
    QComboBox *languageComboBox;

    QListWidget *fileListWidget;
    QListWidget *asfListWidget;
    QListWidget *wavListWidget;
    QPushButton *convertAllButton;
    QPushButton *playButton;
    QPushButton *stopButton;
    QPushButton *replaceButton;
    QPushButton *convertToASFButton;
    QPushButton *convertToMUSButton;
    QPushButton *backButton;
    QPushButton *howToUseButton;

    QMediaPlayer *mediaPlayer;
    QAudioOutput *audioOutput;
    QSlider *volumeSlider;
    QSlider *progressSlider;
    QLabel *currentTimeLabel;
    QLabel *totalTimeLabel;
    QTimer *progressTimer;
    
    QLabel *statusLabel;
    QLabel *asfCountLabel;
    QLabel *wavCountLabel;
    QTextEdit *logTextEdit;

    QProcess *extractProcess;
    QProcess *conversionProcess;
    QProcess *decompileProcess;
    QProcess *generateEventProcess;
    QTimer *mpfDecompileTimer;
    QProgressDialog *conversionProgressDialog;
    QProgressDialog *unpackProgressDialog;
    QTranslator *appTranslator;
    QTranslator *qtTranslator;
    QStringList pendingGenerateEventFiles;
    QString currentGenerateEventFile;
    QString currentGenerateEventIdentifier;

    QString currentMusFile;
    QString currentMpfFile;
    QString tempDir;
    QString musTempDir;
    QString asfDir;
    QString wavDir;
    QString txtDir;
    QString currentPlayingFile;
    QString currentLanguage;
    QString currentProcessingMpfFile;

    int currentConversionIndex;
    int totalConversions;
    QStringList filesToConvert;
    bool isConverting;
    bool isPlaying;
    bool shouldOverwriteFiles;
    bool isOneClickMode;
    QString singleConvertFileName;
    bool hasShownExportComplete;
};

#endif
