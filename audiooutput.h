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

#define TONE_COUNT (5)

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

    QByteArray* CreateToneBuffer(const QAudioFormat &format, int frequency);

    QTimer *m_pullTimer;

    // Owned by layout
    QPushButton *m_modeButton;
    QPushButton *m_suspendResumeButton;
    QComboBox *m_networkDeviceBox;
    QComboBox *m_audioDeviceBox;
    QLabel *m_volumeLabel;
    QSlider *m_volumeSlider;
    QSlider *m_frequencySlider;

    QHash<int, Generator*> toneGenerators;
    QHash<int, QByteArray*> toneBuffers;

    tone* Tones[TONE_COUNT];
    QThread* audioThreads[TONE_COUNT];

    QThread* PacketCaptureThread;

    PacketCapturer* pk;

    int currentTone;


private slots:
    void deviceChanged(int index);

    void frequencyChanged(int frequency);
    void SetFrequency(int frequency);
    void PcapButtonPressed();

signals:
   void VolumeChanged(int);
   void Start_Audio();
   void SIGNAL_BEGIN_CAPTURE();
};

#endif // AUDIOOUTPUT_H
