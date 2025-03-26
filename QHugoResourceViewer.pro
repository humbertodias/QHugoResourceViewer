# Define the project
TEMPLATE = app
TARGET = QHugoResourceViewer
VERSION = 0.1
QT += widgets multimedia

DESTDIR = build
OBJECTS_DIR = build/obj
MOC_DIR = build/moc
RCC_DIR = build/rcc
UI_DIR = build/ui

target.path = /usr/local/bin

# Set the C++ standard
QMAKE_CXXFLAGS += -std=c++17

# Include directories
INCLUDEPATH += $$PWD/include

# Define the header and source files
HEADERS += \
    include/hugo/hugoviewer.h \
    include/hugo/hugoresource.h \
    include/formats/cgffile.h \
    include/formats/tilfile.h \
    include/formats/rawfile.h \
    include/formats/lzpfile.h \
    include/formats/palfile.h \
    include/formats/cbrfile.h \
    include/formats/pbrfile.h \
    include/formats/brsfile.h \
    include/formats/wavfile.h \
    include/hugo/hugo_archive_processor.h

SOURCES += \
    main.cpp \
    src/hugo/hugoviewer.cpp \
    src/hugo/hugoresource.cpp \
    src/formats/cgffile.cpp \
    src/formats/tilfile.cpp \
    src/formats/rawfile.cpp \
    src/formats/lzpfile.cpp \
    src/formats/palfile.cpp \
    src/formats/cbrfile.cpp \
    src/formats/pbrfile.cpp \
    src/formats/brsfile.cpp \
    src/formats/wavfile.cpp \
    src/hugo/hugo_archive_processor.cpp

# Additional configuration for macOS (optional)
macx {
    QMAKE_BUNDLE_ID = com.example.QHugoResourceViewer
    QMAKE_MACOSX_BUNDLE = true
    QMAKE_BUNDLE_VERSION = $$VERSION
}

# Android-specific configuration (optional)
android {
    TARGET = $$TARGET.so
    SOURCES += android_specific.cpp
}

# Windows-specific configuration
win32 {
    CONFIG += console
}

# Install target
INSTALLS += target

# Add Qt multimedia and widgets explicitly (already included in CONFIG)
LIBS += -lQt6Widgets -lQt6Multimedia
