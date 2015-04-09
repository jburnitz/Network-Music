#ifndef AUDIOOUTPUT_H
#define AUDIOOUTPUT_H

#include <math.h>

#include <QAudioOutput>
#include <QByteArray>
#include <QComboBox>
#include <QIODevice>
#include <QLabel>
#include <QMainWindow>
#include <QObject>
#include <QPushButton>
#include <QSlider>
#include <QTimer>
#include <QHash>
#include <QStatusBar>

#include <QSpinBox>



class PacketCapturer;

class tone;
class Generator;
class ToneManager;

class AudioTest : public QMainWindow
{
    Q_OBJECT

public:
    AudioTest();
    void Setup();
    ~AudioTest();

private:
    void initializeWindow();
    void initializeAudio();
    void createAudioOutput();
    void AddTone(int i);

    //currently unused
    QTimer *m_pullTimer;

    //graphical elements
    QComboBox *m_networkDeviceBox;
    QComboBox *m_audioDeviceBox;
    QLabel *m_volumeLabel;
    QSlider *m_volumeSlider;
    QSlider *m_frequencySlider;
    QSpinBox *m_numberOfTones;
    QLabel *m_numberOfTonesLabel;
    QStatusBar *m_statusBar;
    QLabel *m_statusBarLabel;

    //useless?
    QString *m_statusBarLabelString;


    QThread* PacketCaptureThread;
    QThread* ToneManagerThread;

    PacketCapturer* pk;

    int currentTone;


private slots:
    void deviceChanged(int index);
    void PcapButtonPressed();

signals:
   void VolumeChanged(int);
   void Resume_Audio();
   void Pause_Audio();
   void Start_Audio();
   void SIGNAL_BEGIN_CAPTURE();
};

#endif // AUDIOOUTPUT_H
