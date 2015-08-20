TEMPLATE = app
TARGET = audiooutput

QT += multimedia widgets

win32: INCLUDEPATH += $$PWD/win/WpdPack/Include

unix: LIBS += -lpcap
win32: LIBS += "-L$$PWD/win/WpdPack/Lib" -lwpcap -lws2_32

HEADERS       = audiooutput.h \
    tone.h \
    generator.h \
    packetcapturer.h \
    tonemanager.h \
    win/WpdPack/Include/pcap/bluetooth.h \
    win/WpdPack/Include/pcap/bpf.h \
    win/WpdPack/Include/pcap/namedb.h \
    win/WpdPack/Include/pcap/pcap.h \
    win/WpdPack/Include/pcap/sll.h \
    win/WpdPack/Include/pcap/usb.h \
    win/WpdPack/Include/pcap/vlan.h \
    win/WpdPack/Include/bittypes.h \
    win/WpdPack/Include/ip6_misc.h \
    win/WpdPack/Include/Packet32.h \
    win/WpdPack/Include/pcap-bpf.h \
    win/WpdPack/Include/pcap-namedb.h \
    win/WpdPack/Include/pcap-stdinc.h \
    win/WpdPack/Include/pcap.h \
    win/WpdPack/Include/remote-ext.h \
    win/WpdPack/Include/Win32-Extensions.h

SOURCES       = audiooutput.cpp \
                main.cpp \
    tone.cpp \
    generator.cpp \
    packetcapturer.cpp \
    tonemanager.cpp

target.path = $$[QT_INSTALL_EXAMPLES]/multimedia/audiooutput
INSTALLS += target

SUBDIRS += \
