#ifndef TONEMANAGER_H
#define TONEMANAGER_H

class tone;

class QByteArray;
class QAudioFormat;

class QThread;

#include <QHash>
#include <QObject>
#include <QList>

struct ToneObject{
    tone* theTone;
    QThread* theThread;
};

/** \abstract Class that acts as the controller to the tones */
class ToneManager : public QObject
{
    Q_OBJECT

public:
    ToneManager();
    ToneManager(int BaseNumberOfTones);
    ~ToneManager();

protected:
    void AddTone(int i);

    int numberOfTones;
    int currentTone;

    QHash<int, QByteArray*> toneBuffers;
    QList<ToneObject> Tones;

public slots:
    void SLOT_NumberOfTonesChanged(int newNumberOfTones);
    void SLOT_VolumeChanged(int newVolume);
    void SLOT_SetFrequency(int frequency);
    void SLOT_InitializeTones();

signals:
    void Set_Volume(int);
    void Resume_Audio();
    void Pause_Audio();
    void Start_Audio();
};

#endif // TONEMANAGER_H
