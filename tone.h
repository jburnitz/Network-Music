#ifndef TONE_H
#define TONE_H

#include <QAudioDeviceInfo>
#include <QAudioOutput>
#include "generator.h"

class tone : public QAudioOutput
{
    Q_OBJECT

public:
    tone(int volumePercent, int frequency, QObject *parent);
    ~tone();

    void Play();
    void Pause();
    void SetFrequency(qreal value);
    void SetGenerator(Generator* gen);
    QAudioFormat GetFormat();
    Generator *m_generator;

protected:
    QAudioDeviceInfo m_device;
    //Generator *m_generator;
    QAudioOutput *m_audioOutput;
    QIODevice *m_output; // not owned
    QAudioFormat m_format;

    QByteArray m_buffer;
    //QThread* thread;

public slots:
    void OnVolumeChanged(int value);
    void Process();

    void DoResumeAudio();
    void DoPauseAudio();
    void DoStartPlaying();

};

#endif // TONE_H
