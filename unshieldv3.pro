TEMPLATE = app
CONFIG += console c++1z
CONFIG -= app_bundle
CONFIG -= qt
LIBS += -lstdc++fs
CXXFLAGS += -Wall
QMAKE_CXXFLAGS += -Wno-unused-variable

SOURCES += \
    main.cpp \
    blast/blast.c \
    installshieldarchivev3.cpp

HEADERS += \
    blast/blast.h \
    installshieldarchivev3.h

DISTFILES += \
    README.md

