TEMPLATE = app
TARGET = audiooutput

QT += multimedia widgets

LIBS += -lpcap

HEADERS       = audiooutput.h \
    tone.h \
    generator.h \
    packetcapturer.h \
    tonemanager.h

SOURCES       = audiooutput.cpp \
                main.cpp \
    tone.cpp \
    generator.cpp \
    packetcapturer.cpp \
    tonemanager.cpp

target.path = $$[QT_INSTALL_EXAMPLES]/multimedia/audiooutput
INSTALLS += target
