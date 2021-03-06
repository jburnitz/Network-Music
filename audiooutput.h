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

protected:
    void initializeWindow();
    void initializeAudio();
    void createAudioOutput();

    //graphical elements
    QComboBox *m_networkDeviceBox;
    //Isomorphic array of pcap compatible names
    QStringList m_networkDeviceList;
    //QComboBox *m_audioDeviceBox;
    QLabel *m_volumeLabel;
    QSlider *m_volumeSlider;
    QSlider *m_frequencySlider;

    QSpinBox *m_numberOfTones;
    QLabel *m_numberOfTonesLabel;

    QSpinBox *m_baseFrequency;
    QLabel *m_baseFrequencyLabel;

    QStatusBar *m_statusBar;
    QLabel *m_statusBarLabel;

    //useless?
    QString *m_statusBarLabelString;

    //handling the tones
    ToneManager* toneManager;
    QThread* ToneManagerThread;
    QThread* PacketCaptureThread;

    //the packetcapturer, could be a singleton? probably
    PacketCapturer* pk;

    int currentTone;

private:
    void SetupGraph();


private slots:
    void PcapButtonPressed();
    void UpdateChart(int freq, int priority);

signals:
   void VolumeChanged(int);

   void SIGNAL_BEGIN_TONES();
   void SIGNAL_BEGIN_CAPTURE();
};

#endif // AUDIOOUTPUT_H
