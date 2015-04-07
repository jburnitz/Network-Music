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

#include <QSpinBox>



class PacketCapturer;

class tone;
class Generator;

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

    QByteArray* CreateToneBuffer(const QAudioFormat &format, int frequency);

    //currently unused
    QTimer *m_pullTimer;

    //graphical elements
    QPushButton *m_modeButton;
    QPushButton *m_suspendResumeButton;
    QComboBox *m_networkDeviceBox;
    QComboBox *m_audioDeviceBox;
    QLabel *m_volumeLabel;
    QSlider *m_volumeSlider;
    QSlider *m_frequencySlider;
    QSpinBox *m_numberOfTones;
    QLabel *m_numberOfTonesLabel;

    QHash<int, Generator*> toneGenerators;
    QHash<int, QByteArray*> toneBuffers;

    int numberOfTones;
    //tone* Tones[TONE_COUNT];
    QList<tone*> Tones;
    //QThread* audioThreads[TONE_COUNT];
    QList<QThread*> audioThreads;

    QThread* PacketCaptureThread;

    PacketCapturer* pk;

    int currentTone;


private slots:
    void deviceChanged(int index);
    void DoNumberOfTonesChanged(int value);

    void frequencyChanged(int frequency);
    void SetFrequency(int frequency);
    void PcapButtonPressed();

signals:
   void VolumeChanged(int);
   void Resume_Audio();
   void Pause_Audio();
   void Start_Audio();
   void SIGNAL_BEGIN_CAPTURE();
};

#endif // AUDIOOUTPUT_H
