#ifndef TONEMANAGER_H
#define TONEMANAGER_H

class tone;

class QByteArray;
class QAudioFormat;

class QThread;

#include <QHash>;
#include <QList>

struct ToneObject{
    tone* theTone;
    QThread* theThread;
};

/** \abstract Class that acts as the controller to the tones */
class ToneManager
{
public:
    ToneManager();
    ~ToneManager();

protected:
    void AddTone(int i);

    int numberOfTones;

    QHash<int, QByteArray*> toneBuffers;
    QList<ToneObject> Tones;

public slots:
    void SLOT_NumberOfTonesChanged(int newNumberOfTones);
    void SLOT_VolumeChanged(int newVolume);
    void SLOT_SetFrequency(int frequency);

    void SLOT_InitializeTones();

signals:
    void Resume_Audio();
    void Pause_Audio();
    void Start_Audio();
};

#endif // TONEMANAGER_H
