#-------------------------------------------------
#
# Project created by QtCreator 2024-01-01T12:00:00
#
#-------------------------------------------------

QT       += core gui widgets multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# 发布版本配置
CONFIG(release, debug|release) {
    DESTDIR = C:/Users/Shynian/Documents/source/音乐编辑器/TEST
    OBJECTS_DIR = C:/QtBuild/NFSMusicEd/release/.obj
    MOC_DIR = C:/QtBuild/NFSMusicEd/release/.moc
    RCC_DIR = C:/QtBuild/NFSMusicEd/release/.rcc
    UI_DIR = C:/QtBuild/NFSMusicEd/release/.ui

    # 发布版本优化
    win32-msvc {
        QMAKE_CXXFLAGS_RELEASE -= -O
        QMAKE_CXXFLAGS_RELEASE -= -O1
        QMAKE_CXXFLAGS_RELEASE -= -O2
        QMAKE_CXXFLAGS_RELEASE *= /O2
    } else {
        QMAKE_CXXFLAGS_RELEASE -= -O
        QMAKE_CXXFLAGS_RELEASE -= -O1
        QMAKE_CXXFLAGS_RELEASE -= -O2
        QMAKE_CXXFLAGS_RELEASE *= -O3
    }

    # 发布版本定义
    DEFINES += QT_NO_DEBUG_OUTPUT \
               QT_NO_WARNING_OUTPUT \
               NDEBUG
}

# 调试版本配置
CONFIG(debug, debug|release) {
    DESTDIR = C:/QtBuild/NFSMusicEd/debug/bin
    OBJECTS_DIR = C:/QtBuild/NFSMusicEd/debug/.obj
    MOC_DIR = C:/QtBuild/NFSMusicEd/debug/.moc
    RCC_DIR = C:/QtBuild/NFSMusicEd/debug/.rcc
    UI_DIR = C:/QtBuild/NFSMusicEd/debug/.ui

    # 调试版本定义
    DEFINES += DEBUG \
               QT_DEBUG

    # 调试信息
    QMAKE_CXXFLAGS_DEBUG += -g
}

# 高DPI支持
greaterThan(QT_MAJOR_VERSION, 5.5) {
    # Qt 5.6+ 高DPI缩放
    greaterThan(QT_MINOR_VERSION, 5) {
        DEFINES += QT_HIGHDPI_SUPPORT
        QMAKE_CXXFLAGS += -DQT_ENABLE_HIGHDPI_SCALING
        QMAKE_CXXFLAGS += -DQT_AUTO_SCREEN_SCALE_FACTOR=1
    }
}

# 目标配置
TARGET = "Orange NFS MusicEd UI"
TEMPLATE = app

# 源文件
SOURCES += \
    main.cpp \
    MainWindow.cpp

# 头文件
HEADERS += \
    MainWindow.h

# 资源文件
RESOURCES += \
    resources.qrc

# 翻译文件
TRANSLATIONS +=

# 版本信息
VERSION = 1.0.1

# 包含路径
INCLUDEPATH += .

# 平台特定配置
win32 {
    # Windows平台配置
    LIBS += -luser32

    # 资源文件
    RC_FILE = app.rc

    # Windows版本要求
    win32-g++ {
        # MinGW配置
        QMAKE_CXXFLAGS += -std=c++17
        QMAKE_LFLAGS += -Wl,-subsystem,windows
    }

    win32-msvc {
        # MSVC配置
        QMAKE_CXXFLAGS += /std:c++17
        QMAKE_LFLAGS += /SUBSYSTEM:WINDOWS,5.01
    }

    # 图标文件
    ICON = app.ico
}

unix:!macx {
    # Linux配置
    LIBS += -lpthread
}

macx {
    # macOS配置
    LIBS += -framework Cocoa

    # 应用包配置
    QMAKE_INFO_PLIST = Info.plist

    # macOS部署
    ICON = app.icns
}

# 编译器特定标志
gcc {
    # GCC编译器配置
    QMAKE_CXXFLAGS += -Wall
    QMAKE_CXXFLAGS += -Wextra
    QMAKE_CXXFLAGS += -Wno-unused-parameter

    # C++17支持
    QMAKE_CXXFLAGS += -std=c++17
}

msvc {
    # MSVC编译器配置
    QMAKE_CXXFLAGS += /W3
    QMAKE_CXXFLAGS += /MP

    # 禁用特定警告
    QMAKE_CXXFLAGS += /wd4996  # 不安全函数警告
    QMAKE_CXXFLAGS += /wd4251  # DLL接口警告

    # C++17支持
    QMAKE_CXXFLAGS += /std:c++17
}

clang {
    # Clang编译器配置
    QMAKE_CXXFLAGS += -Wall
    QMAKE_CXXFLAGS += -Wextra
    QMAKE_CXXFLAGS += -Wno-unused-parameter

    # C++17支持
    QMAKE_CXXFLAGS += -std:c++17
}

# 禁用特定警告
QMAKE_CXXFLAGS_WARN_ON -= -Wunused-parameter
QMAKE_CXXFLAGS_WARN_OFF += -Wunused-variable

# 定义
DEFINES += \
    QT_DEPRECATED_WARNINGS \
    QT_MULTIMEDIA_LIB \
    QT_WIDGETS_LIB

# 复制依赖文件到输出目录（Windows）
win32 {
    QMAKE_POST_LINK += $$quote(cmd /c if not exist $$shell_path($$DESTDIR/tools) mkdir $$shell_path($$DESTDIR/tools)$$escape_expand(\\n\\t))
    QMAKE_POST_LINK += $$quote(cmd /c copy /y $$shell_path($$PWD/tools/*) $$shell_path($$DESTDIR/tools/)$$escape_expand(\\n\\t))
    QMAKE_POST_LINK += $$quote(cmd /c windeployqt --release --no-system-d3d-compiler --no-opengl-sw --no-translations $$shell_path($$DESTDIR/$$TARGET).exe$$escape_expand(\\n\\t))
}

# 安装后处理
unix {
    # Linux安装后设置权限
    QMAKE_POST_LINK += chmod +x $$DESTDIR/$$TARGET
}

# 生成的文件
FORMS +=

# 其他文件
OTHER_FILES += \
    README.md \
    LICENSE \
    CHANGELOG.md

# 部署配置
contains(QT_VERSION, ^5\\.[0-9]+\\.) {
    # Qt5部署配置
    load(qt_toolchains)
}

# 测试配置（可选）
CONFIG += console
CONFIG -= app_bundle
