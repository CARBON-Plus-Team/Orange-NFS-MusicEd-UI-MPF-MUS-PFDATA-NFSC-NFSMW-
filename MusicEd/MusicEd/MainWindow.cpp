#include "MainWindow.h"

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QDebug>
#include <QTranslator>
#include <QLibraryInfo>
#include <QStandardPaths>
#include <QTimer>
#include <QSettings>
#include <QScreen>
#include <QGroupBox>
#include <QListWidget>
#include <QProgressBar>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QLabel>
#include <QPushButton>
#include <QCloseEvent>
#include <QEvent>
#include <QSlider>
#include <QProgressDialog>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QRegularExpression>
#include <QElapsedTimer>   
#include <QTextEdit>
#include <QTime>
#include <QTextCursor>
#include <QThread>
#include <QDateTime>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , stackedWidget(nullptr)
    , mainPage(nullptr)
    , fileManagementPage(nullptr)
    , selectMusButton(nullptr)
    , selectedFileLabel(nullptr)
    , extractButton(nullptr)
    , languageLabel(nullptr)
    , languageComboBox(nullptr)
    , fileListWidget(nullptr)
    , convertAllButton(nullptr)
    , playButton(nullptr)
    , stopButton(nullptr)
    , replaceButton(nullptr)
    , convertToASFButton(nullptr)
    , convertToMUSButton(nullptr)
    , backButton(nullptr)
    , mediaPlayer(nullptr)
    , audioOutput(nullptr)
    , volumeSlider(nullptr)
    , progressSlider(nullptr)
    , currentTimeLabel(nullptr)
    , totalTimeLabel(nullptr)
    , progressTimer(nullptr)
    , statusLabel(nullptr)
    , asfCountLabel(nullptr)
    , wavCountLabel(nullptr)
    , logTextEdit(nullptr)
    , appTranslator(nullptr)
    , qtTranslator(nullptr)
    , extractProcess(nullptr)
    , conversionProcess(nullptr)
    , decompileProcess(nullptr) 
    , generateEventProcess(nullptr) 
    , mpfDecompileTimer(nullptr) 
    , conversionProgressDialog(nullptr)
    , unpackProgressDialog(nullptr)
    , currentMusFile("")
    , currentMpfFile("")
    , tempDir(QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/NFSMusicEd_temp")
    , musTempDir(tempDir + "/MUS_Temp")
    , asfDir(tempDir + "/asf")
    , wavDir(tempDir + "/wav")
    , txtDir(tempDir + "/txt")
    , currentPlayingFile("")
    , currentLanguage("en")
    , currentConversionIndex(0)
    , totalConversions(0)
    , isConverting(false)
    , isPlaying(false)
    , shouldOverwriteFiles(false)
    , hasShownExportComplete(false)
{

    QDir dir;
    if (!dir.exists(tempDir)) {
        dir.mkpath(tempDir);
    }
    if (!dir.exists(musTempDir)) {
        dir.mkpath(musTempDir);
    }

    ensureAsfDirectoryExists();
    ensureWavDirectoryExists();
    ensureTxtDirectoryExists();

    copyResourceToTemp(":/tools/convert.bat", "convert.bat");
    copyResourceToTemp(":/tools/convert_back.bat", "convert_back.bat");
    copyResourceToTemp(":/tools/MPFmaster.exe", "MPFmaster.exe");
    copyResourceToTemp(":/tools/sx.exe", "sx.exe");
    copyResourceToTemp(":/tools/ffmpeg.exe", "ffmpeg.exe");
    copyResourceToTemp(":/tools/MPFmaster_GenerateEvent.exe", "MPFmaster_GenerateEvent.exe");

    setupUI();
    applyDarkOrangeTheme();
    setupConnections();
    initializeMediaComponents();

    loadLanguage("en");

    checkTools();
    loadLastMusFile();
}

MainWindow::~MainWindow()
{

    if (mediaPlayer) {
        mediaPlayer->stop();
        mediaPlayer->deleteLater();
    }
    if (audioOutput) {
        audioOutput->deleteLater();
    }
    if (progressTimer) {
        progressTimer->stop();
        delete progressTimer;
    }
    if (appTranslator) {
        delete appTranslator;
    }
    if (extractProcess) {
        extractProcess->kill();
        extractProcess->deleteLater();
    }
    if (conversionProcess) {
        conversionProcess->kill();
        conversionProcess->deleteLater();
    }
    if (decompileProcess) {
        decompileProcess->kill();
        decompileProcess->deleteLater();
    }
    if (generateEventProcess) {
        generateEventProcess->kill();
        generateEventProcess->deleteLater();
    }
    if (mpfDecompileTimer) {
        mpfDecompileTimer->stop();
        mpfDecompileTimer->deleteLater();
    }
    if (conversionProgressDialog) {
        conversionProgressDialog->deleteLater();
    }
}

void MainWindow::setupUI()
{
    setWindowTitle("Orange NFS MusicEd UI By:Shynian");
    setMinimumSize(900, 650);

    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    stackedWidget = new QStackedWidget(this);
    mainLayout->addWidget(stackedWidget);

    setupMainPage();

    setupFileManagementPage();

    stackedWidget->setCurrentWidget(mainPage);
}

void MainWindow::applyDarkOrangeTheme()
{
    QString style = R"(
        QWidget {
            background-color: #F5EFE0;
            color: #2C2C2C;
            font-family: "Segoe UI", "Microsoft YaHei", sans-serif;
            font-size: 10pt;
        }
        QMainWindow {
            background-color: #F5EFE0;
        }
        QGroupBox {
            border: 1px solid #D9CFB8;
            border-radius: 6px;
            margin-top: 12px;
            padding-top: 16px;
            font-weight: bold;
            color: #F97316;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 12px;
            padding: 0 6px 0 6px;
        }
        QPushButton {
            background-color: #E8DFCC;
            border: 1px solid #CFC2A3;
            border-radius: 5px;
            padding: 8px 18px;
            color: #2C2C2C;
            font-weight: normal;
        }
        QPushButton:hover {
            background-color: #F0E8D5;
            border: 1px solid #F97316;
            color: #F97316;
        }
        QPushButton:pressed {
            background-color: #F97316;
            color: #FFFFFF;
        }
        QPushButton:disabled {
            background-color: #EDE8D8;
            color: #A89E88;
            border-color: #D9CFB8;
        }
        QLineEdit, QTextEdit, QListWidget, QTableWidget, QComboBox {
            background-color: #FBF7ED;
            border: 1px solid #CFC2A3;
            border-radius: 4px;
            padding: 4px 8px;
            color: #2C2C2C;
            selection-background-color: #F97316;
            selection-color: #FFFFFF;
        }
        QLineEdit:focus, QTextEdit:focus, QListWidget:focus, QTableWidget:focus, QComboBox:focus {
            border: 1px solid #F97316;
        }
        QComboBox::drop-down {
            border: none;
            width: 24px;
        }
        QComboBox::down-arrow {
            image: none;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 5px solid #F97316;
            width: 0;
            height: 0;
        }
        QComboBox QAbstractItemView {
            background-color: #FBF7ED;
            border: 1px solid #F97316;
            selection-background-color: #F97316;
            selection-color: #FFFFFF;
        }
        QProgressBar {
            border: 1px solid #D9CFB8;
            border-radius: 4px;
            text-align: center;
            background-color: #FBF7ED;
            color: #2C2C2C;
            height: 20px;
        }
        QProgressBar::chunk {
            background-color: #F97316;
            border-radius: 3px;
        }
        QSlider::groove:horizontal {
            border: none;
            height: 6px;
            background: #D9CFB8;
            border-radius: 3px;
            margin: 0 8px;
        }
        QSlider::handle:horizontal {
            background: #F97316;
            width: 16px;
            height: 16px;
            margin: -5px -8px;
            border-radius: 8px;
        }
        QSlider::sub-page:horizontal {
            background: #F97316;
            border-radius: 3px;
        }
        QCheckBox {
            spacing: 8px;
        }
        QCheckBox::indicator {
            width: 16px;
            height: 16px;
            border: 1px solid #CFC2A3;
            border-radius: 3px;
            background-color: #FBF7ED;
        }
        QCheckBox::indicator:checked {
            background-color: #F97316;
            border: 1px solid #F97316;
        }
        QScrollBar:vertical {
            background: #F5EFE0;
            width: 12px;
            margin: 0;
        }
        QScrollBar::handle:vertical {
            background: #CFC2A3;
            border-radius: 6px;
            min-height: 30px;
        }
        QScrollBar::handle:vertical:hover {
            background: #F97316;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0;
        }
        QScrollBar:horizontal {
            background: #F5EFE0;
            height: 12px;
            margin: 0;
        }
        QScrollBar::handle:horizontal {
            background: #CFC2A3;
            border-radius: 6px;
            min-width: 30px;
        }
        QScrollBar::handle:horizontal:hover {
            background: #F97316;
        }
        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
            width: 0;
        }
        QMenuBar {
            background-color: #F5EFE0;
            color: #2C2C2C;
            border-bottom: 1px solid #D9CFB8;
        }
        QMenuBar::item {
            padding: 6px 12px;
            background-color: transparent;
        }
        QMenuBar::item:selected {
            background-color: #F97316;
            color: #FFFFFF;
        }
        QMenu {
            background-color: #FBF7ED;
            border: 1px solid #CFC2A3;
            color: #2C2C2C;
        }
        QMenu::item {
            padding: 6px 24px 6px 24px;
        }
        QMenu::item:selected {
            background-color: #F97316;
            color: #FFFFFF;
        }
        QHeaderView::section {
            background-color: #E8DFCC;
            color: #F97316;
            padding: 6px;
            border: none;
            border-right: 1px solid #F5EFE0;
            font-weight: bold;
        }
        QTabWidget::pane {
            border: 1px solid #D9CFB8;
            border-radius: 4px;
            top: -1px;
        }
        QTabBar::tab {
            background-color: #E8DFCC;
            border: 1px solid #D9CFB8;
            border-bottom: none;
            border-top-left-radius: 4px;
            border-top-right-radius: 4px;
            padding: 8px 16px;
            color: #888877;
        }
        QTabBar::tab:selected {
            background-color: #F5EFE0;
            color: #F97316;
            border-bottom: 2px solid #F97316;
        }
        QTabBar::tab:hover {
            color: #2C2C2C;
        }
        QToolTip {
            background-color: #FBF7ED;
            color: #2C2C2C;
            border: 1px solid #F97316;
            padding: 4px 8px;
        }
        QStatusBar {
            background-color: #F5EFE0;
            color: #777766;
            border-top: 1px solid #D9CFB8;
        }
        QDialog {
            background-color: #F5EFE0;
        }
        QMessageBox {
            background-color: #F5EFE0;
        }
    )";
    qApp->setStyleSheet(style);
}

void MainWindow::setupMainPage()
{
    mainPage = new QWidget();
    QVBoxLayout* mainPageLayout = new QVBoxLayout(mainPage);

    QHBoxLayout* languageLayout = new QHBoxLayout();
    languageLabel = new QLabel(tr("Language/语言:"));
    languageLabel->setProperty("originalText", "Language/语言:");
    languageComboBox = new QComboBox();
    languageComboBox->addItem(tr("English/英文"), "en");
    languageComboBox->addItem(tr("Chinese/中文"), "zh");
    languageLayout->addWidget(languageLabel);
    languageLayout->addWidget(languageComboBox);
    languageLayout->addStretch();

    QGroupBox* fileGroup = new QGroupBox(tr("MUS File Processing"));
    fileGroup->setProperty("originalText", "MUS File Processing");
    QVBoxLayout* fileLayout = new QVBoxLayout(fileGroup);

    QHBoxLayout* selectLayout = new QHBoxLayout();
    selectMusButton = new QPushButton(tr("Select MUS File"));
    selectMusButton->setProperty("originalText", "Select MUS File");
    selectedFileLabel = new QLabel(tr("No file selected"));
    selectedFileLabel->setProperty("originalText", "No file selected");
    selectLayout->addWidget(selectMusButton);
    selectLayout->addWidget(selectedFileLabel);
    selectLayout->addStretch();

    extractButton = new QPushButton();
    updateExtractButtonText();
    extractButton->setEnabled(false);

    fileLayout->addLayout(selectLayout);
    fileLayout->addWidget(extractButton);
    
    // 添加说明文本
    QLabel* instructionLabel = new QLabel(tr("Please select a MUS file. The system will automatically extract the ASF files and related MPF data."));
    instructionLabel->setProperty("originalText", "Please select a MUS file. The system will automatically extract the ASF files and related MPF data.");
    instructionLabel->setWordWrap(true);
    instructionLabel->setStyleSheet("color: #888888; font-size: 12px; margin-top: 10px;");
    fileLayout->addWidget(instructionLabel);

    mainPageLayout->addLayout(languageLayout);
    mainPageLayout->addWidget(fileGroup);
    mainPageLayout->addStretch();

    stackedWidget->addWidget(mainPage);
}

void MainWindow::setupFileManagementPage()
{
    fileManagementPage = new QWidget();
    QVBoxLayout* managementLayout = new QVBoxLayout(fileManagementPage);
    managementLayout->setSpacing(10);


    // Removed File Information section as requested

    // 文件列表区域
    QGroupBox* listGroup = new QGroupBox(tr("Extracted Files"));
    listGroup->setProperty("originalText", "Extracted Files");
    QVBoxLayout* listLayout = new QVBoxLayout(listGroup);
    
    // 文件列表
    fileListWidget = new QListWidget();
    fileListWidget->setStyleSheet(
        "QListWidget {"
        "    background-color: #FBF7ED;"
        "    border: 1px solid #D9CFB8;"
        "    border-radius: 6px;"
        "    padding: 4px;"
        "    font-size: 15px;"
        "    color: #2C2C2C;"
        "    outline: none;"
        "}"
        "QListWidget::item {"
        "    padding: 8px 12px;"
        "    margin: 2px 0px;"
        "    border-radius: 4px;"
        "}"
        "QListWidget::item:selected {"
        "    background-color: #F97316;"
        "    color: #FFFFFF;"
        "    font-weight: bold;"
        "}"
        "QListWidget::item:hover:!selected {"
        "    background-color: #E8DFCC;"
        "    color: #F97316;"
        "}"
        "QScrollBar:vertical {"
        "    background: #FBF7ED;"
        "    width: 10px;"
        "    border-radius: 5px;"
        "}"
        "QScrollBar::handle:vertical {"
        "    background: #CFC2A3;"
        "    border-radius: 5px;"
        "    min-height: 30px;"
        "}"
        "QScrollBar::handle:vertical:hover {"
        "    background: #F97316;"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "    height: 0px;"
        "}"
        "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {"
        "    background: none;"
        "}"
    );
    fileListWidget->setIconSize(QSize(20, 20));
    listLayout->addWidget(fileListWidget);

    QHBoxLayout* countLayout = new QHBoxLayout();
    asfCountLabel = new QLabel(tr("ASF Files: 0"));
    asfCountLabel->setProperty("originalText", "ASF Files: 0");
    wavCountLabel = new QLabel(tr("WAV Files: 0"));
    wavCountLabel->setProperty("originalText", "WAV Files: 0");
    countLayout->addWidget(asfCountLabel);
    countLayout->addStretch();
    countLayout->addWidget(wavCountLabel);
    listLayout->addLayout(countLayout);

    // 播放器控件区域
    setupPlayerControls(managementLayout);

    // 日志输出区域
    QGroupBox* logGroup = new QGroupBox(tr("Conversion Log"));
    logGroup->setProperty("originalText", "Conversion Log");
    QVBoxLayout* logLayout = new QVBoxLayout(logGroup);
    logTextEdit = new QTextEdit();
    logTextEdit->setReadOnly(true);
    logTextEdit->setMaximumHeight(120);
    logTextEdit->setStyleSheet("QTextEdit { font-family: Consolas, monospace; font-size: 10pt; background-color: #FBF7ED; color: #C2410C; border: 1px solid #D9CFB8; border-radius: 4px; }");
    logLayout->addWidget(logTextEdit);
    managementLayout->addWidget(logGroup);

    // 文件操作控制区域
    QGroupBox* operationGroup = new QGroupBox(tr("File Operations"));
    operationGroup->setProperty("originalText", "File Operations");
    QVBoxLayout* operationLayout = new QVBoxLayout(operationGroup);
    
    // 第一行按钮：批量操作
    QHBoxLayout* batchOperationLayout = new QHBoxLayout();
    convertAllButton = new QPushButton();
    updateConvertAllButtonText();
    batchOperationLayout->addWidget(convertAllButton);
    
    convertToMUSButton = new QPushButton();
    updateConvertToMUSButtonText();
    batchOperationLayout->addWidget(convertToMUSButton);
    
    operationLayout->addLayout(batchOperationLayout);
    
    // 第二行按钮：单个文件操作
    QHBoxLayout* singleOperationLayout = new QHBoxLayout();
    playButton = new QPushButton(tr("Play"));
    playButton->setProperty("originalText", "Play");
    singleOperationLayout->addWidget(playButton);
    
    stopButton = new QPushButton(tr("Stop"));
    stopButton->setProperty("originalText", "Stop");
    singleOperationLayout->addWidget(stopButton);
    
    replaceButton = new QPushButton();
    updateReplaceButtonText();
    singleOperationLayout->addWidget(replaceButton);
    
    convertToASFButton = new QPushButton(tr("Convert to ASF"));
    convertToASFButton->setProperty("originalText", "Convert to ASF");
    singleOperationLayout->addWidget(convertToASFButton);
    
    operationLayout->addLayout(singleOperationLayout);
    
    // 返回按钮和帮助按钮
    QHBoxLayout* bottomLayout = new QHBoxLayout();
    howToUseButton = new QPushButton(tr("How to Use"));
    howToUseButton->setProperty("originalText", "How to Use");
    howToUseButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #3a3a3a;"
        "    color: #F97316;"
        "    border: 1px solid #F97316;"
        "    border-radius: 6px;"
        "    padding: 8px 20px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background-color: #F97316;"
        "    color: #2C2C2C;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #e06a0f;"
        "}"
    );
    bottomLayout->addWidget(howToUseButton);
    bottomLayout->addStretch();
    backButton = new QPushButton();
    updateBackButtonText();
    bottomLayout->addWidget(backButton);
    
    managementLayout->addWidget(listGroup);
    managementLayout->addWidget(operationGroup);
    managementLayout->addLayout(bottomLayout);

    stackedWidget->addWidget(fileManagementPage);

    

    // 初始化按钮状态
    playButton->setEnabled(false);
    stopButton->setEnabled(false);
    replaceButton->setEnabled(false);
    convertToASFButton->setEnabled(false);
    convertToMUSButton->setEnabled(false);
    convertAllButton->setEnabled(false);
}

void MainWindow::setupPlayerControls(QVBoxLayout* parentLayout)
{
    QGroupBox* playerGroup = new QGroupBox(tr("Audio Player"));
    playerGroup->setProperty("originalText", "Audio Player");
    QVBoxLayout* playerLayout = new QVBoxLayout(playerGroup);

    QHBoxLayout* volumeLayout = new QHBoxLayout();
    QLabel* volumeLabel = new QLabel(tr("Volume:"));
    volumeLabel->setProperty("originalText", "Volume:");
    volumeLabel->setStyleSheet("font-size: 14px;");
    volumeSlider = new QSlider(Qt::Horizontal);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(50);
    volumeSlider->setMinimumHeight(30);
    volumeLayout->addWidget(volumeLabel);
    volumeLayout->addWidget(volumeSlider, 1);
    volumeLabel->setObjectName("volumeLabel");

    playerLayout->addLayout(volumeLayout);

    parentLayout->addWidget(playerGroup);
}

void MainWindow::initializeMediaComponents()
{
    mediaPlayer = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);

    mediaPlayer->setAudioOutput(audioOutput);
    audioOutput->setVolume(0.5);

    progressTimer = new QTimer(this);
    progressTimer->setInterval(100);
    connect(progressTimer, &QTimer::timeout, this, &MainWindow::updatePlaybackProgress);
}

void MainWindow::setupConnections()
{
    connect(selectMusButton, &QPushButton::clicked, this, &MainWindow::onSelectMusFile);
    connect(extractButton, &QPushButton::clicked, this, &MainWindow::onExtractMusicFiles);
    connect(convertAllButton, &QPushButton::clicked, this, &MainWindow::onConvertAllASF);
    connect(playButton, &QPushButton::clicked, this, &MainWindow::onPlaySelected);
    connect(stopButton, &QPushButton::clicked, this, &MainWindow::onStopPlayback);
    connect(replaceButton, &QPushButton::clicked, this, &MainWindow::onReplaceMusic);
    connect(convertToASFButton, &QPushButton::clicked, this, &MainWindow::onConvertToASF);
    connect(convertToMUSButton, &QPushButton::clicked, this, &MainWindow::onConvertToMUS);
    connect(backButton, &QPushButton::clicked, this, &MainWindow::onBackToMain);
    connect(howToUseButton, &QPushButton::clicked, this, &MainWindow::onHowToUse);

    connect(languageComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &MainWindow::onLanguageChanged);
    connect(fileListWidget, &QListWidget::itemClicked, this, &MainWindow::onFileSelected);
    connect(volumeSlider, &QSlider::valueChanged, this, &MainWindow::onVolumeChanged);

    connect(mediaPlayer, &QMediaPlayer::playbackStateChanged,
        this, &MainWindow::onPlaybackStateChanged);
    connect(mediaPlayer, &QMediaPlayer::positionChanged,
        this, &MainWindow::onMediaPositionChanged);
    connect(mediaPlayer, &QMediaPlayer::durationChanged,
        this, &MainWindow::onMediaDurationChanged);
    connect(progressSlider, &QSlider::sliderMoved,
        this, &MainWindow::onProgressSliderMoved);
}

void MainWindow::updateExtractButtonText()
{
    if (extractButton) {
        extractButton->setText(tr("Extract ASF"));
    }
}

void MainWindow::updateConvertAllButtonText()
{
    if (convertAllButton) {
        convertAllButton->setText(tr("Convert All to WAV"));
    }
}

void MainWindow::updateReplaceButtonText()
{
    if (replaceButton) {
        replaceButton->setText(tr("Replace Music"));
    }
}

void MainWindow::updateConvertToMUSButtonText()
{
    if (convertToMUSButton) {
        convertToMUSButton->setText(tr("Convert to MUS"));
    }
}

void MainWindow::updateBackButtonText()
{
    if (backButton) {
        backButton->setText(tr("Back to Main"));
    }
}

void MainWindow::updateAsfCountLabel(int count)
{
    if (asfCountLabel) {
        asfCountLabel->setText(tr("ASF Files: %1").arg(count));
    }
}

void MainWindow::updateWavCountLabel(int count)
{
    if (wavCountLabel) {
        wavCountLabel->setText(tr("WAV Files: %1").arg(count));
    }
}

void MainWindow::updateStatusLabel(const QString &message, bool isError)
{
    if (statusLabel) {
        statusLabel->setText(message);
        if (isError) {
            statusLabel->setStyleSheet("color: #ff4444; font-weight: bold;");
        } else {
            statusLabel->setStyleSheet("color: #F97316; font-weight: bold;");
        }
    }
}

// Removed Filter functionality as requested

void MainWindow::onSelectMusFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select MUS File"),
        "", tr("MUS Files (*.mus)"));
    if (!fileName.isEmpty()) {
        currentMusFile = fileName;
        selectedFileLabel->setText(QFileInfo(fileName).fileName());
        extractButton->setEnabled(true);

        currentMpfFile = findMatchingMpfFile(fileName);
        if (!currentMpfFile.isEmpty()) {
            QMessageBox::information(this, tr("Info"),
                tr("Found matching MPF file: %1").arg(QFileInfo(currentMpfFile).fileName()));
        }
        else {
            QMessageBox::warning(this, tr("Warning"),
                tr("Could not find matching MPF file. Please ensure the MPF file has the same name as the MUS file."));
        }

        saveMusFilePath(fileName);
    }
}

void MainWindow::saveMusFilePath(const QString &musPath)
{
    QSettings settings(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/NFSMusicEd/settings.ini", QSettings::IniFormat);
    settings.setValue("LastMusFile", musPath);
    settings.sync();
}

void MainWindow::loadLastMusFile()
{
    QSettings settings(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/NFSMusicEd/settings.ini", QSettings::IniFormat);
    QString lastMus = settings.value("LastMusFile", "").toString();

    if (!lastMus.isEmpty() && QFile::exists(lastMus)) {
        currentMusFile = lastMus;
        selectedFileLabel->setText(QFileInfo(lastMus).fileName());
        extractButton->setEnabled(true);

        currentMpfFile = findMatchingMpfFile(lastMus);
        if (!currentMpfFile.isEmpty()) {
            qDebug() << "Loaded last MUS file:" << lastMus;
            qDebug() << "Matching MPF file:" << currentMpfFile;
        }
    }
}

void MainWindow::onExtractMusicFiles()
{
    if (currentMusFile.isEmpty() || currentMpfFile.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("Please select both MUS and MPF files first."));
        return;
    }

    extractButton->setEnabled(false);
    selectMusButton->setEnabled(false);

    try {

        clearTempFilesExceptBats();

        ensureAsfDirectoryExists();

        autoDecompileMpf(currentMpfFile);

    }
    catch (const std::exception& e) {
        QMessageBox::critical(this, tr("Error"), QString(tr("Extraction failed: %1")).arg(e.what()));
        return;
    }
}

void MainWindow::onExtractFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitStatus)

    if (exitCode == 0) {
        qDebug() << "MUS extraction completed successfully";

        stackedWidget->setCurrentWidget(fileManagementPage);
        updateFileList();

        // 无论是否已经显示过，都在解包完成时显示成功消息
        QMessageBox::information(this, tr("Success"),
            tr("Music file extraction completed! You can now convert ASF files to WAV format for playback and editing."));

        extractButton->setEnabled(true);
        selectMusButton->setEnabled(true);
        
        // 重置hasShownExportComplete标志，确保下次操作能正确显示消息
        hasShownExportComplete = false;
    }
    else {
        QMessageBox::critical(this, tr("Error"),
            tr("Extraction failed with exit code: %1").arg(exitCode));
        extractButton->setEnabled(true);
        selectMusButton->setEnabled(true);
    }

    if (extractProcess) {
        extractProcess->deleteLater();
        extractProcess = nullptr;
    }
    
    if (unpackProgressDialog) {
        unpackProgressDialog->deleteLater();
        unpackProgressDialog = nullptr;
    }
}

void MainWindow::onExtractError(QProcess::ProcessError error)
{
    qWarning() << "Extraction process error:" << error;
    QMessageBox::critical(this, tr("Error"),
        tr("Extract process error: %1").arg(error));
    extractButton->setEnabled(true);
    selectMusButton->setEnabled(true);

    if (extractProcess) {
        extractProcess->deleteLater();
        extractProcess = nullptr;
    }
    
    // 清理进度对话框
    if (unpackProgressDialog) {
        unpackProgressDialog->deleteLater();
        unpackProgressDialog = nullptr;
    }
}

void MainWindow::onDecompileFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitStatus)

    if (!decompileProcess) {
        qWarning() << "onDecompileFinished called but decompileProcess is null!";
        return;
    }

    if (mpfDecompileTimer) {
        mpfDecompileTimer->stop();
        mpfDecompileTimer->deleteLater();
        mpfDecompileTimer = nullptr;
    }

    QByteArray stdoutData = decompileProcess->readAllStandardOutput();
    QByteArray stderrData = decompileProcess->readAllStandardError();
    if (!stdoutData.isEmpty()) qDebug() << "MPFmaster stdout:" << stdoutData;
    if (!stderrData.isEmpty()) qWarning() << "MPFmaster stderr:" << stderrData;

    qDebug() << "MPFmaster exit code:" << exitCode;

    ensureAsfDirectoryExists();

    if (exitCode == 0) {
        qDebug() << "MPF decompilation completed successfully";
        qDebug() << "Calling runGenerateEventOnDecompFiles()";
        runGenerateEventOnDecompFiles();
    } else {
        QString errText = QString::fromLocal8Bit(stderrData);
        if (errText.isEmpty()) errText = QString::fromLocal8Bit(stdoutData);
        qWarning() << "MPF decompilation failed, but continuing with extraction. Exit code:" << exitCode << "Error:" << errText;
        qDebug() << "MPF decompilation failed, but still calling runGenerateEventOnDecompFiles()";
        runGenerateEventOnDecompFiles();
    }

    if (decompileProcess) {
        decompileProcess->deleteLater();
        decompileProcess = nullptr;
        qDebug() << "decompileProcess deleted";
    }
}

void MainWindow::onDecompileError(QProcess::ProcessError error)
{
    qWarning() << "MPF decompilation process error:" << error;
    QMessageBox::critical(this, tr("Error"),
        tr("MPF decompile process error: %1").arg(error));
    extractButton->setEnabled(true);
    selectMusButton->setEnabled(true);

    if (mpfDecompileTimer) {
        mpfDecompileTimer->stop();
        mpfDecompileTimer->deleteLater();
        mpfDecompileTimer = nullptr;
    }

    if (decompileProcess) {
        decompileProcess->deleteLater();
        decompileProcess = nullptr;
    }
}

void MainWindow::onConvertAllASF()
{
    QDir asfDirectory(asfDir);
    QStringList asfFiles = asfDirectory.entryList(QStringList() << "*.asf", QDir::Files, QDir::NoSort);

    if (asfFiles.isEmpty()) {
        QMessageBox::information(this, tr("Info"), tr("No ASF files found to convert."));
        return;
    }

    std::sort(asfFiles.begin(), asfFiles.end(), [](const QString& a, const QString& b) {
        int na = QFileInfo(a).baseName().toInt();
        int nb = QFileInfo(b).baseName().toInt();
        return na < nb;
    });

    ensureWavDirectoryExists();

    showConversionProgressDialog(tr("Converting ASF to WAV"), asfFiles.size());
    filesToConvert = asfFiles;

    currentConversionIndex = 0;
    convertNextASFtoWAV();
}

void MainWindow::convertNextASFtoWAV()
{
    if (currentConversionIndex >= filesToConvert.size() || !isConverting) {

        hideConversionProgressDialog();

        if (currentConversionIndex >= filesToConvert.size()) {

            QDir asfDirectory(asfDir);
            QStringList asfFiles = asfDirectory.entryList(QStringList() << "*.asf", QDir::Files);

            int deletedCount = 0;
            for (const QString& asfFile : asfFiles) {
                QString filePath = asfDir + "/" + asfFile;
                if (QFile::remove(filePath)) {
                    deletedCount++;
                    qDebug() << "Deleted ASF file:" << asfFile;
                }
            }

            updateFileList();

            QMessageBox::information(this, tr("Success"),
                tr("Converted %1 ASF files to WAV format and deleted %2 original ASF files.")
                    .arg(filesToConvert.size()).arg(deletedCount));

            convertToMUSButton->setEnabled(false); 
        }
        return;
    }

    QString asfName = filesToConvert[currentConversionIndex];
    QString asfPath = asfDir + "/" + asfName;
    QString wavName = QFileInfo(asfName).completeBaseName() + ".wav";
    QString wavPath = wavDir + "/" + wavName;

    updateConversionProgress(currentConversionIndex,
        tr("Converting %1...").arg(asfName));

    if (conversionProcess) {
        conversionProcess->deleteLater();
    }

    conversionProcess = new QProcess(this);
    connect(conversionProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        this, &MainWindow::onASFtoWAVConversionFinished);
    connect(conversionProcess, &QProcess::errorOccurred, this, &MainWindow::onConversionError);
    connect(conversionProcess, &QProcess::readyReadStandardOutput,
        this, &MainWindow::onConversionOutputReady);
    connect(conversionProcess, &QProcess::readyReadStandardError,
        this, &MainWindow::onConversionOutputReady);

    QString sxPath = tempDir + "/sx.exe";
    conversionProcess->setWorkingDirectory(tempDir);

    QString sxAsfRel = "asf/" + asfName;
    QString sxWavRel = "wav/" + wavName;
    QStringList sxArgs;
    sxArgs << "-wave" << sxAsfRel << "-=" + sxWavRel;

    conversionProcess->setProperty("useFfmpeg", false);
    conversionProcess->setProperty("asfPath", asfPath);
    conversionProcess->setProperty("wavPath", wavPath);
    conversionProcess->start(sxPath, sxArgs);
    qDebug() << "Starting ASF to WAV conversion with sx.exe:" << asfName << "->" << wavName;
    appendLog(tr("Converting %1 to WAV (using sx.exe)...").arg(asfName), false);
}

void MainWindow::onASFtoWAVConversionFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitStatus)

    QProcess* senderProcess = qobject_cast<QProcess*>(sender());
    if (!senderProcess) {
        return;
    }

    if (senderProcess != conversionProcess) {
        qDebug() << "Finished signal from old process, ignoring (already handled by error handler)";
        return;
    }

    bool useFfmpeg = false;
    QString asfPath;
    QString wavPath;
    if (conversionProcess) {
        useFfmpeg = conversionProcess->property("useFfmpeg").toBool();
        asfPath = conversionProcess->property("asfPath").toString();
        wavPath = conversionProcess->property("wavPath").toString();
    }

    bool success = false;
    if (useFfmpeg) {
        success = (exitCode == 0 && QFile::exists(wavPath));
    } else {
        success = QFile::exists(wavPath) && QFileInfo(wavPath).size() > 0;
    }

    if (success) {
        qDebug() << "File conversion completed successfully";
        appendLog(tr("Conversion completed successfully"), false);
        currentConversionIndex++;

        updateConversionProgress(currentConversionIndex,
            tr("Converted %1 of %2 files").arg(currentConversionIndex).arg(totalConversions));

        QTimer::singleShot(100, this, &MainWindow::convertNextASFtoWAV);
    }
    else if (!useFfmpeg && !asfPath.isEmpty() && !wavPath.isEmpty()) {
        QString stderrData;
        if (conversionProcess) {
            stderrData = QString::fromLocal8Bit(conversionProcess->readAllStandardError());
        }
        qWarning() << "sx.exe conversion failed, falling back to FFmpeg. Exit code:" << exitCode << stderrData;
        appendLog(tr("sx.exe failed (exit code %1), falling back to FFmpeg...").arg(exitCode), true);

        QString ffmpegPath = tempDir + "/ffmpeg.exe";
        if (!QFile::exists(ffmpegPath)) {
            qWarning() << "FFmpeg not found, cannot fall back";
            appendLog(tr("FFmpeg not available, conversion failed."), true);
            QMessageBox::critical(this, tr("Error"),
                tr("Conversion failed, exit code: %1\n%2").arg(exitCode).arg(stderrData));
            hideConversionProgressDialog();

            if (conversionProcess) {
                conversionProcess->deleteLater();
                conversionProcess = nullptr;
            }
            return;
        }

        if (conversionProcess) {
            conversionProcess->deleteLater();
            conversionProcess = nullptr;
        }

        conversionProcess = new QProcess(this);
        connect(conversionProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &MainWindow::onASFtoWAVConversionFinished);
        connect(conversionProcess, &QProcess::errorOccurred, this, &MainWindow::onConversionError);
        connect(conversionProcess, &QProcess::readyReadStandardOutput,
            this, &MainWindow::onConversionOutputReady);
        connect(conversionProcess, &QProcess::readyReadStandardError,
            this, &MainWindow::onConversionOutputReady);

        QStringList ffmpegArgs;
        ffmpegArgs << "-y"
                   << "-i" << asfPath
                   << "-sample_fmt" << "s16"
                   << "-ar" << "44100"
                   << "-ac" << "2"
                   << wavPath;

        conversionProcess->setProperty("useFfmpeg", true);
        conversionProcess->setProperty("asfPath", asfPath);
        conversionProcess->setProperty("wavPath", wavPath);
        conversionProcess->setWorkingDirectory(tempDir);
        conversionProcess->start(ffmpegPath, ffmpegArgs);
        qDebug() << "Fallback: starting ASF to WAV conversion with FFmpeg";

        return;
    }
    else {
        QString stderrData;
        if (conversionProcess) {
            stderrData = QString::fromLocal8Bit(conversionProcess->readAllStandardError());
        }
        qWarning() << "Conversion failed with exit code:" << exitCode << stderrData;
        appendLog(tr("Conversion failed, exit code: %1").arg(exitCode), true);
        QMessageBox::critical(this, tr("Error"),
            tr("Conversion failed, exit code: %1\n%2").arg(exitCode).arg(stderrData));
        hideConversionProgressDialog();
    }

    if (conversionProcess) {
        conversionProcess->deleteLater();
        conversionProcess = nullptr;
    }
}

void MainWindow::onPlaySelected()
{
    QList<QListWidgetItem*> selectedItems = fileListWidget->selectedItems();
    if (selectedItems.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("Please select a file to play."));
        return;
    }

    QString fileName = selectedItems[0]->text();

    bool isWavFile = fileName.endsWith(".wav", Qt::CaseInsensitive);
    bool isAsfFile = fileName.endsWith(".asf", Qt::CaseInsensitive);

    if (!isWavFile && !isAsfFile) {
        QMessageBox::warning(this, tr("Error"), tr("Please select a WAV or ASF file."));
        return;
    }

    if (isAsfFile) {
        QMessageBox::information(this, tr("Info"),
            tr("ASF files need to be converted to WAV format before playing. Please use the 'Convert All to WAV' button first."));
        return;
    }

    QString filePath = wavDir + "/" + fileName;

    if (mediaPlayer && mediaPlayer->playbackState() == QMediaPlayer::PlayingState
        && currentPlayingFile == filePath) {
        mediaPlayer->pause();
        return;
    }

    if (mediaPlayer && mediaPlayer->playbackState() == QMediaPlayer::PausedState
        && currentPlayingFile == filePath) {
        mediaPlayer->play();
        return;
    }

    currentPlayingFile = filePath;
    mediaPlayer->setSource(QUrl::fromLocalFile(currentPlayingFile));
    audioOutput->setVolume(volumeSlider->value() / 100.0);
    mediaPlayer->play();
    stopButton->setEnabled(true);
}

void MainWindow::onStopPlayback()
{
    mediaPlayer->stop();
    isPlaying = false;
    stopButton->setEnabled(false);
}

bool MainWindow::copyResourceToTemp(const QString& resourcePath, const QString& fileName)
{
    QFile resourceFile(resourcePath);
    if (!resourceFile.exists()) {
        qWarning() << "Resource file not found:" << resourcePath;
        return false;
    }

    QString destPath = tempDir + "/" + fileName;

    if (QFile::exists(destPath)) {
        QFile::remove(destPath);
    }

    if (resourceFile.copy(destPath)) {
        qDebug() << "Copied" << fileName << "to temp directory";

#ifndef Q_OS_WIN
        QFile(destPath).setPermissions(QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner |
            QFile::ReadGroup | QFile::ExeGroup |
            QFile::ReadOther | QFile::ExeOther);
#endif
        return true;
    }
    else {
        qWarning() << "Failed to copy" << fileName << ":" << resourceFile.errorString();
        return false;
    }
}

void MainWindow::updateFileList() 
{
    fileListWidget->clear();

    QDir wavDirectory(wavDir);
    QStringList wavFiles = wavDirectory.entryList(QStringList() << "*.wav", QDir::Files, QDir::NoSort);

    QDir asfDirectory(asfDir);
    QStringList asfFiles = asfDirectory.entryList(QStringList() << "*.asf", QDir::Files, QDir::NoSort);

    auto numericSort = [](QStringList& list) {
        std::sort(list.begin(), list.end(), [](const QString& a, const QString& b) {
            int na = QFileInfo(a).baseName().toInt();
            int nb = QFileInfo(b).baseName().toInt();
            return na < nb;
        });
    };

    numericSort(wavFiles);
    numericSort(asfFiles);

    updateAsfCountLabel(asfFiles.size());
    updateWavCountLabel(wavFiles.size());

    for (const QString& file : wavFiles) {
        fileListWidget->addItem(file);
    }

    for (const QString& file : asfFiles) {
        fileListWidget->addItem(file);
    }

    bool hasAsfFiles = !asfFiles.isEmpty();
    bool hasWavFiles = !wavFiles.isEmpty();

    convertAllButton->setEnabled(hasAsfFiles);
    playButton->setEnabled(hasWavFiles);
    stopButton->setEnabled(false); 
    replaceButton->setEnabled(hasWavFiles);
    convertToASFButton->setEnabled(hasWavFiles);

    convertToMUSButton->setEnabled(hasAsfFiles && !isConverting);

    qDebug() << "File list updated. WAV files:" << wavFiles.size() << "ASF files:" << asfFiles.size();
}

bool MainWindow::ensureAsfDirectoryExists()
{
    QDir asfDirectory(asfDir);
    if (!asfDirectory.exists()) {
        if (!asfDirectory.mkpath(asfDir)) {
            qWarning() << "Failed to create ASF directory:" << asfDir;
            return false;
        } else {
            qDebug() << "Created ASF directory:" << asfDir;
        }
    }
    return true;
}

void MainWindow::clearTempFiles()
{
    QDir tempDirectory(tempDir);
    if (tempDirectory.exists()) {
        QStringList files = tempDirectory.entryList(QDir::Files);
        for (const QString& file : files) {
            QFile::remove(tempDir + "/" + file);
        }
    }

    QDir asfDirectory(asfDir);
    if (asfDirectory.exists()) {
        QStringList files = asfDirectory.entryList(QDir::Files);
        for (const QString& file : files) {
            QFile::remove(asfDir + "/" + file);
        }
    }

    QDir wavDirectory(wavDir);
    if (wavDirectory.exists()) {
        QStringList files = wavDirectory.entryList(QDir::Files);
        for (const QString& file : files) {
            QFile::remove(wavDir + "/" + file);
        }
    }

    QDir musTempDirectory(musTempDir);
    if (musTempDirectory.exists()) {
        QStringList files = musTempDirectory.entryList(QDir::Files);
        for (const QString& file : files) {
            QFile::remove(musTempDir + "/" + file);
        }
    }
}

void MainWindow::clearTempFilesExceptBats()
{
    QDir tempDirectory(tempDir);
    if (tempDirectory.exists()) {
        QStringList files = tempDirectory.entryList(QDir::Files);
        for (const QString& file : files) {
            if (!file.endsWith(".bat", Qt::CaseInsensitive) &&
                !file.endsWith(".exe", Qt::CaseInsensitive)) {
                QFile::remove(tempDir + "/" + file);
            }
        }
    }

    QDir asfDirectory(asfDir);
    if (asfDirectory.exists()) {
        QStringList files = asfDirectory.entryList(QDir::Files);
        for (const QString& file : files) {
            QFile::remove(asfDir + "/" + file);
        }
    }

    QDir wavDirectory(wavDir);
    if (wavDirectory.exists()) {
        QStringList files = wavDirectory.entryList(QDir::Files);
        for (const QString& file : files) {
            QFile::remove(wavDir + "/" + file);
        }
    }

    QDir musTempDirectory(musTempDir);
    if (musTempDirectory.exists()) {
        QStringList files = musTempDirectory.entryList(QDir::Files);
        for (const QString& file : files) {
            QFile::remove(musTempDir + "/" + file);
        }
    }
}

QString MainWindow::findMatchingMpfFile(const QString& musFilePath)
{
    QFileInfo musFileInfo(musFilePath);
    QString baseName = musFileInfo.completeBaseName();
    QString searchDir = musFileInfo.path();

    QStringList possibleMpfFiles = {
        searchDir + "/" + baseName + ".mpf",
        searchDir + "/" + baseName + ".MPF",
        searchDir + "/" + baseName + "_data.mpf"
    };

    for (const QString& mpfPath : possibleMpfFiles) {
        if (QFile::exists(mpfPath)) {
            return mpfPath;
        }
    }

    return QString();
}



bool MainWindow::ensureWavDirectoryExists()
{
    QDir dir;
    if (!dir.exists(wavDir)) {
        bool created = dir.mkpath(wavDir);
        if (created) {
            qDebug() << "Created WAV directory:" << wavDir;
        }
        else {
            qWarning() << "Failed to create WAV directory:" << wavDir;
        }
        return created;
    }
    return true;
}

bool MainWindow::ensureTxtDirectoryExists()
{
    QDir dir;
    if (!dir.exists(txtDir)) {
        bool created = dir.mkpath(txtDir);
        if (created) {
            qDebug() << "Created TXT directory:" << txtDir;
        }
        else {
            qWarning() << "Failed to create TXT directory:" << txtDir;
        }
        return created;
    }
    return true;
}

QString MainWindow::getAsfDirectoryPath() const
{
    return asfDir;
}

void MainWindow::autoDecompileMpf(const QString& mpfFilePath)
{
    if (!QFile::exists(mpfFilePath)) {
        QMessageBox::critical(this, tr("Error"),
                              tr("Could not find MPF file: %1").arg(mpfFilePath));

        extractButton->setEnabled(true);
        selectMusButton->setEnabled(true);
        return;
    }

    QString mpfmasterPath = tempDir + "/MPFmaster.exe";
    if (!QFile::exists(mpfmasterPath)) {
        QMessageBox::critical(this, tr("Error"),
                              tr("Could not find MPFmaster.exe"));

        extractButton->setEnabled(true);
        selectMusButton->setEnabled(true);
        return;
    }

    QFileInfo mpfInfo(mpfFilePath);
    QString mpfBase = mpfInfo.fileName();
    QString mpfTempPath = tempDir + "/" + mpfBase;
    if (QFile::exists(mpfTempPath)) {
        QFile::remove(mpfTempPath);
    }
    if (!QFile::copy(mpfFilePath, mpfTempPath)) {
        qWarning() << "Failed to copy MPF to temp:" << mpfFilePath << "->" << mpfTempPath;
        
        QMessageBox::critical(this, tr("Error"),
                              tr("Failed to copy MPF file to temp directory."));
        
        extractButton->setEnabled(true);
        selectMusButton->setEnabled(true);
        return;
    }

    currentProcessingMpfFile = mpfFilePath;

    if (decompileProcess) {
        decompileProcess->deleteLater();
        decompileProcess = nullptr;
    }
    decompileProcess = new QProcess(this);
    decompileProcess->setWorkingDirectory(tempDir);

    bool finishedConnected = connect(decompileProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &MainWindow::onDecompileFinished);
    bool errorConnected = connect(decompileProcess, &QProcess::errorOccurred, this, &MainWindow::onDecompileError);
    bool stdoutConnected = connect(decompileProcess, &QProcess::readyReadStandardOutput, [this]() {
        if (decompileProcess) {
            QByteArray output = decompileProcess->readAllStandardOutput();
            qDebug() << "MPFmaster stdout:" << output.trimmed();
        }
    });
    bool stderrConnected = connect(decompileProcess, &QProcess::readyReadStandardError, [this]() {
        if (decompileProcess) {
            QByteArray error = decompileProcess->readAllStandardError();
            qDebug() << "MPFmaster stderr:" << error.trimmed();
        }
    });
    bool stateChangedConnected = connect(decompileProcess, &QProcess::stateChanged, [this](QProcess::ProcessState newState) {
        qDebug() << "MPFmaster process state changed:" << newState;
        if (newState == QProcess::Running) {
            qDebug() << "MPFmaster process is now running...";
            qDebug() << "Process PID:" << decompileProcess->processId();
        } else if (newState == QProcess::NotRunning) {
            qDebug() << "MPFmaster process is now not running...";
            qDebug() << "Process error code:" << decompileProcess->error();
            qDebug() << "Process error string:" << decompileProcess->errorString();
        } else if (newState == QProcess::Starting) {
            qDebug() << "MPFmaster process is starting...";
        }
    });

    qDebug() << "Signal connections - finished:" << finishedConnected << "error:" << errorConnected << "stdout:" << stdoutConnected << "stderr:" << stderrConnected << "stateChanged:" << stateChangedConnected;

    QStringList arguments;
    arguments << "-d" << mpfBase;

    qDebug() << "Starting MPFmaster decompile:" << mpfmasterPath << arguments << "workdir=" << decompileProcess->workingDirectory();
    qDebug() << "MPF temp path:" << mpfTempPath;

    decompileProcess->start(mpfmasterPath, arguments);

    QProcess::ProcessError error = decompileProcess->error();
    bool started = (error == QProcess::UnknownError);
    
    // 检查进程是否成功启动
    if (started) {
        qDebug() << "MPFmaster process started successfully with PID:" << decompileProcess->processId();
    } else {
        qDebug() << "MPFmaster process failed to start with error:" << error;
    }

    qDebug() << "MPFmaster process started:" << started << "Error string:" << decompileProcess->errorString();

    if (!started) {
        qWarning() << "Failed to start MPFmaster process:" << decompileProcess->errorString();

        QMessageBox::critical(this, tr("Error"),
                              tr("Failed to start MPFmaster.exe process: %1").arg(decompileProcess->errorString()));

        extractButton->setEnabled(true);
        selectMusButton->setEnabled(true);

        if (decompileProcess) {
            decompileProcess->deleteLater();
            decompileProcess = nullptr;
        }

        return;
    }

    qDebug() << "MPFmaster process started successfully. Process ID:" << decompileProcess->processId();
    qDebug() << "Process state after start:" << decompileProcess->state();

    if (mpfDecompileTimer) {
        mpfDecompileTimer->stop();
        mpfDecompileTimer->deleteLater();
        mpfDecompileTimer = nullptr;
    }
    mpfDecompileTimer = new QTimer(this);
    mpfDecompileTimer->setSingleShot(true);
    mpfDecompileTimer->setInterval(120000); 

    connect(mpfDecompileTimer, &QTimer::timeout, this, [this]() {
        if (decompileProcess && decompileProcess->state() == QProcess::Running) {
            qWarning() << "MPFmaster process timed out after 120 seconds, forcing termination...";

            decompileProcess->kill();
            decompileProcess->waitForFinished(500);

            decompileProcess->deleteLater();
            decompileProcess = nullptr;

            extractButton->setEnabled(true);
            selectMusButton->setEnabled(true);

            QMessageBox::critical(this, tr("Error"), 
                                  tr("MPF decompilation timed out, please check if the MPF file format is correct."));

            
        }

        mpfDecompileTimer->deleteLater();
        mpfDecompileTimer = nullptr;
    });

    mpfDecompileTimer->start();
}

void MainWindow::extractMusFile(const QString& mpfFilePath, const QString& musFilePath,
    const QString& outputDir)
{
    QStringList arguments;
    arguments << "-sa" << mpfFilePath << musFilePath << outputDir;

    QString mpfmasterPath = tempDir + "/MPFmaster.exe";

    if (!QFile::exists(mpfmasterPath)) {
        QMessageBox::critical(this, tr("Error"),
            tr("MPFmaster.exe not found in temp directory."));
        return;
    }

    if (extractProcess) {
        extractProcess->deleteLater();
        extractProcess = nullptr;
    }

    extractProcess = new QProcess(this);

    connect(extractProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &MainWindow::onExtractFinished);
    connect(extractProcess, &QProcess::errorOccurred, this, &MainWindow::onExtractError);

    qDebug() << "Extracting MUS file with arguments:" << arguments;
    extractProcess->start(mpfmasterPath, arguments);
}

bool MainWindow::convertASFtoWAV(const QString& asfFilePath, const QString& wavFilePath)
{
    if (!QFile::exists(asfFilePath)) {
        qWarning() << "ASF file not found:" << asfFilePath;
        return false;
    }

    QProcess process;
    process.setWorkingDirectory(tempDir);
    QString sxPath = tempDir + "/sx.exe";

    QStringList arguments;
    arguments << "-wave" << asfFilePath << "-=" + wavFilePath;

    qDebug() << "Running ASF to WAV conversion with sx.exe:" << arguments;
    process.start(sxPath, arguments);

    if (process.waitForFinished(10000)) {
        if (process.exitCode() == 0 && QFile::exists(wavFilePath)) {
            qDebug() << "ASF to WAV conversion succeeded using sx.exe";
            return true;
        } else {
            qWarning() << "sx.exe ASF to WAV conversion failed with exit code:" << process.exitCode();
            qWarning() << "sx.exe stderr:" << process.readAllStandardError();
        }
    } else {
        qWarning() << "sx.exe ASF to WAV conversion timed out";
    }

    qDebug() << "Falling back to FFmpeg for ASF to WAV conversion";

    QString ffmpegPath = tempDir + "/ffmpeg.exe";
    if (QFile::exists(ffmpegPath)) {
        QProcess ffmpegProcess;
        ffmpegProcess.setWorkingDirectory(tempDir);

        QStringList ffmpegArgs;
        ffmpegArgs << "-y"
                   << "-i" << asfFilePath
                   << "-sample_fmt" << "s16"
                   << "-ar" << "44100"
                   << "-ac" << "2"
                   << wavFilePath;

        qDebug() << "Running FFmpeg ASF to WAV conversion:" << ffmpegArgs;
        ffmpegProcess.start(ffmpegPath, ffmpegArgs);

        if (ffmpegProcess.waitForFinished(15000)) {
            if (ffmpegProcess.exitCode() == 0 && QFile::exists(wavFilePath)) {
                qDebug() << "ASF to WAV conversion succeeded using FFmpeg";
                return true;
            } else {
                qWarning() << "FFmpeg ASF to WAV conversion failed with exit code:" << ffmpegProcess.exitCode();
                qWarning() << "FFmpeg stderr:" << ffmpegProcess.readAllStandardError();
            }
        } else {
            qWarning() << "FFmpeg ASF to WAV conversion timed out";
        }
    }

    return false;
}

bool MainWindow::convertWAVtoASF(const QString& wavFilePath, const QString& asfFilePath)
{
    if (!QFile::exists(wavFilePath)) {
        qWarning() << "WAV file not found:" << wavFilePath;
        return false;
    }

    QString effectiveWavPath = wavFilePath;
    QString adjustedWavPath;

    QString ffmpegPath = tempDir + "/ffmpeg.exe";
    if (QFile::exists(ffmpegPath)) {
        adjustedWavPath = tempDir + "/adjusted_" + QFileInfo(wavFilePath).fileName();

        QProcess ffmpegProcess;
        ffmpegProcess.setWorkingDirectory(tempDir);

        QStringList ffmpegArgs;
        ffmpegArgs << "-i" << wavFilePath
                   << "-sample_fmt" << "s16"
                   << "-ar" << "44100"
                   << "-ac" << "2"
                   << "-y"
                   << adjustedWavPath;

        qDebug() << "Running ffmpeg to adjust WAV format:" << ffmpegArgs;
        ffmpegProcess.start(ffmpegPath, ffmpegArgs);

        if (ffmpegProcess.waitForFinished(15000)) {
            if (ffmpegProcess.exitCode() == 0 && QFile::exists(adjustedWavPath)) {
                qDebug() << "WAV format adjusted successfully using ffmpeg";
                effectiveWavPath = adjustedWavPath;
            } else {
                qWarning() << "ffmpeg adjustment failed with exit code:" << ffmpegProcess.exitCode();
                qWarning() << "ffmpeg stderr:" << ffmpegProcess.readAllStandardError();
            }
        } else {
            qWarning() << "ffmpeg adjustment timed out";
        }
    } else {
        qWarning() << "ffmpeg.exe not found in temp directory, using original WAV";
    }

    QProcess process;
    process.setWorkingDirectory(tempDir);
    QString sxPath = tempDir + "/sx.exe";

    QStringList arguments;
    arguments << "-pcstream" << "-eaxa_blk"
              << effectiveWavPath << "-=" + asfFilePath;

    qDebug() << "Running WAV to ASF conversion:" << arguments;
    process.start(sxPath, arguments);

    if (!process.waitForFinished(10000)) {
        qWarning() << "WAV to ASF conversion timed out";
        if (!adjustedWavPath.isEmpty() && QFile::exists(adjustedWavPath)) {
            QFile::remove(adjustedWavPath);
        }
        return false;
    }

    if (!adjustedWavPath.isEmpty() && QFile::exists(adjustedWavPath)) {
        QFile::remove(adjustedWavPath);
        qDebug() << "Removed temporary adjusted WAV file:" << adjustedWavPath;
    }

    return process.exitCode() == 0;
}

bool MainWindow::runBatchConversion(const QString& batFileName)
{
    QString batPath = tempDir + "/" + batFileName;

    if (!QFile::exists(batPath)) {
        QMessageBox::critical(this, tr("Error"),
            tr("Batch file not found: %1").arg(batFileName));
        return false;
    }

    QProcess process;
    process.setWorkingDirectory(tempDir);

    qDebug() << "Starting batch file:" << batPath;
    process.start(batPath);

    if (!process.waitForFinished(60000)) { 
        qDebug() << "Batch process timed out";
        return false;
    }

    qDebug() << "Batch process finished with exit code:" << process.exitCode();
    return process.exitCode() == 0;
}

void MainWindow::showConversionProgressDialog(const QString& title, int maxValue)
{
    if (!conversionProgressDialog) {
        conversionProgressDialog = new QProgressDialog(this);
        conversionProgressDialog->setWindowModality(Qt::WindowModal);
        conversionProgressDialog->setCancelButtonText(tr("Cancel"));
        connect(conversionProgressDialog, &QProgressDialog::canceled,
            this, &MainWindow::onConversionProgressCanceled);
    }

    conversionProgressDialog->setWindowTitle(title);
    conversionProgressDialog->setLabelText(tr("Preparing conversion..."));
    conversionProgressDialog->setRange(0, maxValue);
    conversionProgressDialog->setValue(0);
    conversionProgressDialog->show();

    currentConversionIndex = 0;
    totalConversions = maxValue;
    isConverting = true;
}

void MainWindow::updateConversionProgress(int value, const QString& text)
{
    if (conversionProgressDialog) {
        conversionProgressDialog->setValue(value);
        if (!text.isEmpty()) {
            conversionProgressDialog->setLabelText(text);
        }

        QApplication::processEvents();
    }
}

void MainWindow::hideConversionProgressDialog()
{
    if (conversionProgressDialog) {
        conversionProgressDialog->hide();
    }
    isConverting = false;
}

void MainWindow::onConversionProgressCanceled()
{
    if (conversionProcess && conversionProcess->state() == QProcess::Running) {
        conversionProcess->kill();
        conversionProcess->waitForFinished(1000);
    }
    hideConversionProgressDialog();

    QMessageBox::information(this, tr("Info"), tr("User canceled the conversion"));
}

void MainWindow::onConversionError(QProcess::ProcessError error)
{
    bool useFfmpeg = false;
    QString asfPath;
    QString wavPath;
    if (conversionProcess) {
        useFfmpeg = conversionProcess->property("useFfmpeg").toBool();
        asfPath = conversionProcess->property("asfPath").toString();
        wavPath = conversionProcess->property("wavPath").toString();
    }

    if (!useFfmpeg && !asfPath.isEmpty() && !wavPath.isEmpty() && isConverting) {
        QString errorDetails;
        QByteArray stderrData;
        if (conversionProcess) {
            errorDetails = conversionProcess->errorString();
            stderrData = conversionProcess->readAllStandardError();
        }

        qWarning() << "sx.exe process error, falling back to FFmpeg. Error:" << error << "Details:" << errorDetails;
        appendLog(tr("sx.exe crashed (%1), falling back to FFmpeg...").arg(errorDetails), true);

        QString ffmpegPath = tempDir + "/ffmpeg.exe";
        if (!QFile::exists(ffmpegPath)) {
            qWarning() << "FFmpeg not found, cannot fall back";
            appendLog(tr("FFmpeg not available, conversion failed."), true);

            if (conversionProcess) {
                conversionProcess->deleteLater();
                conversionProcess = nullptr;
            }

            QMessageBox::critical(this, tr("Error"),
                tr("Conversion process error: %1\nDetails: %2").arg(error).arg(errorDetails));
            hideConversionProgressDialog();
            return;
        }

        if (conversionProcess) {
            conversionProcess->deleteLater();
            conversionProcess = nullptr;
        }

        conversionProcess = new QProcess(this);
        connect(conversionProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &MainWindow::onASFtoWAVConversionFinished);
        connect(conversionProcess, &QProcess::errorOccurred, this, &MainWindow::onConversionError);
        connect(conversionProcess, &QProcess::readyReadStandardOutput,
            this, &MainWindow::onConversionOutputReady);
        connect(conversionProcess, &QProcess::readyReadStandardError,
            this, &MainWindow::onConversionOutputReady);

        QStringList ffmpegArgs;
        ffmpegArgs << "-y"
                   << "-i" << asfPath
                   << "-sample_fmt" << "s16"
                   << "-ar" << "44100"
                   << "-ac" << "2"
                   << wavPath;

        conversionProcess->setProperty("useFfmpeg", true);
        conversionProcess->setProperty("asfPath", asfPath);
        conversionProcess->setProperty("wavPath", wavPath);
        conversionProcess->setWorkingDirectory(tempDir);
        conversionProcess->start(ffmpegPath, ffmpegArgs);
        qDebug() << "Fallback (from error): starting ASF to WAV conversion with FFmpeg";
        appendLog(tr("Trying FFmpeg as fallback..."), false);

        return;
    }

    QString errorDetails;
    if (conversionProcess) {
        errorDetails = conversionProcess->errorString();
        QByteArray stdoutData = conversionProcess->readAllStandardOutput();
        QByteArray stderrData = conversionProcess->readAllStandardError();
        
        qWarning() << "Conversion process error:" << error;
        qWarning() << "Error details:" << errorDetails;
        if (!stdoutData.isEmpty()) qWarning() << "Process stdout:" << stdoutData;
        if (!stderrData.isEmpty()) qWarning() << "Process stderr:" << stderrData;
        appendLog(tr("Conversion error: %1").arg(errorDetails), true);
        
        QMessageBox::critical(this, tr("Error"),
            tr("Conversion process error: %1\nDetails: %2\nStandard error: %3").arg(error).arg(errorDetails).arg(QString::fromLocal8Bit(stderrData)));
    } else {
        qWarning() << "Conversion process error:" << error;
        appendLog(tr("Conversion process error: %1").arg(error), true);
        QMessageBox::critical(this, tr("Error"),
            tr("Conversion process error: %1").arg(error));
    }
    hideConversionProgressDialog();

    if (conversionProcess) {
        conversionProcess->deleteLater();
        conversionProcess = nullptr;
    }
}

void MainWindow::appendLog(const QString& message, bool isError)
{
    if (!logTextEdit) {
        return;
    }

    QString timestamp = QTime::currentTime().toString("HH:mm:ss");
    QString formattedMessage;

    if (isError) {
        formattedMessage = QString("<span style=\"color:#dc2626;\">[%1] [ERROR] %2</span>")
            .arg(timestamp, message.toHtmlEscaped());
    } else {
        formattedMessage = QString("<span style=\"color:#C2410C;\">[%1] %2</span>")
            .arg(timestamp, message.toHtmlEscaped());
    }

    logTextEdit->append(formattedMessage);

    QTextCursor cursor = logTextEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    logTextEdit->setTextCursor(cursor);
}

void MainWindow::onConversionOutputReady()
{
    if (conversionProcess) {
        QByteArray output = conversionProcess->readAllStandardOutput();
        if (!output.isEmpty()) {
            QList<QByteArray> rawLines = output.split('\n');
            for (const QByteArray& rawLine : rawLines) {
                QByteArray trimmed = rawLine.trimmed();
                if (trimmed.isEmpty()) {
                    continue;
                }
                int highByteCount = 0;
                for (char b : trimmed) {
                    if (static_cast<uchar>(b) >= 0x80) {
                        highByteCount++;
                    }
                }
                if (highByteCount > trimmed.size() * 0.3) {
                    continue;
                }
                QString line = QString::fromLocal8Bit(trimmed);
                qDebug() << "Conversion output:" << line;
                appendLog(line, false);
            }
        }

        QByteArray error = conversionProcess->readAllStandardError();
        if (!error.isEmpty()) {
            QList<QByteArray> rawLines = error.split('\n');
            for (const QByteArray& rawLine : rawLines) {
                QByteArray trimmed = rawLine.trimmed();
                if (trimmed.isEmpty()) {
                    continue;
                }
                int highByteCount = 0;
                for (char b : trimmed) {
                    if (static_cast<uchar>(b) >= 0x80) {
                        highByteCount++;
                    }
                }
                if (highByteCount > trimmed.size() * 0.3) {
                    continue;
                }
                QString line = QString::fromLocal8Bit(trimmed);
                qWarning() << "Conversion stderr:" << line;
                appendLog(line, false);
            }
        }
    }
}

void MainWindow::loadLanguage(const QString& languageCode)
{
    if (currentLanguage == languageCode) {
        return;
    }

    if (appTranslator) {
        qApp->removeTranslator(appTranslator);
        delete appTranslator;
        appTranslator = nullptr;
    }
    if (qtTranslator) {
        qApp->removeTranslator(qtTranslator);
        delete qtTranslator;
        qtTranslator = nullptr;
    }

    if (languageCode != "en") {
        qtTranslator = new QTranslator(this);
        QString qtTrName = QString("qt_%1").arg(languageCode == "zh" ? "zh_CN" : languageCode);
        if (qtTranslator->load(qtTrName, QLibraryInfo::path(QLibraryInfo::TranslationsPath))) {
            qApp->installTranslator(qtTranslator);
            qDebug() << "Loaded Qt translation:" << qtTrName;
        } else {
            qWarning() << "Failed to load Qt translation:" << qtTrName;
            delete qtTranslator;
            qtTranslator = nullptr;
        }
    }

    appTranslator = new QTranslator(this);
    QString translationFile = QString("musicprocessor_%1.qm").arg(languageCode);
    QString translationPath = QString(":/translations/%1").arg(translationFile);

    if (appTranslator->load(translationPath)) {
        qApp->installTranslator(appTranslator);
        currentLanguage = languageCode;
        qDebug() << "Loaded translation:" << translationPath;

        retranslateUI();
    }
    else {
        qWarning() << "Failed to load translation:" << translationPath;
        delete appTranslator;
        appTranslator = nullptr;
        if (qtTranslator) {
            qApp->removeTranslator(qtTranslator);
            delete qtTranslator;
            qtTranslator = nullptr;
        }

        if (languageCode != "en") {
            loadLanguage("en");
        }
    }
}

void MainWindow::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange) {

        retranslateUI();
    }
    QMainWindow::changeEvent(event);
}

void MainWindow::retranslateUI()
{
    setWindowTitle(tr("Orange NFS MusicEd UI By:Shynian"));

    updateExtractButtonText();
    updateConvertAllButtonText();
    updateReplaceButtonText();
    updateConvertToMUSButtonText();
    updateBackButtonText();

    if (selectMusButton) selectMusButton->setText(tr("Select MUS File"));
    if (playButton) playButton->setText(tr("Play"));
    if (stopButton) stopButton->setText(tr("Stop"));
    if (convertToASFButton) convertToASFButton->setText(tr("Convert to ASF"));
    if (selectedFileLabel && currentMusFile.isEmpty())
        selectedFileLabel->setText(tr("No file selected"));

    QList<QGroupBox*> groupBoxes = findChildren<QGroupBox*>();
    for (QGroupBox* groupBox : groupBoxes) {
        QString originalText = groupBox->property("originalText").toString();
        if (!originalText.isEmpty()) {
            groupBox->setTitle(tr(originalText.toUtf8().constData()));
        }
    }

    QList<QLabel*> labels = findChildren<QLabel*>();
    for (QLabel* label : labels) {
        QString originalText = label->property("originalText").toString();
        if (!originalText.isEmpty()) {
            label->setText(tr(originalText.toUtf8().constData()));
        }
    }

    QList<QPushButton*> buttons = findChildren<QPushButton*>();
    for (QPushButton* button : buttons) {
        QString originalText = button->property("originalText").toString();
        if (!originalText.isEmpty()) {
            button->setText(tr(originalText.toUtf8().constData()));
        }
    }

    QList<QComboBox*> comboBoxes = findChildren<QComboBox*>();
    for (QComboBox* comboBox : comboBoxes) {
        QString originalText = comboBox->property("originalText").toString();
        if (!originalText.isEmpty()) {
            if (comboBox->count() > 0) {
                comboBox->setItemText(0, tr("All Files"));
                if (comboBox->count() > 1) comboBox->setItemText(1, tr("ASF Files"));
                if (comboBox->count() > 2) comboBox->setItemText(2, tr("WAV Files"));
            }
        }
    }

    // 更新文件计数标签
    QDir asfDirectory(asfDir);
    int asfCount = asfDirectory.entryList(QStringList() << "*.asf", QDir::Files).size();
    updateAsfCountLabel(asfCount);
    
    QDir wavDirectory(wavDir);
    int wavCount = wavDirectory.entryList(QStringList() << "*.wav", QDir::Files).size();
    updateWavCountLabel(wavCount);
    
    // 更新状态标签
    updateStatusLabel(tr("Ready"));
}

QString MainWindow::formatTime(qint64 ms) const
{
    qint64 totalSeconds = ms / 1000;
    int minutes = static_cast<int>(totalSeconds / 60);
    int seconds = static_cast<int>(totalSeconds % 60);
    return QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0'));
}

void MainWindow::updatePlaybackProgress()
{
    if (mediaPlayer && mediaPlayer->playbackState() == QMediaPlayer::PlayingState) {
        qint64 position = mediaPlayer->position();
        qint64 duration = mediaPlayer->duration();
        if (duration > 0 && progressSlider) {
            int percent = static_cast<int>((position * 100) / duration);
            if (!progressSlider->isSliderDown()) {
                progressSlider->setValue(percent);
            }
        }
        if (currentTimeLabel) {
            currentTimeLabel->setText(formatTime(position));
        }
    }
}

void MainWindow::onMediaPositionChanged(qint64 position)
{
    if (currentTimeLabel) {
        currentTimeLabel->setText(formatTime(position));
    }
    if (progressSlider && mediaPlayer && mediaPlayer->duration() > 0 && !progressSlider->isSliderDown()) {
        int percent = static_cast<int>((position * 100) / mediaPlayer->duration());
        progressSlider->setValue(percent);
    }
}

void MainWindow::onMediaDurationChanged(qint64 duration)
{
    if (totalTimeLabel) {
        totalTimeLabel->setText(formatTime(duration));
    }
    if (progressSlider) {
        progressSlider->setValue(0);
    }
}

void MainWindow::onPlaybackStateChanged(QMediaPlayer::PlaybackState state)
{
    if (state == QMediaPlayer::PlayingState) {
        isPlaying = true;
        playButton->setText(tr("Pause"));
        stopButton->setEnabled(true);
        if (progressTimer) {
            progressTimer->start();
        }
    }
    else if (state == QMediaPlayer::PausedState) {
        isPlaying = false;
        playButton->setText(tr("Play"));
        stopButton->setEnabled(true);
        if (progressTimer) {
            progressTimer->stop();
        }
    }
    else {
        isPlaying = false;
        playButton->setText(tr("Play"));
        stopButton->setEnabled(false);
        if (progressTimer) {
            progressTimer->stop();
        }
        if (progressSlider) {
            progressSlider->setValue(0);
        }
        if (currentTimeLabel) {
            currentTimeLabel->setText("00:00");
        }
    }
}

void MainWindow::onVolumeChanged(int volume)
{

    if (audioOutput) {
        audioOutput->setVolume(volume / 100.0);
    }
}

void MainWindow::onBackToMain()
{

    onStopPlayback();

    clearTempFilesExceptBats();

    currentMusFile = "";
    currentMpfFile = "";
    currentProcessingMpfFile = "";
    pendingGenerateEventFiles.clear();
    currentGenerateEventFile = "";
    currentGenerateEventIdentifier = "";
    selectedFileLabel->setText(tr("No file selected"));
    extractButton->setEnabled(false);



    stackedWidget->setCurrentWidget(mainPage);
}

void MainWindow::onHowToUse()
{
    QDialog* dialog = new QDialog(this);
    dialog->setWindowTitle(tr("How to Use"));
    dialog->setMinimumSize(550, 500);
    dialog->setStyleSheet(
        "QDialog { background-color: #F5EFE0; }"
        "QLabel { color: #2C2C2C; font-size: 13px; }"
        "QPushButton {"
        "    background-color: #F97316;"
        "    color: #FFFFFF;"
        "    border: none;"
        "    border-radius: 6px;"
        "    padding: 8px 24px;"
        "    font-weight: bold;"
        "    font-size: 13px;"
        "}"
        "QPushButton:hover { background-color: #ffa500; }"
        "QPushButton:pressed { background-color: #e06a0f; }"
    );

    QVBoxLayout* layout = new QVBoxLayout(dialog);
    layout->setSpacing(15);
    layout->setContentsMargins(25, 25, 25, 25);

    QLabel* titleLabel = new QLabel(tr("Workflow Guide"));
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #F97316;");
    layout->addWidget(titleLabel);

    QLabel* introLabel = new QLabel(tr("Follow these steps to extract, edit, and repack music files:"));
    introLabel->setWordWrap(true);
    introLabel->setStyleSheet("color: #555555;");
    layout->addWidget(introLabel);

    QString steps = QString(
        "<div style='line-height: 1.8;'>"
        "<p><b style='color:#F97316;'>%1</b><br>"
        "&nbsp;&nbsp;&nbsp;%2</p>"
        "<p><b style='color:#F97316;'>%3</b><br>"
        "&nbsp;&nbsp;&nbsp;%4</p>"
        "<p><b style='color:#F97316;'>%5</b><br>"
        "&nbsp;&nbsp;&nbsp;%6</p>"
        "<p><b style='color:#F97316;'>%7</b><br>"
        "&nbsp;&nbsp;&nbsp;%8</p>"
        "<p><b style='color:#F97316;'>%9</b><br>"
        "&nbsp;&nbsp;&nbsp;%10</p>"
        "</div>"
    ).arg(
        tr("Step 1: Extract ASF from MUS"),
        tr("Click Select MUS File to choose your .mus file. The software will automatically find the matching .mpf file. Then click Extract ASF to unpack all audio files from the MUS archive."),
        tr("Step 2: Convert ASF to WAV"),
        tr("In the Extracted Files panel, click Convert All to WAV to convert all ASF files to WAV format. You can then play, edit, or replace WAV files with your own music."),
        tr("Step 3: Replace Music (optional)"),
        tr("Select a WAV file in the list, click Replace Music, and choose your own WAV file (44100 Hz, 16-bit recommended). The selected file will be replaced."),
        tr("Step 4: Convert WAV back to ASF"),
        tr("After editing, click Convert to ASF to convert all WAV files back to ASF format. Make sure all files are converted before packing."),
        tr("Step 5: Pack back to MUS"),
        tr("Click Convert to MUS to repack all ASF files back into MUS/MPF format. The original MUS/MPF files will be automatically overwritten. Before overwriting, the original files are backed up with timestamps to the MusBackUp folder located in the same directory as your MUS/MPF files.")
    );

    QLabel* stepsLabel = new QLabel(steps);
    stepsLabel->setWordWrap(true);
    stepsLabel->setTextFormat(Qt::RichText);
    layout->addWidget(stepsLabel, 1);

    QLabel* noteLabel = new QLabel(tr("Note: Original MUS/MPF files are automatically backed up to the MusBackUp folder (located in the same folder as your MUS/MPF files) before being overwritten. Backups are timestamped (yyyyMMdd_hhmmss format) so multiple versions are preserved. If backup fails due to permission issues, the overwrite will still proceed."));
    noteLabel->setStyleSheet("color: #F97316; font-style: italic; padding: 8px; background-color: #3a2a1a; border-radius: 4px;");
    noteLabel->setWordWrap(true);
    layout->addWidget(noteLabel);

    QPushButton* closeButton = new QPushButton(tr("Got it!"));
    connect(closeButton, &QPushButton::clicked, dialog, &QDialog::accept);
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    btnLayout->addWidget(closeButton);
    layout->addLayout(btnLayout);

    dialog->exec();
    dialog->deleteLater();
}

void MainWindow::onLanguageChanged(int index)
{
    QString language = languageComboBox->itemData(index).toString();
    loadLanguage(language);

    QSettings settings(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/NFSMusicEd/settings.ini", QSettings::IniFormat);
    settings.setValue("Language", language);
    settings.sync();
}

void MainWindow::onFileSelected(QListWidgetItem* item)
{
    if (item) {
        QString fileName = item->text();
        bool isWavFile = fileName.endsWith(".wav", Qt::CaseInsensitive);
        bool isAsfFile = fileName.endsWith(".asf", Qt::CaseInsensitive);

        playButton->setEnabled(isWavFile);
        replaceButton->setEnabled(isWavFile);

        QString filePath;
        if (isWavFile) {
            filePath = wavDir + "/" + fileName;
        }
        else if (isAsfFile) {
            filePath = asfDir + "/" + fileName;
        }

        QFileInfo fileInfo(filePath);
        if (fileInfo.exists()) {
            qDebug() << "Selected file:" << fileName << "Size:" << fileInfo.size() << "bytes";
        }
    }
}

void MainWindow::runGenerateEventOnDecompFiles()
{
    qDebug() << "runGenerateEventOnDecompFiles called";

    QString toolPath = tempDir + "/MPFmaster_GenerateEvent.exe";
    if (!QFile::exists(toolPath)) {
        qWarning() << "GenerateEvent tool not found:" << toolPath;
        extractButton->setEnabled(true);
        selectMusButton->setEnabled(true);
        return;
    }

    QDir d(tempDir);
    if (!d.exists()) {
        qWarning() << "Temp dir not found:" << tempDir;
        extractButton->setEnabled(true);
        selectMusButton->setEnabled(true);
        return;
    }

    QStringList allFiles = d.entryList(QDir::Files);
    qDebug() << "All files in temp dir:" << allFiles;

    pendingGenerateEventFiles = d.entryList(QStringList() << "*_decomp.txt", QDir::Files);
    qDebug() << "Found" << pendingGenerateEventFiles.count() << "_decomp.txt files:" << pendingGenerateEventFiles;

    if (pendingGenerateEventFiles.isEmpty()) {
        qDebug() << "No _decomp.txt files found in" << tempDir;

        qDebug() << "currentMpfFile:" << currentMpfFile;
        qDebug() << "currentMusFile:" << currentMusFile;
        qDebug() << "asfDir:" << asfDir;

        if (currentMpfFile.isEmpty() || currentMusFile.isEmpty()) {
            qWarning() << "currentMpfFile or currentMusFile is empty!";
            extractButton->setEnabled(true);
            selectMusButton->setEnabled(true);
            return;
        }

        
        // 清空generateEvent相关的变量，确保后续流程不受影响
        pendingGenerateEventFiles.clear();
        currentGenerateEventFile = "";
        currentGenerateEventIdentifier = "";

        if (extractProcess) {
            extractProcess->deleteLater();
            extractProcess = nullptr;
        }
        extractProcess = new QProcess(this);
        connect(extractProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &MainWindow::onExtractFinished);
        connect(extractProcess, &QProcess::errorOccurred, this, &MainWindow::onExtractError);

        QFileInfo mpfInfo(currentMpfFile);
        QString mpfBase = mpfInfo.fileName();
        QString mpfTempPath = tempDir + "/" + mpfBase;
        if (QFile::exists(mpfTempPath)) {
            QFile::remove(mpfTempPath);
        }
        if (!QFile::copy(currentMpfFile, mpfTempPath)) {
            qWarning() << "Failed to copy MPF to temp:" << currentMpfFile << "->" << mpfTempPath;
            extractButton->setEnabled(true);
            selectMusButton->setEnabled(true);
            return;
        }

        QFileInfo musInfo(currentMusFile);
        QString musBase = musInfo.fileName();
        QString musTempPath = tempDir + "/" + musBase;
        if (QFile::exists(musTempPath)) {
            QFile::remove(musTempPath);
        }
        if (!QFile::copy(currentMusFile, musTempPath)) {
            qWarning() << "Failed to copy MUS to temp:" << currentMusFile << "->" << musTempPath;
            extractButton->setEnabled(true);
            selectMusButton->setEnabled(true);
            return;
        }

        QStringList arguments;

        arguments << "-sa" << mpfBase << musBase << "asf"; // 使用相对路径，因为工作目录已设置为tempDir
        qDebug() << "ASF directory absolute path:" << asfDir;
        qDebug() << "Checking if ASF directory exists:" << QDir(asfDir).exists();

        QString mpfmasterPath = tempDir + "/MPFmaster.exe";
        if (!QFile::exists(mpfmasterPath)) {
            qWarning() << "MPFmaster.exe not found:" << mpfmasterPath;
            extractButton->setEnabled(true);
            selectMusButton->setEnabled(true);
            return;
        }

        extractProcess->setWorkingDirectory(tempDir);
        qDebug() << "Extraction working directory:" << tempDir;
        qDebug() << "MPFmaster path:" << mpfmasterPath;
        qDebug() << "MPFmaster exists and executable:" << QFile::exists(mpfmasterPath) << "and" << (QFile::permissions(mpfmasterPath) & QFile::ExeUser);
        
        // 添加进度对话框，避免用户感觉程序无响应
        if (unpackProgressDialog) {
            unpackProgressDialog->deleteLater();
            unpackProgressDialog = nullptr;
        }
        unpackProgressDialog = new QProgressDialog(this);
        unpackProgressDialog->setWindowTitle(tr("Extracting MUS File"));
        unpackProgressDialog->setLabelText(tr("Extracting ASF files from MUS..."));
        unpackProgressDialog->setRange(0, 0); // 不确定进度
        unpackProgressDialog->setCancelButton(nullptr); // 不允许取消
        unpackProgressDialog->show();
        
        extractProcess->start(mpfmasterPath, arguments);
        qDebug() << "Starting MUS extraction process with PID:" << extractProcess->processId();
        qDebug() << "Extraction arguments:" << arguments;
        qDebug() << "Starting MUS extraction asynchronously to ASF directory:" << asfDir;
        qDebug() << "Extraction arguments:" << arguments;
        return;
    }

    processNextGenerateEventFile();
}

QString MainWindow::identifierForNumber(const QString& number)
{
    if (number == "0") return "0x01000000";
    if (number == "1") return "0x12000001";
    if (number == "2") return "0x14000001";
    if (number == "3") return "0x18000001";

    return "0x00000000";
}

void MainWindow::processNextGenerateEventFile()
{
    if (pendingGenerateEventFiles.isEmpty()) {

        qDebug() << "All GenerateEvent processes completed successfully! Starting MUS extraction...";
        qDebug() << "currentMpfFile:" << currentMpfFile;
        qDebug() << "currentMusFile:" << currentMusFile;

        // 确保generateEvent相关的变量已清空
        currentGenerateEventFile = "";
        currentGenerateEventIdentifier = "";

        if (extractProcess) {
            extractProcess->deleteLater();
            extractProcess = nullptr;
        }
        extractProcess = new QProcess(this);
        connect(extractProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &MainWindow::onExtractFinished);
        connect(extractProcess, &QProcess::errorOccurred, this, &MainWindow::onExtractError);

        QFileInfo mpfInfo(currentMpfFile);
        QString mpfBase = mpfInfo.fileName();
        QFileInfo musInfo(currentMusFile);
        QString musBase = musInfo.fileName();

        QString mpfTempPath = tempDir + "/" + mpfBase;
        if (QFile::exists(mpfTempPath)) {
            QFile::remove(mpfTempPath);
        }
        if (!QFile::copy(currentMpfFile, mpfTempPath)) {
            qWarning() << "Failed to copy MPF to temp:" << currentMpfFile << "->" << mpfTempPath;
            extractButton->setEnabled(true);
            selectMusButton->setEnabled(true);
            return;
        }

        QString musTempPath = tempDir + "/" + musBase;
        if (QFile::exists(musTempPath)) {
            QFile::remove(musTempPath);
        }
        if (!QFile::copy(currentMusFile, musTempPath)) {
            qWarning() << "Failed to copy MUS to temp:" << currentMusFile << "->" << musTempPath;
            extractButton->setEnabled(true);
            selectMusButton->setEnabled(true);
            return;
        }

        QStringList arguments;

        arguments << "-sa" << mpfBase << musBase << "asf";

        QString mpfmasterPath = tempDir + "/MPFmaster.exe";
        if (!QFile::exists(mpfmasterPath)) {
            qWarning() << "MPFmaster.exe not found:" << mpfmasterPath;
            extractButton->setEnabled(true);
            selectMusButton->setEnabled(true);
            return;
        }

        extractProcess->setWorkingDirectory(tempDir);
        qDebug() << "Extraction working directory:" << tempDir;
        qDebug() << "MPFmaster path:" << mpfmasterPath;
        qDebug() << "MPFmaster exists and executable:" << QFile::exists(mpfmasterPath) << "and" << (QFile::permissions(mpfmasterPath) & QFile::ExeUser);
        
        // 添加进度对话框，避免用户感觉程序无响应
        if (unpackProgressDialog) {
            unpackProgressDialog->deleteLater();
            unpackProgressDialog = nullptr;
        }
        unpackProgressDialog = new QProgressDialog(this);
        unpackProgressDialog->setWindowTitle(tr("Extracting MUS File"));
        unpackProgressDialog->setLabelText(tr("Extracting ASF files from MUS..."));
        unpackProgressDialog->setRange(0, 0); // 不确定进度
        unpackProgressDialog->setCancelButton(nullptr); // 不允许取消
        unpackProgressDialog->show();
        
        extractProcess->start(mpfmasterPath, arguments);
        qDebug() << "Starting MUS extraction process with PID:" << extractProcess->processId();
        qDebug() << "Extraction arguments:" << arguments;
        qDebug() << "Starting MUS extraction asynchronously to ASF directory:" << asfDir;
        return;
    }

    QString f = pendingGenerateEventFiles.takeFirst();
    currentGenerateEventFile = tempDir + "/" + f;

    QRegularExpression re("(\\d+)");
    QRegularExpressionMatch match = re.match(f);
    QString number;
    if (match.hasMatch()) {
        number = match.captured(1);

        QRegularExpression reAll("(\\d+)");
        auto it = reAll.globalMatch(f);
        QString last;
        while (it.hasNext()) last = it.next().captured(1);
        if (!last.isEmpty()) number = last;
    }
    else {
        number = QString(); 
    }

    currentGenerateEventIdentifier = identifierForNumber(number);
    qDebug() << "Running GenerateEvent for" << currentGenerateEventFile << "number=" << number << "identifier=" << currentGenerateEventIdentifier;

    if (generateEventProcess) {
        generateEventProcess->deleteLater();
        generateEventProcess = nullptr;
    }
    generateEventProcess = new QProcess(this);
    generateEventProcess->setWorkingDirectory(tempDir);

    connect(generateEventProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &MainWindow::onGenerateEventFinished);
    connect(generateEventProcess, &QProcess::errorOccurred, this, &MainWindow::onGenerateEventError);

    QString toolPath = tempDir + "/MPFmaster_GenerateEvent.exe";
    QStringList arguments;
    arguments << "-a" << currentGenerateEventFile << currentGenerateEventIdentifier << "0";
    qDebug() << "Starting GenerateEvent:" << toolPath << arguments << "workdir=" << generateEventProcess->workingDirectory();
    generateEventProcess->start(toolPath, arguments);
}

void MainWindow::onGenerateEventFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitStatus)

    if (!generateEventProcess) {
        qWarning() << "onGenerateEventFinished called but generateEventProcess is null!";
        return;
    }

    QByteArray stdoutData = generateEventProcess->readAllStandardOutput();
    QByteArray stderrData = generateEventProcess->readAllStandardError();
    if (!stdoutData.isEmpty()) qDebug() << "GenerateEvent stdout:" << stdoutData;
    if (!stderrData.isEmpty()) qWarning() << "GenerateEvent stderr:" << stderrData;

    qDebug() << "GenerateEvent exit code:" << exitCode;

    if (exitCode == 0) {
        qDebug() << "GenerateEvent completed successfully for" << currentGenerateEventFile;

        processNextGenerateEventFile();
    } else {
        QString errText = QString::fromLocal8Bit(stderrData);
        if (errText.isEmpty()) errText = QString::fromLocal8Bit(stdoutData);
        QMessageBox::critical(this, tr("Error"),
                              tr("Event generation failed, exit code: %1\n%2").arg(exitCode).arg(errText));
        extractButton->setEnabled(true);
        selectMusButton->setEnabled(true);
    }

    if (generateEventProcess) {
        generateEventProcess->deleteLater();
        generateEventProcess = nullptr;
    }
}

void MainWindow::onGenerateEventError(QProcess::ProcessError error)
{
    qWarning() << "GenerateEvent process error:" << error;
    QMessageBox::critical(this, tr("Error"),
        tr("Event generation process error: %1").arg(error));

    extractButton->setEnabled(true);
    selectMusButton->setEnabled(true);

    if (generateEventProcess) {
        generateEventProcess->deleteLater();
        generateEventProcess = nullptr;
    }
}

void MainWindow::checkTools()
{
    QStringList requiredTools = {
        tempDir + "/convert.bat",
        tempDir + "/convert_back.bat",
        tempDir + "/MPFmaster.exe",
        tempDir + "/sx.exe",
        tempDir + "/MPFmaster_GenerateEvent.exe"
    };

    QStringList missing;
    for (const QString& p : requiredTools) {
        if (!QFile::exists(p)) {
            missing.append(QFileInfo(p).fileName());
        }
    }

    if (!missing.isEmpty()) {
        QMessageBox::warning(this, tr("Warning"),
            tr("The following tools are missing or failed to copy:\n%1\n\n"
                "Some features may not work properly.")
            .arg(missing.join("\n")));
    }
    else {
        qDebug() << "All required tools are present in" << tempDir;
    }
}

void MainWindow::moveFinalFiles()
{
    if (currentMusFile.isEmpty()) {
        qWarning() << "moveFinalFiles: currentMusFile is empty, nothing to move.";
        return;
    }

    QFileInfo musFileInfo(currentMusFile);
    QString baseName = musFileInfo.completeBaseName();
    QString musDir = musFileInfo.absolutePath();
    QString backupDir = musDir + "/MusBackUp";

    bool backupDirOk = true;
    QDir dir;
    if (!dir.exists(backupDir)) {
        if (!dir.mkpath(backupDir)) {
            qWarning() << "Failed to create backup directory:" << backupDir << "- will proceed without backup.";
            backupDirOk = false;
        }
    }

    bool overwrite = shouldOverwriteFiles;
    QString originalMusPath = currentMusFile;
    QString originalMpfPath = currentMpfFile;

    QString timeStamp = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");

    auto removeFileWithRetry = [](const QString& filePath, int retries = 5, int delayMs = 200) -> bool {
        for (int i = 0; i < retries; ++i) {
            if (!QFile::exists(filePath)) return true;
            if (QFile::remove(filePath)) return true;
            QThread::msleep(delayMs);
        }
        return !QFile::exists(filePath);
    };

    auto copyFileWithRetry = [](const QString& src, const QString& dst, int retries = 5, int delayMs = 200) -> bool {
        for (int i = 0; i < retries; ++i) {
            if (QFile::exists(dst)) QFile::remove(dst);
            if (QFile::copy(src, dst)) return true;
            QThread::msleep(delayMs);
        }
        return false;
    };

    if (!currentMpfFile.isEmpty()) {
        QString numericMpfName = "0.mpf";
        QString srcMpf = musTempDir + "/" + numericMpfName;

        if (!QFile::exists(srcMpf)) {
            srcMpf = musTempDir + "/" + baseName + ".mpf";
            if (!QFile::exists(srcMpf)) {
                srcMpf = tempDir + "/" + baseName + ".mpf";
            }
        }

        if (overwrite && !originalMpfPath.isEmpty() && QFile::exists(originalMpfPath) && backupDirOk) {
            QString backupMpf = backupDir + "/" + baseName + "_" + timeStamp + ".mpf";
            if (QFile::copy(originalMpfPath, backupMpf)) {
                qDebug() << "Backed up original MPF to:" << backupMpf;
            } else {
                qWarning() << "Failed to backup original MPF:" << originalMpfPath;
            }
        }

        if (overwrite && !originalMpfPath.isEmpty() && QFile::exists(srcMpf)) {
            if (!removeFileWithRetry(originalMpfPath)) {
                qWarning() << "Failed to remove original MPF after retries:" << originalMpfPath;
            }

            if (!copyFileWithRetry(srcMpf, originalMpfPath)) {
                qWarning() << "Failed to copy MPF to original location:" << srcMpf << "->" << originalMpfPath;
            } else {
                qDebug() << "Overwritten original MPF:" << originalMpfPath;
            }
        }
    }

    QString numericMusName = "0.mus";
    QString srcMus = musTempDir + "/" + numericMusName;
    if (!QFile::exists(srcMus)) {
        srcMus = musTempDir + "/" + baseName + ".mus";
        if (!QFile::exists(srcMus)) {
            srcMus = tempDir + "/" + baseName + ".mus";
        }
    }

    bool musOverwritten = false;
    if (QFile::exists(srcMus)) {

        if (overwrite && QFile::exists(originalMusPath) && backupDirOk) {
            QString backupMus = backupDir + "/" + baseName + "_" + timeStamp + ".mus";
            if (QFile::copy(originalMusPath, backupMus)) {
                qDebug() << "Backed up original MUS to:" << backupMus;
            } else {
                qWarning() << "Failed to backup original MUS:" << originalMusPath;
            }
        }

        if (overwrite) {

            if (!removeFileWithRetry(originalMusPath)) {
                qWarning() << "Failed to remove original MUS after retries:" << originalMusPath;
            }

            if (!copyFileWithRetry(srcMus, originalMusPath)) {
                qWarning() << "Failed to copy MUS to original location:" << srcMus << "->" << originalMusPath;
            } else {
                qDebug() << "Overwritten original MUS:" << originalMusPath;
                musOverwritten = true;
            }
        }
    } else {
        qWarning() << "Generated MUS file not found in temp locations. Tried:" 
                   << musTempDir + "/" + numericMusName 
                   << musTempDir + "/" + baseName + ".mus"
                   << tempDir + "/" + baseName + ".mus";
    }

    QDir temp(tempDir);
    QStringList files = temp.entryList(QDir::Files);
    for (const QString& f : files) {
        if (f.contains("_decomp", Qt::CaseInsensitive)) {
            QString newName = f;
            newName.replace("_decomp", "", Qt::CaseInsensitive);
            QString oldPath = tempDir + "/" + f;
            QString newPath = tempDir + "/" + newName;
            if (QFile::exists(oldPath) && !QFile::exists(newPath)) {
                if (QFile::rename(oldPath, newPath)) {
                    qDebug() << "Renamed" << oldPath << "->" << newPath;
                }
                else {
                    qWarning() << "Failed to rename" << oldPath << "->" << newPath;
                }
            }
        }
    }

    QString message;
    if (overwrite) {
        if (!QFile::exists(srcMus)) {
            message = tr("Packing failed!\n\n"
                        "The generated MUS file was not found in the temp directory.\n"
                        "Original files have NOT been modified.\n\n"
                        "Please check the log output for details.");
            if (!hasShownExportComplete) {
                QMessageBox::critical(this, tr("Export Failed"), message);
                hasShownExportComplete = true;
            }
        } else if (musOverwritten) {
            message = tr("Packing completed successfully!\n\n"
                        "Original MUS/MPF files have been overwritten:\n"
                        "%1\n\n"
                        "%2")
                      .arg(QFileInfo(originalMusPath).absolutePath())
                      .arg(backupDirOk
                           ? tr("Original file backups are saved in the MusBackUp folder:\n%1").arg(backupDir)
                           : tr("Note: Backup could not be created (permission issue), but files were overwritten."));
            if (!hasShownExportComplete) {
                QMessageBox::information(this, tr("Export Complete"), message);
                hasShownExportComplete = true;
            }
        } else {
            message = tr("Packing may have failed!\n\n"
                        "The generated MUS file exists but could not be copied to:\n"
                        "%1\n\n"
                        "The file may be in use by another program. "
                        "Please close any program using the file and try again.")
                      .arg(originalMusPath);
            if (!hasShownExportComplete) {
                QMessageBox::warning(this, tr("Export Warning"), message);
                hasShownExportComplete = true;
            }
        }
    } else {
        message = tr("Files have been successfully exported.\n\n"
                    "Exported files are located at:\n"
                    "%1")
                  .arg(QFileInfo(srcMus).absolutePath());
        if (!hasShownExportComplete) {
            QMessageBox::information(this, tr("Export Complete"), message);
            hasShownExportComplete = true;
        }
    }

    qDebug() << "moveFinalFiles finished. Backup directory:" << backupDir;
}

void MainWindow::closeEvent(QCloseEvent* event)
{

    onStopPlayback();
    clearTempFilesExceptBats();
    if (extractProcess && extractProcess->state() == QProcess::Running) {
        extractProcess->kill();
        extractProcess->waitForFinished(1000);
    }
    if (conversionProcess && conversionProcess->state() == QProcess::Running) {
        conversionProcess->kill();
        conversionProcess->waitForFinished(1000);
    }
    event->accept();
}

void MainWindow::onReplaceMusic()
{

    QList<QListWidgetItem*> sel = fileListWidget->selectedItems();
    if (sel.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("Please select a WAV file to replace."));
        return;
    }

    QString targetName = sel[0]->text();
    if (!targetName.endsWith(".wav", Qt::CaseInsensitive)) {
        QMessageBox::warning(this, tr("Error"), tr("Please select a WAV file."));
        return;
    }

    QString src = QFileDialog::getOpenFileName(this, tr("Select replacement"), "", tr("WAV Files (*.wav)"));
    if (src.isEmpty()) return;

    QString dst = wavDir + "/" + targetName;
    bool targetIsPlaying = (QFileInfo(dst).absoluteFilePath() == QFileInfo(currentPlayingFile).absoluteFilePath());

    if (mediaPlayer && targetIsPlaying) {
        mediaPlayer->stop();
        mediaPlayer->setSource(QUrl());
        currentPlayingFile = "";
        isPlaying = false;
        QCoreApplication::processEvents();
    }

    bool replaced = false;
    QString errorMsg;

    for (int attempt = 0; attempt < 3 && !replaced; attempt++) {
        if (attempt > 0) {
            QThread::msleep(100);
            QCoreApplication::processEvents();
        }

        if (QFile::exists(dst)) {
            if (!QFile::remove(dst)) {
                errorMsg = tr("Cannot remove original file (file in use).");
                continue;
            }
        }

        if (QFile::copy(src, dst)) {
            replaced = true;
        } else {
            errorMsg = tr("Cannot copy file to target location.");
            QFile::remove(dst);
        }
    }

    if (!replaced) {
        QMessageBox::critical(this, tr("Error"),
            tr("Replace failed: %1\n\nSource: %2\nTarget: %3").arg(errorMsg, src, dst));
        return;
    }

    updateFileList();
    if (statusLabel) {
        updateStatusLabel(tr("Replaced: %1").arg(targetName), false);
    }
}

void MainWindow::onConvertToASF()
{

    if (mediaPlayer && isPlaying) {
        mediaPlayer->stop();
        mediaPlayer->setSource(QUrl());
        currentPlayingFile = "";
        isPlaying = false;
        QCoreApplication::processEvents();
    }

    QDir d(wavDir);
    QStringList wavs = d.entryList(QStringList() << "*.wav", QDir::Files, QDir::NoSort);
    if (wavs.isEmpty()) {
        QMessageBox::information(this, tr("Info"), tr("No WAV files found."));
        return;
    }

    std::sort(wavs.begin(), wavs.end(), [](const QString& a, const QString& b) {
        int na = QFileInfo(a).baseName().toInt();
        int nb = QFileInfo(b).baseName().toInt();
        return na < nb;
    });

    filesToConvert = wavs;
    showConversionProgressDialog(tr("Converting WAV to ASF"), filesToConvert.size());
    currentConversionIndex = 0;
    convertNextWAVtoASF();
}

void MainWindow::onConvertToMUS()
{
    QDir d(asfDir);
    QStringList asfs = d.entryList(QStringList() << "*.asf", QDir::Files);

    qDebug() << "Found " << asfs.size() << " ASF files in:" << asfDir;
    if (!asfs.isEmpty()) {
        qDebug() << "ASF files:" << asfs;
    }

    if (asfs.isEmpty()) {
        QMessageBox::information(this, tr("Info"), tr("No ASF files found."));
        return;
    }
    if (currentMusFile.isEmpty() || currentMpfFile.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("Please select MUS and MPF files first."));
        return;
    }

    shouldOverwriteFiles = true;
    filesToConvert = asfs;
    hasShownExportComplete = false;
    showConversionProgressDialog(tr("Packing ASF to MUS"), 1);

    packAllAsfToMus();
}

void MainWindow::packAllAsfToMus()
{
    QFileInfo mpfInfo(currentMpfFile);
    QFileInfo musInfo(currentMusFile);
    QString mpfBase = mpfInfo.fileName();
    QString musBase = musInfo.fileName();

    QString numericMpfName = "0.mpf";
    QString numericMusName = "0.mus";
    QString mpfTempPath = musTempDir + "/" + numericMpfName;
    QString musTempPath = musTempDir + "/" + numericMusName;

    qDebug() << "Current MPF file:" << currentMpfFile;
    qDebug() << "Current MUS file:" << currentMusFile;
    qDebug() << "MPF temp path:" << mpfTempPath;
    qDebug() << "MUS temp path:" << musTempPath;
    qDebug() << "ASF directory:" << asfDir;

    QDir musTempDirObj(musTempDir);
    if (!musTempDirObj.exists()) {
        musTempDirObj.mkpath(musTempDir);
    }

    QStringList oldFiles = musTempDirObj.entryList(QDir::Files);
    for (const QString& oldFile : oldFiles) {
        QFile::remove(musTempDir + "/" + oldFile);
    }
    qDebug() << "Cleaned MUS_Temp directory, removed" << oldFiles.size() << "old files";

    if (!QFile::copy(currentMpfFile, mpfTempPath)) {
        qWarning() << "Failed to copy MPF to temp:" << currentMpfFile << "->" << mpfTempPath;
        QMessageBox::critical(this, tr("Error"), tr("Failed to copy MPF file to temp directory."));
        hideConversionProgressDialog();
        return;
    }
    qDebug() << "Successfully copied MPF to temp:" << mpfTempPath;

    if (!QFile::copy(currentMusFile, musTempPath)) {
        qWarning() << "Failed to copy MUS to temp:" << currentMusFile << "->" << musTempPath;
        QMessageBox::critical(this, tr("Error"), tr("Failed to copy MUS file to temp directory."));
        hideConversionProgressDialog();
        return;
    }
    qDebug() << "Successfully copied MUS to temp:" << musTempPath;

    qDebug() << "Copying all ASF files to MUS temp directory:";
    for (const QString &asfFileName : filesToConvert) {
        QString asfPath = asfDir + "/" + asfFileName;
        QString asfDestPath = musTempDir + "/" + asfFileName;

        if (QFile::exists(asfPath)) {
            if (QFile::exists(asfDestPath)) {
                QFile::remove(asfDestPath);
            }
            if (!QFile::copy(asfPath, asfDestPath)) {
                qWarning() << "Failed to copy ASF to temp:" << asfPath << "->" << asfDestPath;
            } else {
                qDebug() << "Successfully copied ASF to temp:" << asfDestPath;
            }
        }
    }

    QString mpfmasterPath = tempDir + "/MPFmaster.exe";
    if (!QFile::exists(mpfmasterPath)) {
        QString originalToolPath = QApplication::applicationDirPath() + "/tools/MPFmaster.exe";
        if (QFile::exists(originalToolPath)) {
            mpfmasterPath = originalToolPath;
        } else {
            qWarning() << "MPFmaster.exe not found in temp or tools directory!";
            QMessageBox::critical(this, tr("Error"), tr("MPFmaster.exe tool not found, cannot complete packing."));
            hideConversionProgressDialog();
            return;
        }
    }

    if (conversionProcess) {
        conversionProcess->deleteLater();
        conversionProcess = nullptr;
    }

    conversionProcess = new QProcess(this);
    connect(conversionProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        this, &MainWindow::onASFtoMUSConversionFinished);
    connect(conversionProcess, &QProcess::errorOccurred, this, &MainWindow::onConversionError);
    connect(conversionProcess, &QProcess::readyReadStandardOutput, this, &MainWindow::onConversionOutputReady);
    connect(conversionProcess, &QProcess::readyReadStandardError, this, &MainWindow::onConversionOutputReady);

    conversionProcess->setWorkingDirectory(tempDir);

    QDir tempDirObj(tempDir);
    QString relativeMusTempDir = tempDirObj.relativeFilePath(musTempDir);
    QString mpfArg = (relativeMusTempDir + "/" + numericMpfName).replace('/', '\\');
    QString asfDirArg = QString(relativeMusTempDir).replace('/', '\\');

    QStringList arguments;
    arguments << "-sx" << mpfArg << asfDirArg;

    qDebug() << "Starting ASF -> MUS packing:" << mpfmasterPath << arguments;
    conversionProcess->start(mpfmasterPath, arguments);

    updateConversionProgress(0, tr("Packing all ASF files..."));
}

void MainWindow::convertNextWAVtoASF()
{
    if (currentConversionIndex >= filesToConvert.size() || !isConverting) {
        hideConversionProgressDialog();
        updateFileList();

        if (currentConversionIndex >= filesToConvert.size()) {

            QDir asfDirectory(asfDir);
            QStringList asfFiles = asfDirectory.entryList(QStringList() << "*.asf", QDir::Files);
            convertToMUSButton->setEnabled(!asfFiles.isEmpty());
            QMessageBox::information(this, tr("Done"), tr("Batch WAV -> ASF conversion completed."));
        }
        return;
    }

    QString wavName = filesToConvert[currentConversionIndex];
    QString wavPath = wavDir + "/" + wavName;

    QString asfName = QFileInfo(wavName).baseName() + ".asf";
    QString asfPath = asfDir + "/" + asfName;

    updateConversionProgress(currentConversionIndex, tr("Converting %1 to ASF...").arg(wavName));

    if (conversionProcess) {
        conversionProcess->deleteLater();
    }

    conversionProcess = new QProcess(this);
    connect(conversionProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        this, &MainWindow::onWAVtoASFConversionFinished);
    connect(conversionProcess, &QProcess::errorOccurred, this, &MainWindow::onConversionError);
    connect(conversionProcess, &QProcess::readyReadStandardOutput,
        this, &MainWindow::onConversionOutputReady);
    connect(conversionProcess, &QProcess::readyReadStandardError,
        this, &MainWindow::onConversionOutputReady);

    QString sxPath = tempDir + "/sx.exe";
    QString sxAcfRel = "wav/" + wavName;
    QString sxAsfRel = "asf/" + asfName;
    QStringList arguments;
    arguments << "-pcstream" << "-eaxa_blk" << sxAcfRel << "-=" + sxAsfRel;

    conversionProcess->setProperty("asfPath", asfPath);
    conversionProcess->setProperty("wavPath", wavPath);
    conversionProcess->setWorkingDirectory(tempDir);
    conversionProcess->start(sxPath, arguments);

    qDebug() << "Starting WAV to ASF conversion:" << wavName << "->" << asfName;
}

void MainWindow::onWAVtoASFConversionFinished(int exitCode, QProcess::ExitStatus )
{
    Q_UNUSED(exitCode);

    QString asfPath;
    QString wavPath;
    if (conversionProcess) {
        asfPath = conversionProcess->property("asfPath").toString();
        wavPath = conversionProcess->property("wavPath").toString();
    }

    bool success = QFile::exists(asfPath) && QFileInfo(asfPath).size() > 0;

    if (success) {
        if (!wavPath.isEmpty() && QFile::exists(wavPath)) {
            QFile::setPermissions(wavPath, QFile::WriteOwner | QFile::ReadOwner);
            bool removed = false;
            for (int attempt = 0; attempt < 3 && !removed; attempt++) {
                if (attempt > 0) {
                    if (mediaPlayer && QFileInfo(currentPlayingFile).absoluteFilePath() == QFileInfo(wavPath).absoluteFilePath()) {
                        mediaPlayer->stop();
                        mediaPlayer->setSource(QUrl());
                        currentPlayingFile = "";
                        isPlaying = false;
                    }
                    QCoreApplication::processEvents();
                    QThread::msleep(100);
                }
                removed = QFile::remove(wavPath);
            }
            if (removed) {
                qDebug() << "Deleted original WAV file:" << wavPath;
            } else {
                qWarning() << "Failed to delete original WAV file after retries:" << wavPath;
            }
        }
        
        currentConversionIndex++;
        updateConversionProgress(currentConversionIndex, tr("Converted %1 of %2").arg(currentConversionIndex).arg(totalConversions));
        QTimer::singleShot(100, this, &MainWindow::convertNextWAVtoASF);
    }
    else {
        qWarning() << "WAV->ASF conversion failed";
        appendLog(tr("WAV to ASF conversion failed."), true);
        hideConversionProgressDialog();
    }
    if (conversionProcess) {
        conversionProcess->deleteLater();
        conversionProcess = nullptr;
    }
}

void MainWindow::onASFtoMUSConversionFinished(int exitCode, QProcess::ExitStatus exitStatus )
{
    QByteArray stdoutData;
    QByteArray stderrData;

    if (conversionProcess) {
        stdoutData = conversionProcess->readAllStandardOutput();
        stderrData = conversionProcess->readAllStandardError();

        qDebug() << "ASF->MUS conversion finished with exit code:" << exitCode << "exit status:" << exitStatus;
        if (!stdoutData.isEmpty()) qDebug() << "Process stdout:" << stdoutData;
        if (!stderrData.isEmpty()) qWarning() << "Process stderr:" << stderrData;
    }

    if (exitCode == 0) {
        updateConversionProgress(1, tr("Packing complete, finalizing..."));
        moveFinalFiles();
        hideConversionProgressDialog();
        updateFileList();
    }
    else {
        qWarning() << "ASF->MUS packing failed with code" << exitCode << "status:" << exitStatus;

        QMessageBox::critical(this, tr("Error"),
            tr("ASF -> MUS packing failed, exit code: %1\nStandard error: %2")
                .arg(exitCode).arg(QString::fromLocal8Bit(stderrData)));
        hideConversionProgressDialog();
    }

    if (conversionProcess) {
        conversionProcess->deleteLater();
        conversionProcess = nullptr;
    }
}

void MainWindow::onProgressSliderMoved(int position)
{
    if (mediaPlayer && mediaPlayer->duration() > 0) {
        qint64 p = (qint64(position) * mediaPlayer->duration()) / 100;
        mediaPlayer->setPosition(p);
    }
}