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
class AudioGraph;

#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts>

#include <QtDataVisualization>

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
    //QComboBox *m_audioDeviceBox;
    QLabel *m_volumeLabel;
    QSlider *m_volumeSlider;
    QSlider *m_frequencySlider;
    QSpinBox *m_numberOfTones;
    QLabel *m_numberOfTonesLabel;
    QStatusBar *m_statusBar;
    QLabel *m_statusBarLabel;

    QPieSeries *series;
    QChart *chart;
    QChartView *chartView;

    QHash<int, QPieSlice*> slices;

    AudioGraph* audioGraph;
    QMainWindow* audioGraphWindow;

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
